#ifndef		__MT_SERVICE_ROOM_INCREMENT_GOLD_H
#define		__MT_SERVICE_ROOM_INCREMENT_GOLD_H
#include "mtService.h"
#include "mtQueueHall.h"
#include "mtQueueUser.h"
#include "mtQueueMgr.h"


/// 给用户增加金币
class mtServiceRoomIncrementGold : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueMgr*             pkQueueMgr;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long 							lGold;	                /// 增加金币数
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 增加金币的用户id
		long 							lGold;	                /// 增加金币数
	};

public:
	mtServiceRoomIncrementGold();
	virtual ~mtServiceRoomIncrementGold();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueMgr*             m_pkQueueMgr;
};

#endif ///	__MT_SERVICE_ROOM_INCREMENT_GOLD_H
