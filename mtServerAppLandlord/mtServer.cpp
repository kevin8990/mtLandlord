#include "mtServer.h"
#include "mtPlatform.h"
#include "mtDefineDebug.h"

mtServer::mtServer()
{
	
}

mtServer::~mtServer()
{
	fclose(g_pkMtLogFile);
	g_pkMtLogFile = NULL;
}

int	mtServer::init()
{
	GetSystemInfo(&m_kSystemInfo);

	InitializeCriticalSectionAndSpinCount(&g_kCriticalSection, m_kSystemInfo.dwNumberOfProcessors * 4);

	/// 初始化服务器参数
	if (0 == initParams())
	{
		MT_DEBUG_ERROR(PTCL, "INITPARAMS ERROR!\n");
		return 0;
	}

	/// 系统公告
	strcpy(g_pcSystemNotice, MT_SYSTEM_NOTICE2);
	initDataBase();
	MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN, "initDataBase success!\n");
	initQueue();
	MT_DEBUG_COLOR(PTCL, FOREGROUND_GREEN,"initQueue success!\n");
	initService();
	MT_DEBUG_COLOR(PTCL, FOREGROUND_GREEN,"initService success!\n");
	initThread();
	MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN, "initThread success!\n");
	MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN, "Hall server runing success!\n");


	return 1;
}

int	mtServer::exit()
{
	m_kSQLEnv.exit();
	DeleteCriticalSection(&g_kCriticalSection);
	return 1;
}

int mtServer::initDataBase()
{
	m_kSQLEnv.init();

	return 1;
}

int mtServer::initQueue()
{
	m_kQueueMgrDataInit.lStructBytes    = sizeof(m_kQueueMgrDataInit);
	m_kQueueMgrDataInit.iSpinCount      = m_kSystemInfo.dwNumberOfProcessors;
	m_kQueueMgrDataInit.pcServiceExeDir = g_pcServiceExeDir;
	m_kQueueMgrDataInit.pkSQLEnv        = &m_kSQLEnv;

	m_kQueueMgr.init(&m_kQueueMgrDataInit);

	return 1;
}

int mtServer::initService()
{
	/// m_kSQLEnv
	m_kServiceMgrDataInit.lStructBytes	  = sizeof(m_kServiceMgrDataInit);
	m_kServiceMgrDataInit.lSpinCount      = m_kSystemInfo.dwNumberOfProcessors<<1;
	m_kServiceMgrDataInit.pkQueueMgr      = &m_kQueueMgr;
	m_kServiceMgrDataInit.pcServiceExeDir = g_pcServiceExeDir;
	m_kServiceMgrDataInit.pkSQLEnv		= &m_kSQLEnv;

	m_kServiceMgr.init(&m_kServiceMgrDataInit);

	/// service monitor
	m_kServiceMonitorMgrDataInit.lStructBytes	  = sizeof(m_kServiceMonitorMgrDataInit);
	m_kServiceMonitorMgrDataInit.pkDataHall       = &(m_kQueueMgr.m_pkHall->m_kDataHall);
	///m_kServiceMonitorMgrDataInit.pkSpace          = m_kQueueMgr.m_pkHall->m_kDataHall.pkSpace;

	return	m_kServiceMonitorMgr.init(&m_kServiceMonitorMgrDataInit);
}

int	mtServer::initThread()
{
	m_kThreadMgrDataInit.lStructBytes	                = sizeof(m_kThreadMgrDataInit);
	m_kThreadMgrDataInit.pkServiceMgr	                = &m_kServiceMgr;
	m_kThreadMgrDataInit.pkServiceMonitorMgr            = &m_kServiceMonitorMgr;
	m_kThreadMgrDataInit.pkSQLEnv						= &m_kSQLEnv;
	m_kThreadMgrDataInit.pkDataHall                     = &m_kQueueMgr.m_pkHall->m_kDataHall;
	/// m_kThreadMgrDataInit.pkSpace                    = m_kQueueMgr.m_pkHall->m_kDataHall.pkSpace;
	//m_kThreadMgrDataInit.iThreadWorkTotal	            = (m_kSystemInfo.dwNumberOfProcessors<<1);
	m_kThreadMgrDataInit.iThreadWorkTotal	            = (m_kSystemInfo.dwNumberOfProcessors);

	m_kThreadMgrDataInit.kDataInitListen.iPort			= g_lHallServerPort;
	m_kThreadMgrDataInit.kDataInitListen.iListenNum		= SOMAXCONN;
	m_kThreadMgrDataInit.kDataInitListen.ulIpAddress	= g_ulHallServerIP;

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
	if (0 == strcmp(g_pcServerDataDir, ""))
	{
		/// 创建服务器数据目录
		memset(g_pcServerDataDir, 0, sizeof(g_pcServerDataDir));
		strcpy(g_pcServerDataDir, g_pcServiceExeDir);
		strcpy(g_pcServerDataDir + strlen(g_pcServerDataDir), MT_APP_DATA_FILE_DIR_NAME);
	}

	//mtCreateDirectory(g_pcServerDataDir);

	/// 服务端口
	g_lHallServerPort = htons(GetPrivateProfileInt("Hall", "PORT", 0, pcAppSetFilePath));
	if (0 == g_lHallServerPort)
	{
		MT_DEBUG_ERROR(PTCL,"READ SET FILE HALL PORT ERROR !\n");
		return 0;
	}

	/// 监控参数
//	memset(pcValue, 0, sizeof(pcValue));
//	GetPrivateProfileString("Monitor", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
//	g_ulMonitorServerIP  = inet_addr(pcValue);
	g_lMonitorServerPort = htons(GetPrivateProfileInt("Monitor", "PORT", 0, pcAppSetFilePath));
	if (0 == g_lMonitorServerPort)
	{
		MT_DEBUG_ERROR(PTCL,"READ SET FILE MONITOR IP OR PORT ERROR !\n");
		return 0;
	}

	/// 房间参数
	memset(pcValue, 0, sizeof(pcValue));
	GetPrivateProfileString("ROOM1", "SERVER", "0", pcValue, sizeof(pcValue), pcAppSetFilePath);
	g_ulRoomServerIP  = inet_addr(pcValue);
	g_lRoomServerPort = htons(GetPrivateProfileInt("ROOM1", "PORT", 0, pcAppSetFilePath));
	if (0 == g_ulRoomServerIP || 0 == g_lRoomServerPort)
	{
		MT_DEBUG_ERROR(PTCL,"READ SET FILE ROOM IP OR PORT ERROR !\n");
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
		MT_DEBUG_ERROR(PTCL,"READ SET FILE DB PARA ERROR !\n");
		return 0;
	}

	return 1;
}

int main(int argc, char **argv)
{
	
	mtServer*  pkServer =	new mtServer();
	if (0 == pkServer->init())
	{
		MT_DEBUG(PTCL, "HALL SERVER INIT ERROR!\n");
		return 0;
	}

	while(1)
	{
		Sleep(INFINITE);
	}

	return 1;
}