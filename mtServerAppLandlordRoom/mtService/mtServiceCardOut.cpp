#include "mtServiceCardOut.h"
#include "mtQueueDesk.h"
#include "mtQueueHall.h"
#include "mtPrint.h"

mtServiceCardOut::mtServiceCardOut():
	m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL)
{

}

mtServiceCardOut::~mtServiceCardOut()
{

}

int mtServiceCardOut::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_hEventManagerAi       = pkDataInit->hEventManagerAi;
	return	1;
}

int mtServiceCardOut::run( DataRun* pkDataRun )
{
	DataRead*			   pkDataRead          = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode = pkDataRun->pkQueueUserDataNode;
	mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	//mtPrint(pkDataRead);
	if (NULL == pkQueueDeskDataNode)
	{
	 	return 1;
	}

	mtQueueHall::DataRoom*	pkQueueHallDataRoom = m_pkQueueMgr->getQueueHall()->getRoom(pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING != pkQueueDeskDataNode->lDeskExit
	 || mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING != pkQueueUserDataNode->lUserExit)
	{
		LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
		return 1;
	}
	MT_DEBUG_PTCL("\nUser card out![%d]",pkQueueUserDataNode->lUserId);
	char pcCardsCurTmp[80];
	char pcCardsOutTmp[80];
	memset(pcCardsCurTmp, 0, sizeof(pcCardsCurTmp));
	memset(pcCardsOutTmp, 0, sizeof(pcCardsOutTmp));
	getCardsInformation(pkQueueDeskDataNode->pcCardsCur, pcCardsCurTmp);
	getCardsInformation(pkDataRead->pcCard, pcCardsOutTmp);
	pcCardsCurTmp[79] = 0;
	pcCardsOutTmp[79] = 0;


	pkQueueDeskDataNode->ullTimeEnterRun = GetTickCount();
	pkQueueUserDataNode->ullTimeEnterRun = GetTickCount();
	/// 更新当前桌子打出的牌面信息
	if (MT_CARD_TYPE_PASS != pkDataRead->lCardType)
	{
		memcpy(pkQueueDeskDataNode->pcCardsCur, pkDataRead->pcCard, sizeof(pkDataRead->pcCard));
		pkQueueDeskDataNode->lCardType          = pkDataRead->lCardType;
		pkQueueDeskDataNode->lQueueUserDeskIdx  = pkQueueUserDataNode->lUserDeskIndex;

	    /// 处理该用户经打出牌信息
		updateQueueDeskCardsOut(pkQueueDeskDataNode);

		/// 打牌成功，更新该用户桌子上对应的牌信息
		updateCardsFace(pkQueueDeskDataNode->pcCardsTotal[pkQueueUserDataNode->lUserDeskIndex], pkDataRead->pcCard);
		pkQueueDeskDataNode->plPlayerLots[pkQueueUserDataNode->lUserDeskIndex]++;
	}
	else
	{
		memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
	}

	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
	kDataWrite.lCardType         = pkDataRead->lCardType;
	memcpy(kDataWrite.pcCard, pkDataRead->pcCard, sizeof(pkDataRead->pcCard));

	int		iUserIdx;
	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
	{
		//  响应打牌信息给所有真实用户
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
		&& mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("cardout105");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
			//	mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
			}
		}
	}

	pkQueueDeskDataNode->lUserDeskIdxCardOutNext = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
	if (0 < getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[pkQueueUserDataNode->lUserDeskIndex]))
	{
		/// 获得下家用户在桌子上的位置
		int iQueueUserNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
		
		//  下家是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextIdx])
		{
			int iContinue;
			ULONGLONG ullStartTime = GetTickCount();
			playOutCardAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueUserNextIdx, &iContinue, &ullStartTime);

			/// 获得ai用户的下家在桌子上的位置
			int iQueueUserNextNextIdx = getQueueUserNextQueueUser(iQueueUserNextIdx);

			//  ai下家还是是ai用户
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextNextIdx]
			    && 1 == iContinue)
			{
				playOutCardAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueUserNextNextIdx, &iContinue, &ullStartTime);
			}
		}
	}

	pkQueueDeskDataNode->lCardDeal = 0;
	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	return 1;
}

int mtServiceCardOut::exit()
{
	return	1;
}

int mtServiceCardOut::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceCardOut::updateQueueDeskCardsOut(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	/// 加到桌子上该用户已经打出牌里
	char* pcQueueDeskCardsOutStart = pkQueueDeskDataNode->pcCardsOutTotal[pkQueueDeskDataNode->lQueueUserDeskIdx];
	while (MT_CARD_TERMINATE != *pcQueueDeskCardsOutStart)
	{
		pcQueueDeskCardsOutStart++;
	}

	/// 获得当前打出牌的数量
	int iCardsOutNum = 0;
	char* pcCurCardsOutStart = pkQueueDeskDataNode->pcCardsCur;
	while (MT_CARD_TERMINATE != *pcCurCardsOutStart)
	{
		iCardsOutNum++;
		pcCurCardsOutStart++;
	}

	memcpy(pcQueueDeskCardsOutStart, pkQueueDeskDataNode->pcCardsCur, iCardsOutNum);

	/// 判断是不是炸弹，增加炸弹数
	if (MT_CARD_TYPE_BOMB == pkQueueDeskDataNode->lCardType)
	{
		pkQueueDeskDataNode->plBomb[pkQueueDeskDataNode->lQueueUserDeskIdx]++;
	}

	/// 判断是不是火箭，增加火箭数
	if (MT_CARD_TYPE_ROCKET == pkQueueDeskDataNode->lCardType)
	{
		pkQueueDeskDataNode->lRocket  = 1;
	}

	return 1;
}

int mtServiceCardOut::getCardsInformation(char pcCard[], char * pcStr)
{
	int iLen = 0;
	int iIdx = 0;
	char pcTmpStr[24];
	while (MT_CARD_TERMINATE != pcCard[iIdx] && 23 > iIdx)
	{
		sprintf_s(pcTmpStr, " %d", (pcCard[iIdx] & 0x0f) + 3);
		strcpy(pcStr + iLen, pcTmpStr);
		iLen += strlen(pcTmpStr);
		iIdx++;
	}

	return 1;
}

int mtServiceCardOut::getQueueUserLeftCards(char* pcCard)
{
	int iCardNum = 0;
	while(pcCard[iCardNum] != MT_CARD_TERMINATE && 21 > iCardNum)
	{
		iCardNum++;
	}

	return iCardNum;
}

