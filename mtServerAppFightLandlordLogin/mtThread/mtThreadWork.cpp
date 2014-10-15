#include "mtThreadWork.h"
#include "mtProtocol.h"
#include "mtProtocolMgr.h"


DWORD			mtThreadWork::ms_dwLocalAddressLength		= 0;
DWORD			mtThreadWork::ms_dwRemoteAddressLength		= 0;
DWORD			mtThreadWork::ms_dwReceiveDataLength			= 0;

SOCKET			mtThreadWork::ms_uiSocketServer			= INVALID_SOCKET;
HANDLE			mtThreadWork::ms_hIOCP						= INVALID_HANDLE_VALUE;

LPFN_ACCEPTEX	mtThreadWork::ms_lpfnAcceptEx				= NULL;
LPFN_DISCONNECTEX	mtThreadWork::ms_lpfnDisconnectEx		= NULL;

mtThreadWork::mtThreadWork() :
	m_ulTimeWaitMilliseconds(0),
	m_iTimeConnectLimitSencods(0),
	m_eThreadState(E_THREAD_STATE_STOP),
	m_iThreadTotal(0),
	m_pkUserList(NULL),
	m_iUserTotal(0),
	m_iThreadIdx(0),
	m_hThread(NULL),
	m_hEvent(NULL)
{
	m_pkCompleteFunList[E_OLT_ACCEPTEX]			= CompleteAcceptEx;
	m_pkCompleteFunList[E_OLT_WSARECV]			= CompleteWSARecv;
	m_pkCompleteFunList[E_OLT_WSASEND]			= CompleteWSASend;
	m_pkCompleteFunList[E_OLT_WSADISCONNECTEX]	= CompleteWSADisconnectEx;
}

mtThreadWork::~mtThreadWork()
{

}

int mtThreadWork::init( DataInit* pkDataInit )
{
	memcpy (&m_kDataInit, pkDataInit, sizeof(m_kDataInit));

	ms_hIOCP				= pkDataInit->hIOCP;
	ms_uiSocketServer		= pkDataInit->uiSocketServer;

	m_iThreadIdx			= pkDataInit->iThreadIdx;
	m_iThreadTotal			= pkDataInit->iThreadTotal;
	m_eThreadState			= E_THREAD_STATE_RUNNING;

	m_pkUserList			= pkDataInit->pkUserList;
	m_iUserTotal			= pkDataInit->iUserTotal;

	ms_lpfnAcceptEx			= pkDataInit->lpfnAcceptEx;
	ms_lpfnDisconnectEx		= pkDataInit->lpfnDisconnectEx;

	m_ulTimeWaitMilliseconds	= pkDataInit->ulTimeWaitMilliseconds;
	m_iTimeConnectLimitSencods	= pkDataInit->iTimeConnectLimitSencods;

	ms_dwLocalAddressLength		= sizeof(SOCKADDR_IN) + 16;
	ms_dwRemoteAddressLength		= sizeof(SOCKADDR_IN) + 16;
	ms_dwReceiveDataLength		= sizeof(m_pkUserList->pcDataSend) 
		- ms_dwLocalAddressLength - ms_dwRemoteAddressLength;

	m_pkHallMaxIdCs             = pkDataInit->pkHallMaxIdCs;

	mtProtocol::DataInit						kProtocolMgrDataInit;
	m_pkProtocolMgr							= new mtProtocolMgr;
	MT_EXCEPTION_DEBUG(NULL != m_pkProtocolMgr, E_EXCEPTION_CREATESERVCIE_MGR);
	kProtocolMgrDataInit.lStructBytes		= sizeof(kProtocolMgrDataInit);
	kProtocolMgrDataInit.pkSQLEnv				= pkDataInit->pkSQLEnv;
	kProtocolMgrDataInit.pkHallMaxIdCs      = m_pkHallMaxIdCs;
	kProtocolMgrDataInit.lpfnDisconnectEx	= ms_lpfnDisconnectEx;
	kProtocolMgrDataInit.ulIdBeg				= pkDataInit->ulIdBeg;
	m_pkProtocolMgr->init(&kProtocolMgrDataInit);

	m_ulOverlappedEntryTotal				= sizeof(m_pkOverlappedEntryList) / sizeof(OVERLAPPED_ENTRY);

	m_hEvent								= CreateEvent(NULL, FALSE, FALSE, NULL);
	MT_EXCEPTION_DEBUG(NULL != m_hEvent, E_EXCEPTION_CREATE_EVENT);

	m_hThread	= CreateThread(NULL, 0, Run, (void*)this, 0, NULL);
	MT_EXCEPTION_DEBUG(NULL != m_hThread, E_EXCEPTION_CREATETHREAD);

	return	1;
}

