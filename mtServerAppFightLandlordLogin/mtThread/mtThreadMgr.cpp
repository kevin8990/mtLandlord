#include "mtThreadMgr.h"
#include "mtThreadMonitor.h"

mtThreadMgr::mtThreadMgr() :
	m_pkThreadWorkList(NULL)
{
	
}

mtThreadMgr::~mtThreadMgr()
{

}

int mtThreadMgr::init( DataInit* pkDataInit )
{
	memcpy(&m_kThreadMgrDataInit, pkDataInit, sizeof(m_kThreadMgrDataInit));
	m_lpfnAcceptEx		= pkDataInit->lpfnAcceptEx;
	m_lpfnDisconnectEx	= pkDataInit->lpfnDisconnectEx;
	m_pkSQLEnv			= pkDataInit->pkSQLEnv;
	
	initThreadWork();
	initThreadMonitor();

	return 1;
}

int mtThreadMgr::exit()
{
	if (NULL != m_pkThreadWorkList)
	{
		for (int iThreadIdx = 0; iThreadIdx < m_iThreadWorkTotal; iThreadIdx++)
		{
			m_pkThreadWorkList[iThreadIdx].exit();
		}
		Sleep(3000);
		free (m_pkThreadWorkList);
		m_pkThreadWorkList	= NULL;
	}

	DeleteCriticalSection(&m_kHallMaxIdCs);

	return 1;
}

int mtThreadMgr::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_THREADWORK_ALLOC)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		break;
	}

	return	1;
}


HANDLE mtThreadMgr::createIOCP( int iThreadTotal )
{
	HANDLE	hIOCP			= CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, iThreadTotal);
	MT_EXCEPTION_DEBUG(NULL != hIOCP, E_EXCEPTION_IOCP_CREATE);
	return	hIOCP;
}

SOCKET mtThreadMgr::createSocketServer( HANDLE hIOCP, int iListenTotal, 
		unsigned long ulIP, unsigned short usPort )
{
	/// 创建socket
	SOCKET	uiSocketServer	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	MT_EXCEPTION_DEBUG(INVALID_SOCKET != uiSocketServer, E_EXCEPTION_SERVER_SOCKET);
	int		iRcvBufBytes	= 655360;
	int		iRet			= setsockopt(uiSocketServer, SOL_SOCKET, SO_RCVBUF, (char*)&iRcvBufBytes, sizeof(iRcvBufBytes));
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_SERVER_OPT_RCVBUF);
	int		iSndBufBytes	= 655360;
	iRet					= setsockopt(uiSocketServer, SOL_SOCKET, SO_SNDBUF, (char*)&iSndBufBytes, sizeof(iSndBufBytes));
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_SERVER_OPT_SNDBUF);
	BOOL	bReUseAddr		= TRUE;
	iRet					= setsockopt(uiSocketServer, SOL_SOCKET, SO_REUSEADDR, (char*)&bReUseAddr, sizeof(bReUseAddr));
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_SERVER_OPT_REUSEADDR);

	/// bind IOCP
	HANDLE	hRet			= CreateIoCompletionPort((HANDLE)uiSocketServer, hIOCP, 0, NULL);
	MT_EXCEPTION_DEBUG(NULL != hRet, E_EXCEPTION_SERVER_IOCP_BIND);
	/// bind socket
	SOCKADDR_IN		kSockAddrIn;
	memset (&kSockAddrIn, 0, sizeof(kSockAddrIn));
	kSockAddrIn.sin_family		= AF_INET;
	kSockAddrIn.sin_addr.s_addr	= htonl(INADDR_ANY);
	kSockAddrIn.sin_port		= usPort;
	int iBindRet = bind (uiSocketServer, (SOCKADDR*)&kSockAddrIn, sizeof(kSockAddrIn));
	MT_EXCEPTION_DEBUG(0 == iBindRet, E_EXCEPTION_SERVER_BIND);

	/// listen socket
	int iListenRet = listen (uiSocketServer, iListenTotal);
	MT_EXCEPTION_DEBUG(0 == iListenRet, E_EXCEPTION_SERVER_LISTEN);

	return	uiSocketServer;
}