int mtServiceCardOut::playOutCardAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserAiIdx, int* piContinue, ULONGLONG* pullStartTime)
{
	printf("\n\n*************************************888");

	/// 获得桌子上的真实人员列表
	DataWrite kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lUserId           = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserAiIdx]->lUserId;
	kDataWrite.lServiceType      = lServiceType;
	kDataWrite.lCardType         = pkQueueDeskDataNode->lCardType;
	memset(kDataWrite.pcCard, MT_CARD_TERMINATE, sizeof(kDataWrite.pcCard));

	*piContinue = 1;
	memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));

	/// 如果当前没有人打住你的牌，ai继续选择出牌
	if (pkQueueDeskDataNode->lQueueUserDeskIdx == iQueueUserAiIdx)
	{
		MT_DEBUG(4, "User(UserId=%d) getPlayOutCardFirst  100......\n", kDataWrite.lUserId);
		kDataWrite.lCardType = getPlayOutCardFirst(pkQueueDeskDataNode, iQueueUserAiIdx);
	}
	else
	{
		MT_DEBUG(4, "User(UserId=%d) getPlayOutCard  101......\n", kDataWrite.lUserId);
		getPlayOutCard(pkQueueDeskDataNode, iQueueUserAiIdx, &(kDataWrite.lCardType));
	}

	pkQueueDeskDataNode->ullTimeEnterRun = GetTickCount();

	int curOutCardUserDeskIdx = pkQueueDeskDataNode->lQueueUserDeskIdx;
	int curOutCardUserId = curOutCardUserDeskIdx != -1 ? pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lQueueUserDeskIdx]->lUserId : 0;


	char pcCardsCurTmp[80];
	char pcCardsOutTmp[80];
	memset(pcCardsCurTmp, 0, sizeof(pcCardsCurTmp));
	memset(pcCardsOutTmp, 0, sizeof(pcCardsOutTmp));
	getCardsInformation(pkQueueDeskDataNode->pcCardsCur, pcCardsCurTmp);
	getCardsInformation(pkQueueDeskDataNode->pcCardsOutAi, pcCardsOutTmp);
	pcCardsCurTmp[79] = 0;
	pcCardsOutTmp[79] = 0;

	if (MT_CARD_TERMINATE == pkQueueDeskDataNode->pcCardsOutAi[0])
	{
		kDataWrite.lCardType  = MT_CARD_TYPE_PASS;
	}
	else 
	{
		memcpy(kDataWrite.pcCard, pkQueueDeskDataNode->pcCardsOutAi, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
	}

	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	int iQueueUserDataNodeIdx;
	ULONGLONG ullStartTimeTmp = *pullStartTime;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		/// 发送‘不出牌’ 消息
		//  当前真实用户
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]
		 && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("cardout285");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;
				memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
			//	mtPrint(&kDataWrite);
				/// 服务器延迟处理ai出牌
				mtQueueSendPackAi::DataNode* pkDataNodeAi = (mtQueueSendPackAi::DataNode*)m_pkQueueMgr->m_pkQueueSendPackAi->popInitDataNode();
				if (pkDataNodeAi)
				{
					int iInsertHead = 0;
					pkDataNodeAi->ullTimeSend           = *pullStartTime + MT_SERVICE_AI_CARD_OUT_TIME;
					ullStartTimeTmp                     = pkDataNodeAi->ullTimeSend;
					pkDataNodeAi->iType                 = mtQueueSendPackAi::E_SEND_TYPE_SEND;
					pkDataNodeAi->pkQueueUserDataNode   = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx];
					pkDataNodeAi->pkQueuePacketDataNode = pkQueuePacketDataNode;
					m_pkQueueMgr->m_pkListManagerAi->push(pkDataNodeAi, &iInsertHead);
					if (iInsertHead)
					{
						SetEvent(m_hEventManagerAi);
					}
				}
			}
		}
	}

	*pullStartTime = ullStartTimeTmp;
	if (MT_CARD_TYPE_PASS != kDataWrite.lCardType)
	{
		/// 打牌成功，更新ai手中的牌信息
		updateCardsFace(pkQueueDeskDataNode->pcCardsTotal[iQueueUserAiIdx], kDataWrite.pcCard);
		if (MT_CARD_TERMINATE == pkQueueDeskDataNode->pcCardsTotal[iQueueUserAiIdx][0])
		{
			*piContinue = 0;
		}

		/// 更新当前桌子打出的牌面信息
		memcpy(pkQueueDeskDataNode->pcCardsCur, kDataWrite.pcCard, sizeof(kDataWrite.pcCard));
		pkQueueDeskDataNode->lQueueUserDeskIdx    = iQueueUserAiIdx;
		pkQueueDeskDataNode->lCardType            = kDataWrite.lCardType;

		/// 处理该用户经打出牌信息
		updateQueueDeskCardsOut(pkQueueDeskDataNode);
	}

	pkQueueDeskDataNode->lUserDeskIdxCardOutNext = getQueueUserNextQueueUser(iQueueUserAiIdx);
	/// 判断AI是不是已经赢了

	return 1;
}

int mtServiceCardOut::getPlayOutCardType(char pcOutCard[], long* plCardType)
{
	int	iCardCount = 0;
	while(MT_CARD_TERMINATE != pcOutCard[iCardCount])
	{
		iCardCount++;
	}

	if (iCardCount == 2)
	{
		if (((pcOutCard[0] & 0x0f) == 13 && (pcOutCard[1] & 0x0f) == 14) 
			||((pcOutCard[1] & 0x0f) == 13 && (pcOutCard[0] & 0x0f) == 14) )
		{
			*plCardType  = MT_CARD_TYPE_ROCKET;
		}
	}
	else if (iCardCount == 4)
	{
		if ((pcOutCard[0] & 0x0f) == (pcOutCard[1] & 0x0f) 
			&& (pcOutCard[0] & 0x0f) == (pcOutCard[2] & 0x0f) 
			&& (pcOutCard[0] & 0x0f) == (pcOutCard[3] & 0x0f))
		{
			*plCardType  = MT_CARD_TYPE_BOMB;
		}
	}

	return 1;
}

int mtServiceCardOut::getPlayOutCardFirst(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx)
{
	/// 如果我是地主
	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[iQueueUserIdx])
	{
		/// 直接出牌
		return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi);
	}

	/// 自己手里是不是一手牌(可以先走)
	if (-1 != IsSeriesCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi))
	{
		/// 直接出牌
		memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
		return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi);
	}

	/// 如果下家是对家，获得对家手中的剩余牌数(小心别送走对家)
	int iQueueUserNextIdx = getQueueUserNextQueueUser(iQueueUserIdx);
	if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] != pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx])
	{
		/// 获得对家手中剩余牌数
		int iCardsEnemyNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserNextIdx]);

		/// 小于5张，出牌要小心(别把对家给送走了)
		if (5 > iCardsEnemyNum)
		{
			/// 获得自己手中剩余牌数
			int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

			/// 对家手中只有1张
			if (1 == iCardsEnemyNum)
			{
				/// 出单支以外的牌
				if (1 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					                             , pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TYPE_SINGLE, iCardsSelfNum);
				}
			}
			else if(2 == iCardsEnemyNum) /// 等于2张
			{
				/// 出对子以外的牌
				if (2 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					                             , pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TYPE_PAIR, iCardsSelfNum);
				}
			}
			else if(3 == iCardsEnemyNum) /// 等于3张
			{
				/// 出三支以外的牌
				if (3 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					                             , pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TYPE_TRPILE, iCardsSelfNum);
				}
			}
			else if(4 == iCardsEnemyNum) /// 等于4张
			{
				/// 出三带一以外的牌
				if (4 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					                             , pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TYPE_TRIPLE_ONE, iCardsSelfNum);
				}
			}
		}
	}
	
	/// 获得我对方自家手中剩余的牌数(方便送走自家)
	/// 获得自家在桌子上的位置
	int iQueueUserSameFamilyIdx = -1;
	if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] == pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx])
	{
		iQueueUserSameFamilyIdx = iQueueUserNextIdx;
	}
	else
	{
		iQueueUserSameFamilyIdx = getQueueUserNextQueueUser(iQueueUserNextIdx);
	}

	/// 获得对方自家手中剩余牌数
	int iCardsSameFamilyNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserSameFamilyIdx]);

	/// 大于等于5张，直接出牌
	/// 小于5张，可以进行送牌
	if (5 > iCardsSameFamilyNum)
	{
	    /// 等于1张
		if (1 == iCardsSameFamilyNum)
		{
			/// 出单支
			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_SINGLE;
			}
		}
		else if(2 == iCardsSameFamilyNum) /// 等于2张
		{
	        /// 出对子，没有对子出单支
			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_SINGLE;
			}
		}
	    else if(3 == iCardsSameFamilyNum) /// 等于3张
		{
	        /// 出三支，没有三支出对子，没有对子出单支
			if (-1 != getTripleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_TRPILE;
			}

			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_SINGLE;
			}
		}
		else if(4 == iCardsSameFamilyNum) /// 等于4张
		{
	        /// 出三带一，没有三支，出对子，没有对子出单支
			if (-1 != getTripleWithOne(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0, 4))
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}

			if (-1 != getTripleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_TRPILE;
			}

			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi, 0))
			{
				return MT_CARD_TYPE_SINGLE;
			}
		}
	}

	return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi);
}

