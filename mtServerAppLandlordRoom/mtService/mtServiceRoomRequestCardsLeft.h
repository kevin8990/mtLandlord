#ifndef		__MT_SERVICE_ROOM_REQUEST_CARDS_LEFT_H
#define		__MT_SERVICE_ROOM_REQUEST_CARDS_LEFT_H
#include "mtService.h"
#include "mtQueueHall.h"
#include "mtQueueUser.h"
#include "mtQueueMgr.h"

class mtServiceMgr;

/// 一局游戏结束服务(下发其他两家的剩余牌面信息给请求用户)
class mtServiceRoomRequestCardsLeft : public mtService
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
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		char 							pcCardsLast[24];        /// 上家剩余的牌
		char							pcCardsNext[24];		/// 下家剩余的牌
	};

public:
	mtServiceRoomRequestCardsLeft();
	virtual ~mtServiceRoomRequestCardsLeft();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int getQueueUserPreQueueUser(int iQueueUserCurIdx);

	mtQueueMgr*             m_pkQueueMgr;
};

#endif ///	__MT_SERVICE_ROOM_REQUEST_CARDS_LEFT_H
