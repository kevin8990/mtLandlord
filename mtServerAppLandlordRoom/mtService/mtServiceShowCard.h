#ifndef		__MT_SERVICE_SHOW_CARD_H
#define		__MT_SERVICE_SHOW_CARD_H
#include "mtService.h"
#include "mtQueueDesk.h"

/// 用户明牌服务
class mtServiceShowCard : public mtService
{
public:
	struct DataInit
	{
		long		            lStructBytes;
		mtQueueUser*	        pkQueueUser;
		mtQueuePacket*	        pkQueuePacket;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lShowCard;              /// 是否明牌(1 -明牌，0 -不明牌)
		char 							pcCard[24];             /// 牌内容
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 明牌用户id
		long                            lShowCard;              /// 是否明牌(1 -明牌，0 -不明牌)
		char 							pcCard[24];             /// 牌内容
	};

public:
	mtServiceShowCard();
	~mtServiceShowCard();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
};


#endif ///	__MT_SERVICE_SHOW_CARD_H