int mtServiceCardOut::getPlayOutCard(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, long* plCardType)
{
	int iQueueUserNextIdx = getQueueUserNextQueueUser(iQueueUserIdx);
	/// 下家是对家
	if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] != pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx])
	{
		/// 下家手中剩余牌数
		int iUserNextCardsNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserNextIdx]);

		/// 当前打出的牌面的牌数
		int iCardsRecvNum     = getCardsNum(pkQueueDeskDataNode->pcCardsCur);
		/// 当前出牌用户，剩余的牌数
		int iCardsCurNum      = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[pkQueueDeskDataNode->lQueueUserDeskIdx]);

		/// 当前我的牌面的牌数
		int iCardsSelfNum     = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

		/// 如果当前牌面数和地主手里的剩余牌数相等(则要出大牌，别放走对家)
		if (iUserNextCardsNum == iCardsRecvNum)
		{
			/// 当前牌面是单支
			if (1 == iCardsRecvNum || 2 == iCardsRecvNum || 3 == iCardsRecvNum)
			{

				/// 当前已经打出牌的用户是自家,且其手中的牌小于3张
				if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] == pkQueueDeskDataNode->plPlayerType[pkQueueDeskDataNode->lQueueUserDeskIdx]
				   && iCardsCurNum < 3)
				{
					memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
				}
				else if(-1 != playOutMaxCardsByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi
					                      , pkQueueDeskDataNode->lCardType, iCardsSelfNum))
				{

					/// 当前收到的牌的主牌信息
					int iMainCardNumRecv = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur, iCardsRecvNum) & 0x0f);

					/// 当前自己打出的牌的主牌信息
					int iMainCardNumSelf = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsOutAi, iCardsRecvNum) & 0x0f);
					if (iMainCardNumSelf > iMainCardNumRecv && iMainCardNumRecv != 61)
					{
						return 1;
					}
				}
			}
		}
	}

	char pcCardsCurTmp[80];
	memset(pcCardsCurTmp, 0, sizeof(pcCardsCurTmp));
	getCardsInformation(pkQueueDeskDataNode->pcCardsCur, pcCardsCurTmp);
	pcCardsCurTmp[79] = 0;
	MT_DEBUG(4, "CurDeskIdx=%d, CurUserIdx=%d, CardsCur=%s, CardsCurType=%d\n", pkQueueDeskDataNode->lQueueUserDeskIdx
		    , iQueueUserIdx ,pcCardsCurTmp, pkQueueDeskDataNode->lCardType);
	/// 获得本局我要打出的牌信息
	memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));

	bool bSplitCard   =bSplitCards(pkQueueDeskDataNode,iQueueUserIdx) ;//是否要拆牌

	int iMainCardsIdx = getPlayOutCard(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur
		, pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkQueueDeskDataNode->pcCardsOutAi,bSplitCard);

	if (MT_CARD_TERMINATE == pkQueueDeskDataNode->pcCardsOutAi[0])
	{
		////MT_DEBUG(4, "User PlayOutCard  301......\n");
		memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
		*plCardType  = MT_CARD_TYPE_PASS;
		return 1;
	}

	getPlayOutCardType(pkQueueDeskDataNode->pcCardsOutAi, plCardType);
	/// 如果我是一手牌()
	/// 获得自家手中剩余牌数
	int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

	/// 如果手中牌全部可以打住(一手出完，赢了)
	if (iCardsSelfNum == getCardsNum(pkQueueDeskDataNode->pcCardsOutAi))
	{
		/// 正常出牌，赢了
	}
	else
	{
		/// 当前已经打出牌的用户是自家
		if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] == pkQueueDeskDataNode->plPlayerType[pkQueueDeskDataNode->lQueueUserDeskIdx])
		{
			//当前出牌用户手中的牌数
			int iCardCurNumber = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[pkQueueDeskDataNode->lQueueUserDeskIdx]);
			/// 当前打出的牌面的牌数
			int iCardsRecvNum     = getCardsNum(pkQueueDeskDataNode->pcCardsCur);

			//如果自家打出的牌和他手中的牌数一样或者手中只剩1,2张牌，我不出牌
			if(iCardCurNumber == iCardsRecvNum || iCardCurNumber == 1 || iCardCurNumber == 2)
			{
				memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
				*plCardType = MT_CARD_TYPE_PASS;
			}
			////MT_DEBUG(4, "User PlayOutCard  302......\n");
			/// 如果我的主牌是炸弹
			if (MT_CARD_TYPE_ROCKET == *plCardType || MT_CARD_TYPE_BOMB == *plCardType)
			{
				// 			/// 获得自家手中剩余牌数
				// 			int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);
				if (4 == iCardsSelfNum || 2 == iCardsSelfNum)//???主牌是火箭，手里有四张牌
				{
					/// 正常出牌，赢了
				}
				else 
				{
					/// 如果是火箭(双王)
					if (MT_CARD_TYPE_ROCKET == *plCardType)
					{
						if (3 == iCardsSelfNum)
						{
							/// 正常出牌，赢了
						}
						else
						{
							/// 我不出牌 pass
							memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
							*plCardType = MT_CARD_TYPE_PASS;
						}
					}
					else /// 我不出牌 pass
					{
						memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
						*plCardType = MT_CARD_TYPE_PASS;
					}
				}
			}
			else /// 如果我的主牌不是炸弹
			{
				/// 获得我的主牌信息
				int iMainCardsNumSelf = (pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx][iMainCardsIdx] & 0x0f);

				/// 获得当前牌面的主牌信息
				int iCardsRecvNum = getCardsNum(pkQueueDeskDataNode->pcCardsCur);
				int iMainCardNumRecv = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur, iCardsRecvNum) & 0x0f);

				// 			/// 获得自己手中剩余牌数
				// 			int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

				/// 如果自己手中牌数量和打出的牌数量一样
				if (iCardsRecvNum == iCardsSelfNum)
				{
					/// 正常出牌，赢了
				}
				else 
				{
					if (MT_CARD_TYPE_SINGLE == pkQueueDeskDataNode->lCardType)/// 单支
					{
						///  当前牌面主牌 >= A
						if (11 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
							*plCardType = MT_CARD_TYPE_PASS;
						}
						else ///  当前牌面主牌 < A
						{
							/// 我的主牌 <= A
							if (11 >= iMainCardsNumSelf)
							{
								/// 正常出牌
							}
							else /// 我的主牌 > A
							{
								/// 我不出牌 pass
								memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
								*plCardType = MT_CARD_TYPE_PASS;
							}
						}
					}
					else if(MT_CARD_TYPE_PAIR == pkQueueDeskDataNode->lCardType) /// 对子
					{
						/// 当前牌面主牌 >= K
						if (10 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
							*plCardType = MT_CARD_TYPE_PASS;
						}
						else /// 当前牌面主牌 < K
						{
							/// 我的主牌 < K
							if (10 > iMainCardsNumSelf)
							{
								/// 正常出牌
							}
							else /// 我的主牌 >= K
							{
								/// 我不出牌 pass
								memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
								*plCardType = MT_CARD_TYPE_PASS;
							}
						}
					}
					else if(MT_CARD_TYPE_TRPILE == pkQueueDeskDataNode->lCardType
						|| MT_CARD_TYPE_TRIPLE_ONE == pkQueueDeskDataNode->lCardType
						|| MT_CARD_TYPE_TRIPLE_TWO == pkQueueDeskDataNode->lCardType) /// 三支
					{
						/// 当前牌面主牌 >= K
						if (10 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
							*plCardType = MT_CARD_TYPE_PASS;
						}
						else /// 当前牌面主牌 < K
						{
							/// 我的主牌 < J
							if (8 > iMainCardsNumSelf)
							{
								/// 正常出牌
							}
							else /// 我的主牌 >= J
							{
								/// 我不出牌 pass
								memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
								*plCardType = MT_CARD_TYPE_PASS;
							}
						}
					}
					else /// 当前牌面其它牌型
					{
						/// 我不出牌 pass
						memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
						*plCardType = MT_CARD_TYPE_PASS;
					}
				}
			}
		}
		else /// 当前已经打出牌的用户是对家
		{
			///MT_DEBUG(4, "User PlayOutCard  303......\n");
			/// 获得当前牌面的主牌信息
			int iCardsRecvNum = getCardsNum(pkQueueDeskDataNode->pcCardsCur);
			int iMainCardNumRecv = getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur, iCardsRecvNum);

			/// 炸弹
			if (MT_CARD_TYPE_ROCKET == pkQueueDeskDataNode->lCardType 
				|| MT_CARD_TYPE_BOMB == pkQueueDeskDataNode->lCardType)
			{
				////MT_DEBUG(4, "User PlayOutCard  304......\n");
				/// 我正常出牌(我有炸弹就要出)
			}
			else /// 当前牌面其它牌型
			{
				////MT_DEBUG(4, "User PlayOutCard  305......\n");
				/// 如果我的主牌是炸弹
				if (MT_CARD_TYPE_ROCKET == *plCardType || MT_CARD_TYPE_BOMB == *plCardType)
				{
					/// 如果对家手中就剩了1，2张牌，我就出牌(防止对家剩一手牌走掉)
					int iUserCardsNumEnemy = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[pkQueueDeskDataNode->lQueueUserDeskIdx]);
					if (1 == iUserCardsNumEnemy || 2 == iUserCardsNumEnemy)
					{
						/// 我正常出牌(炸弹就要出)
					}
					else
					{
						/// 如果我下家是自家
						if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] == pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx])
						{
							/// 获得下家手中牌的个数
							int iCardsSameFamilyNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserNextIdx]);

							/// 如果我下家的手中牌数少于当前打出的牌数,(我能打就要打)
							if (iCardsSameFamilyNum < iCardsRecvNum)
							{
								/// 我正常出牌(炸弹就要出)
							}
							else /// 我不出牌 pass
							{
								memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
								*plCardType = MT_CARD_TYPE_PASS;
							}

						}
						else /// 如果我下家是对家
						{
							if(iUserCardsNumEnemy > 10)///如果对家的牌大于10张，不出牌
							{
								memset(pkQueueDeskDataNode->pcCardsOutAi, MT_CARD_TERMINATE, sizeof(pkQueueDeskDataNode->pcCardsOutAi));
								*plCardType = MT_CARD_TYPE_PASS;
							}
							/// 我正常出牌(炸弹就要出)
						}
					}
				}
				else /// 如果我的主牌不是炸弹
				{
					////MT_DEBUG(4, "User PlayOutCard  306......\n");
					/// 我正常出牌
				}
			}
		}
	}

	return 1;
}

