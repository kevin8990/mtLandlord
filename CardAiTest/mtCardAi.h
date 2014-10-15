#pragma once
#include <iostream>
using namespace std;

typedef struct 
{
	char cCardType;
	char cCardValue;
	
}mtCardInfo;

#if 0
typedef  char	Card[2];
#endif
#define			MTFL_NUM_CARD_TERMINAL						100
#define			MTFL_NUM_CARD_TWO							13
#define			MTFL_NUM_CARD								20


 enum mteCardType
 {
 	MT_FL_CARD_TYPE_BEG		= 0,
 	MT_FL_CARD_TYPE_PASS	= MT_FL_CARD_TYPE_BEG,	///过牌
 	MT_FL_CARD_TYPE_SINGLE,							///单支
 	MT_FL_CARD_TYPE_PAIR,							///对子
 	MT_FL_CARD_TYPE_ROCKET,							///火箭
 	MT_FL_CARD_TYPE_BOMB,							///炸弹
 	MT_FL_CARD_TYPE_TRIPLE,							///三张
 	MT_FL_CARD_TYPE_TRIPLE_ONE,						///三带一
 	MT_FL_CARD_TYPE_TRIPLE_TWO,						///三带二
 	MT_FL_CARD_TYPE_SINGLE_PROGRESSION,				///单顺
 	MT_FL_CARD_TYPE_PAIR_PROGRESSION,				///双顺
 	MT_FL_CARD_TYPE_TRIPLE_PROGRESSION,				///三顺
 	MT_FL_CARD_TYPE_AIRPLANE,						///飞机带翅膀
 	MT_FL_CARD_TYPE_FOUR_TWO,						///四带二
 
 	MT_FL_CARD_TYPE_END
 };

class mtCardAI
{
public:
	mtCardAI();
	~mtCardAI();
	/// 主要接口函数
	//	参数说明：
	//***************************************************
    //返回值:选出牌的类型
	//pcHoldCard：当前玩家手中的牌
	//pcCardOut: 桌子上当前的出牌，就是玩家要压的牌，为NULL时就是玩家上次出牌没有人压得住
	//pcSelectedCard：从玩家手中选出的牌
	//eCardType：当前桌面上出的牌的类型
	//****************************************************
	mteCardType getCardOut(mtCardInfo *pkHoldCard,mtCardInfo *pkCardOut,mteCardType eCardType,mtCardInfo *pkSelectedCard);

	//	参数说明：
	//***************************************************
    //返回值:能否压死桌子上的牌
	//pcSelectedCard：选中的牌
	//pcCardOut: 桌子上当前的出牌，就是玩家要压的牌，为NULL时就是玩家上次出牌没有人压得住
	//eCardType：当前桌面上出的牌的类型
	//****************************************************
	bool judgeCardOut(mtCardInfo *pkSelectedCard,mtCardInfo *pkCardOut,mteCardType  eCardType);








