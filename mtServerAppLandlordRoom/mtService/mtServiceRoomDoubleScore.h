#ifndef		__MT_SERVICE_ROOM_DOUBLE_SCORE_H
#define		__MT_SERVICE_ROOM_DOUBLE_SCORE_H
#include "mtService.h"
#include "mtQueueDesk.h"
#include "mtQueueMgr.h"

/// 用户加倍服务(踢、拉，不包含炸弹加倍)
class mtServiceRoomDoubleScore : public mtService
{
public:
	struct DataInit
	{
		long		            lStructBytes;
		mtQueueMgr*             pkQueueMgr;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lDoubleFlag;			/// 是否加倍初始值为-1, 1:加倍 0:不加倍
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 发牌用户id
		long							lDoubleFlag;			/// 是否加倍 1:加倍 0:不加倍
	};

public:
	mtServiceRoomDoubleScore();
	~mtServiceRoomDoubleScore();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();
	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	/// ai加倍服务响应
	int playDoubleScoreAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserIdx);

	mtQueueMgr*             m_pkQueueMgr;
};


#endif ///	__MT_SERVICE_ROOM_DOUBLE_SCORE_H