int mtServiceCardOut::updateCardsFace(char* pcSelfCard,char* pcOutCard)
{
	int iCardIdx	= 0;
	if(*pcOutCard == MT_CARD_TERMINATE)
	{
		return 1;
	}
	while(pcOutCard[iCardIdx] != MT_CARD_TERMINATE)
	{
		char cTmpCard	= pcOutCard[iCardIdx];
		rearrangeCards(pcSelfCard,cTmpCard);
		iCardIdx++;
	}

	return	1;
}

void mtServiceCardOut::rearrangeCards(char* pcSelfCards,char cCard)
{
	int iCardIdx	= 0;
	for(;pcSelfCards[iCardIdx] != MT_CARD_TERMINATE; iCardIdx++)
	{
		if(pcSelfCards[iCardIdx] == cCard)
		{
			break;
		}
	}
	for(;pcSelfCards[iCardIdx] != MT_CARD_TERMINATE;iCardIdx++)
	{
		pcSelfCards[iCardIdx]	= pcSelfCards[iCardIdx + 1];
	}
}

int mtServiceCardOut::IsSeriesCard(char pcSelfCard[],char pcOutCard[])
{
	int iSelfCardsNum = 0;
	while(pcSelfCard[iSelfCardsNum] != MT_CARD_TERMINATE)
	{
		iSelfCardsNum++;
	}

	if(iSelfCardsNum <= 10)
	{
		if(iSelfCardsNum == 10 || iSelfCardsNum == 8)
		{
			if(getAirPlane(pcSelfCard,pcOutCard,0,iSelfCardsNum) >= 0)
			{
				return MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_PAIR_PROGRESSION;
			}
		}
		if(iSelfCardsNum == 9)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
		}
		if(iSelfCardsNum == 6)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_PAIR_PROGRESSION;
			}
		}
		if(iSelfCardsNum >= 5)
		{
			if(getSingleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
			if(iSelfCardsNum == 5)
			{
				if(getTripleWithTwo(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
				{
					return MT_CARD_TYPE_TRIPLE_TWO;
				}
			}
		}
		if(iSelfCardsNum == 4)
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}
		}
	}

	return -1;
}

int mtServiceCardOut::getCardsNum(char pcCard[])
{
	int iCardsNum = 0;
	while(pcCard[iCardsNum] != MT_CARD_TERMINATE)
	{
		iCardsNum++;
	}

	return iCardsNum;
}

int mtServiceCardOut::getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[])
{
	int iCardType = 0;

	int iCardsNum = 0;
	while(pcSelfCard[iCardsNum] != MT_CARD_TERMINATE)
	{
		iCardsNum++;
	}

	//if(iCardsNum <= 10)
	{
		if(iCardsNum == 10)
		{
			if(getAirPlane(pcSelfCard,pcOutCard,0,iCardsNum) >= 0)
			{
				return MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iCardsNum)>=0)
			{
				return MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,iCardsNum)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 9)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,9)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,9)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 8)
		{
			if(getAirPlane(pcSelfCard,pcOutCard,0,8) >= 0)
			{
				return MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,8)>=0)
			{
				return MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,8)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 6)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 5)
		{
			if(getSingleProgression(pcSelfCard,pcOutCard,0,5)>=0)
			{
				return MT_CARD_TYPE_SINGLE_PROGRESSION;
			}

			if(getTripleWithTwo(pcSelfCard,pcOutCard,0,5)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_TWO;
			}

		}
		if(iCardsNum >= 4)
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4)>=0)
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}
		}
	}
	if((pcSelfCard[0] & 0x0f) == 13 && (pcSelfCard[1] & 0x0f)==14 && pcSelfCard[2] == MT_CARD_TERMINATE)
	{
		pcOutCard[0]	= pcSelfCard[0];
		pcOutCard[1]	= pcSelfCard[1];
		return MT_CARD_TYPE_ROCKET;
	}

	int iIdx = 0;

	while(pcSelfCard[iIdx] != MT_CARD_TERMINATE)
	{
		if(pcSelfCard[iIdx + 1] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			iCardType	= MT_CARD_TYPE_SINGLE;
			break;
		}
		else if((pcSelfCard[iIdx] & 0x0f) != (pcSelfCard[iIdx + 1] & 0x0f))
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			iCardType	= MT_CARD_TYPE_SINGLE;
			break;
		}
		else if(pcSelfCard[iIdx + 2] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			iCardType		= MT_CARD_TYPE_PAIR;
			break;
		}
		else if((pcSelfCard[iIdx] & 0x0f) != (pcSelfCard[iIdx + 2] & 0x0f))
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			iCardType		= MT_CARD_TYPE_PAIR;
			break;
		}
		else if(pcSelfCard[iIdx + 3] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			pcOutCard[2]	= pcSelfCard[iIdx+2];
			iCardType		= MT_CARD_TYPE_TRPILE;
			break;
		}
		else if((pcSelfCard[iIdx] & 0x0f) != (pcSelfCard[iIdx + 3] & 0x0f))
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			pcOutCard[2]	= pcSelfCard[iIdx+2];
			int iResult	= getSingleCard(pcSelfCard,pcOutCard+3,0);
			if(iResult >= 0)
			{
				iCardType		= MT_CARD_TYPE_TRIPLE_ONE;
				break;
			}
			else
			{
				iResult	= getPairCard(pcSelfCard,pcOutCard+3,0);
				if(iResult >= 0)
				{
					iCardType		= MT_CARD_TYPE_TRIPLE_TWO;

					break;
				}
				else
				{
					iCardType		= MT_CARD_TYPE_TRPILE;
					break;
				}
			}
		}
		else
		{
			iIdx+=4;
		}
	}
	if(pcSelfCard[iIdx] != MT_CARD_TERMINATE)
	{
		return iCardType;
	}
	else
	{
		pcOutCard[0]	= pcSelfCard[0];
		pcOutCard[1]	= pcSelfCard[1];
		pcOutCard[2]	= pcSelfCard[2];
		pcOutCard[3]	= pcSelfCard[3];
		return MT_CARD_TYPE_BOMB;
	}
}