int mtThreadWork::exit()
{
	printf ("Thread:\t%d, 开始关闭", m_iThreadIdx);

	if (NULL != m_hEvent)
	{
		CloseHandle(m_hEvent);
		m_hEvent	= NULL;
	}

	if (NULL != m_hThread)
	{
		m_eThreadState		= E_THREAD_STATE_STOP;
		Sleep(1000);
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		m_hThread	= NULL;
	}

	ms_hIOCP					= NULL;
	ms_uiSocketServer			= INVALID_SOCKET;
	m_ulTimeWaitMilliseconds	= 0;
	m_iTimeConnectLimitSencods	= 0;
	m_eThreadState				= E_THREAD_STATE_STOP;
	m_iThreadTotal				= 0;
	m_pkUserList				= NULL;
	m_iUserTotal				= 0;
	m_iThreadIdx				= 0;
	ms_lpfnAcceptEx				= NULL;
	ms_lpfnDisconnectEx			= NULL;
	ms_dwLocalAddressLength		= 0;
	ms_dwRemoteAddressLength	= 0;
	ms_dwReceiveDataLength		= 0;
	m_hThread					= NULL;

	delete m_pkProtocolMgr;
	m_pkProtocolMgr = NULL;

	return	1;
}

int mtThreadWork::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_CREATETHREAD)
			MT_EXCETPION_STRING(E_EXCEPTION_CREATEFILE_MGR)
			MT_EXCETPION_STRING(E_EXCEPTION_CREATESERVCIE_MGR)
			MT_EXCETPION_STRING(E_EXCEPTION_CREATE_EVENT)
	default:
		MT_DEBUG(7, "该错误没有被处理");
		return	0;
	}

	return	1;
}

int mtThreadWork::run()
{
	DataUser*	pkDataUser	= NULL;

	SYSTEMTIME st;
	GetLocalTime(&st);		//得到本地时间/日期
	char buff[256];			//存放时间/日期信息的缓冲区
	//sprintf(buff,"%u:%u:%u %u:%u:%u",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	sprintf(buff,"%u:%u:%u %u",st.wHour,st.wMinute,st.wSecond,st.wMilliseconds);

	BOOL	bQueueIOCPRet = GetQueuedCompletionStatusEx(ms_hIOCP, m_pkOverlappedEntryList, MAXIMUM_WAIT_OBJECTS, 
		&m_ulOverlappedEntryCount, INFINITE, TRUE);

	MT_DEBUG(PTCL, "\n\nStart [iRes:%d][count:%d][error:%d][%s]",
		bQueueIOCPRet,m_ulOverlappedEntryCount,WSAGetLastError(),buff);

	if ( TRUE == bQueueIOCPRet)
	{
		int iRet = -1;

		for (unsigned long ulOverlappedEntryIdx = 0; ulOverlappedEntryIdx < m_ulOverlappedEntryCount; ulOverlappedEntryIdx++)
		{
			m_pkOverlappedEntry				= m_pkOverlappedEntryList + ulOverlappedEntryIdx;
			pkDataUser			= (DataUser*)m_pkOverlappedEntry->lpOverlapped;

			MT_DEBUG(PTCL, "\nwhile [OEntry:%d][pkDataUser:%d][Fun:%d][error:%d][status:%d]",
				m_pkOverlappedEntry,pkDataUser, (pkDataUser?pkDataUser->eOverlappedType : -1),WSAGetLastError(),
				m_pkOverlappedEntry->lpOverlapped->Internal);

			if (STATUS_WAIT_0 == m_pkOverlappedEntry->lpOverlapped->Internal)
			{
				m_ulTransmitBytes	= m_pkOverlappedEntry->dwNumberOfBytesTransferred;
				int		iFuncResult		= (m_pkCompleteFunList[pkDataUser->eOverlappedType])(pkDataUser, this);

				MT_DEBUG(PTCL, "\nOverlapped1 [FunList:%d] [iResult:%d]",pkDataUser->eOverlappedType, iFuncResult);

				if (mtProtocol::E_RESULT_SUCCESS_NEED_SEND == iFuncResult)		
				{
					if (pkDataUser)
					{
						iRet = RunWSASend(pkDataUser);
					}
				}
				else if (iFuncResult <= 0) 
				{											
					if (pkDataUser)
					{
						iRet = CompleteWSASend(pkDataUser, this);
					}
				}
			} 
			else
			{
				MT_DEBUG(PTCL, "\nOverlapped2");

				if (pkDataUser)
				{
					iRet = CompleteWSASend(pkDataUser, this);
				}
			}
		}

		MT_DEBUG(PTCL, "\nEnd1 [error:%d]",WSAGetLastError());

		return	iRet;
	}
	else 
	{
		MT_DEBUG(PTCL, "\nEnd2 [error:%d] ",GetLastError());

		unsigned long	ulError		= GetLastError();
		switch (ulError)
		{
		case	ERROR_NOT_LOCKED:					/// CloseHandle hFile
		case	ERROR_OPERATION_ABORTED:			/// CancelIoEx	hFile
			return	1;
		default:
			break;
		}

		return	0;
	}


	/// 错误处理
	printf ("%s\t%d\n", __FILE__, __LINE__);

	return	0;

}

