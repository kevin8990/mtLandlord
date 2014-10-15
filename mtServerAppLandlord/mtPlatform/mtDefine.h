#ifndef		__MT_COMMON_H
#define		__MT_COMMON_H

#define		MT_GET_NAME(t)		#t

#include "mtDefineDebug.h"

#define		MT_QUEUE_WORK_SIZE				10000
#define		MT_THREAD_WORK_TIME_VALID		(10*60*1000)		        /// (10min * 60s * 1000milliseconds)
#define		MT_SERVER_RECV_DATA_TIMEOUT		(5*1000)					///接受数据超时时间
#define		MT_SERVER_ACCEPT_TIMEOUT		(5*1000)					///连接超时时间
#define		MT_APP_LOG_FILE_MAX_SIZE        (1024 * 1024 * 8)
#define		MT_APP_FILE_PATH_LEN            512
#define		MT_APP_DATA_FILE_DIR_NAME       "HallServerData\\"
#define		MT_APP_LOG_FILE_DIR_NAME        "HallServerLog\\"
#define		MT_APP_SET_FILE_NAME            "mtHallServerSet.ini"
#define     MT_APP_LOG_FILE_NAME            "mtHallLog.data"
#define     MT_APP_USER_INFO_FILE_NAME      "UserInfo"

#define		MT_BYTES_DBC_DRIVER				64
#define		MT_BYTES_DBC_SERVER				64
#define		MT_BYTES_DBC_UID				64
#define		MT_BYTES_DBC_PWD				64
#define		MT_BYTES_DBC_DATABASE			64
#define		MT_BYTES_DBC_STRING				256			/// 连接字符串的最大长度
#define		MT_BYTES_DBC_FIELD				30
#define     MT_GOLD_USER_NEW                1000
#define     MT_SIZE_HALL                    8

#define		MT_BYTES_THREAD_WORK_DATA_READ	100000
#define		MT_BYTES_THREAD_WORK_DATA_WRITE	100000
#define		MT_NODE_QUEUE_USER_MAX		    999999
#define     MT_SERVER_WORK_USER_ID_BASE     524288
#define     MT_SERVER_WORK_USER_ID_MIN      1024
#define     MT_SERVER_WORK_USER_ID_MAX      999999
#define		MT_NODE_QUEUE_USER_RANK_MAX     10
#define		MT_NODE_QUEUE_PACKET_MAX        65536
#define		MT_BYTES_DATA_PACKET			512
#define		MT_NODE_LIST_USER_ONE           0
#define		MT_NODE_LIST_USER_PROP_COUNT    10
#define		MT_SYSTEM_NOTICE_LEN            1024
#define		MT_SYSTEM_FEED_BACK_LEN         256

#define		MT_SERVER_DATABASE_PARA_LEN		20			//DB参数长度
#define     MT_SERVER_WORK_USER_ID_BASE      524288

#define		MT_SYSTEM_NOTICE2				"最新公告\n\
1、每天送三次金币，每次送三千，近万金币送不停!\n\
2、买的越多，送的越多，充值金币大回馈!\n\
3、系统已新增抽奖功能，中奖率100%，10元-100元话费奖励!\n"

#define     MT_TASK_NUMBER              54

#define     MT_PROP_NUMBER              16
#define     MT_VIP_ONE_MONTHS           13
#define     MT_VIP_THREE_MONTHS         14
#define     MT_VIP_FORVER               15 
#define     MT_VIP_AWARD                3000

#define     MT_BYTES_OF_PHONE           12
#define     MT_CASH_LOTTERY_BASE        11

/*#define		MT_STMT_CHANNEL_CONFIRM	\
	"{call dbo.pADAppFeeConfirm_I(?,?,?,?,?,?,?,?,?)}"
#define		MT_STMT_CHANNEL_SENDSIMI		\
	"{call dbo.pADAppFeeSPNO_I(?,?,?,?,?,?,?,?,?,?)}"

#define     MT_QSTMT_TEXT_QUERY_USER_INFO \
	"{call dbo.SP_QueryUserInfo(?)}"

#define     MT_MSTMT_TEXT_REGISTER \
	"{call dbo.pADAppRegister_I(?,?,?,?,?,?,?,?)}"

#define		MT_MSTMT_TEXT_LOGIN_QUERY_ADDR	\
	"{call dbo.SP_LoginQueryServerAddr(?)}"

#define		MT_MSTMT_TEXT_QUERY_ROOM_INFO    \
	"{call dbo.SP_RoomInfo(?)}"
*/


#endif	///	__MT_COMMON_H