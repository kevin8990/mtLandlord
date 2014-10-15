#ifndef		__MT_SERVICE_HALL_ROOM_2_HALL_H
#define		__MT_SERVICE_HALL_ROOM_2_HALL_H
#include "mtQueueWork.h"
#include "mtService.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"
/// 从房间退出到大厅服务
class mtServiceHallRoom2Hall	: public mtService
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long                            lSpaceId;               /// 场id
		long                            lRoomId;                /// 房间id
		long                            lStatusExit;            /// 用户从房间退到大厅时的状态
		                                                        /// 0 -该用户重置状态
		                                                        /// 2 -该用户正常退出(一局结束或者还未开始游戏时执行退出房间操作)
		                                                        /// 4 -该用户强制从某房间退出(游戏已经开始且还没结束)
		                                                        /// 5 -该用户由于网络异常从某房间退出(游戏已经开始且还没结束
		long                            lUserGold;              /// 用户拥有金币数
		long 				            lUserScore;		        /// 用户积分
		long                            lUserLevel;             /// 用户等级
		long				            lUserDayChess;		    /// 当日局数 
		long                            lUserAllChess;          /// 总局数
		long                            lUserWinChess;          /// 胜局数
		long                            lUserWinRate;           /// 胜率
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 用户退出大厅结果(0 -失败， 1 -成功，-1 -账号错误)
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall*                    pkQueueHall;   		    /// 大厅信息
		mtSQLEnv*		                pkSQLEnv;
	};

public:
	mtServiceHallRoom2Hall();
	virtual ~mtServiceHallRoom2Hall();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	void*   getUserNodeAddr(long lUserId);
	void    UpdateUserInfo(DataRead  * pkDataRead);
public:
	mtQueueHall*            m_pkQueueHall;   		    /// 大厅信息
	mtSQLEnv*		        m_pkSQLEnv;
};

#endif	///	__MT_SERVICE_HALL_ROOM_2_HALL_H