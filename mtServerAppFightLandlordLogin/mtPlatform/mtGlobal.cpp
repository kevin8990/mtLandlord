#include "mtPlatform.h"
#include <stdarg.h>

//long                 g_plVersion[4]            = {0,0,0,1};
FILE*                g_pkMtLogFile             = NULL;

/// 服务器配置数据(从配置文件读取)
// 登录
unsigned long        g_ulLoginServerIP         = 0;
long                 g_lLoginServerPort        = 0;
// 大厅
unsigned long        g_ulHallServerIP          = 0;
long                 g_lHallServerPort         = 0;
// 监控
unsigned long        g_ulMonitorServerIP       = 0;
long                 g_lMonitorServerPort      = 0;
// 收费
unsigned long        g_ulChargeServerIP        = 0;
long                 g_lChargeServerPort       = 0;
// 下载
unsigned long        g_ulDownloadServerIP      = 0;
long                 g_lDownloadServerPort     = 0;
// 软件更新url(暂时未使用)
//char                 g_pcUpdateUrl             = 0;
//long                 g_lUpdatePort             = 0;
//long                 g_lUpdateUrlType          = 0;
// 数据库相关参数
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

//获取当前执行文件的目录，失败返回0或负数
int mtGetRunExeDir(char* pDir, DWORD dwLen)
{	//获取当前执行文件的全路径
	GetModuleFileName(NULL, pDir, dwLen);		
	int iRet = -1;
	int iLen = strlen(pDir);
	if (0 >= iLen){
		return iRet;
	}

	//取出文件路径（删除文件名，保留路径）
	for(int i = iLen; i >= 0; i--) {
		if(pDir[i] == '\\') {   
			pDir[i] = '\0';
			iRet = 1;
			break;
		} 
	}

	return iRet;
}
//根据路径一层一层的创建目录
int mtCreateDirectory(const char* pcDir)
{
	char pcDirTmp[256];
	strcpy(pcDirTmp, pcDir);

	//返回首次出现该字符的位置的指针
	char* pcOne = strchr(pcDirTmp, '\\');		//找到第一个目录
	while(pcOne)
	{
		*pcOne = '\0';							//该指针设置成末尾
		_mkdir(pcDirTmp);						//创建目录
		*pcOne = '\\';							//该指针位置复位
		pcOne = strchr(pcOne + 1, '\\');		//从当前位置的后一个位置继续查找
	}

	_mkdir(pcDirTmp);

	return 1;
}
//将日志写入log文件，并判断文件大小是否超出指定值，超出则重新创建文件
void mtLog( char * pcformat, ... )
{
	va_list kArgs;
	int     iLen;
	char    pcBuffer[10240];

	memset(pcBuffer, 0, sizeof(pcBuffer));
	va_start(kArgs, pcformat);
	//iLen = _vscprintf(pcformat, kArgs) + 1;		//获取pcBuffer最后的长度
	vsprintf(pcBuffer, pcformat, kArgs);
	EnterCriticalSection(&g_kCriticalSection);
	if (NULL == g_pkMtLogFile)
	{
		LeaveCriticalSection(&g_kCriticalSection);
		mtCreateLogFile();
		printf(pcBuffer);
		return;
	}

	/*
	（1）buffer：是一个指针，对fwrite来说，是要获取数据的地址；
	（2）size：要写入内容的单字节数；
	（3）count:要进行写入size字节的数据项的个数；
	（4）stream:目标文件指针；
	（5）返回实际写入的数据项个数count。
	*/
	//fwrite(pcBuffer, iLen, 1, g_pkMtLogFile);
	fwrite(pcBuffer, strlen(pcBuffer)+1, 1, g_pkMtLogFile);
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
//创建日志文件，以日期格式+MT_APP_LOG_FILE_NAME为文件名
int mtCreateLogFile()
{
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);

	/// 创建日志文件
	char pcAppLogFileName[MT_APP_FILE_PATH_LEN] = {0};
	strcpy(pcAppLogFileName, g_pcServerLogDir);
	sprintf(pcAppLogFileName + strlen(pcAppLogFileName), "%d%d%d%d%d%d",curTime.wYear, 
		curTime.wMonth, curTime.wDay, curTime.wHour,curTime.wMinute,curTime.wSecond);
	strcpy(pcAppLogFileName + strlen(pcAppLogFileName), MT_APP_LOG_FILE_NAME);
	EnterCriticalSection(&g_kCriticalSection);
	if (g_pkMtLogFile){
		//关闭一个流。注意：使用fclose()函数就可以把缓冲区内最后剩余的数据输出到磁盘文件中，并释放文件指针和有关的缓冲区
		fclose(g_pkMtLogFile);
		g_pkMtLogFile = NULL;
	}
	//a+ 以附加方式打开可读写的文件。（原来的EOF符不保留）
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