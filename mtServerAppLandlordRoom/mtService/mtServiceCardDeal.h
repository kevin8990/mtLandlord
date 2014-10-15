#ifndef 	__MT_SERVICE_CARD_DEAL_H
#define 	__MT_SERVICE_CARD_DEAL_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 洗牌 发牌服务
class mtServiceMgr;
class 	mtServiceCardDeal : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueMgr*             pkQueueMgr;
		mtQueueUser*	        pkQueueUser;
		mtQueuePacket*	        pkQueuePacket;
		mtServiceMgr*	        pkServiceMgr;
		HANDLE					hEventManagerAi;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lResult;
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long 							lDefaultLandlordUserId;	/// 默认开始叫分的玩家
		char               				pcCards[17];            /// 每个人17张牌
		char                			pcBaseCards[7];         /// 底牌3张
	};
public:
	mtServiceCardDeal();
	virtual ~mtServiceCardDeal();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int shuffleCard(char* pcCards);
	int shuffleCardEveryOne(char* pcCardsDest);
	/// ai接收发牌服务响应
	int playDealCardAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserAiIdx);
	int playAiGrabLandlord(mtQueueUser::DataNode* pkQueueUserDataNodeAi, int iLandlordScore);
	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int shuffleCardEx(char *pcCards,long *PlayerMode);
	int shuffleCardEx(char *pcCards);

	mtQueueMgr*     m_pkQueueMgr;
	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
	mtServiceMgr*	m_pkServiceMgr;
	HANDLE	        m_hEventManagerAi;
};

#endif 	/// __MT_SERVICE_CARD_DEAL_H