int mtServiceCardOut::getPlayOutCard(int iCardType,char pcRecvCard[],char pcSelfCard[],char pcOutCard[],bool bSplitCard)
{
	int iSelfIdx = 0;
	int iRecvIdx = 0;
	int iCardsRecvNum = 0;
	int iMainCard;
	int	iResult	= -1;///>=0 表示得到的牌的起始索引

	int iCardsNum = 0;
	while(pcSelfCard[iCardsNum] != MT_CARD_TERMINATE)
	{
		iCardsNum++;
	}
	if(iCardType == MT_CARD_TYPE_ROCKET)
	{
		return iResult;
	}
	while(pcRecvCard[iCardsRecvNum]!= MT_CARD_TERMINATE)
	{
		iCardsRecvNum++;
	}
	iMainCard	= getMainCardFromRecv(iCardType,pcRecvCard,iCardsRecvNum);
	MT_DEBUG(4, "iMainCard=%d\n", (iMainCard & 0X0F));
	for(;pcSelfCard[iSelfIdx] != MT_CARD_TERMINATE;iSelfIdx++)
	{
		if((pcSelfCard[iSelfIdx] & 0x0f) > (iMainCard & 0x0f))
		{
			break;
		}
	}

	if ((pcSelfCard[iSelfIdx] == MT_CARD_TERMINATE))
	{
		if(iCardType != MT_CARD_TYPE_BOMB)
		{
			iSelfIdx = 0;
			iResult = getBombCard(pcSelfCard,pcOutCard,iSelfIdx);
			return 	iResult;
		}
		else
		{
			return iResult;
		}
	}


	switch(iCardType)
	{
	case MT_CARD_TYPE_SINGLE:
		{
			iResult	= getSingleCard(pcSelfCard,pcOutCard,iSelfIdx);

			if(iResult == -1 && bSplitCard)//如果没有找出单张，且可以拆牌，拆出一个单张
			{
				iResult	= forceGetSingleCard(pcSelfCard,pcOutCard,iSelfIdx);
			}
		}
		break;
	case MT_CARD_TYPE_PAIR:
		{
			iResult	= getPairCard(pcSelfCard,pcOutCard,iSelfIdx);

			if(iResult == -1 && bSplitCard)//如果没有找出对子，且可以拆牌，拆出一个对子
			{
				iResult	= forceGetPairCard(pcSelfCard,pcOutCard,iSelfIdx);
			}
		}
		break;
	case MT_CARD_TYPE_TRPILE:
		{
			if(iCardsNum > 2)
			{
				iResult	= getTripleCard(pcSelfCard,pcOutCard,iSelfIdx);
			}
			
		}
		break;
	case MT_CARD_TYPE_TRIPLE_ONE:
		{
			if(iCardsNum > 3)
			{
				iResult	= getTripleWithOne(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_TRIPLE_TWO:
		{
			if(iCardsNum > 4)
			{
				iResult	= getTripleWithTwo(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_SINGLE_PROGRESSION:
		{
			if(iCardsNum > 4)
			{
				iResult	= getSingleProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_PAIR_PROGRESSION:
		{
			if(iCardsNum > 5)
			{
				iResult	= getPairProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_TRIPLE_PROGRESSION:
		{
			if(iCardsNum > 5)
			{
				iResult	= getTripleProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_AIRPLANE:
		{
			if(iCardsNum > 7)
			{
				iResult	= getAirPlane(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case MT_CARD_TYPE_FOUR_TWO:
		{
			iResult = getBombCard(pcSelfCard,pcOutCard,0);
		}
	case MT_CARD_TYPE_BOMB:
		{
			iResult = getBombCard(pcSelfCard,pcOutCard,iSelfIdx);
		}
		break;
	default:
		break;
	}

	if(iResult < 0 && iCardType != MT_CARD_TYPE_BOMB)
	{
		iSelfIdx = 0;
		iResult = getBombCard(pcSelfCard,pcOutCard,iSelfIdx);
	}
	if(iResult < 0)
	{
		return getRocketCard(pcSelfCard,pcOutCard);
	}
	else
	{
		return iResult;
	}
}

int mtServiceCardOut::playOutMaxCardsByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum)
{
	switch(iCardType)
	{
	case MT_CARD_TYPE_SINGLE:
		{
			pcOutCard[0] = pcSelfCard[iSelfCardsNum - 1];
			return MT_CARD_TYPE_SINGLE;
		}
	case MT_CARD_TYPE_PAIR:
		{
			char pcTmpCards[4];
			memset(pcTmpCards,100,4);
			int iCardIdx = getPairCard(pcSelfCard,pcTmpCards,0);
			while(iCardIdx >= 0)
			{
				pcOutCard[0]	= pcTmpCards[0];
				pcOutCard[1]	= pcTmpCards[1];
				iCardIdx = getPairCard(pcSelfCard,pcTmpCards,iCardIdx + 2);
			}
			if(pcOutCard[0] == 100)
			{
				return -1;
			}
			else
			{
				return MT_CARD_TYPE_PAIR;
			}
		}
	case MT_CARD_TYPE_TRPILE:
		{
			char pcTmpCards[4];
			memset(pcTmpCards,100,4);
			int iCardIdx = getTripleCard(pcSelfCard,pcTmpCards,0);
			while(iCardIdx >= 0)
			{
				pcOutCard[0]	= pcTmpCards[0];
				pcOutCard[1]	= pcTmpCards[1];
				pcOutCard[2]	= pcTmpCards[2];
				iCardIdx = getPairCard(pcSelfCard,pcTmpCards,iCardIdx+3);
			}
			if(pcOutCard[0] == 100)
			{
				return -1;
			}
			else
			{
				return MT_CARD_TYPE_TRPILE;
			}
		}
	}

	return -1;
}

int mtServiceCardOut::judgeSingleProgression(char* pcCards,int iCardsNum)
{
	if(iCardsNum < 5)
	{
		return -1;
	}
	int iIdxCount = 1;
	for(;iIdxCount < iCardsNum; iIdxCount++)
	{
		if((pcCards[0] & 0x0f) + iIdxCount != (pcCards[iIdxCount] & 0x0f)
			|| (pcCards[iIdxCount] & 0x0f) >= 12)
		{
			break;
		}
	}
	if(iIdxCount < iCardsNum)
	{
		return -1;
	}
	else
	{
		return pcCards[0];
	}
}

int mtServiceCardOut::judgePairProgression(char* pcCards,int iCardsNum)
{
	if(iCardsNum < 3)
	{
		return -1;
	}
	int iIdxCount = 0;
	for(;iIdxCount  < iCardsNum; iIdxCount+=2)
	{
		if((pcCards[iIdxCount] & 0x0f) >= 12)
		{
			break;
		}
		else if((pcCards[iIdxCount] & 0x0f) != (pcCards[iIdxCount + 1] & 0x0f))
		{
			break;
		}
		else if(iIdxCount < iCardsNum - 2)
		{
			if((pcCards[iIdxCount] & 0x0f) + 1 != (pcCards[iIdxCount + 2] & 0x0f))
			{
				break;
			}
		}
	}
	if(iIdxCount < iCardsNum)
	{
		return -1;
	}
	else
	{
		return pcCards[0];
	}
}

int mtServiceCardOut::judgeTripleProgression(char* pcCards,int iCardsNum)
{
	if(iCardsNum < 2)
	{
		return -1;
	}
	int iIdxCount = 0;
	for(;iIdxCount  < iCardsNum; iIdxCount+=3)
	{
		if((pcCards[iIdxCount] & 0x0f) >= 12)
		{
			break;
		}
		else if((pcCards[iIdxCount] & 0x0f) != (pcCards[iIdxCount + 2] & 0x0f))
		{
			break;
		}
		else if(iIdxCount < iCardsNum - 3)
		{
			if((pcCards[iIdxCount] & 0x0f) + 1 != (pcCards[iIdxCount + 3] & 0x0f))
			{
				break;
			}
		}
	}
	if(iIdxCount < iCardsNum)
	{
		return -1;
	}
	else
	{
		return pcCards[0];
	}
}

int mtServiceCardOut::judgeTripleWithOne(char* pcCards,int iCardsNum)
{
	if(iCardsNum != 4)
	{
		return -1;
	}
	if((pcCards[0] & 0x0f)== (pcCards[2] & 0x0f) && (pcCards[0] & 0x0f) != (pcCards[3] & 0x0f))
	{
		return pcCards[0];
	}
	else if((pcCards[0] & 0x0f) != (pcCards[1] & 0x0f) && (pcCards[1] & 0x0f) == (pcCards[3] & 0x0f))
	{
		return pcCards[1];
	}
	else
	{
		return -1;
	}
}

int mtServiceCardOut::judgeTripleWithTwo(char* pcCards,int iCardsNum)
{
	if(iCardsNum != 5)
	{
		return -1;
	}
	if((pcCards[0] & 0x0f) == (pcCards[2] & 0x0f) && (pcCards[0] & 0x0f) != (pcCards[3] & 0x0f)
		&& (pcCards[3] & 0x0f) == (pcCards[4] & 0x0f))
	{
		return pcCards[0];
	}
	else if((pcCards[0] & 0x0f) == (pcCards[1] & 0x0f) && (pcCards[0] & 0x0f) != (pcCards[2] & 0x0f)
		&& (pcCards[2] & 0x0f) == (pcCards[4] & 0x0f))
	{
		return pcCards[2];
	}
	else
	{
		return -1;
	}
}

int mtServiceCardOut::judgeFourWithTwo(char* pcCards,int iCardsNum)
{
	if(iCardsNum != 6 && iCardsNum != 8)
	{
		return -1;
	}
	if(iCardsNum == 6)
	{
		if((pcCards[0] & 0x0f) == (pcCards[3] & 0x0f) && (pcCards[4] & 0x0f) != (pcCards[5] & 0x0f))
		{
			return pcCards[0];
		}
		else if((pcCards[1] & 0x0f) == (pcCards[4] & 0x0f) && (pcCards[0] & 0x0f) != (pcCards[5] & 0x0f))
		{
			return pcCards[1];
		}
		else if((pcCards[2] & 0x0f) == (pcCards[5] & 0x0f) && (pcCards[0] & 0x0f) != (pcCards[1] & 0x0f))
		{
			return pcCards[2];
		}
		else
		{
			return -1;
		}
	}
	else if(iCardsNum == 8)
	{
		if((pcCards[0] & 0x0f) == (pcCards[3] & 0x0f) && (pcCards[4] & 0x0f) == (pcCards[5] & 0x0f)
			&& (pcCards[6] & 0x0f) == (pcCards[7] & 0x0f))
		{
			return pcCards[0];
		}
		else if((pcCards[2] & 0x0f) == (pcCards[5] & 0x0f) && (pcCards[0] & 0x0f) == (pcCards[1] & 0x0f)
			&& (pcCards[6] & 0x0f) == (pcCards[7] & 0x0f))
		{
			return pcCards[2];
		}
		else if((pcCards[4] & 0x0f) == (pcCards[7] & 0x0f) && (pcCards[0] & 0x0f) == (pcCards[1] & 0x0f)
			&& (pcCards[2] & 0x0f) == (pcCards[3] & 0x0f))
		{
			return pcCards[4];
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

int mtServiceCardOut::judgeAirPlane(char* pcCards,int iCardsNum)
{
	if((iCardsNum % 4) !=  0 && (iCardsNum % 5) != 0)
	{
		return -1;
	}

	if(iCardsNum % 4 == 0)
	{
		int	iCount	= iCardsNum>>2;
		if((pcCards[0] & 0x0f) +1 == (pcCards[5] & 0x0f))
		{
			return judgeTripleProgression(pcCards,iCardsNum - iCount);
		}
		else if((pcCards[1] & 0x0f) +1 == (pcCards[6] & 0x0f))
		{
			return judgeTripleProgression(pcCards + 1,iCardsNum - iCount);
		}
		else if((pcCards[2] & 0x0f) +1 == (pcCards[7] & 0x0f))
		{
			return judgeTripleProgression(pcCards + 2,iCardsNum - iCount);
		}
		else if((pcCards[3] & 0x0f) + 1 == (pcCards[8] & 0x0f))
		{
			return judgeTripleProgression(pcCards + 3,iCardsNum - iCount);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		int	iCount	= iCardsNum/5;
		int iCardIdx = 0;
		int iResult	= -1;
		while (iCardIdx < iCardsNum)
		{
			if((pcCards[iCardIdx] & 0x0f) != (pcCards[iCardIdx+1] & 0x0f))
			{
				iResult = -1;
				break;
			}
			else if((pcCards[iCardIdx] & 0x0f)== (pcCards[iCardIdx+2] & 0x0f))
			{
				if(judgeTripleProgression(pcCards + iCardIdx,(iCardsNum - iCount*3))>=0)
				{
					iResult = pcCards[iCardIdx];
					iCardIdx += (iCardsNum - iCount*3);
				}
				else
				{
					iResult = -1;
					break;
				}
			}
			else
			{
				iCardIdx+=2;
			}
		}
		return iResult;
	}

}

int mtServiceCardOut:: getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos)
{
	int iSelfIdx	= iCardPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{//0,1,2,3
		if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			break;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			iSelfIdx += 2;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+3] & 0x0f))
		{
			iSelfIdx += 3;
		}
		else
		{
			iSelfIdx += 4;
		}
	}
	if(pcSelfCards[iSelfIdx + 3] == MT_CARD_TERMINATE)
	{
		if(pcSelfCards[iSelfIdx + 2] != MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
			{
				cOutCard[0]	= pcSelfCards[iSelfIdx];
				return iSelfIdx;
			}
			else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
			{
				cOutCard[0]	= pcSelfCards[iSelfIdx+2];
				return iSelfIdx+2;
			}
			else
			{
				return		-1;
			}
		}
		else if(pcSelfCards[iSelfIdx + 1] != MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
			{
				cOutCard[0]	= pcSelfCards[iSelfIdx];
				return iSelfIdx;
			}
			else
			{
				return		-1;
			}
		}
		else if(pcSelfCards[iSelfIdx] != MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx - 1] & 0x0f) != (pcSelfCards[iSelfIdx] & 0x0f))
			{
				cOutCard[0]	= pcSelfCards[iSelfIdx];
				return iSelfIdx;
			}
			else
			{
				return -1;
			}
		}

		return		-1;
	}
	else
	{
		cOutCard[0]	= pcSelfCards[iSelfIdx];
		return iSelfIdx;
	}


}

int mtServiceCardOut:: forceGetSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos)
{
	int iSelfIdx = iCardPos;

	//炸弹不能拆成单张
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
			&&(pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f)
			&&(pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+3] & 0x0f)
			)
		{
			iSelfIdx += 4;
		}
		else
		{
			break;
		}
	}

	//火箭不能拆成单牌
	if((pcSelfCards[iSelfIdx] & 0x0f) == 13 && (pcSelfCards[iSelfIdx + 1] & 0x0f) == 14)
	{
		return -1;
	}
	if(pcSelfCards[iSelfIdx] == MT_CARD_TERMINATE)
	{
		return -1;
	}

	cOutCard[0]  = pcSelfCards[iSelfIdx];
	mtLog("\nforceGetSingleCard");
	return iSelfIdx;
}

int mtServiceCardOut::getPairCard(char* pcSelfCards,char* pcOutCard ,int iCardBegPos)
{
	int iSelfIdx	= iCardBegPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			iSelfIdx++;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			break;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+3] & 0x0f))
		{
			iSelfIdx += 3;
		}
		else
		{
			iSelfIdx += 4;
		}
	}
	if(pcSelfCards[iSelfIdx + 3] == MT_CARD_TERMINATE)
	{
		///不出牌
		if (pcSelfCards[iSelfIdx+2]!= MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
				&& (pcSelfCards[iSelfIdx + 1] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
			{
				pcOutCard[0]	= pcSelfCards[iSelfIdx];
				pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
				return	iSelfIdx;
			}
			else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f) && (pcSelfCards[iSelfIdx + 1] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f))
			{
				pcOutCard[0]	= pcSelfCards[iSelfIdx + 1];
				pcOutCard[1]	= pcSelfCards[iSelfIdx + 2];
				return	iSelfIdx + 1;
			}
			else
			{
				return -1;
			}
		}
		else if(pcSelfCards[iSelfIdx + 1] != MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f))
			{
				pcOutCard[0]	= pcSelfCards[iSelfIdx];
				pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
				return	iSelfIdx;
			}
			else
			{
				return -1;
			}
		}
		return -1;
	}
	else
	{
		pcOutCard[0]	= pcSelfCards[iSelfIdx];
		pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
		return	iSelfIdx;
		///更新手中的牌
	}
}