DWORD mtThreadWork::Run(LPVOID pData)
{
	mtThreadWork*	pkThreadWork	= (mtThreadWork*)pData;

	do
	{
		if (E_THREAD_STATE_SUSPEND == pkThreadWork->m_eThreadState)
		{
			SetEvent(pkThreadWork->m_hEvent);
			SuspendThread(pkThreadWork->m_hThread);
		}

		while (E_THREAD_STATE_RUNNING == pkThreadWork->m_eThreadState) 
		{
			pkThreadWork->run();
		}

	} while (E_THREAD_STATE_STOP != pkThreadWork->m_eThreadState);

	
	SetEvent(pkThreadWork->m_hEvent);
	return	1;
}

int	mtThreadWork::setThreadState(EThreadState eThreadState, unsigned long ulMilliseconds)
{
	int				iResult			= 1;
	EThreadState	eThreadStatePre	= m_eThreadState;
	m_eThreadState	= eThreadState;

	switch (eThreadState)
	{
	case mtThread::E_THREAD_STATE_STOP:
		iResult	= (WAIT_OBJECT_0  == WaitForSingleObject(m_hEvent, ulMilliseconds));
	case mtThread::E_THREAD_STATE_SUSPEND:
		iResult	= (WAIT_OBJECT_0  == WaitForSingleObject(m_hEvent, ulMilliseconds));
	default:
		if (E_THREAD_STATE_SUSPEND == eThreadStatePre)
		{
			iResult = (-1 != ResumeThread(m_hThread));
		}
		break;
	}

	if (0 == iResult)
	{
		m_eThreadState	= eThreadStatePre;
	}

	return	iResult;
}

//setsockopt,成功-CompleteWSARecv，失败-CompleteWSASend
int mtThreadWork::CompleteAcceptEx( DataUser* pkDataUser, mtThreadWork* pkThreadWork )
{
 	int iSetSockOptRet = setsockopt(pkDataUser->uiSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
 		(const char*)&ms_uiSocketServer, sizeof(SOCKET));

	if (0 == iSetSockOptRet)
	{
		return	CompleteWSARecv(pkDataUser, pkThreadWork);
	}
	
	MT_DEBUG_PTCL("\nCompleteAcceptEx setsockopt error");
	return	CompleteWSASend(pkDataUser, pkThreadWork);
}

//处理recv数据后发送回应，否则CompleteWSASend
int mtThreadWork::CompleteWSARecv( DataUser* pkDataUser, mtThreadWork* pkThreadWork )
{
	int		iResult	= 0;
	unsigned long ulRecvBytes	= *((unsigned long*)pkDataUser->pcDataRecv);
	if (pkThreadWork->m_ulTransmitBytes !=0 && ulRecvBytes == pkThreadWork->m_ulTransmitBytes)
	{
		iResult	= pkThreadWork->m_pkProtocolMgr->run(pkDataUser);
	}
	
	if (mtProtocol::E_RESULT_SUCCESS_NEED_SEND == iResult)
	{
		return	RunWSASend(pkDataUser);
	}
	else if (mtProtocol::E_RESULT_SUCCESS == iResult)
	{
		return	1;
	}
	else 
	{
		return	CompleteWSASend(pkDataUser, pkThreadWork);
	}
}

//send，否则CompleteWSASend
int mtThreadWork::RunWSASend( DataUser* pkDataUser )
{
	DWORD		ulSendBytes		= 0;
	pkDataUser->kWSABufSend.buf		= (char*)pkDataUser->pcDataSend;
	pkDataUser->kWSABufSend.len		= *((ULONG*)pkDataUser->pcDataSend);
	pkDataUser->eOverlappedType		= mtThreadWork::E_OLT_WSASEND;
	int		iRet	= WSASend(pkDataUser->uiSocket, &pkDataUser->kWSABufSend, 1, &ulSendBytes, 0, 
		&pkDataUser->kOverlapped, NULL);

	MT_DEBUG(PTCL, "\nRunWSASend [WSASend:%d][error:%d] ",iRet,WSAGetLastError());

	if (SOCKET_ERROR == iRet && ERROR_IO_PENDING != WSAGetLastError())
	{
		return	CompleteWSASend(pkDataUser, NULL);
	}

	return	1;
}

