#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceMgr.h"
#include "mtServiceCardDeal.h"
#include "mtServiceRoomDoubleScore.h"
#include "mtServiceCardOut.h"
#include "mtPrint.h"

mtServiceRoomGrabLandlord::mtServiceRoomGrabLandlord():m_hEventManagerAi(NULL)
{

}

mtServiceRoomGrabLandlord::~mtServiceRoomGrabLandlord()
{

}

int mtServiceRoomGrabLandlord::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_hIOCP                 = pkDataInit->hIOCP;
	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	m_pkServiceMgr          = pkDataInit->pkServiceMgr;
	m_hEventManagerAi       = pkDataInit->hEventManagerAi;

	return	1;
}

int mtServiceRoomGrabLandlord::run( DataRun* pkDataRun )
{
	DataRead*                   pkDataRead     = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode = pkDataRun->pkQueueUserDataNode;
	mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkDataRun->pkQueueUserDataNode->pkQueueDeskDataNode;
	mtPrint(pkDataRead);
	if (NULL == pkQueueDeskDataNode)
	{
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	if (!pkQueueDeskDataNode
	 || !pkQueueUserDataNode
	 || mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING != pkQueueDeskDataNode->lDeskExit
	 || mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING != pkQueueUserDataNode->lUserExit)
	{
		if (pkQueueDeskDataNode)
		{
			LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
		}
		
		return 1;
	}

	MT_DEBUG(4, "User(UserId=%d)(score=%d)(deskidx=%d) GrabLandlord  22222......\n", pkQueueUserDataNode->lUserId, pkDataRead->lScore, pkQueueUserDataNode->lUserDeskIndex);
	/// 更新个人抢地主分数
	pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex] = pkDataRead->lScore;

	pkQueueDeskDataNode->ullTimeEnterRun = GetTickCount();
	pkQueueUserDataNode->ullTimeEnterRun = GetTickCount();

	/// 如果该用户人叫了3分，那这个人就是地主了
	DataWrite kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lUserIdLandlord   = -1;
	
	int iQueueDeskUserIdx;

	kDataWrite.lUserIdLandlord   = getQueueDeskLandlordUserId(pkQueueDeskDataNode, pkQueueUserDataNode, &iQueueDeskUserIdx);

	/// 发送这个人员的 ‘抢地主分数’ 消息 给该桌子的所有人员(标识某某人已经发送抢地主分数了)
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lScore            = pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex];

	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx]
		 && pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomGrabLandlord83");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;
				memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);

				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx], pkQueuePacketDataNode);
			}
		}
	}

	/// 获得下家用户在桌子上的位置
	int iQueueDeskUserNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);

	//  下家是ai用户, 在此通知其他用户 本ai不抢地主
	ULONGLONG ullStartTime = GetTickCount();
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueDeskUserNextIdx])
	{
		/// ai处理抢地主逻辑
		playGrabLandlordAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueDeskUserNextIdx, pkQueueUserDataNode->lUserDeskIndex, &ullStartTime);
		MT_DEBUG(4, "User(UserId=%d)(score=%d)(deskidx=%d) GrabLandlord  33333......\n", pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserNextIdx]->lUserId, pkQueueDeskDataNode->plScore[iQueueDeskUserNextIdx], iQueueDeskUserNextIdx);
		/// 获得ai用户的下家在桌子上的位置
		int iQueueDeskUserNextNextIdx = getQueueUserNextQueueUser(iQueueDeskUserNextIdx);

		//  ai下家还是是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueDeskUserNextNextIdx])
		{
			playGrabLandlordAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueDeskUserNextNextIdx, iQueueDeskUserNextIdx, &ullStartTime);
			MT_DEBUG(4, "User(UserId=%d)(score=%d)(deskidx=%d) GrabLandlord  44444......\n", pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserNextNextIdx]->lUserId, pkQueueDeskDataNode->plScore[iQueueDeskUserNextNextIdx], iQueueDeskUserNextNextIdx);
		}
	}

	/// 三人都不抢地主,重新开下一局
	if (0 == pkQueueDeskDataNode->plScore[0]
		&& 0 == pkQueueDeskDataNode->plScore[1]
		&& 0 == pkQueueDeskDataNode->plScore[2])
	{
		pkQueueDeskDataNode->lCardDeal  = 1;
		LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
		return playRestartCardDeal(pkDataRun->pkQueueUserDataNode);
	}

	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	/// 如果本次叫分 地主产生了，
	long lUserIdLandlord = -1;
	iQueueDeskUserIdx    = -1;
	lUserIdLandlord = getQueueDeskLandlordUserId(pkQueueDeskDataNode, pkQueueUserDataNode, &iQueueDeskUserIdx);
	if (-1 != lUserIdLandlord && NULL != pkQueueDeskDataNode && -1 != iQueueDeskUserIdx)
	{
		/// 先对各家牌面信息进行排序
		playQueueUserSort(pkQueueDeskDataNode);


		pkQueueDeskDataNode->lUserDeskIdxCardOutNext  = iQueueDeskUserIdx;
		/// 判断当前叫分用户的下家是不是ai,
		///	如果下家是ai，则要让ai首次进行加倍请求
		/// 下家用户在桌子上的位置
		int iQueueDeskUserNextIdx = getQueueUserNextQueueUser(iQueueDeskUserIdx);

		/// 是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueDeskUserNextIdx])
		{
			playDoubleScoreAi(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserIdx]);
		}

		/// 如果地主是ai，则ai要出牌了
		if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserIdx] && mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueDeskUserIdx])
		{
			/// ai地主先要出牌了
			playAiLandlordCardOut(pkQueueDeskDataNode, pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserIdx], &ullStartTime);
			MT_DEBUG(4, ">>>>>>>>>>>User(UserId=%d)(score=%d)(deskidx=%d)(cardouttype%d) default card out......\n", pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserIdx]->lUserId, pkQueueDeskDataNode->plScore[iQueueDeskUserIdx], iQueueDeskUserIdx,pkQueueDeskDataNode->lCardType);
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::exit()
{

	return	0;
}

