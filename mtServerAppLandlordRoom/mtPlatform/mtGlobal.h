#ifndef		__MT_GLOBAL_H
#define		__MT_GLOBAL_H

extern FILE* g_pkMtLogFile;
/// 服务器配置数据(从配置文件读取)
// 登录
extern unsigned long        g_ulRoomServerIP;
extern long                 g_lRoomServerPort;
// 监控
extern unsigned long        g_ulHallMonitorServerIP;
extern long                 g_lHallMonitorServerPort;
// 数据和日志存放目录
extern char                 g_pcServerDataDir[MT_APP_FILE_PATH_LEN];
extern char                 g_pcServerLogDir[MT_APP_FILE_PATH_LEN];
extern char                 g_pcServiceExeDir[MT_APP_FILE_PATH_LEN];
extern CRITICAL_SECTION	    g_kCriticalSection;


extern int mtCreateLogFile();
extern void mtLog( char * pcformat, ...);
extern int mtGetRunExeDir(char* pDir, DWORD dwLen);
extern int mtCreateDirectory(const char* pcDir);
extern BOOL SetConsoleColor(WORD wAttributes);
#endif ///	__MT_GLOBAL_H