int mtServiceCardOut::forceGetPairCard(char* pcSelfCards,char* pcOutCard,int iCardPos)
{
	int iSelfIdx	= iCardPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
		//炸弹不能拆出对子
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
			&&(pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f)
			&&(pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+3] & 0x0f)
			)
		{
			iSelfIdx += 4;
			continue;
		}

		if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			iSelfIdx++;
		}
		else
		{
			break;
		}
	}

	char pp[17];

	memcpy(pp,pcSelfCards,17);
	if(pcSelfCards[iSelfIdx] != MT_CARD_TERMINATE && pcSelfCards[iSelfIdx + 1] != MT_CARD_TERMINATE
		&&(pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx + 1] & 0x0f))
	{
		pcOutCard[0] = pcSelfCards[iSelfIdx];
		pcOutCard[1] = pcSelfCards[iSelfIdx + 1];

		mtLog("\nforceGetPairCard");
		return iSelfIdx;
	}
	return -1;

}

int mtServiceCardOut::getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos)
{
	int iSelfIdx	= iCardBegPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			iSelfIdx++;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			iSelfIdx += 2;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+3] & 0x0f))
		{
			break;
		}
		else
		{
			iSelfIdx += 4;
		}
	}
	if(pcSelfCards[iSelfIdx + 3] == MT_CARD_TERMINATE)
	{
		///不出牌
		if(pcSelfCards[iSelfIdx+2] != MT_CARD_TERMINATE)
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
				&& (pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f))
			{
				pcOutCard[0]	= pcSelfCards[iSelfIdx];
				pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
				pcOutCard[2]	= pcSelfCards[iSelfIdx + 2];
				return iSelfIdx;

			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

	}
	else
	{
		pcOutCard[0]	= pcSelfCards[iSelfIdx];
		pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
		pcOutCard[2]	= pcSelfCards[iSelfIdx + 2];
		return iSelfIdx;
		///更新手中的牌
	}

}

