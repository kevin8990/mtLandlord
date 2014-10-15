#include "mtServer.h"
#include "mtPlatform.h"
#include "mtGlobal.h"

mtServer::mtServer() :
	m_lpfnAcceptEx(NULL),
	m_lpfnDisconnectEx(NULL),
	m_iWSADataRet(-1) {

}

mtServer::~mtServer()
{
}

int mtServer::run()
{
	return	m_kThreadMgr.run();
}

int mtServer::exit()				//资源释放
{
	m_kThreadMgr.exit();
	m_kSQLEnv.exit();

	if (0 == m_iWSADataRet)
	{
		WSACleanup();
	}

	DeleteCriticalSection(&g_kCriticalSection);

	return 1;
}

int mtServer::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_WSA_STARTUP)
		MT_EXCETPION_STRING(E_EXCEPTION_CLIENT_SOCKET)
		MT_EXCETPION_STRING(E_EXCEPTION_WSAIOCTL_ACCEPTEX)
		MT_EXCETPION_STRING(E_EXCEPTION_WSAIOCTL_DISCONNECTEX)
		MT_EXCETPION_STRING(E_EXCEPTION_CLOSE_SOCKET)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		return	0;
	}

	return	1;
}

/// 初始化服务器参数，成功返回1，失败返回0
int mtServer::initParams()			
{
	/// 初始化服务器目录
	//获得运行服务exe当前路径
	mtGetRunExeDir(g_pcServiceExeDir, MT_APP_FILE_PATH_LEN);
	strcpy(g_pcServiceExeDir + strlen(g_pcServiceExeDir), "\\");	//末尾加上'\'

	/// 获得配置文件路径
	char   pcAppSetFilePath[MT_APP_FILE_PATH_LEN] = {'\0'};
	strcpy(pcAppSetFilePath, g_pcServiceExeDir);
	strcpy(pcAppSetFilePath + strlen(pcAppSetFilePath), MT_APP_SET_FILE_NAME);

	/// 获得服务器配置信息
	
	/// 数据和日志目录参数
	memset(g_pcServerDataDir, 0, sizeof(g_pcServerDataDir));
	memset(g_pcServerLogDir, 0, sizeof(g_pcServerLogDir));
	GetPrivateProfileString("Path", "DATA", "", g_pcServerDataDir, sizeof(g_pcServerDataDir), pcAppSetFilePath);
	GetPrivateProfileString("Path", "LOG", "", g_pcServerLogDir, sizeof(g_pcServerLogDir), pcAppSetFilePath);
	if (0 == strcmp(g_pcServerLogDir, ""))	//判断目录是否为空，则根据运行目录设置相对目录
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
	g_lLoginServerPort = htons(GetPrivateProfileInt("Login", "PORT", 0, pcAppSetFilePath));
	if (0 == g_lLoginServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE LOGIN PORT ERROR !\n");
		return 0;
	}

	/// 大厅参数
	char pcValue[MT_APP_FILE_PATH_LEN]  = {0};
	memset(pcValue, 0, sizeof(pcValue));
	GetPrivateProfileString("Hall", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
	g_ulHallServerIP  = inet_addr(pcValue);
	g_lHallServerPort = htons(GetPrivateProfileInt("Hall", "PORT", 0, pcAppSetFilePath));
	if (0 == g_ulHallServerIP || 0 == g_lHallServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE HALL IP OR PORT ERROR !\n");
		return 0;
	}

	/// 收费参数
	memset(pcValue, 0, sizeof(pcValue));
	GetPrivateProfileString("Charge", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
	g_ulChargeServerIP  = inet_addr(pcValue);
	g_lChargeServerPort = htons(GetPrivateProfileInt("Charge", "PORT", 0, pcAppSetFilePath));
	if (0 == g_ulChargeServerIP || 0 == g_lChargeServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE CHARGE IP OR PORT ERROR !\n");
		return 0;
	}

	///数据库参数
	GetPrivateProfileString("DataBase", "DBSERVER", "", g_DataBaseServer, sizeof(g_DataBaseServer), pcAppSetFilePath);
	GetPrivateProfileString("DataBase", "DBUSERNAME", "", g_DataBaseUserName, sizeof(g_DataBaseUserName), pcAppSetFilePath);
	GetPrivateProfileString("DataBase", "DBPASSWORD", "", g_DataBasePassWord, sizeof(g_DataBasePassWord), pcAppSetFilePath);
	GetPrivateProfileString("DataBase", "DBNAME", "", g_DataBaseName, sizeof(g_DataBaseName), pcAppSetFilePath);
	GetPrivateProfileString("DataBase", "DBCODE", "", g_DataBaseCODE, sizeof(g_DataBaseCODE), pcAppSetFilePath);
	g_DataBasePoolSize = GetPrivateProfileInt("DataBase", "DBPOOLSIZE", 0, pcAppSetFilePath);
	if (0 == strcmp(g_DataBaseServer, "") || 0 == strcmp(g_DataBaseUserName, "") || 0 == strcmp(g_DataBasePassWord, "")
		|| 0 == strcmp(g_DataBaseName, "") || 0 == strcmp(g_DataBaseCODE, "") || 0 == g_DataBasePoolSize ){
			MT_DEBUG_PTCL("READ SET FILE DB PARA ERROR !\n");
			return 0;
	}

	/// 下载参数
	memset(pcValue, 0, sizeof(pcValue));
	GetPrivateProfileString("Download", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
	g_ulDownloadServerIP  = inet_addr(pcValue);
	g_lDownloadServerPort = htons(GetPrivateProfileInt("Download", "PORT", 0, pcAppSetFilePath));
	if (0 == g_ulDownloadServerIP || 0 == g_lDownloadServerPort)
	{
		MT_DEBUG_PTCL("READ SET FILE DWONLOAD IP OR PORT ERROR !\n");
		return 0;
	}

	return 1;
}

int mtServer::init( DataInit* pkDataInit )
{
	memcpy (&m_kDataInit, pkDataInit, sizeof(DataInit));
	/// 初始化网络模块
	memset (&m_kWSAData, 0, sizeof(m_kWSAData));
	m_iWSADataRet	= WSAStartup(MAKEWORD(2,2), &m_kWSAData);	//成功返回0
	MT_EXCEPTION_DEBUG(0 == m_iWSADataRet, E_EXCEPTION_WSA_STARTUP);

	initDataBase();
	MT_DEBUG(PTCL, "initDataBase success!\n");
	/// 初始化 高级函数
	initSocketFunction();
	MT_DEBUG(PTCL, "initSocketFunction sucess !\n");
	/// 初始化 mtThreadMgr 
	initThreadMgr();
	MT_DEBUG(PTCL, "initThreadMgr sucess !\n");
	MT_DEBUG(PTCL, "Login Server runing sucess !\n");

	return 1;
}

int mtServer::initDataBase()
{
	m_kSQLEnv.init();
	return 1;
}

int mtServer::initSocketFunction()
{
	//创建一个socket,支持重叠I/O操作
	SOCKET	uiSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	MT_EXCEPTION_DEBUG(INVALID_SOCKET != uiSocket, E_EXCEPTION_CLIENT_SOCKET);
	/// AcceptEx
	GUID GuidAcceptEx	= WSAID_ACCEPTEX;
	m_lpfnAcceptEx		= NULL;
	DWORD		dwBytes = 0;
	int iRet = WSAIoctl(uiSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof (GuidAcceptEx),  &m_lpfnAcceptEx, sizeof (m_lpfnAcceptEx), 
		&dwBytes, NULL, NULL);
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_WSAIOCTL_ACCEPTEX);
	/// DisconnectEx
	GUID GuidDisconnectEx	= WSAID_DISCONNECTEX;
	m_lpfnDisconnectEx		= NULL;
	iRet = WSAIoctl(uiSocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidDisconnectEx, sizeof (GuidDisconnectEx),  &m_lpfnDisconnectEx, sizeof (m_lpfnDisconnectEx), 
		&dwBytes, NULL, NULL);
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_WSAIOCTL_DISCONNECTEX);

	iRet = closesocket(uiSocket);
	MT_EXCEPTION_DEBUG(0 == iRet, E_EXCEPTION_CLOSE_SOCKET);
	
	return	1;
}

int mtServer::initThreadMgr()
{
	mtThreadMgr::DataInit		kThreadMgrDataInit;
	memset (&kThreadMgrDataInit, 0, sizeof(kThreadMgrDataInit));
	kThreadMgrDataInit.lStructBytes			    = sizeof(kThreadMgrDataInit);
	kThreadMgrDataInit.ulWorkIP				    = m_kDataInit.ulWorkIP;
	kThreadMgrDataInit.usWorkPort			    = m_kDataInit.usWorkPort;
	kThreadMgrDataInit.ulTimeWaitMilliseconds	= m_kDataInit.ulTimeWaitMilliseconds;
	kThreadMgrDataInit.iTimeConnectLimitSencods	= m_kDataInit.iTimeConnectLimitSencods;
	kThreadMgrDataInit.pkSQLEnv					= &m_kSQLEnv;
	kThreadMgrDataInit.pcFolder				    = g_pcServerDataDir;
	kThreadMgrDataInit.lpfnAcceptEx			    = m_lpfnAcceptEx;
	kThreadMgrDataInit.lpfnDisconnectEx		    = m_lpfnDisconnectEx;
	m_kThreadMgr.init(&kThreadMgrDataInit);

	return	1;
}

int		main()
{
	mtServer*	pkServer	= new mtServer;

	if (NULL == pkServer){
		printf ("创建 mtServer 失败\n");
		return	0;
	}

	try{
		
		if (pkServer)
		{
			SYSTEM_INFO	kSystemInfo;			//获取cpu相关信息
			GetSystemInfo(&kSystemInfo);
			InitializeCriticalSectionAndSpinCount(&g_kCriticalSection, kSystemInfo.dwNumberOfProcessors * 4);

			/// 初始化服务器参数
			if (0 == pkServer->initParams())
			{
				pkServer->exit();
				delete pkServer;
				MT_DEBUG(PTCL, "LOGIN SERVER INIT ERROR !\n");
				return 0;
			}

			mtServer::DataInit		kDataInit;
			kDataInit.lStructBytes	            = sizeof(kDataInit);
			kDataInit.ulWorkIP			        = g_ulLoginServerIP;
			kDataInit.usWorkPort			    = g_lLoginServerPort;
			kDataInit.iUserListTotal		    = MT_NODE_USER;
			kDataInit.ulTimeWaitMilliseconds	= MT_TIME_WAIT_LIMIT_MILLISECONDS;
			kDataInit.iTimeConnectLimitSencods	= MT_TIME_CONNECT_LIMIT_SECONDS;
			pkServer->init(&kDataInit);
			pkServer->run();
		} else  {
			printf ("%s\t%d\n", __FILE__, __LINE__);
		}

	} catch (mtException* pkException) {
		cout << typeid(*pkException).name() << endl;
		pkException->exception();
	}

	pkServer->exit();
	delete	pkServer;

	return	0;
}