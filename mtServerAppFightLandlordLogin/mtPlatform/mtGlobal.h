#ifndef		__MT_GLOBAL_H
#define		__MT_GLOBAL_H

//extern long g_plVersion[4];
extern FILE* g_pkMtLogFile;									//文件指针
/// 服务器配置数据(从配置文件读取)
// 登录
extern unsigned long        g_ulLoginServerIP;
extern long                 g_lLoginServerPort;				//登录端口
// 大厅
extern unsigned long        g_ulHallServerIP;				//大厅IP
extern long                 g_lHallServerPort;				//大厅端口
// 监控
extern unsigned long        g_ulMonitorServerIP;
extern long                 g_lMonitorServerPort;
// 收费
extern unsigned long        g_ulChargeServerIP;
extern long                 g_lChargeServerPort;
// 下载
extern unsigned long        g_ulDownloadServerIP;
extern long                 g_lDownloadServerPort;
// 软件更新url(暂时未使用)
//extern char                 g_pcUpdateUrl;
//extern long                 g_lUpdatePort;
//extern long                 g_lUpdateUrlType;
// 数据库相关参数
extern char					g_DataBaseServer[MT_SERVER_DATABASE_PARA_LEN];
extern char					g_DataBaseUserName[MT_SERVER_DATABASE_PARA_LEN];
extern char					g_DataBasePassWord[MT_SERVER_DATABASE_PARA_LEN];
extern char					g_DataBaseName[MT_SERVER_DATABASE_PARA_LEN];
extern char					g_DataBaseCODE[MT_SERVER_DATABASE_PARA_LEN];
extern long					g_DataBasePoolSize;
/////////////////////////////////////////////////////////////////////////
// 数据和日志存放目录
extern char                 g_pcServerDataDir[MT_APP_FILE_PATH_LEN];			//数据目录
extern char                 g_pcServerLogDir[MT_APP_FILE_PATH_LEN];				//日志目录
extern char                 g_pcServiceExeDir[MT_APP_FILE_PATH_LEN];			//当前exe运行的目录

extern CRITICAL_SECTION	    g_kCriticalSection;				//临界区对象，目前使用的对象有（文件对象指针）保护			


extern int mtCreateLogFile();								///创建日志文件，以日期格式+MT_APP_LOG_FILE_NAME为文件名
extern void mtLog( char * pcformat, ... );					//写日志文件
extern int mtGetRunExeDir(char* pDir, DWORD dwLen);		///获取当前执行文件的目录，失败返回0或负数
extern int mtCreateDirectory(const char* pcDir);			///根据路径一层一层的创建目录
BOOL SetConsoleColor(WORD wAttributes);
#endif ///	__MT_GLOBAL_H