int mtServiceRoomGrabLandlord::playGrabLandlordAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserCurAiIdx, int iQueueUserCurIdx, ULONGLONG* pullStartTime)
{
	/// 该ai已经叫过分了(可能叫得是0分)
	if (0 <= pkQueueDeskDataNode->plScore[iQueueUserCurAiIdx])
	{
		return 1;
	}

	/// 更改自己为已经叫分状态
	int iLandlordScore = getGrabLandlordMaxScore(pkQueueDeskDataNode);
	if (3 <= iLandlordScore)
	{
		pkQueueDeskDataNode->plScore[iQueueUserCurAiIdx] = 0;
		return 1;
	}

	/// 获得桌子上的真实人员列表
	DataWrite kDataWrite;
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lUserId           = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserCurAiIdx]->lUserId;
	kDataWrite.lServiceType      = lServiceType;
	kDataWrite.lUserIdLandlord   = -1;
	kDataWrite.lScore            = 0;

	srand((unsigned)time(NULL));
	if (0 == iLandlordScore || -1 == iLandlordScore)
	{
		/// 随机产生ai叫的分
		//iLandlordScore = (rand() % 3) + 1;
		iLandlordScore = 1;
	}
	else if (1 == iLandlordScore)
	{
		/// 随机产生ai叫的分
		//iLandlordScore = (rand() % 2);
		//if (0 == iLandlordScore)
		//{
		//	iLandlordScore = 3;
		//}
		//else
		//{
		//	iLandlordScore = 2;
		//}
		iLandlordScore = 0;
	}
	else
	{
		/*iLandlordScore = 3;*/
		iLandlordScore = 0;
	}

	kDataWrite.lScore  = iLandlordScore;
	pkQueueDeskDataNode->plScore[iQueueUserCurAiIdx] = kDataWrite.lScore;
	int iQueueDeskUserIdx;
	kDataWrite.lUserIdLandlord   = getQueueDeskLandlordUserId(pkQueueDeskDataNode, pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserCurAiIdx], &iQueueDeskUserIdx);

	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		/// 发送‘叫0分’ 消息 不抢地主
		//  当前真实用户
		if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx]
		 && pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomGrabLandlord226");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;
				memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				/// 服务器延迟处理ai叫分
				mtQueueSendPackAi::DataNode* pkDataNodeAi = (mtQueueSendPackAi::DataNode*)m_pkQueueMgr->m_pkQueueSendPackAi->popInitDataNode();
				if (pkDataNodeAi)
				{
					int iInsertHead = 0;
					pkDataNodeAi->ullTimeSend           = *pullStartTime + MT_SERVICE_GRAB_LANDLORD_TIME;
					*pullStartTime                      = pkDataNodeAi->ullTimeSend;
					pkDataNodeAi->iType                 = mtQueueSendPackAi::E_SEND_TYPE_SEND;
					pkDataNodeAi->pkQueueUserDataNode   = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx];
					pkDataNodeAi->pkQueuePacketDataNode = pkQueuePacketDataNode;
					m_pkQueueMgr->m_pkListManagerAi->push(pkDataNodeAi, &iInsertHead);
					if (iInsertHead)
					{
						SetEvent(m_hEventManagerAi);
					}
				}
				///m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx], pkQueuePacketDataNode);
			}
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceRoomGrabLandlord::getQueueDeskLandlordUserId(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode, int* piQueueDeskUserIdx)
{
	*piQueueDeskUserIdx                = -1;
	long lMaxScore                     = -1;                     /// 本次最大叫分
	int  iQueueUserDataNodeMaxScoreIdx = -1;                     /// 本次最大叫分用户在桌子上的位置
	if (3 <= pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex])
	{
		/// 设置该用户为地主
		lMaxScore                      = pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex];
		iQueueUserDataNodeMaxScoreIdx  = pkQueueUserDataNode->lUserDeskIndex;
		pkQueueDeskDataNode->plPlayerType[pkQueueUserDataNode->lUserDeskIndex] = mtQueueDesk::E_PLAYER_TYPE_LANDLORD;
	}
	else if(0 <= pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex])/// 现在可否确定出地主
	{
		lMaxScore                      = pkQueueDeskDataNode->plScore[pkQueueUserDataNode->lUserDeskIndex];
		iQueueUserDataNodeMaxScoreIdx  = pkQueueUserDataNode->lUserDeskIndex;

		int iQueueUserDataNodeIdx;
		for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
		{
			if (-1 == pkQueueDeskDataNode->plScore[iQueueUserDataNodeIdx])
			{
				if (3 > getGrabLandlordMaxScore(pkQueueDeskDataNode))
				{
					lMaxScore                     = -1;
					iQueueUserDataNodeMaxScoreIdx = -1;
					break;
				}
			}
			else
			{
				if (lMaxScore < pkQueueDeskDataNode->plScore[iQueueUserDataNodeIdx])
				{
					lMaxScore                     =  pkQueueDeskDataNode->plScore[iQueueUserDataNodeIdx];
					iQueueUserDataNodeMaxScoreIdx =  iQueueUserDataNodeIdx;
				}
			}
		}
	}

	/// 有地主产生了
	if (0 < lMaxScore && pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeMaxScoreIdx])
	{
		/// 设置该用户为地主
		*piQueueDeskUserIdx                                              = iQueueUserDataNodeMaxScoreIdx;
		pkQueueDeskDataNode->plPlayerType[iQueueUserDataNodeMaxScoreIdx] = mtQueueDesk::E_PLAYER_TYPE_LANDLORD;
		return pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeMaxScoreIdx]->lUserId;
	}

	*piQueueDeskUserIdx   = -1;
	return - 1;
}

