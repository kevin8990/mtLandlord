#include "mtCardAI.h"
#include <time.h>
#include <iostream>
using namespace std;
extern void  Log( FILE* pFiLE,char * pcformat, ... ); //打日志函数

mtCardAI::mtCardAI()  //初始化牌面数组  开始的时候是顺序排好的
{
	int		iCardIdx	= 0;
	for (int iCardType = 0; iCardType < 4; iCardType ++)
	{
		for (int iCardValue = 1; iCardValue < 14; iCardValue ++)
		{
			m_pkASetOfCards[iCardIdx].cCardType		= iCardType;
			m_pkASetOfCards[iCardIdx].cCardValue	= iCardValue;
			iCardIdx ++;
		}
	}
	m_pkASetOfCards[52].cCardType		= 4;
	m_pkASetOfCards[52].cCardValue		= 14;
	m_pkASetOfCards[53].cCardType		= 4;
	m_pkASetOfCards[53].cCardValue		= 15;
}

mtCardAI::~mtCardAI()
{

}

int mtCardAI::findAGivenSingle( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue )  
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue)
	{
		if (pkCardSrc[iIdx].cCardValue == cCardFindValue)
		{
			memcpy(pkCardDst, pkCardSrc + iIdx, sizeof(mtCardInfo));
			pkCardDst[1].cCardValue		= MTFL_NUM_CARD_TERMINAL;

			return 1;
		}
		else
		{
			if (pkCardSrc[iIdx].cCardValue < cCardFindValue)
			{
				iIdx ++;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

int mtCardAI::findAGivenPair( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue )
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 1].cCardValue)
	{
		if (pkCardSrc[iIdx].cCardValue == cCardFindValue &&
			pkCardSrc[iIdx + 1].cCardValue == cCardFindValue)
		{
			memcpy(pkCardDst, pkCardSrc + iIdx, sizeof(mtCardInfo) * 2);
			pkCardDst[2].cCardValue		= MTFL_NUM_CARD_TERMINAL;

			return 1;
		}
		else
		{
			if (pkCardSrc[iIdx].cCardValue < cCardFindValue)
			{
				iIdx ++;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

int mtCardAI::findAGivenTriple( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, char cCardFindValue )
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue && 
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 1].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 2].cCardValue)
	{
		if (pkCardSrc[iIdx].cCardValue == cCardFindValue &&
			pkCardSrc[iIdx + 1].cCardValue == cCardFindValue &&
			pkCardSrc[iIdx + 2].cCardValue == cCardFindValue)
		{
			memcpy(pkCardDst, pkCardSrc + iIdx, sizeof(mtCardInfo) * 3);
			pkCardDst[3].cCardValue		= MTFL_NUM_CARD_TERMINAL;

			return 1;
		}
		else
		{
			if (pkCardSrc[iIdx].cCardValue < cCardFindValue)
			{
				iIdx ++;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

int mtCardAI::findAllSingleProgression( mtCardInfo* pkCardSrc )
{
	int		iProgressionNum = findAllSimpleProgression(pkCardSrc);

	/// 完全重合的单顺并删除
	iProgressionNum		-= findPairProgressionInSingleProgression(iProgressionNum);
	/// 拼接单顺
	iProgressionNum		-= mergeSingleProgression(iProgressionNum);

	return iProgressionNum;
}

int mtCardAI::findAllSimpleProgression( mtCardInfo* pkCardSrc )
{
	int			iFindTripleNum				= 0;
	int			iFindSingleProgressionNum	= 0;
	mtCardInfo	kFindCard;

	kFindCard.cCardValue					= 0;
	m_pkCardGet[0].cCardValue				= MTFL_NUM_CARD_TERMINAL;

	/// 去掉三条
	copyCard(m_pkCardTmpPrivate, pkCardSrc);
	while ((kFindCard.cCardValue) <= MTFL_NUM_CARD_TWO)
	{
		if (findATripleBiger(m_pkCardGet + iFindTripleNum * 3, m_pkCardTmpPrivate, kFindCard))
		{
			kFindCard.cCardValue	= m_pkCardGet[iFindTripleNum * 3].cCardValue;
			iFindTripleNum	++;
		}
		else
		{
			break;
		}
	}
	if (iFindTripleNum > 0)
	{
		takeAwayCard(m_pkCardTmpPrivate, m_pkCardGet);
	}

	kFindCard.cCardValue		= 0;
	/// 找出所有5张的单顺
	while (iFindSingleProgressionNum < 4)
	{
		if (findASingleProgressionBiger(m_pkSingleProgression[iFindSingleProgressionNum], m_pkCardTmpPrivate, kFindCard, 5))
		{
			takeAwayCard(m_pkCardTmpPrivate, m_pkSingleProgression[iFindSingleProgressionNum]);
			iFindSingleProgressionNum	++;
		}
		else
		{
			if (MTFL_NUM_CARD_TERMINAL != m_pkCardGet[0].cCardValue)
			{
				addInCard(m_pkCardTmpPrivate, m_pkCardGet);
				sortCard(m_pkCardTmpPrivate);
				m_pkCardGet[0].cCardValue		= MTFL_NUM_CARD_TERMINAL;
			}
			else
			{
				break;
			}
		}
	}
	if (iFindSingleProgressionNum <= 0)
	{
		return 0;
	}
	/// 单顺扩展
	int			iProgressionIndex;
	int			iFindSingleNum;
	int			iRet				= iFindSingleProgressionNum;
	iFindSingleProgressionNum	= 0;
	while (iFindSingleProgressionNum < iRet)
	{
		/// 向前扩展
		iFindSingleNum				= 0;
		iProgressionIndex			= 0;
		kFindCard.cCardValue		= m_pkSingleProgression[iFindSingleProgressionNum][iProgressionIndex].cCardValue - 1;
		while (kFindCard.cCardValue > 0)
		{
			if (findAGivenSingle(m_pkCardGet + iFindSingleNum, m_pkCardTmpPrivate, kFindCard.cCardValue))
			{
				kFindCard.cCardValue --;
				iFindSingleNum	++;
			}
			else
			{
				break;
			}
		}
		if (iFindSingleNum > 0)
		{
			addInCard(m_pkSingleProgression[iFindSingleProgressionNum], m_pkCardGet);
			sortCard(m_pkSingleProgression[iFindSingleProgressionNum]);
			takeAwayCard(m_pkCardTmpPrivate, m_pkCardGet);
		}
		/// 向后扩展
		iProgressionIndex			= 4 + iFindSingleNum;
		iFindSingleNum				= 0;
		kFindCard.cCardValue		= m_pkSingleProgression[iFindSingleProgressionNum][iProgressionIndex].cCardValue + 1;
		while (kFindCard.cCardValue < MTFL_NUM_CARD_TWO)
		{
			if (findAGivenSingle(m_pkCardGet + iFindSingleNum, m_pkCardTmpPrivate, kFindCard.cCardValue))
			{
				kFindCard.cCardValue ++;
				iFindSingleNum	++;
			}
			else
			{
				break;
			}
		}
		if (iFindSingleNum > 0)
		{
			addInCard(m_pkSingleProgression[iFindSingleProgressionNum], m_pkCardGet);
			takeAwayCard(m_pkCardTmpPrivate, m_pkCardGet);
		}
		iFindSingleProgressionNum ++;
	}

	return iRet;
}

int mtCardAI::findPairProgressionInSingleProgression( int iSingleProgressionNum )
{
	int		iIdx;
	int		iIdxIn;
	int		iSingleProgressionIdx;
	int		iRet		= 0;	/// 有几个单顺重合

	for (iIdx = 0; iIdx < iSingleProgressionNum; iIdx ++)
	{
		for (iIdxIn = iIdx + 1; iIdxIn < iSingleProgressionNum; iIdxIn ++)
		{
			iSingleProgressionIdx		= 0;
			while (MTFL_NUM_CARD_TERMINAL != m_pkSingleProgression[iIdx][iSingleProgressionIdx].cCardValue &&
				MTFL_NUM_CARD_TERMINAL != m_pkSingleProgression[iIdxIn][iSingleProgressionIdx].cCardValue &&
				m_pkSingleProgression[iIdx][iSingleProgressionIdx].cCardValue ==  
				m_pkSingleProgression[iIdxIn][iSingleProgressionIdx].cCardValue)
			{
				iSingleProgressionIdx ++;
			}
			if (MTFL_NUM_CARD_TERMINAL == m_pkSingleProgression[iIdx][iSingleProgressionIdx].cCardValue &&
				MTFL_NUM_CARD_TERMINAL == m_pkSingleProgression[iIdxIn][iSingleProgressionIdx].cCardValue)
			{
				delASingleProgression(iIdxIn, iSingleProgressionNum);
				iSingleProgressionNum --;
				delASingleProgression(iIdx, iSingleProgressionNum);
				iRet += 2;
			}
		}
	}

	return iRet;
}

int mtCardAI::delASingleProgression( int iSingleProgressionIdx, int iSingleProgressionNum )
{
	if (iSingleProgressionIdx == iSingleProgressionNum - 1)
	{
		m_pkSingleProgression[iSingleProgressionIdx][0].cCardValue = 0;
	}
	else
	{
		while (iSingleProgressionIdx < iSingleProgressionNum - 1)
		{
			copyCard(m_pkSingleProgression[iSingleProgressionIdx], m_pkSingleProgression[iSingleProgressionIdx + 1]);
			iSingleProgressionIdx ++;
		}
	}

	return 1;
}

int mtCardAI::mergeSingleProgression( int iSingleProgressionNum )
{
	int		iIdx;
	int		iIdxIn;
	int		iLastCardIdx;
	int		iRet		= 0;	/// 有几个单顺合并

	for (iIdx = 0; iIdx < iSingleProgressionNum; iIdx ++)
	{
		iLastCardIdx		= getCardNum(m_pkSingleProgression[iIdx]) - 1;
		for (iIdxIn = iIdx + 1; iIdxIn < iSingleProgressionNum; iIdxIn ++)
		{

			if (m_pkSingleProgression[iIdx][iLastCardIdx].cCardValue + 1 ==
				m_pkSingleProgression[iIdxIn][0].cCardValue)
			{
				addInCard(m_pkSingleProgression[iIdx], m_pkSingleProgression[iIdxIn]);
				delASingleProgression(iIdxIn, iSingleProgressionNum);
				iSingleProgressionNum --;
				iRet ++;
				continue;
			}
		}
	}

	return iRet;
}

int mtCardAI::calcBombTripleSinglePair( mtCardInfo* pkCardSrc )
{
	m_iBombNum		= 0;
	m_iSingleNum	= 0;
	m_iPairNum		= 0;
	m_iTripleNum	= 0;

	mtCardInfo		kCardFind;
	kCardFind.cCardValue		= 0;
	copyCard(m_pkCardTmp, pkCardSrc);
	while (1)
	{
		if (findABombBiger(m_pkCardTmpPrivate, m_pkCardTmp, kCardFind))
		{
			m_iBombNum ++;
			takeAwayCard(m_pkCardTmp, m_pkCardTmpPrivate);
		}
		else
		{
			break;
		}
	}
	while (1)
	{
		if (findATripleBiger(m_pkCardTmpPrivate, m_pkCardTmp, kCardFind))
		{
			m_iTripleNum ++;
			takeAwayCard(m_pkCardTmp, m_pkCardTmpPrivate);
		}
		else
		{
			break;
		}
	}
	while (1)
	{
		if (findAPairBiger(m_pkCardTmpPrivate, m_pkCardTmp, kCardFind))
		{
			m_iPairNum ++;
			takeAwayCard(m_pkCardTmp, m_pkCardTmpPrivate);
		}
		else
		{
			break;
		}
	}
	while (1)
	{
		if (findASingleBiger(m_pkCardTmpPrivate, m_pkCardTmp, kCardFind))
		{
			m_iSingleNum ++;
			takeAwayCard(m_pkCardTmp, m_pkCardTmpPrivate);
		}
		else
		{
			break;
		}
	}

	return 0;
}

int mtCardAI::tidyCard( mtCardInfo* pkCardSrc )
{
	int				iCardNum	= getCardNum(pkCardSrc);
	mtCardInfo		kCardFind;
	kCardFind.cCardValue	= 0;
	copyCard(m_pkCardTmp, pkCardSrc);
	if (4 == iCardNum)
	{
		if (findATripleBiger(pkCardSrc, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			memcpy(pkCardSrc + 3, m_pkCardTmp, sizeof(mtCardInfo) * 2);

			return 1;
		}
	}
	else if (5 == iCardNum)
	{
		if (findATripleBiger(pkCardSrc, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			memcpy(pkCardSrc + 3, m_pkCardTmp, sizeof(mtCardInfo) * 3);

			return 1;
		}
	}
	else if (6 == iCardNum)
	{
		if (findABombBiger(pkCardSrc, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			memcpy(pkCardSrc + 4, m_pkCardTmp, sizeof(mtCardInfo) * 3);

			return 1;
		}
	}
	else if (8 == iCardNum)
	{
		if (findABombBiger(pkCardSrc, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			if (findAPairBiger(pkCardSrc + 4, m_pkCardTmp, kCardFind))
			{
				takeAwayCard(m_pkCardTmp, pkCardSrc + 4);
				memcpy(pkCardSrc + 6, m_pkCardTmp, sizeof(mtCardInfo) * 3);

				return 1;
			}
		}
		else if (findATripleProgression(pkCardSrc, m_pkCardTmp))
		{
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			if (findASingleBiger(pkCardSrc + 6, m_pkCardTmp, kCardFind))
			{
				takeAwayCard(m_pkCardTmp, pkCardSrc + 6);
				memcpy(pkCardSrc + 7, m_pkCardTmp, sizeof(mtCardInfo) * 2);

				return 1;
			}
		}
	}
	else 
	{
		if (findATripleProgression(pkCardSrc, m_pkCardTmp))
		{
			int		iCardNum	= getCardNum(pkCardSrc);
			int		iIdx		= 0;
			takeAwayCard(m_pkCardTmp, pkCardSrc);
			while (1)
			{
				if (findAPairBiger(pkCardSrc + iCardNum + iIdx * 2, m_pkCardTmp, kCardFind))
				{
					takeAwayCard(m_pkCardTmp, pkCardSrc + iCardNum + iIdx * 2);
					iIdx ++;
				}
				else
				{
					break;
				}
			}

			if (0 == iIdx)
			{
				while (1)
				{
					if (findASingleBiger(pkCardSrc + iCardNum + iIdx, m_pkCardTmp, kCardFind))
					{
						takeAwayCard(m_pkCardTmp, pkCardSrc + iCardNum + iIdx);
						iIdx ++;
					}
					else
					{
						break;
					}
				}
			}
			if (iIdx == iCardNum / 3)
			{
				return 1;
			}
		}
	}

	return 0;
}

int mtCardAI::testCardType( mtCardInfo* pkCardSrc, mteCardType eCardType )
{
	if (MT_FL_CARD_TYPE_PASS == eCardType)
	{
		if (0 == getCardNum(pkCardSrc))
		{
			return 1;
		}
	}
	mtCardInfo		kCardFind;
	kCardFind.cCardValue	= 0;

	copyCard(m_pkCardTmpPublic, pkCardSrc);
	if (MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType)
	{
		if (4 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 0;
		}
		if (findATripleBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
	{
		if (5 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 0;
		}
		if (findATripleBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
			if (findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
			{
				return 1;
			}
		}
	}
	else if (MT_FL_CARD_TYPE_AIRPLANE == eCardType)
	{
		int		iTripleNum;
		int		iFindTime	= 0;
		if (!findATripleProgression(m_pkCardTmp, m_pkCardTmpPublic))
		{
			return 0;
		}
		iTripleNum		= getCardNum(m_pkCardTmp) / 3;
		takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
		/// 带双
		while (iFindTime < iTripleNum)
		{
			if (iTripleNum != getCardNum(m_pkCardTmpPublic) / 2)
			{
				return 0;
			}
			if (findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
			{
				iFindTime ++;
				takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
			}
			else
			{
				break;
			}
		}
		if (iFindTime == iTripleNum)
		{
			return 1;
		}
		/// 带单
		iFindTime	= 0;
		if (iTripleNum != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (!findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_FOUR_TWO == eCardType)
	{
		int		iFindTime	= 0;
		if (!findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 0;
		}
		takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
		/// 带双
		if (4 == getCardNum(m_pkCardTmpPublic))
		{
			if (findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
			{
				return 0;
			}
			while (iFindTime < 2)
			{
				if (findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
				{
					takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
					iFindTime ++;
				}
				else
				{
					break;
				}
			}
			if (2 == iFindTime)
			{
				return 1;
			}
		}
		/// 带单
		else if (2 == getCardNum(m_pkCardTmpPublic))
		{
			if (!findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
			{
				return 1;
			}
		}
	}
	else if (MT_FL_CARD_TYPE_SINGLE == eCardType)
	{
		if (1 == getCardNum(m_pkCardTmpPublic))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_PAIR == eCardType)
	{
		if (2 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findAPairBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_ROCKET == eCardType)
	{
		if (2 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findRocket(m_pkCardTmp, m_pkCardTmpPublic))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_BOMB == eCardType)
	{
		if (4 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_TRIPLE == eCardType)
	{
		if (3 != getCardNum(m_pkCardTmpPublic))
		{
			return 0;
		}
		if (findATripleBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_SINGLE_PROGRESSION == eCardType)
	{
		if (!findASingleProgression(m_pkCardTmp, m_pkCardTmpPublic))
		{
			return 0;
		}
		if (getCardNum(m_pkCardTmpPublic) == getCardNum(m_pkCardTmp))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_PAIR_PROGRESSION == eCardType)
	{
		if (!findAPairProgression(m_pkCardTmp, m_pkCardTmpPublic))
		{
			return 0;
		}
		if (getCardNum(m_pkCardTmpPublic) == getCardNum(m_pkCardTmp))
		{
			return 1;
		}
	}
	else if (MT_FL_CARD_TYPE_TRIPLE_PROGRESSION == eCardType)
	{
		if (!findATripleProgression(m_pkCardTmp, m_pkCardTmpPublic))
		{
			return 0;
		}
		if (getCardNum(m_pkCardTmpPublic) == getCardNum(m_pkCardTmp))
		{
			return 1;
		}
	}

	return 0;
}

int mtCardAI::testIncludeCardsInHand( mtCardInfo* pkHandCards, mtCardInfo* pkTestCards )
{
	int		iTestCardsNum	= getCardNum(pkTestCards);
	int		iIdx;

	for (iIdx = 0; iIdx < iTestCardsNum; iIdx++)
	{
		if (!findASpecifyCardOfValueType(pkHandCards, pkTestCards + iIdx))
		{
			break;
		}
	}
	if (iIdx == iTestCardsNum)
	{
		return 1;
	}

	return 0;
}

int mtCardAI::takeAwayCard( mtCardInfo* pkCardSrc, mtCardInfo* pkCardTakeAway )
{
	int		iIdx	= 0;
	int		iIdxIn	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardTakeAway[iIdx].cCardValue)
	{
		while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn].cCardValue)
		{
			if (pkCardSrc[iIdxIn].cCardValue == pkCardTakeAway[iIdx].cCardValue &&
				pkCardSrc[iIdxIn].cCardType	== pkCardTakeAway[iIdx].cCardType)
			{
				break;
			}
			iIdxIn++;
		}
		if (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn].cCardValue)
		{
			do 
			{
				memcpy(pkCardSrc + iIdxIn, pkCardSrc + iIdxIn + 1, sizeof(mtCardInfo));
				iIdxIn ++;
			} while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn].cCardValue);
		}
		iIdx++;
		iIdxIn	= 0;
	}

	return 1;
}

/************************************************************************/
/* 返回参数牌组的分数：火箭8、炸弹6、大王4、小王3、 一张二2                                                                    */
/************************************************************************/
int mtCardAI::getCardScore( mtCardInfo* pkCardSrc )
{
	int				iCardScore	= 0;
	mtCardInfo		kCardFind;
	kCardFind.cCardValue	= 0;
	copyCard(m_pkCardTmpPublic, pkCardSrc);
	/// 找火箭
	if (findRocket(m_pkCardTmp, m_pkCardTmpPublic))
	{
		iCardScore	+= 8;
		takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
	}
	/// 找炸弹
	while (1)
	{
		if (findABombBiger(m_pkCardTmp, m_pkCardTmpPublic, kCardFind))
		{
			iCardScore	+= 6;
			takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
		}
		else
		{
			break;
		}
	}
	/// 找大王
	if (findAGivenSingle(m_pkCardTmp, m_pkCardTmpPublic, 15))
	{
		iCardScore	+= 4;
		takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
	}
	/// 找小王
	if (findAGivenSingle(m_pkCardTmp, m_pkCardTmpPublic, 14))
	{
		iCardScore	+= 3;
		takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
	}
	/// 找2
	while (1)
	{
		if (findAGivenSingle(m_pkCardTmp, m_pkCardTmpPublic, 13))
		{
			iCardScore	+= 2;
			takeAwayCard(m_pkCardTmpPublic, m_pkCardTmp);
		}
		else
		{
			break;
		}
	}

	return iCardScore;
}

mtCardInfo* mtCardAI::shuffle( void )
{
	mtCardInfo	kCardTmp;
	int		iRandPosition;
	int		iIdx	= 0;
	srand((int)time(0));
	while (iIdx < 54)
	{
		iRandPosition			= rand() % 54;
		memcpy(&kCardTmp, m_pkASetOfCards + iIdx, sizeof(mtCardInfo));
		memcpy(m_pkASetOfCards + iIdx, m_pkASetOfCards + iRandPosition, sizeof(mtCardInfo));
		memcpy(m_pkASetOfCards + iRandPosition, &kCardTmp, sizeof(mtCardInfo));
		iIdx ++;
	}

	return m_pkASetOfCards;
}

int mtCardAI::getTripleNum( mtCardInfo* pkCardSrc )
{
	mtCardInfo		kCardFind;
	int				iTripleNum	= 0;
	kCardFind.cCardValue		= 0;
	while (1)
	{
		if (!findATripleBiger(m_pkCardTmpPrivate, pkCardSrc, kCardFind))
		{
			break;
		}
		iTripleNum ++;
		kCardFind.cCardValue	= m_pkCardTmpPrivate[0].cCardValue;
	}

	return iTripleNum;
}

int mtCardAI::getCardNum( mtCardInfo* pkCardSrc )
{
	int		iCardNum	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iCardNum].cCardValue)
	{
		iCardNum ++;
	}

	return iCardNum;
}

int mtCardAI::addInCard( mtCardInfo* pkCardDst, mtCardInfo* pkCardAddIn )
{
	int		iIdxDst		= -1;
	int		iIdxAddIn	= -1;
	while (MTFL_NUM_CARD_TERMINAL != pkCardDst[iIdxDst + 1].cCardValue)
	{
		iIdxDst ++;
	}

	do 
	{
		iIdxAddIn ++;
		iIdxDst ++;
		memcpy(pkCardDst + iIdxDst, pkCardAddIn + iIdxAddIn, sizeof(mtCardInfo));
	} while (MTFL_NUM_CARD_TERMINAL != pkCardAddIn[iIdxAddIn].cCardValue);

	return 1;
}

int mtCardAI::copyCard( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	if (pkCardDst == NULL || pkCardSrc == NULL)
	{
		return 0;
	}
	int		iIdx	= -1;
	do
	{
		iIdx ++;
		if (iIdx > 19)
		{
			pkCardDst[iIdx].cCardValue = MTFL_NUM_CARD_TERMINAL;
			return 0;
		}
		if (&pkCardSrc[iIdx] == NULL)
		{
			cout<<"\n*************************"<<iIdx<<"88888";
			
		}
		//cout<<"\n*******"<<iIdx<<"    "<<pkCardSrc[iIdx].cCardValue<<"****************";
		memcpy(pkCardDst + iIdx, pkCardSrc + iIdx, sizeof(mtCardInfo));

	} while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue);
	return 1;
}

int mtCardAI::findRocket( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 1].cCardValue)
	{
		if (14 == pkCardSrc[iIdx].cCardValue && 
			15 == pkCardSrc[iIdx + 1].cCardValue)
		{
			memcpy(pkCardDst, pkCardSrc + iIdx, sizeof(mtCardInfo) * 2);
			pkCardDst[2].cCardValue		= MTFL_NUM_CARD_TERMINAL;

			return 1;
		}
		iIdx ++;
	}

	return 0;
}

int mtCardAI::findABombBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kCardTakeout )
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 1].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 2].cCardValue &&
		MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx + 3].cCardValue)
	{
		if (pkCardSrc[iIdx].cCardValue == pkCardSrc[iIdx + 1].cCardValue &&
			pkCardSrc[iIdx].cCardValue == pkCardSrc[iIdx + 2].cCardValue &&
			pkCardSrc[iIdx].cCardValue == pkCardSrc[iIdx + 3].cCardValue &&
			pkCardSrc[iIdx].cCardValue > kCardTakeout.cCardValue)
		{
			memcpy(pkCardDst, pkCardSrc + iIdx, sizeof(mtCardInfo) * 4);
			pkCardDst[4].cCardValue		= MTFL_NUM_CARD_TERMINAL;

			return 1;
		}
		iIdx ++;
	}

	return 0;
}

int mtCardAI::findASingleBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kCardTakeout )
{
	mtCardInfo		kCardFind;
	kCardFind.cCardValue	= kCardTakeout.cCardValue + 1;
	while (kCardFind.cCardValue <= MTFL_NUM_CARD_TWO + 2)
	{
		if (findAGivenSingle(pkCardDst, pkCardSrc, kCardFind.cCardValue))
		{
			return 1;
		}
		else
		{
			kCardFind.cCardValue ++;
		}
	}

	return 0;
}

int mtCardAI::findAPairBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kCardTakeout )
{
	mtCardInfo	kCardFind;
	kCardFind.cCardValue	= kCardTakeout.cCardValue + 1;
	while (kCardFind.cCardValue <= MTFL_NUM_CARD_TWO)
	{
		if (findAGivenPair(pkCardDst, pkCardSrc, kCardFind.cCardValue))
		{
			return 1;
		}
		else
		{
			kCardFind.cCardValue ++;
		}
	}

	return 0;
}

int mtCardAI::findATripleBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kCardTakeout )
{
	mtCardInfo		kCardFind;
	kCardFind.cCardValue	= kCardTakeout.cCardValue + 1;
	while ((kCardFind.cCardValue) <= MTFL_NUM_CARD_TWO)
	{
		if (findAGivenTriple(pkCardDst, pkCardSrc, kCardFind.cCardValue))
		{
			return 1;
		}
		else
		{
			kCardFind.cCardValue++;
		}
	}

	return 0;
}

