#ifndef		__MT_SERVICE_ROOM_EXIT_H
#define		__MT_SERVICE_ROOM_EXIT_H
#include "mtService.h"
#include "mtQueueHall.h"
#include "mtQueueUser.h"
#include "mtQueueMgr.h"

class mtServiceMgr;
/// 退出房间服务
class mtServiceRoomExit : public mtService
{
public:
	enum EExitStatus
	{
		E_STATUS_EXIT_FAIL,
		E_STATUS_EXIT_NORMAL = 2,
		E_STATUS_EXIT_FORCE = 4,
		E_STATUS_EXIT_NETWORK_ANOMALY
	};
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueMgr*             pkQueueMgr;
		mtServiceMgr*	        pkServiceMgr;
	};

	struct DataRead
	{
		long 							lStructBytes;	/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lStatusExit;            /// 真实用户退出房间时的状态(1 -正常退出，-1 -改我叫分，-2 -该我加倍，
		                                                        /// -3 -该我明牌，-4 -该我出牌，下面两个字段表示出牌信息)
		long							lCardType;              /// 出牌类型
		char 							pcCard[24];             /// 出牌内容
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long                            lStatusExit;            /// 用户从房间退到大厅时的状态
		                                                        /// 0 -该用户退出房间失败
		                                                        /// 2 -该用户正常退出(一局结束或者还未开始游戏时执行退出房间操作)
		                                                        /// 4 -该用户强制从某房间退出(游戏已经开始且还没结束)
		                                                        /// 5 -该用户由于网络异常从某房间退出(游戏已经开始且还没结束)
	};

public:
	mtServiceRoomExit();
	virtual ~mtServiceRoomExit();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();
	int getQueueUserLeftCards(char* pcCard);

	int runRoomExitQueueDeskTwo(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
		                        , mtQueueHall::DataRoom* pkQueueHallDataRoom);
// 	int runRoomExitQueueDeskThree(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
// 		                        , mtQueueHall::DataRoom* pkQueueHallDataRoom, DataRead* pkDataRead);

	int runRoomExitQueueDeskThree(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
		, mtQueueHall::DataRoom* pkQueueHallDataRoom, DataRead* pkDataRead
		, mtQueueUser::DataNode** pkQueueUserNodeReplaceAi, int* iSeriveType, int* iCardsOutStyle);

	int playReplaceAiGrabLandlord(mtQueueUser::DataNode* pkQueueUserDataNodeAi);
	int playReplaceAiDoubleScore(mtQueueUser::DataNode* pkQueueUserDataNode);
	int playReplaceAiRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNodeAi);
	int playReplaceAiShowCard(mtQueueUser::DataNode* pkQueueUserDataNodeAi);
	int playReplaceAiCardOut(mtQueueUser::DataNode* pkQueueUserDataNodeAi, DataRead* pkDataRead);
	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int getQueueUserPreQueueUser(int iQueueUserCurIdx);
	int initQueueUserDataNodeAiReplace(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
		                               , int iQueueUserDeskIdx, mtQueueUser::DataNode* pkQueueUserDataNodeAi);
	int calcRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode, long* plGold);

	/// ai
	int getPlayOutCardFirst(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, DataRead* pkDataRead);
	int getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[]);
	int getPlayOutCard(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, DataRead* pkDataRead);
	int getPlayOutCard(int iCardType,char pcRecvCard[],char pcSelfCard[],char pcOutCard[]);

	int IsSeriesCard(char pcSelfCard[],char pcOutCard[]);
	int getCardsNum(char pcCard[]);
	int playOutCardsNotByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum);
	int getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos);
	int getPairCard(char* pcSelfCards,char* pcOutCard ,int iCardBegPos);
	int getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos);
	int	getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int playOutMaxCardsByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum);
	int getMainCardFromRecv(int iCardType,char* pcCardsRecv,int iCardsRecvNum);
	int getPlayOutCardType(char pcOutCard[], long* plCardType);
	int getBombCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos);
	int getRocketCard(char* pcSelfCards,char* pcOutCard);
	int judgeTripleWithOne(char* pcCards,int iCardsNum);
	int judgeTripleWithTwo(char* pcCards,int iCardsNum);
	int judgeAirPlane(char* pcCards,int iCardsNum);
	int judgeFourWithTwo(char* pcCards,int iCardsNum);
	int judgeTripleProgression(char* pcCards,int iCardsNum);

	mtQueueMgr*             m_pkQueueMgr;
	mtServiceMgr*	        m_pkServiceMgr;
};

#endif ///	__MT_SERVICE_ROOM_EXIT_H