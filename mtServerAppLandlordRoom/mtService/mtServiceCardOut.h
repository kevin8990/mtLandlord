#ifndef		__MT_SERVICE_CARD_OUT_H
#define		__MT_SERVICE_CARD_OUT_H
#include "mtService.h"
#include "mtQueueDesk.h"
#include "mtQueueMgr.h"

/// 用户出牌服务
class mtServiceCardOut : public mtService
{
public:
	enum
	{
		MT_CARD_TYPE_BEG,
		MT_CARD_TYPE_PASS 	= MT_CARD_TYPE_BEG,    /// 过牌
		MT_CARD_TYPE_SINGLE,				       /// 单支
		MT_CARD_TYPE_PAIR,					       /// 对子
		MT_CARD_TYPE_ROCKET,				       /// 火箭
		MT_CARD_TYPE_BOMB,					       /// 炸弹
		MT_CARD_TYPE_TRPILE,				       /// 三张
		MT_CARD_TYPE_TRIPLE_ONE,			       /// 三带一
		MT_CARD_TYPE_TRIPLE_TWO,			       /// 三带二
		MT_CARD_TYPE_SINGLE_PROGRESSION,	       /// 单顺
		MT_CARD_TYPE_PAIR_PROGRESSION,		       /// 双顺
		MT_CARD_TYPE_TRIPLE_PROGRESSION,	       /// 三顺
		MT_CARD_TYPE_AIRPLANE,				       /// 飞机带翅膀
		MT_CARD_TYPE_FOUR_TWO,				       /// 四带二
		MT_CARD_TYPE_END
	};

	struct DataInit
	{
		long		            lStructBytes;
		mtQueueMgr*             pkQueueMgr;
		mtQueueUser*	        pkQueueUser;
		mtQueuePacket*	        pkQueuePacket;
		HANDLE					hEventManagerAi;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lCardType;              /// 出牌类型
		char 							pcCard[24];             /// 出牌内容
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 发牌用户id
		long							lCardType;              /// 出牌类型
		char 							pcCard[24];             /// 出牌内容
	};

public:
	mtServiceCardOut();
	~mtServiceCardOut();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();
	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	int updateQueueDeskCardsOut(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int getCardsInformation(char pcCard[], char * pcStr);
	int getQueueUserLeftCards(char* pcCard);

	/// ai出牌服务响应
	int playOutCardAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserAiIdx, int* piContinue, ULONGLONG* pullStartTime);
	int getPlayOutCardType(char pcOutCard[], long* plCardType);
	int getPlayOutCardFirst(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx);
	int getPlayOutCard(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, long* plCardType);
	int IsSeriesCard(char pcSelfCard[],char pcOutCard[]);
	int getCardsNum(char pcCard[]);

	/// 
// 	void arraggeCard(char pcCard[], long lNum);
// 	void insertCard(char pcCard[], char cCard, long lCardSortNum);
	int updateCardsFace(char* pcSelfCard,char* pcOutCard);
	void rearrangeCards(char* pcSelfCards,char cCard);
	int getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[]);
	int getPlayOutCard(int iCardType,char pcRecvCard[],char pcSelfCard[],char pcOutCard[],bool bSplitCard);
	int playOutMaxCardsByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum);
	int judgeSingleProgression(char* pcCards,int iCardsNum);
	int judgePairProgression(char* pcCards,int iCardsNum);
	int judgeTripleProgression(char* pcCards,int iCardsNum);
	int judgeTripleWithOne(char* pcCards,int iCardsNum);
	int judgeTripleWithTwo(char* pcCards,int iCardsNum);
	int judgeFourWithTwo(char* pcCards,int iCardsNum);
	int judgeAirPlane(char* pcCards,int iCardsNum);

	int getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos);
	int forceGetSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos);

	int getPairCard(char* pcSelfCards,char* pcOutCard,int iCardPos);
	int forceGetPairCard(char* pcSelfCards,char* pcOutCard,int iCardPos);

	int getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardPos);
	int getBombCard(char* pcSelfCards,char* pcOutCard,int iCardPos);
	int getRocketCard(char* pcSelfCards,char* pcOutCard);
	int getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardPos,int iCardsNum);
	int getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardPos,int iCardsNum);
	int	getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	int	getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum);
	void resortCardsOut(char* pcCardsOut);
	int getMainCardFromRecv(int iCardType,char* pcCardsRecv,int iCardsRecvNum);
	int playOutCardsNotByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum);
	void printfOutCards(char *pcCurOutCards,char * pcSelfOutCards,char *pcSelfCards,long UserId,long curOutCardUserId);
	bool bSplitCards(mtQueueDesk::DataNode* pkQueueDeskDataNode,int iQueueUserAiIdx);
	mtQueueMgr*     m_pkQueueMgr;
	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
	HANDLE	        m_hEventManagerAi;
};


#endif ///	__MT_SERVICE_CARD_OUT_H