int mtCardAI::findATripleProgression( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	int			iFindTripleNum		= 0;
	mtCardInfo	kFindCard;
	kFindCard.cCardValue			= 0;
	while (kFindCard.cCardValue < MTFL_NUM_CARD_TWO)
	{
		if (findATripleBiger(pkCardDst, pkCardSrc, kFindCard))
		{
			kFindCard.cCardValue	= pkCardDst[0].cCardValue;
			iFindTripleNum	++;
			while (1)
			{
				if (kFindCard.cCardValue + iFindTripleNum < MTFL_NUM_CARD_TWO &&
					findAGivenTriple(pkCardDst + iFindTripleNum * 3, pkCardSrc, kFindCard.cCardValue + iFindTripleNum))
				{
					iFindTripleNum ++;
				}
				else
				{
					if (iFindTripleNum >= 2)
					{
						return 1;
					}
					else
					{
						kFindCard.cCardValue	+= iFindTripleNum;
						iFindTripleNum	= 0;
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

int mtCardAI::findATripleProgressionBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kMaxCardTakeOut, int iTripleNumTakeOut )
{
	int			iFindTripleNum	= 0;
	mtCardInfo	kFindCard;

	kFindCard.cCardValue		= kMaxCardTakeOut.cCardValue;
	while (kFindCard.cCardValue + iTripleNumTakeOut < MTFL_NUM_CARD_TWO)
	{
		if (findATripleBiger(pkCardDst, pkCardSrc, kFindCard))
		{
			kFindCard.cCardValue	= pkCardDst[0].cCardValue;
			iFindTripleNum	++;
			while (iFindTripleNum <= iTripleNumTakeOut)
			{
				if (kFindCard.cCardValue + iFindTripleNum < MTFL_NUM_CARD_TWO &&
					findAGivenTriple(pkCardDst + iFindTripleNum * 3, pkCardSrc, kFindCard.cCardValue + iFindTripleNum))
				{
					iFindTripleNum ++;
				}
				else
				{
					if (iFindTripleNum == iTripleNumTakeOut)
					{
						return 1;
					}
					else
					{
						kFindCard.cCardValue += iFindTripleNum;
						iFindTripleNum	= 0;
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

int mtCardAI::findASingleProgression( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	int		iProgressionNum;
	int		iProgressionIdx		= 1;
	int		iProgressionGetIdx	= 0;

	if (iProgressionNum = findAllSingleProgression(pkCardSrc))
	{
		if (iProgressionNum > 1)
		{
			while (iProgressionIdx < iProgressionNum)
			{
				if ((m_pkSingleProgression[iProgressionIdx][0].cCardValue) < 
					(m_pkSingleProgression[iProgressionGetIdx][0].cCardValue))
				{
					iProgressionGetIdx		= iProgressionIdx;
				}
				iProgressionIdx ++;
			}
		}
		copyCard(pkCardDst, m_pkSingleProgression[iProgressionGetIdx]);
		return 1;
	}
	else
	{
		return 0;
	}
}

int mtCardAI::findASingleProgressionBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kMinCardTakeout, int iCardNumTakeout )
{
	int			iFindCardNum	= 0;
	mtCardInfo	kFindCard;
	kFindCard.cCardValue		= kMinCardTakeout.cCardValue;
	while (kFindCard.cCardValue + iCardNumTakeout < MTFL_NUM_CARD_TWO 
		&& kFindCard.cCardValue < MTFL_NUM_CARD_TWO)
	{
		if (findASingleBiger(pkCardDst, pkCardSrc, kFindCard))
		{
			kFindCard.cCardValue	= pkCardDst[0].cCardValue;
			iFindCardNum	++;
			while (iFindCardNum < iCardNumTakeout)
			{
				if (kFindCard.cCardValue + iFindCardNum < MTFL_NUM_CARD_TWO &&
					findAGivenSingle(pkCardDst + iFindCardNum, pkCardSrc, kFindCard.cCardValue + iFindCardNum))
				{
					iFindCardNum ++;
				}
				else
				{
					kFindCard.cCardValue += iFindCardNum;
					iFindCardNum	= 0;
					break;
				}
			}
			if (iFindCardNum == iCardNumTakeout)
			{
				return 1;
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

int mtCardAI::findAPairProgression( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	int			iFindPairNum	= 0;
	mtCardInfo	kFindCard;
	kFindCard.cCardValue		= 0;
	while (kFindCard.cCardValue < MTFL_NUM_CARD_TWO)
	{
		if (findAPairBiger(pkCardDst, pkCardSrc, kFindCard))
		{
			kFindCard.cCardValue		= pkCardDst[0].cCardValue;
			iFindPairNum	++;
			while (1)
			{
				if (kFindCard.cCardValue + iFindPairNum < MTFL_NUM_CARD_TWO &&
					findAGivenPair(pkCardDst + iFindPairNum * 2, pkCardSrc, kFindCard.cCardValue + iFindPairNum))
				{
					iFindPairNum ++;
				}
				else
				{
					if (iFindPairNum >= 3)
					{
						return 1;
					}
					else
					{
						kFindCard.cCardValue += iFindPairNum;
						iFindPairNum		= 0;
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

int mtCardAI::findAPairProgressionBiger( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mtCardInfo kMinCardTakeout, int iPairNumTakeOut )
{
	int			iFindCardNum	= 0;
	mtCardInfo	kFindCard;
	kFindCard.cCardValue		= kMinCardTakeout.cCardValue;
	while (kFindCard.cCardValue + iPairNumTakeOut < MTFL_NUM_CARD_TWO)
	{
		if (findAPairBiger(pkCardDst, pkCardSrc, kFindCard)) //先找到一对大于它的
		{
			kFindCard.cCardValue		= pkCardDst[0].cCardValue;
			iFindCardNum	++;
			while (iFindCardNum <= iPairNumTakeOut)    
			{
				if (kFindCard.cCardValue + iFindCardNum < MTFL_NUM_CARD_TWO &&
					findAGivenPair(pkCardDst + iFindCardNum*2, pkCardSrc, kFindCard.cCardValue + iFindCardNum))  //再找 +1 的对子
				{
					iFindCardNum ++;

					//add by wsd start
					if (iFindCardNum == iPairNumTakeOut) //已经够了
					{
						return 1;
					}

					//add by wsd end
				}
				else
				{
					if (iFindCardNum == iPairNumTakeOut) //已经够了
					{
						return 1;
					}
					else //不行 重新找下一个大的对子
					{
						kFindCard.cCardValue += iFindCardNum;
						iFindCardNum		= 0;
						break;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	return 0;
}

mteCardType mtCardAI::getCardFromSpecifyType( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout )
{
	mteCardType		eCardTypeRet	= MT_FL_CARD_TYPE_PASS;
	mtCardInfo		kCardFind;

	copyCard(m_pkCardTmp, pkCardSrc);
	///change by wsd start
	/// 火箭
	if (eCardType == MT_FL_CARD_TYPE_ROCKET)
	{
		if (findRocket(pkCardDst,m_pkCardTmp)) //牌里面有火箭
		{
			return MT_FL_CARD_TYPE_ROCKET;
		}

	}
	else                                     //找的不是火箭 也要先把火箭牌 选出来  即现在实现的是 不能拆牌   后续的都一样道理
	{
		if (findRocket(m_pkCardGet,m_pkCardTmp)) //牌里面有火箭 
		{
			takeAwayCard(m_pkCardTmp, m_pkCardGet);
		}
		
	}
	//change by wsd end


	////原始的 start
	////火箭
	//if (findRocket(pkCardDst, m_pkCardTmp)) //牌里面有火箭
	//{
	//	if (MT_FL_CARD_TYPE_ROCKET == eCardType) 
	//	{
	//		return MT_FL_CARD_TYPE_ROCKET;
	//	}
	//	else //要找的不是火箭牌
	//	{
	//		takeAwayCard(pkCardDst, m_pkCardTmp); //就把 pkCardDst 清理一下
	//	}
	//}
	////原始的 end

	/// 炸弹
	if (MT_FL_CARD_TYPE_BOMB == eCardType)
	{
		kCardFind.cCardValue	= pkCardTakeout[0].cCardValue;
		if (findABombBiger(pkCardDst, m_pkCardTmp, kCardFind))
		{
			return MT_FL_CARD_TYPE_BOMB;
		}
	}
	else 
	{
		kCardFind.cCardValue	= 0;
		while (1)
		{
			if (findABombBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
				kCardFind.cCardValue	= m_pkCardGet[0].cCardValue;
			}
			else
			{
				break;
			}
		}
	}
	/// 三顺,飞机
	if (MT_FL_CARD_TYPE_TRIPLE_PROGRESSION == eCardType ||
		MT_FL_CARD_TYPE_AIRPLANE == eCardType)
	{
		//int			iTripleNum	= getTripleNum(m_pkCardTmp);       //得到三顺的位数  这个地方应该有问题
		int			    iTripleNum	= getTripleNum(pkCardTakeout);     //改后
		kCardFind.cCardValue	= pkCardTakeout[0].cCardValue + iTripleNum - 1 ; //改后

		if (findATripleProgressionBiger(pkCardDst, m_pkCardTmp, kCardFind, iTripleNum))
		{
			return MT_FL_CARD_TYPE_TRIPLE_PROGRESSION;
		}
	}
	else
	{
		while (1)
		{
			if (findATripleProgression(m_pkCardGet, m_pkCardTmp))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
			}
			else
			{
				break;
			}
		}
	}
	/// 单顺
	if (MT_FL_CARD_TYPE_SINGLE_PROGRESSION == eCardType)
	{
		if (findASingleProgressionBiger(pkCardDst, m_pkCardTmp, pkCardTakeout[0], getCardNum(pkCardTakeout)))
		{
			return MT_FL_CARD_TYPE_SINGLE_PROGRESSION;
		}
	}
	else
	{
		while (1)
		{
			if (findASingleProgression(m_pkCardGet, m_pkCardTmp))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
			}
			else
			{
				break;
			}
		}
	}
	/// 三条 三带一　三带二
	if (MT_FL_CARD_TYPE_TRIPLE == eCardType ||
		MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType ||
		MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
	{
		if (findATripleBiger(pkCardDst, m_pkCardTmp, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_TRIPLE;
		}
	}
	else
	{
		while (1)
		{
			kCardFind.cCardValue	= 0;
			if (findATripleBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
				kCardFind.cCardValue	= m_pkCardGet[0].cCardValue;
			}
			else
			{
				break;
			}
		}
	}
	/// 双顺
	if (MT_FL_CARD_TYPE_PAIR_PROGRESSION == eCardType)
	{
		if (findAPairProgressionBiger(pkCardDst, m_pkCardTmp, pkCardTakeout[0], getCardNum(pkCardTakeout) / 2))
		{
			return MT_FL_CARD_TYPE_PAIR_PROGRESSION;
		}
	}
	else
	{
		while (1)
		{
			if (findAPairProgression(m_pkCardGet, m_pkCardTmp))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
			}
			else
			{
				break;
			}
		}
	}
	/// 对子
	if (MT_FL_CARD_TYPE_PAIR == eCardType)
	{
		if (findAPairBiger(pkCardDst, m_pkCardTmp, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_PAIR;
		}
	}
	else 
	{
		while (1)
		{
			kCardFind.cCardValue	= 0;
			if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
			{
				takeAwayCard(m_pkCardTmp, m_pkCardGet);
			}
			else
			{
				break;
			}
		}
	}
	/// 单牌
	if (MT_FL_CARD_TYPE_SINGLE == eCardType)
	{
		if (findASingleBiger(pkCardDst, m_pkCardTmp, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_SINGLE;
		}
	}

	return eCardTypeRet;
}

mteCardType mtCardAI::getCardDirectFromType( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout )
{
	/// 单顺
	if (MT_FL_CARD_TYPE_SINGLE_PROGRESSION == eCardType)
	{
		if (findASingleProgressionBiger(pkCardDst, pkCardSrc, pkCardTakeout[0], getCardNum(pkCardTakeout)))
		{
			return MT_FL_CARD_TYPE_SINGLE_PROGRESSION;
		}
	}
	/// 三条 三带一　三带二
	if (MT_FL_CARD_TYPE_TRIPLE == eCardType ||
		MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType ||
		MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
	{
		if (findATripleBiger(pkCardDst, pkCardSrc, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_TRIPLE;
		}
	}
	/// 双顺
	if (MT_FL_CARD_TYPE_PAIR_PROGRESSION == eCardType)
	{
		if (findAPairProgressionBiger(pkCardDst, pkCardSrc, pkCardTakeout[0], getCardNum(pkCardTakeout) / 2))
		{
			return MT_FL_CARD_TYPE_PAIR_PROGRESSION;
		}
	}
	/// 对子
	if (MT_FL_CARD_TYPE_PAIR == eCardType)
	{
		if (findAPairBiger(pkCardDst, pkCardSrc, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_PAIR;
		}
	}
	/// 单牌
	if (MT_FL_CARD_TYPE_SINGLE == eCardType)
	{
		if (findASingleBiger(pkCardDst, pkCardSrc, pkCardTakeout[0]))
		{
			return MT_FL_CARD_TYPE_SINGLE;
		}
	}

	return MT_FL_CARD_TYPE_PASS;
}

mteCardType mtCardAI::getTypeFromCard( mtCardInfo* pkCardSrc )
{
	int				iCardNum;
	mtCardInfo		kCardFind;

	kCardFind.cCardValue		= 0;
	copyCard(m_pkCardTmp, pkCardSrc);
	/// 检查牌的数量
	iCardNum	= getCardNum(m_pkCardTmp);
	/// 没有牌直接返回
	if (0 == iCardNum ||13 == iCardNum || 17 == iCardNum || 19 == iCardNum)
	{
		return MT_FL_CARD_TYPE_PASS;
	}
	/// 检查牌型
	/// 单牌
	if (1 == iCardNum)
	{
		return MT_FL_CARD_TYPE_SINGLE;
	}
	/// 对子,火箭
	else if (2 == iCardNum)
	{
		if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			return MT_FL_CARD_TYPE_PAIR;
		}
		else if (findRocket(m_pkCardGet, m_pkCardTmp))
		{
			return MT_FL_CARD_TYPE_ROCKET;
		}
	}
	/// 三张
	else if (3 == iCardNum)
	{
		if (findATripleBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			return MT_FL_CARD_TYPE_TRIPLE;
		}
	}
	/// 炸弹,三带一
	else if (4 == iCardNum)
	{
		if (findABombBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			return MT_FL_CARD_TYPE_BOMB;
		}
		else if (findATripleBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			return MT_FL_CARD_TYPE_TRIPLE_ONE;
		}
	}
	/// 单顺, 三带二
	else if (5 == iCardNum)
	{
		if (findASingleProgression(m_pkCardGet, m_pkCardTmp))
		{
			return MT_FL_CARD_TYPE_SINGLE_PROGRESSION;
		}
		else if (findATripleBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, m_pkCardGet);
			if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
			{
				return MT_FL_CARD_TYPE_TRIPLE_TWO;
			}
		}
	}
	else
	{
		/// 单顺
		if (findASingleProgression(m_pkCardGet, m_pkCardTmp))
		{
			if (iCardNum == getCardNum(m_pkCardGet))
			{
				return MT_FL_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		/// 双顺
		if (findAPairProgression(m_pkCardGet, m_pkCardTmp))
		{
			if (iCardNum == getCardNum(m_pkCardGet))
			{
				return MT_FL_CARD_TYPE_PAIR_PROGRESSION;
			}
		}
		/// 三顺
		if (findATripleProgression(m_pkCardGet, m_pkCardTmp))
		{
			if (iCardNum == getCardNum(m_pkCardGet))
			{
				return MT_FL_CARD_TYPE_TRIPLE_PROGRESSION;
			}
		}
		/// 四带二
		if (findABombBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			takeAwayCard(m_pkCardTmp, m_pkCardGet);
			/// 四带两个对子
			if (4 == getCardNum(m_pkCardTmp))
			{
				if (!findABombBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
				{
					if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
					{
						takeAwayCard(m_pkCardTmp, m_pkCardGet);
						if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
						{
							return MT_FL_CARD_TYPE_FOUR_TWO;
						}
					}
				}
			}
			/// 四带两个单牌
			else if (2 == getCardNum(m_pkCardTmp))
			{
				return MT_FL_CARD_TYPE_FOUR_TWO;
			}
		}
		/// 飞机
		if (findATripleProgression(m_pkCardGet, m_pkCardTmp))
		{
			int		iGetCardNum	= 0;
			int		iTripleNum	= getTripleNum(m_pkCardGet);
			takeAwayCard(m_pkCardTmp, m_pkCardGet);
			/// 带对子
			while (1)
			{
				cout<<"**********************"<<endl;
				if (findAPairBiger(m_pkCardGet, m_pkCardTmp, kCardFind))  //有对子 那么就是带对子了  不会是带单的了
				{
					iGetCardNum		+= 2;                  //不为零 即证明找到了对子
					takeAwayCard(m_pkCardTmp, m_pkCardGet);
					if (iCardNum == iGetCardNum + iTripleNum * 3 &&
						iGetCardNum == iTripleNum*2)
					{
						return MT_FL_CARD_TYPE_AIRPLANE;
					}
				}
				else
				{
					if (iGetCardNum)  //一开始有对子 但是后面没有对子了。。。所以不是类型
					{
						return MT_FL_CARD_TYPE_PASS;
					}
					break;
				}
			}
			/// 带单牌
			while (1)
			{
				
				if (findASingleBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
				{
					iGetCardNum		+= 1;
					takeAwayCard(m_pkCardTmp, m_pkCardGet);
					if (iCardNum == iGetCardNum + iTripleNum * 3 &&
						iGetCardNum == iTripleNum)
					{
						return MT_FL_CARD_TYPE_AIRPLANE;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return MT_FL_CARD_TYPE_PASS;
}

mteCardType mtCardAI::getCardSmallest( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	mtCardInfo		kCardFind;
	mtCardInfo		kCardSmallest	= pkCardSrc[0];
	mteCardType		eRet			= MT_FL_CARD_TYPE_PASS;
	int				iFindNum;

	pkCardDst[0].cCardValue		= MTFL_NUM_CARD_TERMINAL;
	copyCard(m_pkCardTmp, pkCardSrc);

	/// 火箭
	if (findRocket(pkCardDst, m_pkCardTmp))
	{
		if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
		{
			return MT_FL_CARD_TYPE_ROCKET;
		}
		takeAwayCard(m_pkCardTmp, pkCardDst);
	}
	/// 炸弹
	iFindNum					= 0;
	kCardFind.cCardValue		= 0;
	while (1)
	{
		if (findABombBiger(m_pkCardGet, m_pkCardTmp, kCardFind))
		{
			if (0 == iFindNum)
			{
				copyCard(pkCardDst, m_pkCardGet);
				eRet	= MT_FL_CARD_TYPE_BOMB;
				iFindNum ++;
			}
			kCardFind.cCardValue		= m_pkCardGet[0].cCardValue;
			takeAwayCard(m_pkCardTmp, m_pkCardGet);
			kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
		}
		else
		{
			break;
		}
	}
	/// 三顺
	while (1)
	{
		if (findATripleProgression(pkCardDst, m_pkCardTmp))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			else
			{
				takeAwayCard(m_pkCardTmp, pkCardDst);
				kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
			}
		}
		else
		{
			break;
		}
	}
	/// 单顺
	while (1)
	{
		if (findASingleProgression(pkCardDst, m_pkCardTmp))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_SINGLE_PROGRESSION;
			}
			else
			{
				takeAwayCard(m_pkCardTmp, pkCardDst);
				kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
			}
		}
		else
		{
			break;
		}
	}
	/// 三条
	kCardFind.cCardValue		= 0;
	while (1)
	{
		if (findATripleBiger(pkCardDst, m_pkCardTmp, kCardFind))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_TRIPLE;
			}
			else
			{
				takeAwayCard(m_pkCardTmp, pkCardDst);
				kCardFind.cCardValue		= pkCardDst[0].cCardValue;
				kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
			}
		}
		else
		{
			break;
		}
	}
	/// 双顺
	while (1)
	{
		if (findAPairProgression(pkCardDst, m_pkCardTmp))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_PAIR_PROGRESSION;
			}
			else
			{
				takeAwayCard(m_pkCardTmp, pkCardDst);
				kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
			}
		}
		else
		{
			break;
		}
	}
	/// 对子
	kCardFind.cCardValue		= 0;
	while (1)
	{
		if (findAPairBiger(pkCardDst, m_pkCardTmp, kCardFind))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_PAIR;
			}
			kCardFind.cCardValue	= pkCardDst[0].cCardValue;
			takeAwayCard(m_pkCardTmp, pkCardDst);
			kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
		}
		else
		{
			break;
		}
	}
	/// 单牌
	iFindNum					= 0;
	kCardFind.cCardValue		= 0;
	while (1)
	{
		if (findASingleBiger(pkCardDst, m_pkCardTmp, kCardFind))
		{
			if (kCardSmallest.cCardValue == pkCardDst[0].cCardValue)
			{
				return MT_FL_CARD_TYPE_SINGLE;
			}
			takeAwayCard(m_pkCardTmp, pkCardDst);
			kCardSmallest.cCardValue	= m_pkCardTmp[0].cCardValue;
		}
		else
		{
			break;
		}
	}

	return eRet;
}

int mtCardAI::findASpecifyCardOfValueType( mtCardInfo* pkCardDst, mtCardInfo* pkCardFind )
{
	int		iIdx	= 0;
	while (MTFL_NUM_CARD_TERMINAL != pkCardDst[iIdx].cCardValue)
	{
		if (pkCardDst[iIdx].cCardValue == pkCardFind->cCardValue &&
			pkCardDst[iIdx].cCardType == pkCardFind->cCardType)
		{
			return 1;
		}
		iIdx ++;
	}

	return 0;
}

int mtCardAI::sortCard( mtCardInfo* pkCardSrc )
{
	/// 从小到大排列
	int			iIdx;
	int			iIdxIn;
	mtCardInfo	kCardTmp;

	for (iIdx = 0; MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdx].cCardValue; iIdx++)
	{
		for (iIdxIn = 1; MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn].cCardValue && 
			MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn - 1].cCardValue && 
			MTFL_NUM_CARD_TERMINAL != pkCardSrc[iIdxIn + iIdx].cCardValue; iIdxIn++)
		{
			if ((pkCardSrc[iIdxIn - 1].cCardValue) > (pkCardSrc[iIdxIn].cCardValue))
			{
				memcpy(&kCardTmp, pkCardSrc + iIdxIn - 1, sizeof(mtCardInfo));
				memcpy(pkCardSrc + iIdxIn - 1, pkCardSrc + iIdxIn, sizeof(mtCardInfo));
				memcpy(pkCardSrc + iIdxIn, &kCardTmp, sizeof(mtCardInfo));
			}
		}
	}

	return 1;
}

mteCardType mtCardAI::testTakeoutCard( mtCardInfo* pkCardSrc, mtCardInfo kMinCardTakeout, mteCardType eCardType )
{
	if (MT_FL_CARD_TYPE_ROCKET == eCardType)
	{
		return MT_FL_CARD_TYPE_PASS;
	}
	mteCardType		eCardTypeTakeout	= getTypeFromCard(pkCardSrc);
	if (MT_FL_CARD_TYPE_PASS == eCardType)
	{
		sortCard(pkCardSrc);
		return eCardTypeTakeout;
	}
	if (MT_FL_CARD_TYPE_ROCKET == eCardTypeTakeout || MT_FL_CARD_TYPE_PASS == eCardTypeTakeout)
	{
		return eCardTypeTakeout;
	}
	if (MT_FL_CARD_TYPE_BOMB != eCardType)
	{
		if (MT_FL_CARD_TYPE_BOMB == eCardTypeTakeout)
		{
			return MT_FL_CARD_TYPE_BOMB;
		}
		else
		{
			if (eCardTypeTakeout == eCardType)	
			{
				sortCard(pkCardSrc);
				if (pkCardSrc[0].cCardValue > kMinCardTakeout.cCardValue)
				{
					return eCardTypeTakeout;
				}
			}
		}
	}
	else
	{
		if (MT_FL_CARD_TYPE_BOMB == eCardTypeTakeout)
		{
			if (pkCardSrc[0].cCardValue > kMinCardTakeout.cCardValue)
			{
				return eCardTypeTakeout;
			}
		}
	}

	return MT_FL_CARD_TYPE_PASS;
}

mteCardType mtCardAI::getCardToTakeout( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc )
{
	mteCardType		eCardTypeGet	= MT_FL_CARD_TYPE_PASS;
	mtCardInfo		kCardFind;

	eCardTypeGet	= getCardSmallest(pkCardDst, pkCardSrc);
	copyCard(m_pkCardTmpPublic, pkCardSrc);
	takeAwayCard(m_pkCardTmpPublic, pkCardDst);

	if (MT_FL_CARD_TYPE_TRIPLE == eCardTypeGet ||
		MT_FL_CARD_TYPE_TRIPLE_PROGRESSION == eCardTypeGet)
	{
		int		iTripleNum		= getTripleNum(pkCardDst);
		int		iFindCardNum	= 0;
		/// 带单
		kCardFind.cCardValue	= 0;
		while (iFindCardNum < iTripleNum)
		{
			if (getCardFromSpecifyType(pkCardDst + iTripleNum * 3 + iFindCardNum, m_pkCardTmpPublic, MT_FL_CARD_TYPE_SINGLE, &kCardFind))
			{
				takeAwayCard(m_pkCardTmpPublic, pkCardDst + iTripleNum * 3 + iFindCardNum);
				kCardFind.cCardValue	= pkCardDst[iTripleNum * 3 + iFindCardNum].cCardValue;
				iFindCardNum ++;
			}
			else
			{
				break;
			}
		}
		if (iFindCardNum == iTripleNum)
		{
			if (MT_FL_CARD_TYPE_TRIPLE == eCardTypeGet)
			{
				return MT_FL_CARD_TYPE_TRIPLE_ONE;
			}
			else
			{
				return MT_FL_CARD_TYPE_AIRPLANE;
			}
		}
		/// 带双
		iFindCardNum			= 0;
		kCardFind.cCardValue	= 0;
		while (iFindCardNum < iTripleNum)
		{
			if (getCardFromSpecifyType(pkCardDst + iTripleNum * 3 + iFindCardNum * 2, m_pkCardTmpPublic, MT_FL_CARD_TYPE_PAIR, &kCardFind))
			{
				takeAwayCard(m_pkCardTmpPublic, pkCardDst + iTripleNum * 3 + iFindCardNum * 2);
				kCardFind.cCardValue	= pkCardDst[iTripleNum * 3 + iFindCardNum * 2].cCardValue;
				iFindCardNum ++;
			}
			else
			{
				break;
			}
		}
		if (iFindCardNum == iTripleNum)
		{
			if (MT_FL_CARD_TYPE_TRIPLE == eCardTypeGet)
			{
				return MT_FL_CARD_TYPE_TRIPLE_TWO;
			}
			else
			{
				return MT_FL_CARD_TYPE_AIRPLANE;
			}
		}
		pkCardDst[iTripleNum * 3].cCardValue	= MTFL_NUM_CARD_TERMINAL;
	}

	return eCardTypeGet;
}

mteCardType mtCardAI::getCardFromType( mtCardInfo* pkCardDst, mtCardInfo* pkCardSrc, mteCardType eCardType, mtCardInfo* pkCardTakeout, int iUnglue )
{
	if (MT_FL_CARD_TYPE_ROCKET == eCardType)  //如果是 火箭了 就直接pass
	{
		return MT_FL_CARD_TYPE_PASS;
	}
	mtCardInfo			kCardFind;
	int					iOtherCardsType;	/// 1:带单 2:带双
	int					iFindCardNum;
	kCardFind.cCardValue	= 0;
	copyCard(m_pkCardTmpPublic, pkCardSrc);
	/// 复杂类型
	/// 三带一,三带二
	if (MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType ||
		MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
	{
		if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, MT_FL_CARD_TYPE_TRIPLE, pkCardTakeout))
		{
			if (MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType)
			{
				if (getCardFromSpecifyType(pkCardDst + 3, m_pkCardTmpPublic, MT_FL_CARD_TYPE_SINGLE, &kCardFind))
				{
					return MT_FL_CARD_TYPE_TRIPLE_ONE;
				}
			}
			else if (MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
			{
				if (getCardFromSpecifyType(pkCardDst + 3, m_pkCardTmpPublic, MT_FL_CARD_TYPE_PAIR, &kCardFind))
				{
					return MT_FL_CARD_TYPE_TRIPLE_TWO;
				}
			}
		}
	}
	/// 飞机
	else if (MT_FL_CARD_TYPE_AIRPLANE == eCardType)
	{
		if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, MT_FL_CARD_TYPE_TRIPLE_PROGRESSION, pkCardTakeout))
		{
			int		iTripleNum			= getTripleNum(pkCardTakeout);
			iFindCardNum				= 0;
			iOtherCardsType				= (getCardNum(pkCardTakeout) - iTripleNum * 3) / iTripleNum;

			if (1 == iOtherCardsType)
			{
				while (iFindCardNum < iTripleNum)
				{
					if (!getCardFromSpecifyType(pkCardDst + iTripleNum * 3 + iFindCardNum, m_pkCardTmpPublic, MT_FL_CARD_TYPE_SINGLE, &kCardFind))
					{
						break;
					}
					takeAwayCard(m_pkCardTmpPublic, pkCardDst + iTripleNum * 3 + iFindCardNum);
					iFindCardNum ++;
				}
				if (iFindCardNum == iTripleNum)
				{
					return MT_FL_CARD_TYPE_AIRPLANE;
				}
			}
			else if (2 == iOtherCardsType)
			{
				while (iFindCardNum < iTripleNum)
				{
					if (!getCardFromSpecifyType(pkCardDst + iTripleNum * 3 + iFindCardNum * 2, m_pkCardTmpPublic, MT_FL_CARD_TYPE_PAIR, &kCardFind))
					{
						break;
					}
					takeAwayCard(m_pkCardTmpPublic, pkCardDst + iTripleNum * 3 + iFindCardNum * 2);
					iFindCardNum ++;
				}
				if (iFindCardNum == iTripleNum)
				{
					return MT_FL_CARD_TYPE_AIRPLANE;
				}
			}
		}
	}
	/// 四带二
	else if (MT_FL_CARD_TYPE_FOUR_TWO == eCardType)
	{
		if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, MT_FL_CARD_TYPE_BOMB, pkCardTakeout))
		{
			iOtherCardsType		= (getCardNum(pkCardTakeout) - 4) / 2;
			iFindCardNum		= 0;
			if (1 == iOtherCardsType)
			{
				while (iFindCardNum < 2)
				{
					if (!getCardFromSpecifyType(pkCardDst + 4 + iFindCardNum, m_pkCardTmpPublic, MT_FL_CARD_TYPE_SINGLE, pkCardTakeout))
					{
						break;
					}
					takeAwayCard(m_pkCardTmpPublic, pkCardDst + 4 + iFindCardNum);
					iFindCardNum ++;
				}
				if (2 == iFindCardNum)
				{
					return MT_FL_CARD_TYPE_FOUR_TWO;
				}
			}
			else if (2 == iOtherCardsType)
			{
				while (iFindCardNum < 2)
				{
					if (!getCardFromSpecifyType(pkCardDst + 4 + iFindCardNum * 2, m_pkCardTmpPublic, MT_FL_CARD_TYPE_PAIR, pkCardTakeout))
					{
						break;
					}
					takeAwayCard(m_pkCardTmpPublic, pkCardDst + 4 + iFindCardNum * 2);
					iFindCardNum ++;
				}
				if (2 == iFindCardNum)
				{
					return MT_FL_CARD_TYPE_FOUR_TWO;
				}
			}
		}
	}
	/// 简单类型
	else
	{
		if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, eCardType, pkCardTakeout))
		{
			return eCardType;
		}
	}
	/// 没找到大的牌
	/// 别人不是炸弹,找炸弹
	copyCard(m_pkCardTmpPublic, pkCardSrc);
	if (MT_FL_CARD_TYPE_BOMB != eCardType)
	{
		if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, MT_FL_CARD_TYPE_BOMB, &kCardFind))
		{
			return MT_FL_CARD_TYPE_BOMB;
		}
	}
	/// 找火箭
	if (getCardFromSpecifyType(pkCardDst, m_pkCardTmpPublic, MT_FL_CARD_TYPE_ROCKET, &kCardFind))
	{
		return MT_FL_CARD_TYPE_ROCKET;
	}
	/// 拆牌打
	if (iUnglue)
	{
		mteCardType	eCardTypeGet = getCardDirectFromType(pkCardDst, m_pkCardTmpPublic, eCardType, pkCardTakeout);
		if (eCardType == eCardTypeGet)
		{
			return eCardType;
		}
		else if (MT_FL_CARD_TYPE_TRIPLE == eCardTypeGet)
		{
			//add by wsd start  加条语句
			(pkCardDst + 3)->cCardValue = MTFL_NUM_CARD_TERMINAL;
			mtCardInfo  pkTempCard[21];
			for (int i = 0; i<21; i++)
			{
				pkTempCard[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
			}
			memcpy(pkTempCard,m_pkCardTmpPublic,sizeof(mtCardInfo)*21);
			takeAwayCard(pkTempCard,pkCardDst);  //把之前选择的要剪掉
			//add by wsd end
			if (MT_FL_CARD_TYPE_TRIPLE_ONE == eCardType)
			{
				//if (getCardFromSpecifyType(pkCardDst + 3, m_pkCardTmpPublic, MT_FL_CARD_TYPE_SINGLE, &kCardFind))  //以前的
				if (getCardFromSpecifyType(pkCardDst + 3, pkTempCard, MT_FL_CARD_TYPE_SINGLE, &kCardFind))   //change by wsd
				{
					return MT_FL_CARD_TYPE_TRIPLE_ONE;
				}
				else
				{
					
					//if (findASingleBiger(pkCardDst + 3, m_pkCardTmpPublic, kCardFind))  //以前的
					if (findASingleBiger(pkCardDst + 3, pkTempCard, kCardFind))           //change by wsd
					{
						return MT_FL_CARD_TYPE_TRIPLE_ONE;
					}
				}
			}
			else if (MT_FL_CARD_TYPE_TRIPLE_TWO == eCardType)
			{
				//if (getCardFromSpecifyType(pkCardDst + 3, m_pkCardTmpPublic, MT_FL_CARD_TYPE_PAIR, &kCardFind)) //以前的
				if (getCardFromSpecifyType(pkCardDst + 3, pkTempCard, MT_FL_CARD_TYPE_PAIR, &kCardFind))

				{
					return MT_FL_CARD_TYPE_TRIPLE_TWO;
				}
				else
				{
					//if (findAPairBiger(pkCardDst + 3, m_pkCardTmpPublic, kCardFind))  //原来的
					if (findAPairBiger(pkCardDst + 3, pkTempCard, kCardFind))    //change by wsd
					{
						return MT_FL_CARD_TYPE_TRIPLE_TWO;
					}
				}
			}
		}
	}

	pkCardDst->cCardValue	= MTFL_NUM_CARD_TERMINAL;
	return MT_FL_CARD_TYPE_PASS;
}

void  mtCardAI::dispalyCardInfo(mtCardInfo* pkCardSrc,FILE* pLogFile,int logFlag)
{
	int iDex = 0;
	int iNum = getCardNum(pkCardSrc);
	for (iDex = 0; iDex != iNum; iDex++ )
	{
		outPutSingleCard(pkCardSrc+iDex,pLogFile,logFlag);

	}
	Log(pLogFile,"\n");
}

void mtCardAI::outPutSingleCard(mtCardInfo *pkcard,FILE* pLogFile,int logFlag)
{	
	switch(pkcard->cCardType)
	{
	case 0://黑桃
		{
			if (pkcard->cCardValue < 9)
			{
				Log(pLogFile,"%c%d ",6,pkcard->cCardValue+2 );

			}
			else
			{
				switch (pkcard->cCardValue)
				{
				case 9:
					Log(pLogFile,"%c%c ",6,74 );
					break;
				case 10:
					Log(pLogFile,"%c%c ",6,81 );
					break;
				case 11:
					Log(pLogFile,"%c%c ",6,75 );
					break;
				case 12:
					Log(pLogFile,"%c%c ",6,65 );
					break;
				case 13:
					Log(pLogFile,"%c%c ",6,50 );
					break;
				default:
					break;

				}

			}

		}
		break;
	case 1://红桃
		{
			if (pkcard->cCardValue < 9)
			{
				Log(pLogFile,"%c%d ",3,pkcard->cCardValue+2 );

			}
			else
			{
				switch (pkcard->cCardValue)
				{
				case 9:
					Log(pLogFile,"%c%c ",3,74 );
					break;
				case 10:
					Log(pLogFile,"%c%c ",3,81 );
					break;
				case 11:
					Log(pLogFile,"%c%c ",3,75 );
					break;
				case 12:
					Log(pLogFile,"%c%c ",3,65 );
					break;
				case 13:
					Log(pLogFile,"%c%c ",3,50 );
					break;
				default:
					break;

				}

			}

		}
		break;
	case 2://梅花
		{
			if (pkcard->cCardValue < 9)
			{
				Log(pLogFile,"%c%d ",5,pkcard->cCardValue+2 );

			}
			else
			{
				switch (pkcard->cCardValue)
				{
				case 9:
					Log(pLogFile,"%c%c ",5,74 );
					break;
				case 10:
					Log(pLogFile,"%c%c ",5,81 );
					break;
				case 11:
					Log(pLogFile,"%c%c ",5,75 );
					break;
				case 12:
					Log(pLogFile,"%c%c ",5,65 );
					break;
				case 13:
					Log(pLogFile,"%c%c ",5,50 );
					break;
				default:
					break;

				}

			}

		}
		break;
	case 3://方片
		{
			if (pkcard->cCardValue < 9)
			{
				Log(pLogFile,"%c%d ",4,pkcard->cCardValue+2 );

			}
			else
			{
				switch (pkcard->cCardValue)
				{
				case 9:
					Log(pLogFile,"%c%c ",4,74 );
					break;
				case 10:
					Log(pLogFile,"%c%c ",4,81 );
					break;
				case 11:
					Log(pLogFile,"%c%c ",4,75 );
					break;
				case 12:
					Log(pLogFile,"%c%c ",4,65 );
					break;
				case 13:
					Log(pLogFile,"%c%c ",4,50 );
					break;
				default:
					break;

				}

			}

		}
		break;
	case 4:
		{
			if (pkcard->cCardValue == 14)
			{
				Log(pLogFile,"小王 ");
			}
			else
			{
				Log(pLogFile,"大王 ");
			}
		}
		break;
	default:
		break;
	}

}

void mtCardAI::findTheMostSomeCard(mtCardInfo* pkCardSrc,char *pcCardFindValue)
{
	int iNum = getCardNum(pkCardSrc);
	int iCount[21] = {0};
	int max = 0;
	int i = 0;
	for ( i = 0; i != iNum; iCount[(pkCardSrc+i)->cCardValue]++,i++)
	{
		if (iCount[(pkCardSrc+i)->cCardValue] > iCount[(pkCardSrc+max)->cCardValue])
		{
			max = i;
		}
	}
	*pcCardFindValue = (pkCardSrc+max)->cCardValue;
}

void mtCardAI::findTheMaxCard(mtCardInfo* pkCardSrc,char *pcCardFindValue)
{
	*pcCardFindValue = '\0';
	for (int i = 0; (pkCardSrc+i)->cCardValue != MTFL_NUM_CARD_TERMINAL; i++ )
	{
		if ((pkCardSrc+i)->cCardValue > *pcCardFindValue)
		{
			*pcCardFindValue = (pkCardSrc+i)->cCardValue;
		}

	}
}

mteCardType mtCardAI::getCardOut(mtCardInfo *pkHoldCard,mtCardInfo *pkCardOut,mteCardType eCardType,mtCardInfo *pkSelectedCard)
{
	if (pkCardOut == NULL) //第一次出牌
	{
		return getCardToTakeout(pkSelectedCard, pkHoldCard);

	}
	else
	{
		return getCardFromType( pkSelectedCard, pkHoldCard, eCardType, pkCardOut, 1);

	}
}

bool mtCardAI::judgeCardOut(mtCardInfo *pkSelectedCard,mtCardInfo *pkCardOut,mteCardType eCardType)
{ 
	int iNum = getCardNum(pkSelectedCard);
	mteCardType eSlectCardType = getTypeFromCard(pkSelectedCard);
	if (eSlectCardType == MT_FL_CARD_TYPE_PASS )              //不能成为类型
	{
		return false;
	}
	if(pkCardOut == NULL)  //如果是主动出牌 
	{
		return true;
	}
	//压牌
	int i = 0;
	mtCardInfo pkCardDst[21];
	for (i = 0; i != 21; i++)
	{
		pkCardDst[i].cCardValue = MTFL_NUM_CARD_TERMINAL;
	}
	if (getCardFromType(pkCardDst,pkSelectedCard,eCardType,pkCardOut,1) == MT_FL_CARD_TYPE_PASS) //压不起
	{
		return false;
	}
	else if (memcmp(pkCardDst,pkSelectedCard,iNum*sizeof(mtCardInfo)) != 0)        //能压住，但是两个牌如果不相等 则选的牌也不可以
	{
		return false;
	}
	return true;
	
}

void mtCardAI::cardConvert(char *cCardSrc,mtCardInfo *pkCardDst)
{
	for(int index = 0;cCardSrc[index] != MTFL_NUM_CARD_TERMINAL;index ++)
	{
		if(cCardSrc[index] == 61)
		{
			pkCardDst[index].cCardType = 4;
			pkCardDst[index].cCardValue = 14;
			continue;
		}
		if(cCardSrc[index] == 62)
		{
			pkCardDst[index].cCardType = 4;
			pkCardDst[index].cCardValue = 15;
			continue;
		}

		pkCardDst[index].cCardType = (cCardSrc[index]&0xF0)>>4;
		pkCardDst[index].cCardValue = (cCardSrc[index]&0x0f) + 1;
	}
}

void mtCardAI::cardConvert(mtCardInfo *pkCardSrc,char *cCardDst)
{
	for(int index = 0;pkCardSrc[index].cCardValue != MTFL_NUM_CARD_TERMINAL;index ++)
	{
		if(pkCardSrc[index].cCardValue == 14)
		{
			cCardDst[index] = 61;
			continue;
		}
		if(pkCardSrc[index].cCardValue == 15)
		{
			cCardDst[index] = 62;
			continue;
		} 
		char type = pkCardSrc[index].cCardType<<4;
		char value = pkCardSrc[index].cCardValue -1;
		cCardDst[index ] = type + value;
	}
}
