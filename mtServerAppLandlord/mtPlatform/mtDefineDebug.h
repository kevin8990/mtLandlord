#ifndef		_MT_DEFINE_DEBUG_H
#define		_MT_DEFINE_DEBUG_H

#define		MT_DEBUG_0(...)			printf(__VA_ARGS__)
#define		MT_DEBUG_1(...)			printf(__VA_ARGS__)
#define		MT_DEBUG_2(...)			printf(__VA_ARGS__)
#define		MT_DEBUG_3(...)			printf("%d\t%s\n", __LINE__, __FILE__)
#define		MT_DEBUG_4(...)			printf(__VA_ARGS__)			/// 连接线程
#define		MT_DEBUG_5(...)			printf(__VA_ARGS__)			/// 工作线程
#define		MT_DEBUG_6(...)			printf(__VA_ARGS__)			/// 监听线程
#define		MT_DEBUG_7(...)			printf(__VA_ARGS__)			/// 局部错误处理
#define		MT_DEBUG_8(...)			printf(__VA_ARGS__)			/// 正确行号
#define		MT_DEBUG_9(...)			printf(__VA_ARGS__)			/// 错误行号

#define		MT_DEBUG_PTCL(...)		mtLog(__VA_ARGS__)// printf(__VA_ARGS__)

#define		MT_DEBUG(iLevel, ...)	MT_DEBUG_##iLevel(__VA_ARGS__); 

#define     MT_DEBUG_ERROR(iLevel,...)  SetConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);\
	        MT_DEBUG_##iLevel(__VA_ARGS__);\
	        SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

#define     MT_DEBUG_COLOR(iLevel,color,...)  SetConsoleColor(color);\
	MT_DEBUG_##iLevel(__VA_ARGS__);\
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);


#endif	/// _MT_DEFINE_DEBUG_H