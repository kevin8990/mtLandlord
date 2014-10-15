

#include "mtServiceRoomExit.h"
#include "mtServiceMgr.h"
#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomDoubleScore.h"
#include "mtServiceShowCard.h"
#include "mtServiceCardOut.h"
#include "mtPrint.h"

mtServiceRoomExit::mtServiceRoomExit()
{

}

mtServiceRoomExit::~mtServiceRoomExit()
{

}

int mtServiceRoomExit::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	m_pkServiceMgr          = pkDataInit->pkServiceMgr;


	return	1;
}

int mtServiceRoomExit::run( DataRun* pkDataRun )
{
	
	DataRead*					pkDataRead            = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode*      pkQueueUserDataNode	  = pkDataRun->pkQueueUserDataNode;
	mtPrint(pkDataRead);
	if (NULL == pkQueueUserDataNode)
	{
		MT_DEBUG_PTCL("\nExit Room,but userDataNode is NULL!");
		return 0;
	}
	SYSTEMTIME curTime;
	GetLocalTime(&curTime);
	MT_DEBUG_PTCL("\nUser exit room![%x - %d] ThreadId:%d %d:%d:%d",pkQueueUserDataNode,pkQueueUserDataNode->lUserId,GetCurrentThreadId(),
		curTime.wHour,curTime.wMinute,curTime.wSecond);

	mtQueueHall::DataRoom*	pkQueueHallDataRoom       = m_pkQueueMgr->getQueueHall()->getRoom(pkQueueUserDataNode->lSpaceId
		                                                                                    , pkQueueUserDataNode->lRoomId);

	if (NULL == pkQueueHallDataRoom)
	{
		return 0;
	}

	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataRun->pkQueuePacketDataNode;
	mtQueueDesk::DataNode* pkQueueDeskDataNode        = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;

	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
	if (NULL == pkQueueDeskDataNode)
	{
		/// 用户还没入桌子 现在在单人队列
		if (NULL != pkQueueHallDataRoom)
		{
			kDataWrite.lStatusExit         = E_STATUS_EXIT_NORMAL;
			if (0 < kDataWrite.lUserId && kDataWrite.lUserId < MT_QUEUE_USER_ID_AI_START)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
			}

			pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
				                                         ? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);

			/// pkQueueHallDataRoom->kQueueOneList.remove(&pkQueueUserDataNode);

			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit78");
			if (NULL != pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
			}
			
			pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
		}
		
///		SetEvent(pkQueueHallDataRoom->hEvent);
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);

	if(pkQueueUserDataNode->pkQueueDeskDataNode == NULL) 
	{
		/// 用户还没入桌子 现在在单人队列
		if (pkQueueHallDataRoom)
		{
			kDataWrite.lStatusExit         = E_STATUS_EXIT_NORMAL;
			if (0 < kDataWrite.lUserId && kDataWrite.lUserId < MT_QUEUE_USER_ID_AI_START)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
			}
			 
			pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
				? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
			/// pkQueueHallDataRoom->kQueueOneList.remove(&pkQueueUserDataNode);

			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit113");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
			}

			pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
		}

		LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
		return 1;
	}

	pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

	if (   NULL == pkQueueDeskDataNode->pkQueueUserDataNode[1]
	    || NULL == pkQueueDeskDataNode->pkQueueUserDataNode[2]
		|| 0 == pkQueueDeskDataNode->plIsWaitStart[0]
		|| 0 == pkQueueDeskDataNode->plIsWaitStart[1]
		|| 0 == pkQueueDeskDataNode->plIsWaitStart[2])
	{
		int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
		int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);
		if (0 == pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]
		&& (1 == pkQueueDeskDataNode->plRoundOver[iIdxNext] || 1 == pkQueueDeskDataNode->plRoundOver[iIdxNextNext]))
		{
			/// 该我上传结果，我退出
			kDataWrite.lStatusExit         = E_STATUS_EXIT_FORCE;
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit104");
			if (NULL != pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
			}
		}
		else
		{
			kDataWrite.lStatusExit         = E_STATUS_EXIT_NORMAL;
			int		iUserIdx;
			for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
			{
				if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
				&& mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
				{
					pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit122");
					if (NULL != pkQueuePacketDataNode)
					{
						pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
						memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);

						mtPrint(&kDataWrite);

						m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
					}
				}
			}
		}
	}
	else /// 三人都已经点击‘等待开始’
	{
		kDataWrite.lStatusExit         = E_STATUS_EXIT_FORCE;
		pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit137");
		if (NULL != pkQueuePacketDataNode)
		{
			pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
			memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
			mtPrint(&kDataWrite);
			m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
		}
	}
	
	if (NULL == pkQueueHallDataRoom)
	{
		pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITED;
		LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
		return 1;
	}

	/// 该用户在两人桌子队列
	int iCardsOutStyle      = 0;
	int iExeServiceTypeNext = mtServiceMgr::E_SERVICE_ROOM_END;
	mtQueueUser::DataNode* pkQueueUserDataNodeReplaceAi = NULL;
	if (2 == pkQueueDeskDataNode->lQueue)
	{
	    runRoomExitQueueDeskTwo(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueHallDataRoom);

	}
	else if(3 == pkQueueDeskDataNode->lQueue) /// 用户在三人桌子
	{
		int iDeskExit = pkQueueDeskDataNode->lDeskExit;

        runRoomExitQueueDeskThree(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueHallDataRoom
			                     , pkDataRead, &pkQueueUserDataNodeReplaceAi, &iExeServiceTypeNext, &iCardsOutStyle);


		if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER == iDeskExit)
		{
			pkQueueDeskDataNode->lDeskExit  = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER;

		}
	}
	else /// 用户在单人队列
	{
		MT_DEBUG_PTCL("\nExit Queue:%d Desk:%x",pkQueueDeskDataNode->lQueue,pkQueueUserDataNode->pkQueueDeskDataNode);
	}

	pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITED;
	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	if (NULL != pkQueueUserDataNodeReplaceAi)
	{
		/// 抢地主
		if (mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD == iExeServiceTypeNext)
		{
			MT_DEBUG(4, "User(UserId=%d) Grab Landlord  12121212121212......\n", pkQueueUserDataNodeReplaceAi->lUserId);
			if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueDeskDataNode->lDeskExit)
			{
				playReplaceAiGrabLandlord(pkQueueUserDataNodeReplaceAi);
			}
		}

		/// 出牌
		if (mtServiceMgr::E_SERVICE_ROOM_CARD_OUT == iExeServiceTypeNext)
		{
			MT_DEBUG(4, "User(UserId=%d, CardsOutStyle=%d) PlayOutCard  11111111111111......\n", pkQueueUserDataNodeReplaceAi->lUserId, iCardsOutStyle);
			/// 先判断是否有人已经赢了(要上传结果了)
			if (0 < getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[0])
			 && 0 < getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[1])
			 && 0 < getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[2]))
			{
				if (iCardsOutStyle)
				{
					/// 轮到你出牌
					if (pkQueueDeskDataNode->lUserDeskIdxCardOutNext == pkQueueUserDataNodeReplaceAi->lUserDeskIndex)
					{
						/// 如果当前没有人打住你的牌，继续选择出牌
						if (pkQueueDeskDataNode->lQueueUserDeskIdx == pkQueueUserDataNodeReplaceAi->lUserDeskIndex)
						{
							MT_DEBUG(4, "User(UserId=%d) PlayOutCard  888888888888......\n", pkQueueUserDataNodeReplaceAi->lUserId);
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							pkDataRead->lCardType = getPlayOutCardFirst(pkQueueDeskDataNode, pkQueueUserDataNodeReplaceAi->lUserDeskIndex, pkDataRead);
						}
						else /// 压别人的牌
						{
							MT_DEBUG(4, "User(UserId=%d) PlayOutCard  999999999999......\n", pkQueueUserDataNodeReplaceAi->lUserId);
							pkDataRead->lCardType = pkQueueDeskDataNode->lCardType;
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							getPlayOutCard(pkQueueDeskDataNode, pkQueueUserDataNodeReplaceAi->lUserDeskIndex, pkDataRead);

						}
					}
				}

				if (mtServiceCardOut::MT_CARD_TYPE_PASS == pkDataRead->lCardType)
				{
					memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
				}

				if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueDeskDataNode->lDeskExit)
				{
					playReplaceAiCardOut(pkQueueUserDataNodeReplaceAi, pkDataRead);

				}
			}
		}

		/// 上传结果
		if (mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER == iExeServiceTypeNext)
		{
			MT_DEBUG(4, "User(UserId=%d) RoundOver  32132131213321......\n", pkQueueUserDataNodeReplaceAi->lUserId);
			if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueDeskDataNode->lDeskExit)
			{
				playReplaceAiRoundOver(pkQueueDeskDataNode, pkQueueUserDataNodeReplaceAi);
			}
		}
	}