int mtServiceCardOut::getBombCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos)
{
	int iSelfIdx	= iCardBegPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			iSelfIdx++;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			iSelfIdx += 2;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) != (pcSelfCards[iSelfIdx+3] & 0x0f))
		{
			iSelfIdx += 3;
		}
		else
		{
			break;
		}
	}
	if(pcSelfCards[iSelfIdx+3] == MT_CARD_TERMINATE)
	{
		return -1;///不出牌
	}
	else
	{
		pcOutCard[0]	= pcSelfCards[iSelfIdx];
		pcOutCard[1]	= pcSelfCards[iSelfIdx + 1];
		pcOutCard[2]	= pcSelfCards[iSelfIdx + 2];
		pcOutCard[3]	= pcSelfCards[iSelfIdx + 3];
		return	 iSelfIdx;
		///更新手中的牌
	}
}

int mtServiceCardOut::getRocketCard(char* pcSelfCards,char* pcOutCard)
{
	int iCardIdx = 0;
	while(pcSelfCards[iCardIdx] != MT_CARD_TERMINATE)
	{
		iCardIdx++;
	}
	if((pcSelfCards[iCardIdx-1] & 0x0f) == 14 && (pcSelfCards[iCardIdx - 2] & 0x0f) == 13)
	{
		pcOutCard[0] = pcSelfCards[iCardIdx - 2];
		pcOutCard[1] = pcSelfCards[iCardIdx - 1];
		return iCardIdx - 2;
	}
	else
	{
		return -1;
	}
}

int mtServiceCardOut::getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	while(pcSelfCards[iSelfIdx + 1] != MT_CARD_TERMINATE && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)-1)
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			iSelfIdx++;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			iSelfIdx ++;
		}
		else
		{
			iTmpCount	= 0;
			iSelfIdx++;
		}
		if(iTmpCount >= iCardsNum)
		{
			break;
		}
	}
	if(pcSelfCards[iSelfIdx + 1] == MT_CARD_TERMINATE && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx - 1] & 0x0f) == (pcSelfCards[iSelfIdx] & 0x0f)-1)
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			iSelfIdx++;
		}
	}
	if(iTmpCount < iCardsNum)
	{
		return -1;///不出牌
	}
	else
	{
		int iIdx  = 0;
		for(;iIdx < iCardsNum;iIdx++)
		{
			pcOutCards[iIdx]	= pcTmpCards[iIdx];
		}
		return	 iSelfIdx;
		///更新手中的牌
	}
}

int	mtServiceCardOut::getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	if(iCardsNum < 6 || iCardsNum % 2 != 0)
	{
		return -1;
	}
	while(pcSelfCards[iSelfIdx + 2] != MT_CARD_TERMINATE && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
			&& (pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f) - 1)
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 1];
			iSelfIdx+=2;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f)
			&& (pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+3] & 0x0f))
			{
				iTmpCount	= 0;
				iSelfIdx+=4;
			}
			else
			{
				iSelfIdx++;
			}
		}
		else
		{
			iTmpCount	= 0;
			iSelfIdx++;
		}
		if(iTmpCount >= iCardsNum)
		{
			break;
		}
	}
	if(pcSelfCards[iSelfIdx + 2] == MT_CARD_TERMINATE && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+1] & 0x0f))
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 1];
			iSelfIdx+=2;
		}
	}
	if(iTmpCount < iCardsNum)
	{
		return -1;
	}
	else
	{
		int iIdx  = 0;
		for(;iIdx < iCardsNum;iIdx++)
		{
			pcOutCards[iIdx]	= pcTmpCards[iIdx];
		}
		return	 iSelfIdx;
		///更新手中的牌
	}
}

int	mtServiceCardOut::getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	if(iCardsNum < 6 || iCardsNum % 3 != 0)
	{
		return -1;
	}
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f)
			&& (pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+3] & 0x0f)-1)
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 1];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 2];
			iSelfIdx+=3;
		}
		else if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f)
			&& (pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+3] & 0x0f))
		{
			iTmpCount = 0;
			iSelfIdx +=4;
		}
		else
		{
			iTmpCount	= 0;
			iSelfIdx++;
		}
		if(iTmpCount >= iCardsNum)
		{
			break;
		}
	}
	if((pcSelfCards[iSelfIdx + 3] == MT_CARD_TERMINATE) && ((pcSelfCards[iSelfIdx] & 0x0f) < 12))
	{
		if((pcSelfCards[iSelfIdx] & 0x0f) == (pcSelfCards[iSelfIdx+2] & 0x0f))
		{
			pcTmpCards[iTmpCount++]	= pcSelfCards[iSelfIdx];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 1];
			pcTmpCards[iTmpCount++] = pcSelfCards[iSelfIdx + 2];
			iSelfIdx+=3;
		}
	}
	if(iTmpCount < iCardsNum)
	{
		return -1;///不出牌
	}
	else
	{
		int iIdx  = 0;
		for(;iIdx < iCardsNum;iIdx++)
		{
			pcOutCards[iIdx]	= pcTmpCards[iIdx];
		}
		return	 iSelfIdx;
		///更新手中的牌
	}
}

int	mtServiceCardOut::getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	int  iResult	= 0;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	if(iCardsNum != 4)
	{
		return -1;
	}
	if((iSelfIdx = getTripleCard(pcSelfCards,pcTmpCards,iCardBegPos))>=0)
	{
		iTmpCount += 3;
		if(getSingleCard(pcSelfCards,pcTmpCards+iTmpCount,0)< 0)
		{
			return -1;
		}
		else
		{
			iTmpCount++;
		}
	}
	else
	{
		return -1;
	}

	int iIdx  = 0;
	for(;iIdx < iCardsNum;iIdx++)
	{
		pcOutCards[iIdx]	= pcTmpCards[iIdx];
	}
	return	 iSelfIdx;
	///更新手中的牌
}

