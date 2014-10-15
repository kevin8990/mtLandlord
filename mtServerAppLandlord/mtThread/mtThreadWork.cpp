#include "mtThreadWork.h"
#include "mtServiceMgr.h"
///#include "mtSQLQDBCAppLoadlord.h"
///#include "mtSQLQStmtRoomInfo.h"



mtThreadWork::mtThreadWork() :
	m_pkQueueWorkList(NULL),
	m_hThread(0),
	m_ullTimeEnterRun(0)
{
	memset (&m_kQueueWorkNodeClient, 0, sizeof(m_kQueueWorkNodeClient));
	m_kQueueWorkNodeClient.kSocketClient = INVALID_SOCKET;
}

mtThreadWork::~mtThreadWork()
{

}

int mtThreadWork::init( DataInit* pkInitData )
{
	m_pkQueueWorkList		= pkInitData->pkQueueWorkList;

	memset (&m_kQueueWorkNodeClient, 0, sizeof(m_kQueueWorkNodeClient));
	m_kQueueWorkNodeClient.kSocketClient = INVALID_SOCKET;

	/// 
	m_pkServiceMgr			= pkInitData->pkServiceMgr;

	/// 初始化 m_iTimeEnterRun
	m_ullTimeEnterRun		= GetTickCount();

	//select timer 
	m_kTimeVal.tv_sec			= 0;
	m_kTimeVal.tv_usec		    = 10;

	/// 创建线程
	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__BEGINTHREAD);	

	m_eThreadState	= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadWork::exit()
{
	if ((HANDLE)-1L != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= (HANDLE)-1L;
	}

	if (INVALID_SOCKET != m_kQueueWorkNodeClient.kSocketClient)
	{
		m_pkQueueWorkList->push(&m_kQueueWorkNodeClient);
	}

	m_pkQueueWorkList	= NULL;

	return	1;
}

int mtThreadWork::run()
{
	m_ullTimeEnterRun		= GetTickCount();
	int iRet = 0;
	if (m_pkQueueWorkList->pop(&m_kQueueWorkNodeClient) > 0)
	{
		FD_ZERO(&m_kReadSet);
		FD_SET(m_kQueueWorkNodeClient.kSocketClient, &m_kReadSet);

		if (select(m_kQueueWorkNodeClient.kSocketClient + 1, &m_kReadSet, NULL, NULL, &m_kTimeVal) > 0)
		{
			if(m_kQueueWorkNodeClient.bIsAccept)
			{
				m_kQueueWorkNodeClient.timeSec = GetTickCount();
				m_kQueueWorkNodeClient.bIsAccept = FALSE;
			}

			//if(FD_ISSET(m_kQueueWorkNodeClient.kSocketClient,&m_kReadSet)){
				iRet = m_pkServiceMgr->run(&m_kQueueWorkNodeClient.kSocketClient);
				//MT_DEBUG_PTCL("\nmtThreadWork run [socket:%d][iRet:%d][time:%d]\n",m_kQueueWorkNodeClient.kSocketClient,iRet,
					//(m_kQueueWorkNodeClient.timeSec == 0)?0:(GetTickCount()-m_kQueueWorkNodeClient.timeSec));
			/*}else{
				closesocket(m_kQueueWorkNodeClient.kSocketClient);
				m_kQueueWorkNodeClient.kSocketClient = -1;
				m_kQueueWorkNodeClient.timeSec		 = 0;
			}*/

			if(GetTickCount()-m_kQueueWorkNodeClient.timeSec >= MT_SERVER_RECV_DATA_TIMEOUT)
			{
				MT_DEBUG_COLOR(PTCL,FOREGROUND_RED | FOREGROUND_INTENSITY,"\nCLOSE Recv TimeOut [socket:%d][time:%d] ThreadId:%d",m_kQueueWorkNodeClient.kSocketClient
					,(GetTickCount()-m_kQueueWorkNodeClient.timeSec),GetCurrentThreadId());
				//shutdown(m_kQueueWorkNodeClient.kSocketClient,1);
				//closesocket(m_kQueueWorkNodeClient.kSocketClient);
				m_kQueueWorkNodeClient.kSocketClient = INVALID_SOCKET;
				m_kQueueWorkNodeClient.timeSec		 = 0;
			}
		}
		else
		{
			if(!m_kQueueWorkNodeClient.bIsAccept)
			{
				m_kQueueWorkNodeClient.bIsAccept = TRUE;
			}
		}

		//监控连接socket超时
		if(m_kQueueWorkNodeClient.bIsAccept)
		{
			if(GetTickCount()-m_kQueueWorkNodeClient.timeSec >= MT_SERVER_ACCEPT_TIMEOUT)
			{
				MT_DEBUG_COLOR(PTCL,FOREGROUND_RED | FOREGROUND_INTENSITY,"\nCLOSE Accept TimeOut [socket:%d][time:%d] %d",m_kQueueWorkNodeClient.kSocketClient
					,(GetTickCount()-m_kQueueWorkNodeClient.timeSec),GetCurrentThreadId());

				shutdown(m_kQueueWorkNodeClient.kSocketClient,2);
				closesocket(m_kQueueWorkNodeClient.kSocketClient);
				m_kQueueWorkNodeClient.kSocketClient = INVALID_SOCKET;
				m_kQueueWorkNodeClient.timeSec		 = 0;
			}
		}
		//MT_DEBUG(4, "%s\t%p\n", typeid(*this).name(), this);
	}	

	if (INVALID_SOCKET != m_kQueueWorkNodeClient.kSocketClient) 
	{
		m_pkQueueWorkList->push(&m_kQueueWorkNodeClient);
	}

	m_kQueueWorkNodeClient.kSocketClient	= INVALID_SOCKET;
	return	iRet;
}

int mtThreadWork::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION__BEGINTHREAD)
	default:
		MT_DEBUG(7, "该异常没有被处理");
		return	0;
	}

	return	1;

}