///	SetEvent(pkQueueHallDataRoom->hEvent);
	return	1;
}

int mtServiceRoomExit::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceRoomExit::getQueueUserPreQueueUser(int iQueueUserCurIdx)
{
	if (2 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx - 2;
	}

	return iQueueUserCurIdx + 1;
}

int mtServiceRoomExit::initQueueUserDataNodeAiReplace(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
													  , int iQueueUserDeskIdx, mtQueueUser::DataNode* pkQueueUserDataNodeAi)
{
	*pkQueueUserDataNodeAi                            = *pkQueueUserDataNode;
	pkQueueUserDataNodeAi->uiSockets                  = INVALID_SOCKET;
	pkQueueUserDataNodeAi->lUserDeskIndex             = iQueueUserDeskIdx;
	pkQueueUserDataNodeAi->pkQueueDeskDataNode        = pkQueueDeskDataNode;
	pkQueueUserDataNodeAi->lUserExit                  = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING;
	memcpy(pkQueueUserDataNodeAi->pcUserAccountNum, pkQueueUserDataNode->pcUserAccountNum, sizeof(pkQueueUserDataNode->pcUserAccountNum));
	memcpy(pkQueueUserDataNodeAi->pcUserNickName, pkQueueUserDataNode->pcUserNickName, sizeof(pkQueueUserDataNode->pcUserNickName));
	pkQueueUserDataNodeAi->pkQueuePacketDataNodeHead  = NULL;
	memset(pkQueueUserDataNodeAi->pkListNode, 0, sizeof(pkQueueUserDataNodeAi->pkListNode));
	pkQueueDeskDataNode->plPlayerMode[pkQueueUserDataNodeAi->lUserDeskIndex]        = mtQueueDesk::E_PLAYER_MODE_AI;
	pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueUserDataNodeAi->lUserDeskIndex] = pkQueueUserDataNodeAi;

	m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode    = pkQueueUserDataNodeAi;
	MT_DEBUG_PTCL("initQueueUserDataNodeAiReplace:%x",pkQueueUserDataNodeAi);

	return 1;
}

