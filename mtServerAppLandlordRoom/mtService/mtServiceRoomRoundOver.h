#ifndef		__MT_SERVICE_ROOM_ROUND_OVER_H
#define		__MT_SERVICE_ROOM_ROUND_OVER_H
#include "mtService.h"
#include "mtQueueHall.h"
#include "mtQueueUser.h"
#include "mtQueueMgr.h"

class mtServiceMgr;

/// 一局游戏结束服务(各家上报分数)
class mtServiceRoomRoundOver : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueMgr*             pkQueueMgr;
		mtServiceMgr*	        pkServiceMgr;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lGold;                  /// 本 局赢取的金币数(负值表示输的金币)

	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		DataUserInfo             		kUserInfo;              /// 玩家一局更新后的信息
	};

public:
	mtServiceRoomRoundOver();
	virtual ~mtServiceRoomRoundOver();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int runWaitStartRecvAi(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int playWaitStartRecvAi(mtQueueUser::DataNode* pkQueueUserDataNode);
	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int playUpdateQueueUserInfoAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserIdx, long lGold, int iQueueUserNextAiIdx);
	int playUpdateQueueDesk(mtQueueDesk::DataNode* pkQueueDeskDataNode);

	int playReplaceAiExitRoom(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int playUpdateQueueUserInfo(mtQueueUser::DataNode* pkQueueUserDataNode, long lGold);
	int calculatePlayerLevel(mtQueueUser::DataNode* pkQueueUserDataNode);
	int resetWaitStartStatus(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int playRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType);
	int calcDoubleScore(mtQueueUser::DataNode*      pkQueueUserDataNode);

	mtQueueMgr*             m_pkQueueMgr;
	mtServiceMgr*	        m_pkServiceMgr;
};

#endif ///	__MT_SERVICE_ROOM_ROUND_OVER_H