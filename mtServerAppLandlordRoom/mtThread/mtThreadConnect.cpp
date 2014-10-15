#include "mtThreadConnect.h"
											
mtThreadConnect::mtThreadConnect() :
	m_iSocketServer(INVALID_SOCKET),
	m_uiSocketClient(INVALID_SOCKET),
	m_hThread(INVALID_HANDLE_VALUE),
	m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL)
{
	memset (&m_kSockAddrInServer, 0, sizeof(m_kSockAddrInServer));
	memset (&m_kSockAddrInClient, 0, sizeof(m_kSockAddrInClient));
}

mtThreadConnect::~mtThreadConnect()
{

}

int mtThreadConnect::run()
{
	int		iSockAddrInBytes = sizeof(m_kSockAddrInClient);
	m_uiSocketClient	= WSAAccept(m_iSocketServer, (SOCKADDR*)&m_kSockAddrInClient, 
		&iSockAddrInBytes, NULL, NULL);

	if (INVALID_SOCKET != m_uiSocketClient)
	{
		/// 创建用户结点
		mtQueueUser::DataNode*		pkDataNodeUser = (mtQueueUser::DataNode*)m_pkQueueUser->popInitDataNode(m_uiSocketClient);
		pkDataNodeUser->lUserExit       = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING;
		pkDataNodeUser->lUserIOCPStatus = 0;

		mtQueuePacket::DataNode*	pkDataNodePacket = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("threadconnect36");

		if ((NULL != pkDataNodeUser) && (NULL != pkDataNodePacket))
		{
			int iRet = m_pkQueueUser->DataRead(pkDataNodeUser, pkDataNodePacket);
		}
		/// 设为无效
		m_uiSocketClient	= INVALID_SOCKET;
	}

	return	0;
}

int	mtThreadConnect::init( DataInit* pkInitData )
{
	/// 判断 pkInitData 是否为空
	MT_EXCEPTION_DEBUG(NULL == pkInitData, E_EXCEPTION_INIT_DATA_IS_NULL);

	m_pkQueueUser			= pkInitData->pkQueueUser;
	m_pkQueuePacket			= pkInitData->pkQueuePacket;
	memset (&m_kSockAddrInClient, 0, sizeof(m_kSockAddrInClient));
	m_uiSocketClient			= INVALID_SOCKET;

	/// create socket
	m_iSocketServer	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	MT_EXCEPTION_DEBUG (INVALID_SOCKET == m_iSocketServer, E_EXCEPTION_SERVER_SOCKET);

	/// bind socket
	memset (&m_kSockAddrInServer, 0, sizeof(m_kSockAddrInServer));
	m_kSockAddrInServer.sin_family		= AF_INET;
	m_kSockAddrInServer.sin_addr.s_addr = htonl(INADDR_ANY);
	m_kSockAddrInServer.sin_port		= pkInitData->iPort;
	MT_EXCEPTION_DEBUG (SOCKET_ERROR == bind (m_iSocketServer, (SOCKADDR*)&m_kSockAddrInServer, sizeof(m_kSockAddrInServer)), E_EXCEPTION_SERVER_BIND);

	/// Starting Listening
	MT_EXCEPTION_DEBUG (SOCKET_ERROR == listen(m_iSocketServer, pkInitData->iListenNum), E_EXCEPTION_SERVER_LISTEN);

	/// 设置为非堵塞状态
	unsigned long 	ulIoctlSocketFionbioFlagCur	= 1;
	MT_EXCEPTION_DEBUG(0 != ioctlsocket(m_iSocketServer, FIONBIO, &ulIoctlSocketFionbioFlagCur), E_EXCEPTION_SERVER_IOCTLSOCKET_SET);

	/// 创建线程
	m_hThread	=	(HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, E_EXCEPTION__BEGINTHREAD);	

	m_eThreadState			= E_THREAD_STATE_RUN;

	return	1;
}

int mtThreadConnect::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_INIT_DATA_IS_NULL)
		MT_EXCETPION_STRING(E_EXCEPTION_SERVER_SOCKET)
		MT_EXCETPION_STRING(E_EXCEPTION_SERVER_BIND)
		MT_EXCETPION_STRING(E_EXCEPTION_SERVER_LISTEN)
		MT_EXCETPION_STRING(E_EXCEPTION_SERVER_IOCTLSOCKET_GET)
		MT_EXCETPION_STRING(E_EXCEPTION_SERVER_IOCTLSOCKET_SET)
		MT_EXCETPION_STRING(E_EXCEPTION__BEGINTHREAD)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		break;
	}

	return	1;
}

int mtThreadConnect::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}

	if (INVALID_SOCKET != m_uiSocketClient)
	{
		closesocket(m_uiSocketClient);

		m_uiSocketClient	= INVALID_SOCKET;
		
	}
	memset (&m_kSockAddrInClient, 0, sizeof(m_kSockAddrInClient));

	if (INVALID_SOCKET != m_iSocketServer)
	{
		closesocket(m_iSocketServer);
		m_iSocketServer	= INVALID_SOCKET;
	}
	memset (&m_kSockAddrInServer, 0, sizeof(m_kSockAddrInServer));

	m_pkQueuePacket	= NULL;
	m_pkQueueUser	= NULL;

	return	1;
}