int mtServiceRoomExit::playReplaceAiGrabLandlord(mtQueueUser::DataNode* pkQueueUserDataNodeAi)
{
	/// 响应抢地主
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit292");
	if (pkQueuePacketDataNode)
	{
		mtServiceRoomGrabLandlord::DataRead* pkDataRead = (mtServiceRoomGrabLandlord::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes                        = sizeof(mtServiceRoomGrabLandlord::DataRead);
		pkDataRead->lServiceType                        = mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD;
		pkDataRead->lScore                              = 0;

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeAi;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomExit::playReplaceAiDoubleScore(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	/// 响应加倍
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit318");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceRoomDoubleScore::DataRead* pkDataRead = (mtServiceRoomDoubleScore::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes           = sizeof(mtServiceRoomDoubleScore::DataRead);
		pkDataRead->lServiceType           = mtServiceMgr::E_SERVICE_ROOM_DOUBLE_SCORE;
		pkDataRead->lDoubleFlag            = -1;

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNode;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomExit::playReplaceAiShowCard(mtQueueUser::DataNode* pkQueueUserDataNodeAi)
{
	/// 响应明牌
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit343");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceShowCard::DataRead* pkDataRead = (mtServiceShowCard::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes                        = sizeof(mtServiceShowCard::DataRead);
		pkDataRead->lServiceType                        = mtServiceMgr::E_SERVICE_ROOM_SHOW_CARD;
		pkDataRead->lShowCard                           = 0;
		memset(pkDataRead->pcCard, 100, sizeof(pkDataRead->pcCard));

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeAi;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomExit::playReplaceAiCardOut(mtQueueUser::DataNode* pkQueueUserDataNodeAi, DataRead* pkDataRead)
{
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit369");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceCardOut::DataRead* pkDataReadCardOut = (mtServiceCardOut::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataReadCardOut->lStructBytes                        = sizeof(mtServiceCardOut::DataRead);
		pkDataReadCardOut->lServiceType                        = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
		pkDataReadCardOut->lCardType                           = pkDataRead->lCardType;
		memcpy(pkDataReadCardOut->pcCard, pkDataRead->pcCard, sizeof(pkDataRead->pcCard));

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeAi;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomExit::playReplaceAiRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNodeAi)
{
	/// 响应建立三人桌子
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("RoomExit395");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceRoomRoundOver::DataRead* pkDataReadRoundOver = (mtServiceRoomRoundOver::DataRead*)pkQueuePacketDataNode->pcData;
		pkDataReadRoundOver->lStructBytes                     = sizeof(mtServiceRoomRoundOver::DataRead);
		pkDataReadRoundOver->lServiceType                     = mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER;
		calcRoundOver(pkQueueDeskDataNode, pkQueueUserDataNodeAi, &pkDataReadRoundOver->lGold);

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeAi;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomExit::calcRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode, long* plGold)
{
	*plGold = 0;
	if (NULL == pkQueueDeskDataNode || NULL == pkQueueUserDataNode)
	{
		return 0;
	}

	/// 这个人是地主
	int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
	int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);
	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[pkQueueUserDataNode->lUserDeskIndex])
	{
		if (0 != pkQueueDeskDataNode->plRoundOverGold[iIdxNext])
		{
			*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNext] * 2;
		}
		else
		{
			*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNextNext] * 2;
		}
	}
	else /// 这个人是农民
	{
		if (mtQueueDesk::E_PLAYER_TYPE_FARMER == pkQueueDeskDataNode->plPlayerType[iIdxNext])
		{
			if (0 != pkQueueDeskDataNode->plRoundOverGold[iIdxNext])
			{
				*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNext];
			}
			else
			{
				*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNextNext] / 2;
			}
		}
		else
		{
			if (0 != pkQueueDeskDataNode->plRoundOverGold[iIdxNextNext])
			{
				*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNextNext];
			}
			else
			{
				*plGold = pkQueueDeskDataNode->plRoundOverGold[iIdxNext] / 2;
			}
		}
	}

	return 1;
}

int mtServiceRoomExit::runRoomExitQueueDeskTwo(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
											   , mtQueueHall::DataRoom* pkQueueHallDataRoom)
{
	/// 获得下家用户在桌子上的位置
	int iQueueUserDeskNextIdx = -1;
	if (0 == pkQueueUserDataNode->lUserDeskIndex)
	{
		iQueueUserDeskNextIdx = 1;
	}
	else
	{
		iQueueUserDeskNextIdx = 0;
	}

	//  下家是ai用户
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDeskNextIdx] 
		&& NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx])
	{
		/// 把这个人从桌子上移除
		if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
		{
			m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 0;
			m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
			m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusRun            = 2;
			///m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode            = NULL;
		}

		pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 回收ai节点
		long lNextUserId = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserId;
		if (0 < lNextUserId && lNextUserId < MT_QUEUE_USER_ID_AI_START)
		{
			m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iReplaceAi             = 0;
			m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusExit            = 1;
			m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusRun             = 2;
			m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->pkUserNode             = NULL;
		}
		pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 从两人桌子队列，移除桌子
		pkQueueDeskDataNode->lDeskExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 在线人数更新
		pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
			                                        ? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
	}
	else /// 从新回到单人队列
	{
		if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx])
		{
			/// 从两人桌子队列，移除桌子
			pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
				? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);

			/// 把这个人从桌子上移除
			if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusRun            = 2;
				///m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode            = NULL;
			}

			pkQueueUserDataNode->lUserExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 从两人桌子队列，移除桌子
			pkQueueDeskDataNode->lDeskExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 把另外这个人重新放入到单人队列等待重新组桌子 
		//	pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lIsWaitStart = 0;
			
		}
	}

	return 1;
}