SOCKET	mtThreadMgr::createSocketClient( HANDLE hIOCP, void* pData )
{
	SOCKET	uiSocketClient	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	MT_EXCEPTION_DEBUG(INVALID_SOCKET != uiSocketClient, E_EXCEPTION_CLIENT_SOCKET);
	int		iRcvBufBytes	= 0;
	int		iRet		= setsockopt(uiSocketClient, SOL_SOCKET, SO_RCVBUF, (char*)&iRcvBufBytes, sizeof(iRcvBufBytes));
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_CLIENT_OPT_RCVBUF);
	int		iSndBufBytes	= 0;
	iRet				= setsockopt(uiSocketClient, SOL_SOCKET, SO_SNDBUF, (char*)&iSndBufBytes, sizeof(iSndBufBytes));
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_CLIENT_OPT_SNDBUF);

	/// bind IOCP
	HANDLE	hRet	= CreateIoCompletionPort((HANDLE)uiSocketClient, hIOCP, (ULONG_PTR)pData, NULL);
	MT_EXCEPTION_DEBUG(NULL != hRet, E_EXCEPTION_CLIENT_IOCP_BIND);

	return	uiSocketClient;
}


int mtThreadMgr::initThreadWork()
{
	/// 获取
	SYSTEM_INFO		kSystemInfo;
	GetSystemInfo(&kSystemInfo);
	m_iThreadWorkTotal	= kSystemInfo.dwNumberOfProcessors;
	//m_iThreadWorkTotal		=	3;

	InitializeCriticalSectionAndSpinCount(&m_kHallMaxIdCs, m_iThreadWorkTotal);

	HANDLE	hIOCP			= createIOCP(m_iThreadWorkTotal);
	SOCKET	uiSocketServer	= createSocketServer(hIOCP, SOMAXCONN, 
		m_kThreadMgrDataInit.ulWorkIP, m_kThreadMgrDataInit.usWorkPort);

	m_iThreadWorkUserTotal				= m_iThreadWorkTotal;
	//m_iThreadWorkUserTotal				= MT_SERVER_WORK_SOCKET_TOTAL;
	m_pkThreadWorkUserList				= (mtThreadWork::DataUser*)
		malloc(m_iThreadWorkUserTotal * sizeof(mtThreadWork::DataUser));
	memset (m_pkThreadWorkUserList, 0, m_iThreadWorkUserTotal * sizeof(mtThreadWork::DataUser));

	DWORD dwLocalAddressLength			= sizeof(SOCKADDR_IN) + 16;
	DWORD dwRemoteAddressLength			= sizeof(SOCKADDR_IN) + 16;
	DWORD dwReceiveDataLength			= sizeof(m_pkThreadWorkUserList->pcDataRecv) 
		- dwLocalAddressLength - dwRemoteAddressLength;
	DWORD dwBytesRece					= 0;

	for (int iUserIdx = 0; iUserIdx < m_iThreadWorkUserTotal; iUserIdx++)
	{
		mtThreadWork::DataUser*	pkDataUser	= m_pkThreadWorkUserList + iUserIdx;
		pkDataUser->uiSocket				= createSocketClient( hIOCP, pkDataUser);
		pkDataUser->eOverlappedType			= mtThreadWork::E_OLT_ACCEPTEX;
		pkDataUser->closeSocketMilliSec		= 0;
		pkDataUser->closeFlag				= TRUE;
//		pkDataUser->hFile					= INVALID_HANDLE_VALUE;

		BOOL	bAcceptExRet				= m_lpfnAcceptEx(uiSocketServer, 
			pkDataUser->uiSocket, pkDataUser->pcDataRecv, dwReceiveDataLength, 
			dwLocalAddressLength, dwRemoteAddressLength,  &dwBytesRece, &(pkDataUser->kOverlapped));
		MT_EXCEPTION_DEBUG_IOCP(bAcceptExRet, E_EXCEPTION_SERVER_ACCEPTEX);

		MT_DEBUG(PTCL, "\nm_lpfnAcceptEx [error:%d] !\n",WSAGetLastError());
	}

	/// 初始化工作线程
	m_pkThreadWorkList								= new mtThreadWork[m_iThreadWorkTotal];
	MT_EXCEPTION_DEBUG(NULL != m_pkThreadWorkList, E_EXCEPTION_THREADWORK_ALLOC);
	memset (&m_kThreadWorkDataInit, 0, sizeof(m_kThreadWorkDataInit));
	m_kThreadWorkDataInit.lStructBytes				= sizeof(m_kThreadWorkDataInit);
	m_kThreadWorkDataInit.hIOCP						= hIOCP;
	m_kThreadWorkDataInit.uiSocketServer			= uiSocketServer;
	m_kThreadWorkDataInit.ulTimeWaitMilliseconds	= m_kThreadMgrDataInit.ulTimeWaitMilliseconds;
	m_kThreadWorkDataInit.iTimeConnectLimitSencods	= m_kThreadMgrDataInit.iTimeConnectLimitSencods;
	m_kThreadWorkDataInit.pkSQLEnv					= m_pkSQLEnv;
	m_kThreadWorkDataInit.pcFolder					= m_kThreadMgrDataInit.pcFolder;
	m_kThreadWorkDataInit.pkHallMaxIdCs             = &m_kHallMaxIdCs;
	m_kThreadWorkDataInit.pkUserList				= m_pkThreadWorkUserList;
	m_kThreadWorkDataInit.iUserTotal				= m_iThreadWorkUserTotal;
	m_kThreadWorkDataInit.iThreadTotal				= m_iThreadWorkTotal;
	m_kThreadWorkDataInit.lpfnAcceptEx				= m_lpfnAcceptEx;
	m_kThreadWorkDataInit.lpfnDisconnectEx			= m_lpfnDisconnectEx;
	for (int iThreadIdx = 0; iThreadIdx < m_iThreadWorkTotal; iThreadIdx++)
	{
		m_kThreadWorkDataInit.ulIdBeg				= 0xFFFFFFFF / m_iThreadWorkTotal * iThreadIdx + 0xFFFF;
		m_kThreadWorkDataInit.iThreadIdx			= iThreadIdx;
		m_pkThreadWorkList[iThreadIdx].init(&m_kThreadWorkDataInit);
	}

	return		1;
}