int mtServiceRoomGrabLandlord::playRestartCardDeal(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	/// post消息给完端口，让他启动洗牌和发牌功能，给每个人发牌
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomGrabLandlord322");
	if (pkQueuePacketDataNode)
	{
		mtServiceCardDeal::DataRead    kDataRead;
		kDataRead.lStructBytes      = sizeof(kDataRead);
		kDataRead.lServiceType      = mtServiceMgr::E_SERVICE_ROOM_CARD_DEAL;
		kDataRead.lResult           = -1;
		pkQueuePacketDataNode->kWSABuf.len  = kDataRead.lStructBytes;
		memcpy(pkQueuePacketDataNode->pcData, &kDataRead, kDataRead.lStructBytes);
		pkQueuePacketDataNode->eDataType    = mtQueuePacket::E_DATA_TYPE_NOT_USED;
		if (m_pkQueueMgr->getQueueUser()->addDataNodePacket(pkQueueUserDataNode, pkQueuePacketDataNode))
		{ 
			PostQueuedCompletionStatus(m_hIOCP, MT_QUEUE_COMPLETION_STATUS_CUSTOM, (ULONG_PTR)pkQueueUserDataNode, &(pkQueuePacketDataNode->kDataOverLapped));
		}
		else
		{
			m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::playDoubleScoreAi(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	/// post消息给完端口，让他启加倍功能，
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomGrabLandlord348");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceRoomDoubleScore::DataRead  kDataRead;
		kDataRead.lStructBytes              = sizeof(kDataRead);
		kDataRead.lServiceType              = mtServiceMgr::E_SERVICE_ROOM_DOUBLE_SCORE;
		kDataRead.lDoubleFlag               = -1;
		pkQueuePacketDataNode->kWSABuf.len  = kDataRead.lStructBytes;
		memcpy(pkQueuePacketDataNode->pcData, &kDataRead, kDataRead.lStructBytes);
		pkQueuePacketDataNode->eDataType    = mtQueuePacket::E_DATA_TYPE_NOT_USED;
		if (m_pkQueueMgr->getQueueUser()->addDataNodePacket(pkQueueUserDataNode, pkQueuePacketDataNode))
		{
			PostQueuedCompletionStatus(m_hIOCP, MT_QUEUE_COMPLETION_STATUS_CUSTOM, (ULONG_PTR)pkQueueUserDataNode, &(pkQueuePacketDataNode->kDataOverLapped));
		}
		else
		{
			m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::playQueueUserSort(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	int iQueueUserDataNodeIdx;
	char pcCardTmp[17] = {0};
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[iQueueUserDataNodeIdx])
		{
			memcpy(pkQueueDeskDataNode->pcCardsTotal[iQueueUserDataNodeIdx] + sizeof(pcCardTmp), pkQueueDeskDataNode->pcCardsBase, sizeof(pkQueueDeskDataNode->pcCardsBase));
			arraggeCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserDataNodeIdx], sizeof(pcCardTmp) + sizeof(pkQueueDeskDataNode->pcCardsBase));
		}
		else
		{
			arraggeCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserDataNodeIdx], sizeof(pcCardTmp));
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::getGrabLandlordMaxScore(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	int iMaxScore = -1;
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx])
		{
			if (pkQueueDeskDataNode->plScore[iQueueUserDataNodeIdx] > iMaxScore)
			{
				iMaxScore = pkQueueDeskDataNode->plScore[iQueueUserDataNodeIdx];
			}
		}
	}

	return iMaxScore;
}