//断开连接,否则ResetSocket
int mtThreadWork::CompleteWSASend( DataUser* pkDataUser, mtThreadWork* pkThreadWork )
{	
	return	ResetSocket(pkDataUser);
}

//投递accpet,否则ResetSocket
int mtThreadWork::CompleteWSADisconnectEx( DataUser* pkDataUser, mtThreadWork* pkThreadWork )
{
	/// 服务器和客户端都断开
	DWORD dwBytesReceived			= 0;
	pkDataUser->eOverlappedType		= E_OLT_ACCEPTEX;
	pkDataUser->closeSocketMilliSec	= 0;
	pkDataUser->closeFlag			= TRUE;
	BOOL	bAcceptExRet = ms_lpfnAcceptEx(ms_uiSocketServer, pkDataUser->uiSocket, 
		pkDataUser->pcDataRecv, ms_dwReceiveDataLength, ms_dwLocalAddressLength, 
		ms_dwRemoteAddressLength, &dwBytesReceived, &(pkDataUser->kOverlapped));
 	int		iAcceptError = WSAGetLastError();

	MT_DEBUG(PTCL, "\nCompleteWSADisconnectEx [ms_lpfnAcceptEx:%d][error:%d] ",bAcceptExRet,WSAGetLastError());

	if ( (TRUE == bAcceptExRet) || (WSA_IO_PENDING == iAcceptError) || (WSAEINVAL == iAcceptError))
	{
		return	1;
	}

	return	ResetSocket(pkDataUser);
}

//重新创建socket并绑定到完成端口，投递accpet
int mtThreadWork::ResetSocket( DataUser* pkDataUser )
{
	if (INVALID_SOCKET != pkDataUser->uiSocket)
	{
		closesocket(pkDataUser->uiSocket);
	}

	pkDataUser->closeSocketMilliSec	= 0;
	pkDataUser->closeFlag			= TRUE;

	if (INVALID_SOCKET == (pkDataUser->uiSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)))
	{
		MT_DEBUG_PTCL("\nResetSocket WSASocket error");
		return	0;
	}

	int		iRcvBufBytes	= 0;
	if (0 != setsockopt(pkDataUser->uiSocket, SOL_SOCKET, SO_RCVBUF, (char*)&iRcvBufBytes, sizeof(iRcvBufBytes)))
	{
		MT_DEBUG_PTCL("\nResetSocket setsockopt1 error");
		goto	MT_LOCAL_ERROR;
	}

	int		iSndBufBytes	= 0;
	if (0 != setsockopt(pkDataUser->uiSocket, SOL_SOCKET, SO_SNDBUF, (char*)&iSndBufBytes, sizeof(iSndBufBytes)))
	{
		MT_DEBUG_PTCL("\nResetSocket setsockopt2 error");
		goto	MT_LOCAL_ERROR;
	}

	/// bind IOCP
	if (NULL == CreateIoCompletionPort((HANDLE)pkDataUser->uiSocket, ms_hIOCP, (ULONG_PTR)pkDataUser, NULL))
	{
		MT_DEBUG_PTCL("\nResetSocket CreateIoCompletionPort error");
		goto	MT_LOCAL_ERROR;
	}

	/// 服务器和客户端都断开
	DWORD	dwBytesReceived			= 0;
	pkDataUser->eOverlappedType		= E_OLT_ACCEPTEX;
	pkDataUser->closeSocketMilliSec	= 0;
	pkDataUser->closeFlag				= TRUE;
	BOOL	bAcceptExRet = ms_lpfnAcceptEx(ms_uiSocketServer, pkDataUser->uiSocket, 
		pkDataUser->pcDataRecv, ms_dwReceiveDataLength, ms_dwLocalAddressLength, 
		ms_dwRemoteAddressLength, &dwBytesReceived, &(pkDataUser->kOverlapped));

	MT_DEBUG(PTCL, "\nResetSocket [ms_lpfnAcceptEx:%d][socket:%d][error:%d] ",bAcceptExRet,
		pkDataUser->uiSocket,WSAGetLastError());

	int		iAcceptError = WSAGetLastError();
	if ( (TRUE == bAcceptExRet) || (WSA_IO_PENDING == iAcceptError) || (WSAEINVAL == iAcceptError))
	{
		return	1;
	}

MT_LOCAL_ERROR:

	MT_DEBUG(PTCL, "\nMT_LOCAL_ERROR [error:%d] ",WSAGetLastError());

	closesocket(pkDataUser->uiSocket);
	pkDataUser->uiSocket		= INVALID_SOCKET;
	return	0;
}
