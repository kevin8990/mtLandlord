//#include "log.h"
//#include <iostream>
//#include <direct.h>
//using namespace std;
//char     g_LogDir[FILE_PATH_LEN]={0};
//char     g_ExeDir[FILE_PATH_LEN]={0};
//
//bool GetRunExeDir(char* pDir, DWORD dwLen)  //得到当前运行程序的路径保存到参数指针里面
//{
//	GetModuleFileName(NULL, pDir, dwLen);//返回当前应用程序全路径，此为api函数
//	bool flag = NULL;
//	int iLen = strlen(pDir);
//	if (0 >= iLen)
//	{
//		return FALSE;
//	}
//
//	//取出文件路径  
//	for(int i = iLen; i >= 0; i--)
//	{
//		if(pDir[i] == '\\')
//		{   
//			pDir[i] = '\0';
//			break;
//		} 
//	}
//
//	return TRUE;
//}
//
//int  CreateLogFile(char *logFile,FILE* pLogFile)  //创建日志文件函数
//{
//	//SYSTEMTIME curTime;//得到当前时间
//	//GetLocalTime(&curTime);
//
//	/// 创建日志文件
//	char logFileName[FILE_PATH_LEN] = {0};
//	strcpy(logFileName, g_LogDir);
//	strcpy(logFileName + strlen(logFileName),"\\");
//	//sprintf(logFileName + strlen(logFileName), "%d%d%d%d%d%d",curTime.wYear, curTime.wMonth, curTime.wDay, curTime.wHour,curTime.wMinute,curTime.wSecond);
//	strcpy(logFileName + strlen(logFileName),logFile);
//	strcpy(logFileName + strlen(logFileName), ".log");
//
//	if (pLogFile)
//	{
//		fclose(pLogFile);
//		pLogFile = NULL;
//	}
//
//	pLogFile = fopen(logFileName, "a+");
//
//	if (NULL == pLogFile)
//	{
//		return 0;
//	}
//
//	return 1;
//}
//
//
//int WSDCreateDirectory(const char* pcDir)    //新建目录 参数为全路径 其实就是新建一个文件夹
//{
//	char pcDirTmp[256];
//	strcpy(pcDirTmp, pcDir);
//
//	char* pcOne = strchr(pcDirTmp, '\\');    
//	while(pcOne)
//	{
//		*pcOne = '\0';
//		_mkdir(pcDirTmp);
//		*pcOne = '\\';
//		pcOne = strchr(pcOne + 1, '\\');
//	}
//
//	_mkdir(pcDirTmp);
//
//	return 1;
//}
//
//
//int   LogInit()
//{
//	if(!GetRunExeDir(g_ExeDir,FILE_PATH_LEN))  
//	{
//		cout<<"get dir fail"<<endl;
//		return 0;
//	}
//	strcpy(g_ExeDir + strlen(g_ExeDir), "\\");
//	strcpy(g_LogDir, g_ExeDir);
//	strcpy(g_LogDir + strlen(g_LogDir), "RunLog");
//	//strcpy(g_LogDir + strlen(g_LogDir), "MoniterOutNetTestlog");
//
//	WSDCreateDirectory(g_LogDir);//创建日志目录 即.log文件夹
//	return 1;
//
//}