void mtServiceRoomGrabLandlord::arraggeCard(char pcCard[], long lNum )
{
	int iIdx = 0;
	char pcTmpCards[24];
	memcpy(pcTmpCards,pcCard,sizeof(pcTmpCards));
	memset(pcCard,MT_CARD_TERMINATE,lNum);
	for(; iIdx < lNum;iIdx++)
	{
		insertCard(pcCard, pcTmpCards[iIdx], iIdx);
	}
	//	insertCard(MTFL_CARD_TERMINATE);
}

void mtServiceRoomGrabLandlord::insertCard(char pcCard[], char cCard, long lCardSortNum)
{
	int iIdx = lCardSortNum - 1;
	//	if(MTFL_CARD_TERMINATE == cCard)
	//	{
	//		m_pcCards[m_iCardSortNum]	= MTFL_CARD_TERMINATE;
	//	}
	for(; iIdx >= 0; iIdx--)
	{
		if((pcCard[iIdx] & 0x0f) > (cCard & 0x0f))
		{
			pcCard[iIdx + 1]	= pcCard[iIdx];
		}
		else
		{
			break;
		}
	}

	pcCard[iIdx+1]	= cCard;
}

int mtServiceRoomGrabLandlord::playAiLandlordCardOut(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNodeAi, ULONGLONG* pullStartTime)
{
	mtQueuePacket::DataNode* pkQueuePacketDataNode    = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomGrabLandlord446");
	if (pkQueuePacketDataNode)
	{
		mtServiceCardOut::DataRead* pkDataReadCardOut = (mtServiceCardOut::DataRead*)pkQueuePacketDataNode->pcData;
		pkDataReadCardOut->lStructBytes               = sizeof(mtServiceCardOut::DataRead);
		pkDataReadCardOut->lServiceType               = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
		memset(pkDataReadCardOut->pcCard, MT_CARD_TERMINATE, sizeof(pkDataReadCardOut->pcCard));
		pkDataReadCardOut->lCardType                  = getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[pkQueueUserDataNodeAi->lUserDeskIndex], pkDataReadCardOut->pcCard);

// 		mtService::DataRun kDataRun;
// 		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
// 		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeAi;
// 
// 		m_pkServiceMgr->run(&kDataRun);

		/// 服务器延迟ai第一次出牌
		mtQueueSendPackAi::DataNode* pkDataNodeAi = (mtQueueSendPackAi::DataNode*)m_pkQueueMgr->m_pkQueueSendPackAi->popInitDataNode();
		if (pkDataNodeAi)
		{
			int iInsertHead = 0;
			pkDataNodeAi->ullTimeSend           = *pullStartTime + MT_SERVICE_AI_CARD_OUT_TIME;
			pkDataNodeAi->iType                 = mtQueueSendPackAi::E_SEND_TYPE_SEVICE;
			pkDataNodeAi->pkQueueUserDataNode   = pkQueueUserDataNodeAi;
			pkDataNodeAi->pkQueuePacketDataNode = pkQueuePacketDataNode;
			m_pkQueueMgr->m_pkListManagerAi->push(pkDataNodeAi, &iInsertHead);
			if (iInsertHead)
			{
				SetEvent(m_hEventManagerAi);
			}
		}
	}

	return 1;
}