int mtThreadMgr::initThreadMonitor()
{
	memset (&m_kThreadMonitorDataInit, 0, sizeof(m_kThreadMonitorDataInit));
	m_kThreadMonitorDataInit.lStructBytes				= sizeof(m_kThreadMonitorDataInit);
	m_kThreadMonitorDataInit.hIOCP						= NULL;
	m_kThreadMonitorDataInit.uiSocketServer				= INVALID_SOCKET;
	m_kThreadMonitorDataInit.lpfnAcceptEx				= m_lpfnAcceptEx;
	m_kThreadMonitorDataInit.lpfnDisconnectEx			= m_lpfnDisconnectEx;
	m_kThreadMonitorDataInit.ulTimeWaitMilliseconds		= 0;
	m_kThreadMonitorDataInit.iTimeConnectLimitSencods	= 0;
	//m_kThreadMonitorDataInit.pkUserList					= NULL;
	//m_kThreadMonitorDataInit.iUserTotal					= 0;
	m_kThreadMonitorDataInit.pkThreadMgr				= this;

	return	m_kThreadMonitor.init(&m_kThreadMonitorDataInit);
	
}

int mtThreadMgr::run()
{
	//while (m_kThreadMonitor.run());

	DWORD sec;
	while (true)
	{
		//printf("Socket超时监控线程运行\n");
		Sleep(1*1000);
		//sec = GetTickCount();
		m_kThreadMonitor.run();
		//MT_DEBUG(PTCL, "Socket超时监控线程运行 sec:%d\n",(GetTickCount()-sec));
	}

	return	1;
}



