#include "mtPlatform.h"
#include <stdarg.h>

char g_pcSystemNotice[MT_SYSTEM_NOTICE_LEN];
char g_pcSystemNoticeNew[MT_SYSTEM_NOTICE_LEN];
FILE* g_pkMtLogFile = NULL;
/// 服务器配置数据(从配置文件读取)
// 登录
unsigned long        g_ulHallServerIP          = 0;
long                 g_lHallServerPort         = 0;
// 监控
unsigned long        g_ulMonitorServerIP       = 0;
long                 g_lMonitorServerPort      = 0;
// 房间
unsigned long        g_ulRoomServerIP          = 0;
long                 g_lRoomServerPort         = 0;

char				g_DataBaseServer[MT_SERVER_DATABASE_PARA_LEN] = {0};
char				g_DataBaseUserName[MT_SERVER_DATABASE_PARA_LEN] = {0};
char				g_DataBasePassWord[MT_SERVER_DATABASE_PARA_LEN] = {0};
char				g_DataBaseName[MT_SERVER_DATABASE_PARA_LEN] = {0};
char				g_DataBaseCODE[MT_SERVER_DATABASE_PARA_LEN] = {0};
long				g_DataBasePoolSize;

//////////////////////////////////////////////////////////////////


// 数据和日志存放目录
char                 g_pcServerDataDir[MT_APP_FILE_PATH_LEN] = {0};
char                 g_pcServerLogDir[MT_APP_FILE_PATH_LEN]  = {0};
char                 g_pcServiceExeDir[MT_APP_FILE_PATH_LEN] = {0};
CRITICAL_SECTION	 g_kCriticalSection;
int mtCreateDirectory(const char* pcDir)
{
	char pcDirTmp[256];
	strcpy(pcDirTmp, pcDir);

	char* pcOne = strchr(pcDirTmp, '\\');
	while(pcOne)
	{
		*pcOne = '\0';
		_mkdir(pcDirTmp);
		*pcOne = '\\';
		pcOne = strchr(pcOne + 1, '\\');
	}

	_mkdir(pcDirTmp);

	return 1;
}

int mtGetRunExeDir(char* pDir, DWORD dwLen)
{
	GetModuleFileName(NULL, pDir, dwLen);
	int iRet = -1;
	int iLen = strlen(pDir);
	if (0 >= iLen)
	{
		return iRet;
	}

	//取出文件路径
	for(int i = iLen; i >= 0; i--)
	{
		if(pDir[i] == '\\')
		{   
			pDir[i] = '\0';
			iRet = 1;
			break;
		} 
	}

	return iRet;
}

void mtLog( char * pcformat, ... )
{
	va_list kArgs;
	int     iLen;
	char    pcBuffer[10240];

	memset(pcBuffer, 0, sizeof(pcBuffer));
	va_start(kArgs, pcformat);
	iLen = _vscprintf(pcformat, kArgs) + 1;
	vsprintf(pcBuffer, pcformat, kArgs);
	EnterCriticalSection(&g_kCriticalSection);
	if (NULL == g_pkMtLogFile)
	{
		LeaveCriticalSection(&g_kCriticalSection);
		mtCreateLogFile();
		printf(pcBuffer);
		return;
	}

	fwrite(pcBuffer, iLen, 1, g_pkMtLogFile);
	fflush(g_pkMtLogFile);
	va_end(kArgs);

	/// 判断文件大小
	long lFileSize = ftell(g_pkMtLogFile);
	if (MT_APP_LOG_FILE_MAX_SIZE <= lFileSize)
	{
		LeaveCriticalSection(&g_kCriticalSection);
		mtCreateLogFile();
		printf(pcBuffer);
		return;
	}

	LeaveCriticalSection(&g_kCriticalSection);
	printf(pcBuffer);
}

int mtCreateLogFile()
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	/// 创建日志文件
	char pcAppLogFileName[MT_APP_FILE_PATH_LEN] = {0};
	strcpy(pcAppLogFileName, g_pcServerLogDir);
	sprintf(pcAppLogFileName + strlen(pcAppLogFileName), "%d%d%d%d%d%d",curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,curTime.wMinute,curTime.wSecond);
	strcpy(pcAppLogFileName + strlen(pcAppLogFileName), MT_APP_LOG_FILE_NAME);
	EnterCriticalSection(&g_kCriticalSection);
	if (g_pkMtLogFile)
	{
		fclose(g_pkMtLogFile);
		g_pkMtLogFile = NULL;
	}

	g_pkMtLogFile = fopen(pcAppLogFileName, "a+");
	LeaveCriticalSection(&g_kCriticalSection);
	if (NULL == g_pkMtLogFile)
	{
		return 0;
	}

	return 1;
}

BOOL SetConsoleColor(WORD wAttributes)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bResult = SetConsoleTextAttribute(hConsole, wAttributes);

	//	CloseHandle(hConsole);
	return bResult;
}