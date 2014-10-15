#include "mtThreadMonitorConnect.h"
#include "mtServiceMonitorMgr.h"
											
mtThreadMonitorConnect::mtThreadMonitorConnect() :
	m_iSocketServer(INVALID_SOCKET),
	m_iSocketClient(INVALID_SOCKET),
	m_uiThread(-1L),
	m_pkServiceMonitorMgr(NULL),
	m_pkDataHall(NULL)
{
	memset (&m_kSockAddrInServer, 0, sizeof(m_kSockAddrInServer));
	memset (&m_kSockAddrInClient, 0, sizeof(m_kSockAddrInClient));
}

mtThreadMonitorConnect::~mtThreadMonitorConnect()
{

}

//监听来自房间的连接，并处理数据
int mtThreadMonitorConnect::run()
{
	int		iSockAddrInBytes	= sizeof(m_kSockAddrInClient);
	m_iSocketClient	= accept(m_iSocketServer, (SOCKADDR*)&m_kSockAddrInClient, &iSockAddrInBytes);

	if (INVALID_SOCKET != m_iSocketClient)
	{
 		u_long  ulSocketClientFlag = 1;
 		ioctlsocket(m_iSocketClient, FIONBIO, &ulSocketClientFlag);

		long uStartTime = GetTickCount();
		while(1 != m_pkServiceMonitorMgr->run(&m_iSocketClient))
		{
			//如果数据同步超过5秒未完成，则放弃
			if(GetTickCount()-uStartTime >= 5*1000)
			{
				MT_DEBUG_PTCL("close syn socket:%d\n",GetTickCount()-uStartTime);
				break;
			}
			Sleep(500);
		}

		shutdown(m_iSocketClient, 1);
		closesocket(m_iSocketClient);
		m_iSocketClient	= INVALID_SOCKET;
	}

	return	0;
}

int	mtThreadMonitorConnect::init( DataInit* pkInitData )
{
	/// 判断 pkInitData 是否为空
	MT_EXCEPTION_DEBUG(NULL == pkInitData, EXCEPTION_INIT_DATA_IS_NULL);

	m_pkServiceMonitorMgr   = pkInitData->pkServiceMonitorMgr;
	m_pkDataHall            = pkInitData->pkDataHall;
	/// m_pkSpace               = pkInitData->pkSpace;

	/// create socket
	m_iSocketServer	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	MT_EXCEPTION_DEBUG (INVALID_SOCKET == m_iSocketServer, EXCEPTION_SERVER_socket);

	/// bind socket
	memset (&m_kSockAddrInServer, 0, sizeof(m_kSockAddrInServer));
	m_kSockAddrInServer.sin_family		= AF_INET;
	m_kSockAddrInServer.sin_addr.s_addr = htonl(INADDR_ANY);
	m_kSockAddrInServer.sin_port		= pkInitData->iPort;
	MT_EXCEPTION_DEBUG (SOCKET_ERROR == bind (m_iSocketServer, (SOCKADDR*)&m_kSockAddrInServer, sizeof(m_kSockAddrInServer)), EXCEPTION_SERVER_bind);

	/// Starting Listening
	MT_EXCEPTION_DEBUG (SOCKET_ERROR == listen(m_iSocketServer, pkInitData->iListenNum), EXCEPTION_SERVER_listen);

	/// 设置为非堵塞状态
	m_ulIoctlSocketFionbioFlagCur	= 1;
	MT_EXCEPTION_DEBUG(0 != ioctlsocket(m_iSocketServer, FIONBIO, &m_ulIoctlSocketFionbioFlagCur), EXCEPTION_SERVER_ioctlsocket_set);

	/// 创建线程
	m_uiThread	= _beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (-1L == m_uiThread, EXCEPTION__beginthread);	

	m_eThreadState			= E_THREAD_STATE_RUN;

	return	1;
}

int mtThreadMonitorConnect::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION_INIT_DATA_IS_NULL)
		MT_EXCETPION_STRING(EXCEPTION_SERVER_socket)
		MT_EXCETPION_STRING(EXCEPTION_SERVER_bind)
		MT_EXCETPION_STRING(EXCEPTION_SERVER_listen)
		MT_EXCETPION_STRING(EXCEPTION_SERVER_ioctlsocket_get)
		MT_EXCETPION_STRING(EXCEPTION_SERVER_ioctlsocket_set)
		MT_EXCETPION_STRING(EXCEPTION__beginthread)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		break;
	}

	return	1;
}

int mtThreadMonitorConnect::exit()
{
	if (-1L != m_uiThread)
	{
		TerminateThread((HANDLE)m_uiThread, 0);
		m_uiThread	= -1L;
	}

	if (INVALID_SOCKET != m_iSocketClient)
	{
		closesocket(m_iSocketClient);
		m_iSocketClient	= INVALID_SOCKET;
	}

	if (INVALID_SOCKET != m_iSocketServer)
	{
		closesocket(m_iSocketServer);
		m_iSocketServer	= INVALID_SOCKET;
	}

	return	1;
}

