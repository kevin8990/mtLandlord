#include "mtServer.h"
#include "mtException.h"
#include "mtDSQueueMemory.h"

mtServer::mtServer(void)
{
	memset (&m_kThreadMgrDataInit, 0, sizeof(m_kThreadMgrDataInit));
}


mtServer::~mtServer(void)
{
	fclose(g_pkMtLogFile);
	g_pkMtLogFile = NULL;
}

int mtServer::init()
{
	GetSystemInfo(&m_kSystemInfo);
	InitializeCriticalSectionAndSpinCount(&g_kCriticalSection, m_kSystemInfo.dwNumberOfProcessors * 4);

	/// 初始化服务器参数
	initParams();

	m_hIOCP	= CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	m_hEventManagerAi = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(m_hEventManagerAi);
	initQueue();
	MT_DEBUG_PTCL("initQueue success !");
	initService();
	MT_DEBUG_PTCL("\ninitService success !");
	initThread();
	MT_DEBUG_PTCL("\ninitThread success !");

	return 1;
}

int mtServer::exit()
{
	DeleteCriticalSection(&g_kCriticalSection);
	return	1;
}

int mtServer::initQueue()
{
	m_kQueueMgrDataInit.lStructBytes    = sizeof(m_kQueueMgrDataInit);
	m_kQueueMgrDataInit.hIOCP           = m_hIOCP;
	m_kQueueMgrDataInit.iSpinCount      = m_kSystemInfo.dwNumberOfProcessors;
	m_kQueueMgrDataInit.lSpaceTotal     = 2;
	m_kQueueMgrDataInit.plRoomTotal[0]  = 6;
	m_kQueueMgrDataInit.plRoomTotal[1]  = 6;

	m_kQueueMgr.init(&m_kQueueMgrDataInit);

	return 1;
}

int mtServer::initService()
{
	/// m_kSQLEnv
	m_kServiceMgrDataInit.lStructBytes	  = sizeof(m_kServiceMgrDataInit);
	m_kServiceMgrDataInit.iSpinCount      = m_kSystemInfo.dwNumberOfProcessors;
	m_kServiceMgrDataInit.hIOCP           = m_hIOCP;
	m_kServiceMgrDataInit.hEventManagerAi = m_hEventManagerAi;
	m_kServiceMgrDataInit.pkQueueMgr      = &m_kQueueMgr;

	m_kServiceMgr.init(&m_kServiceMgrDataInit);

	/// servie monitor
	m_kServiceMonitorMgrDataInit.lStructBytes	= sizeof(m_kServiceMonitorMgrDataInit);
	m_kServiceMonitorMgrDataInit.pkQueueMgr     = &m_kQueueMgr;

	m_kServiceMonitorMgr.init(&m_kServiceMonitorMgrDataInit);

	return 1;
}

int	mtServer::initThread()
{
	m_kThreadMgrDataInit.lStructBytes	                      = sizeof(m_kThreadMgrDataInit);

	m_kThreadMgrDataInit.hIOCP                                = m_hIOCP;
	m_kThreadMgrDataInit.hEventManagerAi                      = m_hEventManagerAi;
	m_kThreadMgrDataInit.pkServiceMgr	                      = &m_kServiceMgr;
	m_kThreadMgrDataInit.pkServiceMonitorMgr                  = &m_kServiceMonitorMgr;
	m_kThreadMgrDataInit.pkQueueMgr                           = &m_kQueueMgr;
	//m_kThreadMgrDataInit.lThreadTotalWork	                  = m_kSystemInfo.dwNumberOfProcessors;
	m_kThreadMgrDataInit.lThreadTotalWork	                  = 2;
	m_kThreadMgrDataInit.lThreadTotalRoom                     = m_kQueueMgrDataInit.plRoomTotal[0] + m_kQueueMgrDataInit.plRoomTotal[1];

	m_kThreadMgrDataInit.kThreadConnectDataInit.lStructBytes  = sizeof(m_kThreadMgrDataInit.kThreadConnectDataInit);

	m_kThreadMgrDataInit.kThreadConnectDataInit.iPort		  = g_lRoomServerPort;
	m_kThreadMgrDataInit.kThreadConnectDataInit.iListenNum	  = SOMAXCONN;
	m_kThreadMgrDataInit.kThreadConnectDataInit.ulIpAddress	  = g_ulRoomServerIP;
	m_kThreadMgrDataInit.kThreadConnectDataInit.pkQueueUser   = m_kQueueMgr.getQueueUser();
	m_kThreadMgrDataInit.kThreadConnectDataInit.pkQueuePacket = m_kQueueMgr.getQueuePacket();

	m_kThreadMgr.init(&m_kThreadMgrDataInit);

	return	1;
}