int mtServiceRoomExit::runRoomExitQueueDeskThree(mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueueUser::DataNode* pkQueueUserDataNode
											   , mtQueueHall::DataRoom* pkQueueHallDataRoom, DataRead* pkDataRead
											   , mtQueueUser::DataNode** pkQueueUserNodeReplaceAi, int* iServiceType, int* iCardsOutStyle)
{
	MT_DEBUG_PTCL("\nUser Exit QueueDeskThreee![%x - %d] [D:%x]",pkQueueUserDataNode,pkQueueUserDataNode->lUserId,pkQueueDeskDataNode);

	*pkQueueUserNodeReplaceAi  = NULL;
	*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_END;
	*iCardsOutStyle            = 0; /// (0 -客户端退出时告知的方式，1 -主动判断方式)
	/// 获得下家用户在桌子上的位置
	int iQueueUserDeskNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);

	/// 获得下下家用户在桌子上的位置
	int iQueueUserDeskNextNextIdx = getQueueUserNextQueueUser(iQueueUserDeskNextIdx);

	//  下家是ai用户
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDeskNextIdx])
	{
		//  ai下家还是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDeskNextNextIdx])
		{
			///// 把这个人从桌子上移除
			if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
			{
				mtQueueMgr::UserNodeReplaceAi* userNodeReplaceAi = m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId);
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi             = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit            = 1;
				m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusRun             = 2;
				//m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode             = NULL;

			}

			pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueUserDataNode->lUserDeskIndex]->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 回收第一个ai节点
			long lNextUserId = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserId;
			if (0 < lNextUserId && lNextUserId < MT_QUEUE_USER_ID_AI_START)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iReplaceAi             = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusExit            = 1;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusRun             = 2;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->pkUserNode             = NULL;
			}
			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserExit               = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 回收第二个ai节点
			lNextUserId = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextNextIdx]->lUserId;
			if (0 < lNextUserId && lNextUserId < MT_QUEUE_USER_ID_AI_START)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iReplaceAi             = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusExit            = 1;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->iStatusRun             = 2;
				m_pkQueueMgr->getUserNodeReplaceAi(lNextUserId)->pkUserNode             = NULL;
			}
			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextNextIdx]->lUserExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 从三人桌子队列，移除桌子
			pkQueueDeskDataNode->lDeskExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 在线人数更新+
			pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
				? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
		}
		else /// 可能需要从新回到两人桌子  下下家是真实用户
		{
			if (0 == pkQueueDeskDataNode->plIsWaitStart[0]
			|| 0 == pkQueueDeskDataNode->plIsWaitStart[1]
			|| 0 == pkQueueDeskDataNode->plIsWaitStart[2])
			{
				int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
				int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);

				if (0 == pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]
				&& (1 == pkQueueDeskDataNode->plRoundOver[iIdxNext] || 1 == pkQueueDeskDataNode->plRoundOver[iIdxNextNext]))
				{
					/// 在线人数更新
					pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
						? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
					/// 用ai代替这个用户
					/// 启动一个ai节点
					mtQueueUser::DataNode*		pkQueueUserDataNodeAi = (mtQueueUser::DataNode*)m_pkQueueMgr->getQueueUser()->pop();
					if(NULL != pkQueueUserDataNodeAi)
					{
						initQueueUserDataNodeAiReplace(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueUserDataNode->lUserDeskIndex, pkQueueUserDataNodeAi);

						if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
						{
							m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi   = 1;
							m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit  = 1;
						}

						pkQueueUserDataNode->lUserExit                                                 = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
						pkQueueUserDataNode->lUserDeskIndex      = -1;
						pkQueueUserDataNode->pkQueueDeskDataNode = NULL;

						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						/// 接着就要出牌了
						MT_DEBUG(4, "User(UserId=%d) RoundOver  222222222222222222......\n", pkQueueUserDataNodeAi->lUserId);
					}
				}
				else
				{
					/// 把要退出的人从桌子上移除
					if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
					{
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi    = 0;
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit   = 1;
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusRun    = 2;
						///m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode    = NULL;
					}
					pkQueueUserDataNode->lUserExit                                                  = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
					MT_DEBUG_PTCL("\nUser exit desk three line_701! [%x - %d]",pkQueueUserDataNode,pkQueueUserDataNode->lUserId);
					/// 在线人数更新
					pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
						? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
				}
			}
			else /// 三个人都已经开始等待了，需要启动ai来代替该人
			{
				/// 在线人数更新
				pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
					? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
				/// 用ai代替这个用户
				/// 启动一个ai节点
				mtQueueUser::DataNode*		pkQueueUserDataNodeAi = (mtQueueUser::DataNode*)m_pkQueueMgr->getQueueUser()->pop();
				if(NULL != pkQueueUserDataNodeAi)
				{
					initQueueUserDataNodeAiReplace(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueUserDataNode->lUserDeskIndex, pkQueueUserDataNodeAi);

					if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
					{
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi   = 1;
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit  = 1;
					}

					pkQueueUserDataNode->lUserExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
					pkQueueUserDataNode->lUserDeskIndex      = -1;
					pkQueueUserDataNode->pkQueueDeskDataNode = NULL;

					/// 如果这个用户还没有叫地主，ai就立即开始叫地主服务
					if (-1 == pkDataRead->lStatusExit)
					{
						/// 接着就要开始抢地主了
						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						MT_DEBUG(4, "User(UserId=%d) Grab Landlord  131313131313131313......\n", pkQueueUserDataNodeAi->lUserId);
					}
					else if(-2 == pkDataRead->lStatusExit)
					{
						/// 如果这个用户还没有加倍，ai就立即开始加倍服务
						/// 获得ai上家家用户在桌子上的位置
						int iQueueUserDeskPreIdx = getQueueUserPreQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);

						//这里是：下一家是AI，下下一家是真实用户
						/// 如果AI上家还是AI 
						if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDeskPreIdx])
						{
							int iQueueUserDeskPrePreIdx = getQueueUserPreQueueUser(iQueueUserDeskPreIdx);
							playReplaceAiDoubleScore(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskPrePreIdx]);
						}
						else
						{
							playReplaceAiDoubleScore(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskPreIdx]);
						}
					}
					else if (-3 == pkDataRead->lStatusExit) /// 该我明牌，ai就立即开始明牌服务
					{
						playReplaceAiShowCard(pkQueueUserDataNodeAi);

						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;

						/// 接着就要出牌了
						MT_DEBUG(4, "User(UserId=%d) PlayOutCard  4444444444444444444......\n", pkQueueUserDataNodeAi->lUserId);
					}
					else if (-4 == pkDataRead->lStatusExit) /// 该我出牌，ai就立即开始出牌服务
					{
						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						/// 接着就要出牌了
						MT_DEBUG(4, "User(UserId=%d) PlayOutCard  222222222222222222......\n", pkQueueUserDataNodeAi->lUserId);
					}
					else
					{
						int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
						int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);

						*iCardsOutStyle          = 1;
						int iQueueUserNodePreIdx = getQueueUserPreQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);
						int iQueueUserNodeNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);
						if ((-1 != pkQueueDeskDataNode->plScore[iQueueUserNodePreIdx] || pkQueueDeskDataNode->lQueueUserDefaultLandlord == pkQueueUserDataNodeAi->lUserId)
							&& -1 == pkQueueDeskDataNode->plScore[pkQueueUserDataNodeAi->lUserDeskIndex]
							&& 3 > pkQueueDeskDataNode->plScore[iQueueUserNodePreIdx]
							&& 0 != pkQueueDeskDataNode->plPlayerType[iQueueUserNodeNextIdx])
						{
							/// 接着就要开始抢地主了
							*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD;
							*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
							MT_DEBUG(4, "User(UserId=%d) Grab Landlord  141414141414141414......\n", pkQueueUserDataNodeAi->lUserId);
						}
						else if (pkQueueDeskDataNode->lUserDeskIdxCardOutNext == pkQueueUserDataNodeAi->lUserDeskIndex)
						{
							*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
							*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
							/// 接着就要出牌了
							MT_DEBUG(4, "User(UserId=%d) PlayOutCard  333333333333333333333......\n", pkQueueUserDataNodeAi->lUserId);
						}
						else if (0 == pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]
						&& (1 == pkQueueDeskDataNode->plRoundOver[iIdxNext] || 1 == pkQueueDeskDataNode->plRoundOver[iIdxNextNext]))
						{
							*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER;
							*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
							/// 接着就要出牌了
							MT_DEBUG(4, "User(UserId=%d) RoundOver  3333333333......\n", pkQueueUserDataNodeAi->lUserId);
							
						}
					}
				}
			}
		}
	}
	else///下一家是真实用户
	{
		if (0 == pkQueueDeskDataNode->plIsWaitStart[0]
		|| 0 == pkQueueDeskDataNode->plIsWaitStart[1]
		|| 0 == pkQueueDeskDataNode->plIsWaitStart[2])
		{
			int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
			int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);
			if (0 == pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]
			&& (1 == pkQueueDeskDataNode->plRoundOver[iIdxNext] || 1 == pkQueueDeskDataNode->plRoundOver[iIdxNextNext]))
			{
				/// 在线人数更新

				pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
					? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
				/// 用ai代替这个用户
				/// 启动一个ai节点
				mtQueueUser::DataNode*		pkQueueUserDataNodeAi = (mtQueueUser::DataNode*)m_pkQueueMgr->getQueueUser()->pop();
				if(NULL != pkQueueUserDataNodeAi)
				{
					initQueueUserDataNodeAiReplace(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueUserDataNode->lUserDeskIndex, pkQueueUserDataNodeAi);

					if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
					{
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi   = 1;
						m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit  = 1;
					}

					pkQueueUserDataNode->lUserExit                                                 = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
					pkQueueUserDataNode->lUserDeskIndex      = -1;
					pkQueueUserDataNode->pkQueueDeskDataNode = NULL;

					*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER;
					*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
					/// 接着就要出牌了
					MT_DEBUG(4, "User(UserId=%d) RoundOver  222222222222222222......\n", pkQueueUserDataNodeAi->lUserId);
				}
			}
			else
			{
				/// 把要退出的人从桌子上移除
				if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
				{
					m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 0;
					m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
					m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusRun            = 2;
					///m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->pkUserNode            = NULL;
				}
				pkQueueUserDataNode->lUserExit                              = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
				MT_DEBUG_PTCL("\nUser exit desk three line_860! [%x - %d]",pkQueueUserDataNode,pkQueueUserDataNode->lUserId);
				/// 在线人数更新
				pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
					? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);
			}
		}
		else /// 三个人都已经开始等待了，需要启动ai来代替该人
		{
			/// 在线人数更新
			pkQueueHallDataRoom->lOnlinePersonNumber = ((0 < pkQueueHallDataRoom->lOnlinePersonNumber)
				? (pkQueueHallDataRoom->lOnlinePersonNumber - 1) : 0);

			/// 用ai代替这个用户
			/// 启动一个ai节点
			mtQueueUser::DataNode*		pkQueueUserDataNodeAi = (mtQueueUser::DataNode*)m_pkQueueMgr->getQueueUser()->pop();
			if(NULL != pkQueueUserDataNodeAi)
			{
				initQueueUserDataNodeAiReplace(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueUserDataNode->lUserDeskIndex, pkQueueUserDataNodeAi);

				if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START)
				{
					m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iReplaceAi            = 1;
					m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId)->iStatusExit           = 1;
				}
				pkQueueUserDataNode->lUserExit           = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;
				pkQueueUserDataNode->lUserDeskIndex      = -1;
				pkQueueUserDataNode->pkQueueDeskDataNode = NULL;

				/// 如果这个用户还没有叫地主，ai就立即开始叫地主服务
				if (-1 == pkDataRead->lStatusExit)
				{
					/// 接着就要开始抢地主了
					*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD;
					*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
					MT_DEBUG(4, "User(UserId=%d) Grab Landlord  1515151515151515......\n", pkQueueUserDataNodeAi->lUserId);
				}
				else if(-2 == pkDataRead->lStatusExit)
				{
					/// 如果这个用户还没有加倍，ai就立即开始加倍服务
					/// 获得ai上家家用户在桌子上的位置
					int iQueueDeskUserPreIdx = getQueueUserPreQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);

					/// 如果AI上家还是AI
					if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueDeskUserPreIdx])
					{
						int iQueueDeskUserPrePreIdx = getQueueUserPreQueueUser(iQueueDeskUserPreIdx);
						playReplaceAiDoubleScore(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserPrePreIdx]);
					}
					else
					{
						playReplaceAiDoubleScore(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueDeskUserPreIdx]);
					}
				}
				else if (-3 == pkDataRead->lStatusExit) /// 该我明牌，ai就立即开始明牌服务
				{
					playReplaceAiShowCard(pkQueueUserDataNodeAi);
					*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
					*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;

					/// 接着就要出牌了
					MT_DEBUG(4, "User(UserId=%d) PlayOutCard  5555555555555555......\n", pkQueueUserDataNodeAi->lUserId);
				}
				else if (-4 == pkDataRead->lStatusExit) /// 该我出牌，ai就立即开始出牌服务
				{
					*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
					*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
					/// 接着就要出牌了
					MT_DEBUG(4, "User(UserId=%d) PlayOutCard  6666666666666666666......\n", pkQueueUserDataNodeAi->lUserId);
				}
				else
				{
					int iIdxNext     = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);
					int iIdxNextNext = getQueueUserNextQueueUser(iIdxNext);

					*iCardsOutStyle          = 1;
					int iQueueUserNodePreIdx = getQueueUserPreQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);
					int iQueueUserNodeNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNodeAi->lUserDeskIndex);
					if ((-1 != pkQueueDeskDataNode->plScore[iQueueUserNodePreIdx] || pkQueueDeskDataNode->lQueueUserDefaultLandlord == pkQueueUserDataNodeAi->lUserId)
						&& -1 == pkQueueDeskDataNode->plScore[pkQueueUserDataNodeAi->lUserDeskIndex]
						&& 3 > pkQueueDeskDataNode->plScore[iQueueUserNodePreIdx]
						&& 0 != pkQueueDeskDataNode->plPlayerType[iQueueUserNodeNextIdx])
					{
						/// 接着就要开始抢地主了
						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_GRAD_LANDLORD;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						MT_DEBUG(4, "User(UserId=%d) Grab Landlord  1616161616161616......\n", pkQueueUserDataNodeAi->lUserId);
					}
					else if (pkQueueDeskDataNode->lUserDeskIdxCardOutNext == pkQueueUserDataNodeAi->lUserDeskIndex)
					{
						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_CARD_OUT;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						/// 接着就要出牌了
						MT_DEBUG(4, "User(UserId=%d) PlayOutCard  77777777777777777777......\n", pkQueueUserDataNodeAi->lUserId);
					}
					else if (0 == pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]
					&& (1 == pkQueueDeskDataNode->plRoundOver[iIdxNext] || 1 == pkQueueDeskDataNode->plRoundOver[iIdxNextNext]))
					{
						*iServiceType              = mtServiceMgr::E_SERVICE_ROOM_ROUND_OVER;
						*pkQueueUserNodeReplaceAi  = pkQueueUserDataNodeAi;
						/// 接着就要出牌了
						MT_DEBUG(4, "User(UserId=%d) RoundOver  4444444444......\n", pkQueueUserDataNodeAi->lUserId);

					}
				}
			}
		}
	}

	return 1;
}