int	mtServiceCardOut::getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	int  iResult	= 0;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	if(iCardsNum != 5)
	{
		return -1;
	}
	if((iSelfIdx = getTripleCard(pcSelfCards,pcTmpCards,iCardBegPos))>=0)
	{
		iTmpCount += 3;
		if(getPairCard(pcSelfCards,pcTmpCards+iTmpCount,0)< 0)
		{
			return -1;
		}
		else
		{
			iTmpCount+=2;
		}
	}
	else
	{
		return -1;
	}

	int iIdx  = 0;
	for(;iIdx < iCardsNum;iIdx++)
	{
		pcOutCards[iIdx]	= pcTmpCards[iIdx];
	}
	return	 iSelfIdx;
		///更新手中的牌
}

int	mtServiceCardOut::getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	int	 iTmpCount	= 0;
	int  iTripleNum;
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	if(iCardsNum % 4 == 0)
	{
		iTripleNum = iCardsNum>> 2;
	}
	else if(iCardsNum % 5 == 0)
	{
		iTripleNum = iCardsNum/5;
	}
	else
	{
		return - 1;
	}
	if(getTripleProgression(pcSelfCards,pcTmpCards,iCardBegPos,iTripleNum*3) >= 0)
	{
		int iIdx  = 0;
		int iPos = 0;
		iTmpCount += iTripleNum*3;
		if(iCardsNum % 4 == 0)
		{
			for(iIdx  = 0;iIdx < iTripleNum;iIdx++)
			{
				iPos	= getSingleCard(pcSelfCards,pcTmpCards + iTmpCount,iPos);
				if(iPos < 0)
				{
					break;
				}
				iTmpCount++;
				iPos++;
			}
			if(iIdx < iTripleNum)
			{
				return -1;
			}
		}
		else
		{
			for(iIdx  = 0;iIdx < iTripleNum;iIdx++)
			{
				iPos	= getPairCard(pcSelfCards,pcTmpCards+iTmpCount,iPos);
				if(iPos < 0)
				{
					break;
				}
				iTmpCount+=2;
				iPos += 2;
			}
			if(iIdx < iTripleNum)
			{
				return -1;
			}
		}
		for(iIdx  = 0;iIdx < iCardsNum;iIdx++)
		{
			pcOutCards[iIdx]	= pcTmpCards[iIdx];
		}
		return	 iSelfIdx;
	}
	else
	{
		return -1;
	}

}

void mtServiceCardOut::resortCardsOut(char* pcCardsOut)
{
	int iIdx = 0;
	int jIdx = 0;
	char cTmpCard;
	for(;pcCardsOut[iIdx] != 100;iIdx++)
	{
		for(jIdx = iIdx + 1;pcCardsOut[jIdx] != 100;jIdx++)
		{
			if((pcCardsOut[iIdx] & 0x0f) > (pcCardsOut[jIdx] & 0x0f))
			{
				cTmpCard = pcCardsOut[iIdx];
				pcCardsOut[iIdx] = pcCardsOut[jIdx];
				pcCardsOut[jIdx] = cTmpCard;
			}
		}
	}
}

int mtServiceCardOut::getMainCardFromRecv(int iCardType,char* pcCardsRecv,int iCardsRecvNum)
{
	switch(iCardType)
	{
	case MT_CARD_TYPE_PASS:
		{
			return -1;
		}
	case MT_CARD_TYPE_SINGLE:
	case MT_CARD_TYPE_PAIR:
	case MT_CARD_TYPE_TRPILE:
	case MT_CARD_TYPE_BOMB:
	case MT_CARD_TYPE_SINGLE_PROGRESSION:
	case MT_CARD_TYPE_PAIR_PROGRESSION:
	case MT_CARD_TYPE_TRIPLE_PROGRESSION:
		{
			return pcCardsRecv[0];
		}
	case MT_CARD_TYPE_TRIPLE_ONE:
		{
			return judgeTripleWithOne(pcCardsRecv,iCardsRecvNum);
		}
	case MT_CARD_TYPE_TRIPLE_TWO:
		{
			return judgeTripleWithTwo(pcCardsRecv,iCardsRecvNum);
		}
	case MT_CARD_TYPE_AIRPLANE:
		{
			return judgeAirPlane(pcCardsRecv,iCardsRecvNum);
		}
	case MT_CARD_TYPE_FOUR_TWO:
		{
			return judgeFourWithTwo(pcCardsRecv,iCardsRecvNum);
		}
	}

	return -1;
}

int mtServiceCardOut::playOutCardsNotByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum)
{
	switch(iCardType)
	{
	case MT_CARD_TYPE_SINGLE:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_TRPILE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[iSelfCardsNum -1];
				return MT_CARD_TYPE_SINGLE;
			}
		}
	case MT_CARD_TYPE_PAIR:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_TRPILE;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return MT_CARD_TYPE_SINGLE;
			}
		}
	case MT_CARD_TYPE_TRPILE:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return MT_CARD_TYPE_SINGLE;
			}
		}
	case MT_CARD_TYPE_TRIPLE_ONE:
		{
			if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_TRPILE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return MT_CARD_TYPE_SINGLE;
			}
		}
	}

	return -1;
}

bool mtServiceCardOut::bSplitCards(mtQueueDesk::DataNode* pkQueueDeskDataNode,int iQueueUserAiIdx)
{
	//出牌用户是自家，不拆牌
   if (pkQueueDeskDataNode->plPlayerType[iQueueUserAiIdx] == pkQueueDeskDataNode->plPlayerType[pkQueueDeskDataNode->lQueueUserDeskIdx])
   {
	   return false;
   }

   /// 当前打出的牌面的牌数
   int iCardsRecvNum     = getCardsNum(pkQueueDeskDataNode->pcCardsCur);
   int iMainCardNumRecv  = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur, iCardsRecvNum) & 0x0f);
   int iQueueUserNextIdx = getQueueUserNextQueueUser(iQueueUserAiIdx);

   //如果对方出牌的主牌小于10，且下家是自家，也不拆牌
   if(iMainCardNumRecv < 7 
	   && pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx] == pkQueueDeskDataNode->plPlayerType[iQueueUserAiIdx])
   {
	   return false;
   }

   return true;
}

void printCards(char *cards)
{
	static char cardtype[] = {'3','4','5','6','7','8','9','10','J','Q','K' ,'A','2','(',')'};

	if(*cards == MT_CARD_TERMINATE)
		mtLog("Pass");
	while(*cards != MT_CARD_TERMINATE)
	{
		char card = cardtype[*cards & 0x0f];
		if(card != '0')
			mtLog("%c ",card);
		else
			mtLog("10 ");
		++cards;
	}
}

int CardsNumber(char *pcCards)
{
	int iNumber  = 0;
	while(*pcCards != MT_CARD_TERMINATE)
	{
		iNumber++;
		pcCards++;
	}
	return iNumber;
}

void mtServiceCardOut::printfOutCards(char *pcCurOutCards,char * pcSelfOutCards,char *pcSelfCards,long UserId,long curOutCardUserId)
{
	static long landlord = 0;
	if(curOutCardUserId != 0)
	{
		if(curOutCardUserId == UserId)
		{
           mtLog("\nOthe one pass,I Continue to Out Card!");
		}
		else
		{
			mtLog("\n[%d] out cards: ",curOutCardUserId);
			printCards(pcCurOutCards);
		}
	}
	else
	{
		mtLog("\n********************** A new start ******************************************");
		mtLog("\n------------------- [%d] is Landlord ---------------------------------------",UserId);
		landlord = UserId;
	}


	mtLog("\n[%d] Self Cards: ",UserId);
	printCards(pcSelfCards);
    
	mtLog("\n[%d] Out Cards: ",UserId);
    printCards(pcSelfOutCards);


	if(CardsNumber(pcSelfOutCards) == CardsNumber(pcSelfCards))
	{
		if(UserId == landlord)
		{
		  mtLog("\n\n-------------The game over,landlord win!-----------------");
		}
		else
		{
		  mtLog("\n\n-------------The game over,farmer win!--------------------");
		}
	}

	mtLog("\n\n");
	
}