	/// 将牌从小到大排序
	int sortCard(mtCardInfo* pkCardSrc);
	/// 从src中删除takeaway的牌
	int takeAwayCard(mtCardInfo* pkCardSrc, mtCardInfo* pkCardTakeAway);
	/// 获得手中牌的分数, >=7:三倍 >=5:二倍 >=3:一倍
	int getCardScore(mtCardInfo* pkCardSrc);
	/// 获得牌的张数
	int getCardNum(mtCardInfo* pkCardSrc);
	/// 将addin的牌加入到dst中
	int addInCard(mtCardInfo* pkCardDst, mtCardInfo* pkCardAddIn);
	/// 整理牌(将三带一,三带二,飞机带翅膀,四带二等中的有效牌放在前面并排序)
	int tidyCard(mtCardInfo* pkCardSrc);
	/// 测试src中的牌是否是cardtype类型
	int	testCardType(mtCardInfo* pkCardSrc, mteCardType eCardType);
	/// 测试手牌中是否有testcards(服务器用)
	int testIncludeCardsInHand(mtCardInfo* pkHandCards, mtCardInfo* pkTestCards);
	/// 洗牌(服务器用)
	mtCardInfo* shuffle(void);  //洗好的牌打乱放置在m_pkASetOfCards[54]里面
	/// 测试打出的牌是否能组成牌型,并测试能否打过指定的cardType和最小的minCardTakeout;
	mteCardType testTakeoutCard(mtCardInfo* pkCardSrc, mtCardInfo kMinCardTakeout, mteCardType eCardType = MT_FL_CARD_TYPE_PASS);
	/// 主动出牌:从牌中选出最小的牌(dst:选出的牌,src:手中的牌)
	mteCardType getCardToTakeout(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	/// 跟牌:从牌中选出最小的能打住上家的牌(dst:选出的牌,src:手中的牌,cardType:上次打出的牌型,cardTakeout:上次打出的牌,unglue:是否拆牌:1拆,0不拆)
	mteCardType getCardFromType(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout, int iUnglue);
	/// 输出牌面的信息 便于观察
	void dispalyCardInfo(mtCardInfo* pkCardSrc,FILE* pLogFile,int logFlag = 0);   //logFlag = 0,不写进日志，只打印到控制台，logFlag = 1，写日志到文件

	static void cardConvert(char *cCardSrc,mtCardInfo *pkCardDst); //服务器的牌 转换为 客户端牌

	static void cardConvert(mtCardInfo *pkCardSrc,char *cCardDst); //客户端的牌 转换为服务器的牌



//private:
public:
	mteCardType	getTypeFromCard(mtCardInfo* pkCardSrc);
	mteCardType getCardFromSpecifyType(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout);
	mteCardType getCardDirectFromType(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout);
	mteCardType getCardSmallest(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int findASpecifyCardOfValueType(mtCardInfo* pkCardDst, mtCardInfo* pkCardFind);
	int calcBombTripleSinglePair(mtCardInfo* pkCardSrc);
	int	getTripleNum(mtCardInfo* pkCardSrc);
	int copyCard(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int findAGivenSingle(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue);
	int findAGivenPair(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue);
	int findAGivenTriple(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue);
	int findAllSingleProgression(mtCardInfo* pkCardSrc);
	int findAllSimpleProgression(mtCardInfo* pkCardSrc);
	int findPairProgressionInSingleProgression(int iSingleProgressionNum);
	int delASingleProgression(int iSingleProgressionIdx, int iSingleProgressionNum);
	int mergeSingleProgression(int iSingleProgressionNum);

	int findRocket(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int findABombBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cCardTakeout);
	int findASingleBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cCardTakeout);
	int findAPairBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cCardTakeout);
	int	findATripleBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cCardTakeout);
	int findATripleProgression(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int	findATripleProgressionBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cMaxCardTakeOut, int iTripleNumTakeOut);
	int findASingleProgression(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int findASingleProgressionBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cMinCardTakeOut, int iCardNumTakeout);
	int findAPairProgression(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc);
	int	findAPairProgressionBiger(mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo cMinCardTakeOut, int iPairNumTakeOut);
	void findTheMostSomeCard(mtCardInfo* pkCardSrc,char *pcCardFindValue);//找出pkCardSrc里面最多的相同的牌的值 存放在cCardFindValue里面  比如 33344 那么即是3
	void findTheMaxCard(mtCardInfo* pkCardSrc,char *pcCardFindValue);
	void outPutSingleCard(mtCardInfo *pkcard,FILE* pLogFile,int logFlag);

//private:
public:
	mtCardInfo		m_pkCardGet[MTFL_NUM_CARD + 1];
	mtCardInfo		m_pkCardTmpPrivate[MTFL_NUM_CARD + 1];	/// findAllSingleProgression, getTripleNum
	mtCardInfo		m_pkCardTmp[MTFL_NUM_CARD + 1];
	mtCardInfo		m_pkCardTmpPublic[MTFL_NUM_CARD + 1];	/// getCardToTakeout, getCardFromType
	mtCardInfo		m_pkSingleProgression[4][13];
	mtCardInfo		m_pkASetOfCards[54];
	int				m_iBombNum;
	int				m_iTripleNum;
	int				m_iPairNum;
	int				m_iSingleNum;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	a) 先确定火箭：判断是否有大小王。
// 	b) 再确定炸弹：判明是否有四头。
// 	c) 再确定三条：在除了炸弹以外的牌中判断是否包含三条。
// 	d) 再确定三顺：在已经确定的三条中判断是否包含相邻的三条，如果有，则将其组成三顺。注意，应该使三顺的数量尽可能大。即如果有
//	   444555666，则将其合成一个三顺，而不是分成444555一个三顺和666一个三条。
// 	e) 再确定单顺：判断单顺时必须去除四个2以外的所有炸弹。首先判断是否存在除了三条牌（这里的三条是指所有的三条）以外的连牌，如果
//	   有，则将其提取出来。其次，将剩余的牌与每一个三条（不包含三顺）进行试组合，如果能够重新组成单顺和对子，则将原有的三条取消，
//	   重新组合成连牌和单顺（例子4566678重新组成45678和66）。最后，将已知的连牌、三条（不包含三顺）和剩下的牌张再试组合，将所有
//	   如45678（已知的连牌）999（三条）10J（单牌），重新组合成45678910J和99。通过以上的方法，就能将连牌和三条很好地重新组合。
// 	f) 再确定双顺：首先，如果两单顺牌完全重合，则将其重新组合成双顺。其次，在除炸弹、三顺、三条、单顺以外的牌中检测是否包含双顺。
// 	   如果有，将其提取出来。
// 	g) 再确定对子：在炸弹、三顺、三条、连牌、双顺以外的牌中检测是否存在对子，如果存在将其提取出来。
// 	h) 再确定单牌：除了炸弹、三顺、三条、连牌、双顺、对子以外的所有牌张都是单牌。
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////