int mtServiceRoomExit::exit()
{

	return	0;
}

int mtServiceRoomExit::getQueueUserLeftCards(char* pcCard)
{
	int iCardNum = 0;
	while(pcCard[iCardNum] != MT_CARD_TERMINATE)
	{
		iCardNum++;
	}

	return iCardNum;
}

int mtServiceRoomExit::getPlayOutCardFirst(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, DataRead* pkDataRead)
{
	/// 如果我是地主
	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[iQueueUserIdx])
	{
		/// 直接出牌
		return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard);
	}

	/// 自己手里是不是一手牌(可以先走)
	if (-1 != IsSeriesCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard))
	{
		/// 直接出牌
		return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard);
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
					, pkDataRead->pcCard, mtServiceCardOut::MT_CARD_TYPE_SINGLE, iCardsSelfNum);
				}
			}
			else if(2 == iCardsEnemyNum) /// 等于2张
			{
				/// 出对子以外的牌
				if (2 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					, pkDataRead->pcCard, mtServiceCardOut::MT_CARD_TYPE_PAIR, iCardsSelfNum);
				}
			}
			else if(3 == iCardsEnemyNum) /// 等于3张
			{
				/// 出三支以外的牌
				if (3 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					, pkDataRead->pcCard, mtServiceCardOut::MT_CARD_TYPE_TRPILE, iCardsSelfNum);
				}
			}
			else if(4 == iCardsEnemyNum) /// 等于4张
			{
				/// 出三带一以外的牌
				if (4 <= iCardsSelfNum)
				{
					return playOutCardsNotByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]
					, pkDataRead->pcCard, mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE, iCardsSelfNum);
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
			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
		else if(2 == iCardsSameFamilyNum) /// 等于2张
		{
			/// 出对子，没有对子出单支
			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
		else if(3 == iCardsSameFamilyNum) /// 等于3张
		{
			/// 出三支，没有三支出对子，没有对子出单支
			if (-1 != getTripleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}

			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
		else if(4 == iCardsSameFamilyNum) /// 等于4张
		{
			/// 出三带一，没有三支，出对子，没有对子出单支
			if (-1 != getTripleWithOne(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0, 4))
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}

			if (-1 != getTripleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}

			if (-1 != getPairCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}

			if (-1 != getSingleCard(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard, 0))
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
	}

	return getPlayOutCardFirst(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard);
}