int mtServer::initParams()
{
	/// 初始化服务器目录
	//获得运行服务exe当前路径
	mtGetRunExeDir(g_pcServiceExeDir, MT_APP_FILE_PATH_LEN);
	strcpy(g_pcServiceExeDir + strlen(g_pcServiceExeDir), "\\");

	/// 获得配置文件路径
	char   pcAppSetFilePath[MT_APP_FILE_PATH_LEN] = {'\0'};
	strcpy(pcAppSetFilePath, g_pcServiceExeDir);
	strcpy(pcAppSetFilePath + strlen(pcAppSetFilePath), MT_APP_SET_FILE_NAME);

	/// 获得服务器配置信息
	char pcValue[MT_APP_FILE_PATH_LEN]  = {0};
	/// 数据和日志目录参数
	/// 数据
	memset(g_pcServerDataDir, 0, sizeof(g_pcServerDataDir));
	memset(g_pcServerLogDir, 0, sizeof(g_pcServerLogDir));
	GetPrivateProfileString("Path", "DATA", "", g_pcServerDataDir, sizeof(g_pcServerDataDir), pcAppSetFilePath);
	GetPrivateProfileString("Path", "LOG", "", g_pcServerLogDir, sizeof(g_pcServerLogDir), pcAppSetFilePath);
	if (0 == strcmp(g_pcServerLogDir, ""))
	{
		/// 创建服务器日志目录
		memset(g_pcServerLogDir, 0, sizeof(g_pcServerLogDir));
		strcpy(g_pcServerLogDir, g_pcServiceExeDir);
		strcpy(g_pcServerLogDir + strlen(g_pcServerLogDir), MT_APP_LOG_FILE_DIR_NAME);
	}

	mtCreateDirectory(g_pcServerLogDir);
	/// 创建日志文件
	mtCreateLogFile();

	/// 默认是exe的相对目录
	//if (0 == strcmp(g_pcServerDataDir, ""))
	//{
	//	/// 创建服务器数据目录
	//	memset(g_pcServerDataDir, 0, sizeof(g_pcServerDataDir));
	//	strcpy(g_pcServerDataDir, g_pcServiceExeDir);
	//	strcpy(g_pcServerDataDir + strlen(g_pcServerDataDir), MT_APP_DATA_FILE_DIR_NAME);
	//}

	//mtCreateDirectory(g_pcServerDataDir);

	/// 服务端口
	g_lRoomServerPort = htons(GetPrivateProfileInt("Room", "PORT", 0, pcAppSetFilePath));
	if (0 == g_lRoomServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE ROOM PORT ERROR !\n");
		return 0;
	}



	/// 监控参数
	memset(pcValue, 0, sizeof(pcValue));
	GetPrivateProfileString("HallMonitor", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
	g_ulHallMonitorServerIP  = inet_addr(pcValue);
	g_lHallMonitorServerPort = htons(GetPrivateProfileInt("HallMonitor", "PORT", 0, pcAppSetFilePath));
	if (0 == g_ulHallMonitorServerIP || 0 == g_lHallMonitorServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE HALL MONITOR IP OR PORT ERROR !\n");
		return 0;
	}

	return 1;
}

int	main()
{
	
	try 
	{
		mtServer*	pkServer = new mtServer;
		pkServer->init();
		
		//while(1);
        while(1)
		{
			SleepEx(INFINITE, TRUE);
		}
	}
	catch (mtException* pkException)
	{
		cout << typeid(*pkException).name() << endl;
		MT_NOT_USED(pkException);
	}

	return	0;
}
