#ifndef		__MT_SERVICE_HALL_HALL_2_ROOM_H
#define		__MT_SERVICE_HALL_HALL_2_ROOM_H
#include "mtQueueWork.h"
#include "mtService.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"
/// 从大厅要进入房间的服务
class mtServiceHallHall2Room	: public mtService
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lSpaceId;               /// 场id
		long                            lRoomId;                /// 房间id
		long                            lUserId;                /// 用户id
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 用户从大厅进入房间结果(0 -失败，1 -成功，-1 -用户id错误，
		                                                            /// 3 -用户账号已经正常在某房间里)
		                                                            /// 4 -该用户之前从某房间强制退出(游戏已经开始且还没结束)
		                                                            /// 5 -该用户之前由于网络原因从某房间异常退出(游戏已经开始且还没结束)
		long                            lSpaceId;               /// 场id
		long                            lRoomId;                /// 房间id
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall*                    pkQueueHall;   		    /// 大厅信息
	};

public:
	mtServiceHallHall2Room();
	virtual ~mtServiceHallHall2Room();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
public:
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
};

#endif	///	__MT_SERVICE_HALL_HALL_2_ROOM_H