int mtServiceRoomExit::getPlayOutCard(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iQueueUserIdx, DataRead* pkDataRead)
{
	/// 如果下家是对家(小心出牌，别送走对家)
	int iQueueUserNextIdx = getQueueUserNextQueueUser(iQueueUserIdx);
	/// 下家是对家
	if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] != pkQueueDeskDataNode->plPlayerType[iQueueUserNextIdx])
	{
		///MT_DEBUG(4, "User PlayOutCard  200......\n");
		/// 下家手中剩余牌数
		int iUserNextCardsNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserNextIdx]);

		/// 当前打出的牌面的牌数
		int iCardsRecvNum     = getCardsNum(pkQueueDeskDataNode->pcCardsCur);

		/// 当前我的牌面的牌数
		int iCardsSelfNum     = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

		/// 如果当前牌面数和地主手里的剩余牌数相等(则要出大牌，别放走对家)
		if (iUserNextCardsNum == iCardsRecvNum)
		{
			////MT_DEBUG(4, "User PlayOutCard  201......\n");
			/// 当前牌面是单支
			if (1 == iCardsRecvNum || 2 == iCardsRecvNum
				|| 3 == iCardsRecvNum)
			{
				////MT_DEBUG(4, "User PlayOutCard  202......\n");
				/// 打出最大牌当
				if (-1 != playOutMaxCardsByType(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard
					, pkQueueDeskDataNode->lCardType, iCardsSelfNum))
				{
					////MT_DEBUG(4, "User PlayOutCard  203......\n");
					/// 当前收到的牌的主牌信息
					int iMainCardNumRecv = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur, iCardsRecvNum) & 0x0f);

					/// 当前自己打出的牌的主牌信息
					int iMainCardNumSelf = (getMainCardFromRecv(pkQueueDeskDataNode->lCardType, pkDataRead->pcCard, iCardsRecvNum) & 0x0f);
					if (iMainCardNumSelf > iMainCardNumRecv)
					{
						////MT_DEBUG(4, "User PlayOutCard  204......\n");
						return 1;
					}
				}
				else
				{
					////MT_DEBUG(4, "User PlayOutCard  205......\n");
					memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
				}
			}
		}
	}

	////MT_DEBUG(4, "User PlayOutCard  300......\n");
	/// 获得本局我要打出的牌信息
	memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
	int iMainCardsIdx = getPlayOutCard(pkQueueDeskDataNode->lCardType, pkQueueDeskDataNode->pcCardsCur
		, pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx], pkDataRead->pcCard);
	if (MT_CARD_TERMINATE == pkDataRead->pcCard[0])
	{
		////MT_DEBUG(4, "User PlayOutCard  301......\n");
		memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
		pkDataRead->lCardType  = mtServiceCardOut::MT_CARD_TYPE_PASS;
		return 1;
	}

	getPlayOutCardType(pkDataRead->pcCard, &pkDataRead->lCardType);
	/// 如果我是一手牌()
	/// 获得自家手中剩余牌数
	int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);

	/// 如果手中牌全部可以打住(一手出完，赢了)
	if (iCardsSelfNum == getCardsNum(pkDataRead->pcCard))
	{
		/// 正常出牌，赢了
	}
	else
	{
		/// 当前已经打出牌的用户是自家
		if (pkQueueDeskDataNode->plPlayerType[iQueueUserIdx] == pkQueueDeskDataNode->plPlayerType[pkQueueDeskDataNode->lQueueUserDeskIdx])
		{
			////MT_DEBUG(4, "User PlayOutCard  302......\n");
			/// 如果我的主牌是炸弹
			if (mtServiceCardOut::MT_CARD_TYPE_ROCKET == pkDataRead->lCardType || mtServiceCardOut::MT_CARD_TYPE_BOMB == pkDataRead->lCardType)
			{
				// 			/// 获得自家手中剩余牌数
				// 			int iCardsSelfNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserIdx]);
				if (4 == iCardsSelfNum || 2 == iCardsSelfNum)
				{
					/// 正常出牌，赢了
				}
				else 
				{
					/// 如果是火箭(双王)
					if (mtServiceCardOut::MT_CARD_TYPE_ROCKET == pkDataRead->lCardType)
					{
						if (3 == iCardsSelfNum)
						{
							/// 正常出牌，赢了
						}
						else
						{
							/// 我不出牌 pass
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
						}
					}
					else /// 我不出牌 pass
					{
						memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
						pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
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
				else /// 单支
				{
					if (mtServiceCardOut::MT_CARD_TYPE_SINGLE == pkQueueDeskDataNode->lCardType)
					{
						///  当前牌面主牌 >= A
						if (11 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
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
								memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
								pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
							}
						}
					}
					else if(mtServiceCardOut::MT_CARD_TYPE_PAIR == pkQueueDeskDataNode->lCardType) /// 对子
					{
						/// 当前牌面主牌 >= K
						if (10 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
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
								memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
								pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
							}
						}
					}
					else if(mtServiceCardOut::MT_CARD_TYPE_TRPILE == pkQueueDeskDataNode->lCardType
						|| mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE == pkQueueDeskDataNode->lCardType
						|| mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO == pkQueueDeskDataNode->lCardType) /// 三支
					{
						/// 当前牌面主牌 >= K
						if (10 <= iMainCardNumRecv)
						{
							/// 我不出牌 pass
							memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
							pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
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
								memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
								pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
							}
						}
					}
					else /// 当前牌面其它牌型
					{
						/// 我不出牌 pass
						memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
						pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
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
			if (mtServiceCardOut::MT_CARD_TYPE_ROCKET == pkQueueDeskDataNode->lCardType 
				|| mtServiceCardOut::MT_CARD_TYPE_BOMB == pkQueueDeskDataNode->lCardType)
			{
				////MT_DEBUG(4, "User PlayOutCard  304......\n");
				/// 我正常出牌(我有炸弹就要出)
			}
			else /// 当前牌面其它牌型
			{
				////MT_DEBUG(4, "User PlayOutCard  305......\n");
				/// 如果我的主牌是炸弹
				if (mtServiceCardOut::MT_CARD_TYPE_ROCKET == pkDataRead->lCardType || mtServiceCardOut::MT_CARD_TYPE_BOMB == pkDataRead->lCardType)
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
							/// 获得下家自家手中牌面信息
							/// 获得下家自家手中牌的个数
							int iCardsSameFamilyNum = getCardsNum(pkQueueDeskDataNode->pcCardsTotal[iQueueUserNextIdx]);

							/// 如果我下家的手中牌数少于当前打出的牌数(我能打就要打)
							if (iCardsSameFamilyNum < iCardsRecvNum)
							{
								/// 我正常出牌(炸弹就要出)
							}
							else /// 我不出牌 pass
							{
								memset(pkDataRead->pcCard, MT_CARD_TERMINATE, sizeof(pkDataRead->pcCard));
								pkDataRead->lCardType = mtServiceCardOut::MT_CARD_TYPE_PASS;
							}
						}
						else /// 如果我下家是对家
						{
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

int mtServiceRoomExit::getPlayOutCard(int iCardType,char pcRecvCard[],char pcSelfCard[],char pcOutCard[])
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
	if(iCardType == mtServiceCardOut::MT_CARD_TYPE_ROCKET)
	{
		return iResult;
	}
	while(pcRecvCard[iCardsRecvNum]!= MT_CARD_TERMINATE)
	{
		iCardsRecvNum++;
	}
	iMainCard	= getMainCardFromRecv(iCardType,pcRecvCard,iCardsRecvNum);
	for(;pcSelfCard[iSelfIdx] != MT_CARD_TERMINATE;iSelfIdx++)
	{
		if((pcSelfCard[iSelfIdx] & 0x0f) > (iMainCard & 0x0f))
		{
			break;
		}
	}

	if ((pcSelfCard[iSelfIdx] == MT_CARD_TERMINATE))
	{
		if(iCardType != mtServiceCardOut::MT_CARD_TYPE_BOMB)
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
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE:
		{
			iResult	= getSingleCard(pcSelfCard,pcOutCard,iSelfIdx);
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_PAIR:
		{
			iResult	= getPairCard(pcSelfCard,pcOutCard,iSelfIdx);
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_TRPILE:
		{
			if(iCardsNum > 2)
			{
				iResult	= getTripleCard(pcSelfCard,pcOutCard,iSelfIdx);
			}

		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE:
		{
			if(iCardsNum > 3)
			{
				iResult	= getTripleWithOne(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO:
		{
			if(iCardsNum > 4)
			{
				iResult	= getTripleWithTwo(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION:
		{
			if(iCardsNum > 4)
			{
				iResult	= getSingleProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION:
		{
			if(iCardsNum > 5)
			{
				iResult	= getPairProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION:
		{
			if(iCardsNum > 5)
			{
				iResult	= getTripleProgression(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_AIRPLANE:
		{
			if(iCardsNum > 7)
			{
				iResult	= getAirPlane(pcSelfCard,pcOutCard,iSelfIdx,iCardsRecvNum);
			}
		}
		break;
	case mtServiceCardOut::MT_CARD_TYPE_FOUR_TWO:
		{

			iResult = getBombCard(pcSelfCard,pcOutCard,0);
		}
	case mtServiceCardOut::MT_CARD_TYPE_BOMB:
		{
			iResult = getBombCard(pcSelfCard,pcOutCard,iSelfIdx);
		}
		break;
	default:
		break;
	}

	if(iResult < 0 && iCardType != mtServiceCardOut::MT_CARD_TYPE_BOMB)
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

int mtServiceRoomExit::getPlayOutCardFirst(char pcSelfCard[],char pcOutCard[])
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

int mtServiceRoomExit::IsSeriesCard(char pcSelfCard[],char pcOutCard[])
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
				return mtServiceCardOut::MT_CARD_TYPE_AIRPLANE;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION;
			}
		}
		if(iSelfCardsNum == 9)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
		}
		if(iSelfCardsNum == 6)
		{
			if(getTripleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION;
			}
			if(getPairProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION;
			}
		}
		if(iSelfCardsNum >= 5)
		{
			if(getSingleProgression(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION;
			}
			if(iSelfCardsNum == 5)
			{
				if(getTripleWithTwo(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
				{
					return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO;
				}
			}
		}
		if(iSelfCardsNum == 4)
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,iSelfCardsNum)>=0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}
		}
	}

	return -1;
}

int mtServiceRoomExit::getCardsNum(char pcCard[])
{
	int iCardsNum = 0;
	while(pcCard[iCardsNum] != MT_CARD_TERMINATE)
	{
		iCardsNum++;
	}

	return iCardsNum;
}

int mtServiceRoomExit::playOutCardsNotByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum)
{
	switch(iCardType)
	{
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[iSelfCardsNum -1];
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
	case mtServiceCardOut::MT_CARD_TYPE_PAIR:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
	case mtServiceCardOut::MT_CARD_TYPE_TRPILE:
		{
			if(getTripleWithOne(pcSelfCard,pcOutCard,0,4) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE:
		{
			if(getTripleCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}
			else if(getPairCard(pcSelfCard,pcOutCard,0) >= 0)
			{
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}
			else
			{
				pcOutCard[0]= pcSelfCard[0];
				return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
			}
		}
	}

	return -1;
}

int mtServiceRoomExit:: getSingleCard(char* pcSelfCards, char* cOutCard,int iCardPos)
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

int mtServiceRoomExit::getPairCard(char* pcSelfCards,char* pcOutCard ,int iCardBegPos)
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

int mtServiceRoomExit::getTripleCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos)
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

int	mtServiceRoomExit::getTripleWithOne(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomExit::getAirPlane(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomExit::getPairProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomExit::getTripleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
{
	int iSelfIdx	= iCardBegPos;
	char pcTmpCards[20];
	memset(pcTmpCards,MT_CARD_TERMINATE,20);
	int	 iTmpCount	= 0;
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

int mtServiceRoomExit::getSingleProgression(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int	mtServiceRoomExit::getTripleWithTwo(char* pcSelfCards,char* pcOutCards,int iCardBegPos,int iCardsNum)
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

int mtServiceRoomExit::playOutMaxCardsByType(char pcSelfCard[],char pcOutCard[] ,int iCardType, int iSelfCardsNum)
{
	switch(iCardType)
	{
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE:
		{
			pcOutCard[0] = pcSelfCard[iSelfCardsNum - 1];
			return mtServiceCardOut::MT_CARD_TYPE_SINGLE;
		}
	case mtServiceCardOut::MT_CARD_TYPE_PAIR:
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
				return mtServiceCardOut::MT_CARD_TYPE_PAIR;
			}
		}
	case mtServiceCardOut::MT_CARD_TYPE_TRPILE:
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
				return mtServiceCardOut::MT_CARD_TYPE_TRPILE;
			}
		}
	}

	return -1;
}

int mtServiceRoomExit::getMainCardFromRecv(int iCardType,char* pcCardsRecv,int iCardsRecvNum)
{
	switch(iCardType)
	{
	case mtServiceCardOut::MT_CARD_TYPE_PASS:
		{
			return -1;
		}
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE:
	case mtServiceCardOut::MT_CARD_TYPE_PAIR:
	case mtServiceCardOut::MT_CARD_TYPE_TRPILE:
	case mtServiceCardOut::MT_CARD_TYPE_BOMB:
	case mtServiceCardOut::MT_CARD_TYPE_SINGLE_PROGRESSION:
	case mtServiceCardOut::MT_CARD_TYPE_PAIR_PROGRESSION:
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_PROGRESSION:
		{
			return pcCardsRecv[0];
		}
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_ONE:
		{
			return judgeTripleWithOne(pcCardsRecv,iCardsRecvNum);
		}
	case mtServiceCardOut::MT_CARD_TYPE_TRIPLE_TWO:
		{
			return judgeTripleWithTwo(pcCardsRecv,iCardsRecvNum);
		}
	case mtServiceCardOut::MT_CARD_TYPE_AIRPLANE:
		{
			return judgeAirPlane(pcCardsRecv,iCardsRecvNum);
		}
	case mtServiceCardOut::MT_CARD_TYPE_FOUR_TWO:
		{
			return judgeFourWithTwo(pcCardsRecv,iCardsRecvNum);
		}
	}

	return -1;
}

int mtServiceRoomExit::getPlayOutCardType(char pcOutCard[], long* plCardType)
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
			*plCardType  = mtServiceCardOut::MT_CARD_TYPE_ROCKET;
		}
	}
	else if (iCardCount == 4)
	{
		if ((pcOutCard[0] & 0x0f) == (pcOutCard[1] & 0x0f) 
			&& (pcOutCard[0] & 0x0f) == (pcOutCard[2] & 0x0f) 
			&& (pcOutCard[0] & 0x0f) == (pcOutCard[3] & 0x0f))
		{
			*plCardType  = mtServiceCardOut::MT_CARD_TYPE_BOMB;
		}
	}

	return 1;
}

int mtServiceRoomExit::getBombCard(char* pcSelfCards,char* pcOutCard,int iCardBegPos)
{
	int iSelfIdx	= iCardBegPos;
	while(pcSelfCards[iSelfIdx] != MT_CARD_TERMINATE)
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
	if(pcSelfCards[iSelfIdx] == MT_CARD_TERMINATE)
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

int mtServiceRoomExit::getRocketCard(char* pcSelfCards,char* pcOutCard)
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

int mtServiceRoomExit::judgeTripleWithOne(char* pcCards,int iCardsNum)
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

int mtServiceRoomExit::judgeTripleWithTwo(char* pcCards,int iCardsNum)
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

int mtServiceRoomExit::judgeAirPlane(char* pcCards,int iCardsNum)
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

int mtServiceRoomExit::judgeFourWithTwo(char* pcCards,int iCardsNum)
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

int mtServiceRoomExit::judgeTripleProgression(char* pcCards,int iCardsNum)
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