int mtServiceRoomGrabLandlord::getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[])
{
	int iIdx = 0;
	int iCardType = 0;
	iIdx = 0;

	int iCardsNum = 0;
	while(pcSelfCard[iCardsNum] != MT_CARD_TERMINATE)
	{
		iCardsNum++;
	}

	if(iCardsNum <= 10)
	{
		if(iCardsNum == 10)
		{
			if(getAirPlane(pcSelfCard,pcOutCard,0,iCardsNum) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,iCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 9)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,9)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,9)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 8)
		{
			if(getAirPlane(pcSelfCard,pcOutCard,0,8) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,8)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,8)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 6)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION;
			}
			if(getSingleProgression(pcSelfCard,pcOutCard,0,6)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
		}
		if(iCardsNum >= 5)
		{
			if(getSingleProgression(pcSelfCard,pcOutCard,0,5)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}

			if(getTripleWithTwo(pcSelfCard,pcOutCard,0,5)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO;
			}

		}
		if(iCardsNum >= 4)
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}
		}
	}
	if((pcSelfCard[0] & 0x0f) == 13 && (pcSelfCard[1] & 0x0f)==14 && pcSelfCard[2] == MT_CARD_TERMINATE)
	{
		pcOutCard[0]	= pcSelfCard[0];
		pcOutCard[1]	= pcSelfCard[1];
		return mtServiceCardOut::MT_CARD_TYPE_ROCKET;
	}

	while(pcSelfCard[iIdx] != MT_CARD_TERMINATE)
	{
		if(pcSelfCard[iIdx + 1] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			iCardType	= mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			break;
		}
		else if((pcSelfCard[iIdx] & 0x0f) != (pcSelfCard[iIdx + 1] & 0x0f))
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			iCardType	= mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			break;
		}
		else if(pcSelfCard[iIdx + 2] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			iCardType		= mtServiceCardOut::MT_CARD_TYPE_PAIR;
			break;
		}
		else if((pcSelfCard[iIdx] & 0x0f) != (pcSelfCard[iIdx + 2] & 0x0f))
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			iCardType		= mtServiceCardOut::MT_CARD_TYPE_PAIR;
			break;
		}
		else if(pcSelfCard[iIdx + 3] == MT_CARD_TERMINATE)
		{
			pcOutCard[0]	= pcSelfCard[iIdx];
			pcOutCard[1]	= pcSelfCard[iIdx+1];
			pcOutCard[2]	= pcSelfCard[iIdx+2];
			iCardType		= mtServiceCardOut::MT_CARD_TYPE_TRPILE;
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
				iCardType		= mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
				break;
			}
			else
			{
				iResult	= getPairCard(pcSelfCard,pcOutCard+3,0);
				if(iResult >= 0)
				{
					iCardType		= mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO;
					break;
				}
				else
				{
					iCardType		= mtServiceCardOut::MT_CARD_TYPE_TRPILE;
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
		return mtServiceCardOut::MT_CARD_TYPE_BOMB;
	}
}

int	mtServiceRoomGrabLandlord::getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomGrabLandlord::getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomGrabLandlord::getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int mtServiceRoomGrabLandlord::getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomGrabLandlord::getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomGrabLandlord::getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int mtServiceRoomGrabLandlord:: getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos)
{
	int iSelfIdx	= iCardPos;
	while(pcSelfCards[iSelfIdx + 3] != MT_CARD_TERMINATE)
	{
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

int mtServiceRoomGrabLandlord::getPairCard(char* pcSelfCards,char* pcOutCard ,int iCardBegPos)
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

int mtServiceRoomGrabLandlord::getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos)
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

