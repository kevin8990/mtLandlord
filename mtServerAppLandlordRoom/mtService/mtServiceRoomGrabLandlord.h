
#ifndef __MT_SERVICE_ROOM_GRAB_LANDLORD_H
#define __MT_SERVICE_ROOM_GRAB_LANDLORD_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 用户叫分抢地主服务
class mtServiceMgr;
class mtServiceRoomGrabLandlord : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		HANDLE	                hIOCP;
		mtQueueMgr*             pkQueueMgr;
		mtServiceMgr*	        pkServiceMgr;
		HANDLE					hEventManagerAi;
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lUserId;				/// 玩家ID 0,1,2
		long							lScore;	     			/// 玩家叫的分数
		long                            lUserIdLandlord;        /// 地主用户id(-1 当前还没有地主, 0< 标识玩家用户id)
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lScore;                 /// 玩家叫的分数
	};

public:
	mtServiceRoomGrabLandlord();
	virtual ~mtServiceRoomGrabLandlord();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int getQueueDeskLandlordUserId(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode, int* piQueueDeskUserIdx);
	/// ai抢地主服务响应
	int playGrabLandlordAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserCurAiIdx, int iQueueUserCurIdx, ULONGLONG* pullStartTime);
	int playRestartCardDeal(mtQueueUser::DataNode* pkQueueUserDataNode);
	int playDoubleScoreAi(mtQueueUser::DataNode* pkQueueUserDataNode);
	int playAiLandlordCardOut(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNodeAi, ULONGLONG* pullStartTime);
	int playQueueUserSort(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int getGrabLandlordMaxScore(mtQueueDesk::DataNode* pkQueueDeskDataNode);

	void arraggeCard(char pcCard[], long lNum);
	void insertCard(char pcCard[], char cCard, long lCardSortNum);


	/// ai
	int getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[]);
	int	getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos);
	int getPairCard(char* pcSelfCards,char* pcOutCard ,int iCardBegPos);
	int getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos);

	HANDLE	                m_hIOCP;
	mtQueueMgr*             m_pkQueueMgr;
	mtServiceMgr*	        m_pkServiceMgr;
	HANDLE	                m_hEventManagerAi;
};


#endif	///__MT_SERVICE_ROOM_GRAB_LANDLORD_H
