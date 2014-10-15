#ifndef		__MT_COMMON_H
#define		__MT_COMMON_H

#define		MT_GET_NAME(t)		#t

#include "mtDefineDebug.h"

// #define		MT_SERVER_IP					"127.0.0.1"
// #define		MT_SERVER_PORT					9066
// #define		MT_SERVER_Monitor_Hall_IP		(inet_addr("180.168.138.218"))      /// 0xDA8AA8B4 - 180.168.138.218
// #define		MT_SERVER_Monitor_Hall_PORT	    (htons(9064))                       /// 9064 - 0x6823
#define		MT_APP_LOG_FILE_MAX_SIZE        (1024 * 1024 * 8)
#define		MT_APP_FILE_PATH_LEN            512
#define		MT_APP_DATA_FILE_DIR_NAME       "RoomServerData\\"
#define		MT_APP_LOG_FILE_DIR_NAME        "RoomServerLog\\"
#define		MT_APP_SET_FILE_NAME            "mtRoomServerSet.ini"
#define     MT_APP_LOG_FILE_NAME            "mtRoomLog.data"

#define		MT_THREAD_WORK_TIME_VALID		600000		/// (10min * 60s * 1000milliseconds)
#define		MT_THREAD_SERVICE_MONITOR_TIME	1000*10		/// (5min * 60s * 1000milliseconds)
#define		MT_SERVICE_HEART_BEAT_TIME	    25 * 1000		/// (35s * 1000milliseconds)
#define     MT_SERVICE_USER_CARD_OUT_TIME   35 * 1000/*35*1000*/
#define		MT_SERVICE_AI_CARD_OUT_TIME	    2000		/// (2s * 1000milliseconds)
//#define		MT_SERVICE_WAIT_START_TIME	    2000		/// (2s * 1000milliseconds)
//#define		MT_SERVICE_CARD_DEAL_TIME	    2000		/// (2s * 1000milliseconds)
#define		MT_SERVICE_GRAB_LANDLORD_TIME	2000		/// (2s * 1000milliseconds)
#define     MT_USER_WIN_SCORE               5000         //赢一局的基础积分

/// 
#define     MT_QUEUE_USER_ID_AI_START       999999
#define		MT_NOT_USED(x)					((void)(x))
#define		MT_BYTES_DATA_PACKET				10240
#define		MT_NODE_QUEUE_PACKET				6000//65535				//包NODE
#define		MT_NODE_QUEUE_USER					500//1500//10240		//用户NODE
#define		MT_NODE_QUEUE_DESK					500//5120				//桌子NODE
#define		MT_NODE_QUEUE_ONE					300//10000
#define		MT_NODE_QUEUE_TWO					300//7000
#define		MT_NODE_QUEUE_THREE					300//5000
#define		MT_NODE_QUEUE_SPACE					3
#define		MT_NODE_PLAYER						3
#define		MT_NODE_QUEUE_HELP_TWO_LIST			50
#define		MT_NODE_QUEUE_HELP_THREE_LIST		5
#define		MT_NODE_QUEUE_TIMEOUT				4 * 1000
#define		MT_NODE_QUEUE_IOCP_TIMEOUT			1000
#define     MT_QUEUE_COMPLETION_STATUS_CUSTOM   -888
#define     MT_NODE_CARD_COUNT                  54
#define     MT_NODE_CARD_COUNT_EVERY_BODY       17
#define     MT_NODE_CARD_COUNT_BASE             3
#define 	MT_CARD_TERMINATE		            100  /// 扑克牌的结束符号


#endif	///	__MT_COMMON_H
