#include "mtServiceRoomRoundOver.h"
#include "mtServiceRoomWaitStartRecv.h"
#include "mtServiceMgr.h"
#include "mtServiceRoomExit.h"
#include "mtPrint.h"

mtServiceRoomRoundOver::mtServiceRoomRoundOver()
{

}

mtServiceRoomRoundOver::~mtServiceRoomRoundOver()
{

}

int mtServiceRoomRoundOver::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	m_pkServiceMgr          = pkDataInit->pkServiceMgr;

	return	1;
}

int mtServiceRoomRoundOver::run( DataRun* pkDataRun )
{
	mtQueueUser::DataNode*      pkQueueUserDataNode	  = pkDataRun->pkQueueUserDataNode;
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataRun->pkQueuePacketDataNode;
	DataRead*                   pkDataRead            = (DataRead*)pkQueuePacketDataNode->pcData;
	mtQueueDesk::DataNode* pkQueueDeskDataNode        = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	mtQueueHall::DataRoom*	pkQueueHallDataRoom       = m_pkQueueMgr->getQueueHall()->getRoom(pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
	mtPrint(pkDataRead);
	if (NULL == pkQueueDeskDataNode)
	{
///		SetEvent(pkQueueHallDataRoom->hEvent);
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	MT_DEBUG(4, "User(UserId=%d) mtServiceRoomRoundOver  000000000......\n", pkQueueUserDataNode->lUserId);
	/// 更新个人等待下一局标识
	resetWaitStartStatus(pkQueueDeskDataNode);
	/// 更新个人上传一局结果分数标识
	pkQueueDeskDataNode->plRoundOverGold[pkQueueUserDataNode->lUserDeskIndex] = pkDataRead->lGold;
	pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNode->lUserDeskIndex]     = 1;
	pkQueueDeskDataNode->plIsWaitStart[pkQueueUserDataNode->lUserDeskIndex]   = 0;
	pkQueueUserDataNode->lIsWaitStart                                         = 0;

	/// 更新玩家信息
	pkQueueUserDataNode->lUserAllChess++;

	if (0 <= pkDataRead->lGold)
	{
	  pkQueueUserDataNode->lUserWinChess++;
	}

	/// 该用户赢了本局
	/*if (0 <= pkDataRead->lGold)
	{
	pkQueueUserDataNode->lUserWinChess++;
	pkQueueUserDataNode->lUserScore  += 200 * 5;
	}
	else
	{
	if (pkQueueUserDataNode->lUserGold >= abs(pkDataRead->lGold))
	{
	pkQueueUserDataNode->lUserScore  += 200 * 5;
	}
	else
	{
	long lGoldDeduct = (abs(pkDataRead->lGold) - pkQueueUserDataNode->lUserGold);
	if (200 > lGoldDeduct)
	{
	pkQueueUserDataNode->lUserScore  += (200 - lGoldDeduct) * 5;
	}
	}
	}*/

	pkQueueUserDataNode->lUserWinRate        = 100 * (pkQueueUserDataNode->lUserWinChess / (float)(pkQueueUserDataNode->lUserAllChess));

	pkQueueUserDataNode->lUserGold           += pkDataRead->lGold;
	
	int doubleScore = calcDoubleScore(pkQueueUserDataNode);

	pkQueueUserDataNode->lUserScore          += pkDataRead->lGold > 0 ? (pkQueueUserDataNode->lRoomId + 1) * MT_USER_WIN_SCORE * doubleScore : 0;

	if (0 > pkQueueUserDataNode->lUserGold)
	{
		pkQueueUserDataNode->lUserGold = 0;
	}
	calculatePlayerLevel(pkQueueUserDataNode);

// 	//  当前是真实用户
// 	if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkQueueUserDataNode->lUserDeskIndex]
// 	 || MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueUserDataNode->lUserDeskIndex]->lUserId)
// 	{
		/// 获得下家用户在桌子上的位置
		int iQueueUserNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);

		//  下家是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextIdx])
		{
			playUpdateQueueUserInfoAi(pkQueueDeskDataNode, pkDataRead->lServiceType, pkQueueUserDataNode->lUserDeskIndex
				, pkDataRead->lGold, iQueueUserNextIdx);

			/// 获得ai用户的下家在桌子上的位置
			int iQueueUserNextNextIdx = getQueueUserNextQueueUser(iQueueUserNextIdx);

			//  ai下家还是是ai用户
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextNextIdx])
			{
				playUpdateQueueUserInfoAi(pkQueueDeskDataNode, pkDataRead->lServiceType, pkQueueUserDataNode->lUserDeskIndex
					, pkDataRead->lGold, iQueueUserNextNextIdx);
			}

			/// 判断当前桌子上有没有使用ai代替的真实用户节点
			/// playReplaceAiExitRoom(pkQueueDeskDataNode);
		}
//	}

	/// 当前所有的用户都已经上传结果了
	if (1 == pkQueueDeskDataNode->plRoundOver[0]
	&& 1 == pkQueueDeskDataNode->plRoundOver[1]
	&& 1 == pkQueueDeskDataNode->plRoundOver[2])
	{
		MT_DEBUG(4, "User(UserId=%d) mtServiceRoomRoundOver 1111111111........\n", pkQueueUserDataNode->lUserId);
		playRoundOver(pkQueueDeskDataNode, pkDataRead->lServiceType);
		playUpdateQueueDesk(pkQueueDeskDataNode);
		LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
///		SetEvent(pkQueueHallDataRoom->hEvent);
		return 1;
	}

	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
///	 (pkQueueHallDataRoom->hEvent);
	return	1;
}

int mtServiceRoomRoundOver::playReplaceAiExitRoom(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	mtQueueUser::DataNode*      pkQueueUserDataNodeReplaceAi1 = NULL;
	mtQueueUser::DataNode*      pkQueueUserDataNodeReplaceAi2 = NULL;
	int		iUserIdx = 0;
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
	&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
	{
		pkQueueUserDataNodeReplaceAi1 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];

		iUserIdx++;
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
		&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
		{
			pkQueueUserDataNodeReplaceAi2 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];
		}
		else
		{
			iUserIdx++;
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
			&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
			{
				pkQueueUserDataNodeReplaceAi2 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];
			}
		}
	}
	else
	{
		iUserIdx++;
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
		&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
		{
			pkQueueUserDataNodeReplaceAi1 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];

			iUserIdx++;
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
			&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
			{
				pkQueueUserDataNodeReplaceAi2 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];
			}
		}
		else
		{
			iUserIdx++;
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iUserIdx]
			&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]->lUserId)
			{
				pkQueueUserDataNodeReplaceAi1 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];
			}
		}
	}

	if (pkQueueUserDataNodeReplaceAi1)
	{
		/// 响应替代真实用户的ai退出room
		mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomroundover186");
		if (pkQueuePacketDataNode)
		{
			mtServiceRoomExit::DataRead* pkDataRead = (mtServiceRoomExit::DataRead*)pkQueuePacketDataNode->pcData;

			pkDataRead->lStructBytes                        = sizeof(mtServiceRoomExit::DataRead);
			pkDataRead->lServiceType                        = mtServiceMgr::E_SERVICE_ROOM_EXIT;
			pkDataRead->lStatusExit                         = 1;
			pkDataRead->lCardType                           = 0;
			memset(pkDataRead->pcCard, 100, sizeof(pkDataRead->pcCard));

			mtService::DataRun kDataRun;
			kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
			kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeReplaceAi1;

			m_pkServiceMgr->run(&kDataRun);

			/////////////////////////////////////////////				回收packet包节点
			m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
		}

		if (pkQueueUserDataNodeReplaceAi2)
		{
			/// 响应替代真实用户的ai退出room
			pkQueuePacketDataNode = NULL;
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomroundover211");
			if (pkQueuePacketDataNode)
			{
				mtServiceRoomExit::DataRead* pkDataRead = (mtServiceRoomExit::DataRead*)pkQueuePacketDataNode->pcData;

				pkDataRead->lStructBytes                        = sizeof(mtServiceRoomExit::DataRead);
				pkDataRead->lServiceType                        = mtServiceMgr::E_SERVICE_ROOM_EXIT;
				pkDataRead->lStatusExit                         = 1;
				pkDataRead->lCardType                           = 0;
				memset(pkDataRead->pcCard, 100, sizeof(pkDataRead->pcCard));

				mtService::DataRun kDataRun;
				kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
				kDataRun.pkQueueUserDataNode	= pkQueueUserDataNodeReplaceAi1;

				m_pkServiceMgr->run(&kDataRun);

				/////////////////////////////////////////////				回收packet包节点
				m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
			}
		}
	}

	return 1;
}

int mtServiceRoomRoundOver::playUpdateQueueUserInfo(mtQueueUser::DataNode* pkQueueUserDataNode, long lGold)
{
	/// 该用户赢了本局
	pkQueueUserDataNode->lUserAllChess++;
	if (0 <= lGold)
	{
		pkQueueUserDataNode->lUserWinChess++;

		if (10 < lGold)
		{
			pkQueueUserDataNode->lUserGold += lGold;
			if(pkQueueUserDataNode->lUserId == 2302)
			{
				mtLog("2302[%x]:win gold: %d",pkQueueUserDataNode,lGold);
				mtLog("2302[%x]:have gold: %d",pkQueueUserDataNode,pkQueueUserDataNode->lUserGold);
			}
		}
		else
		{
			pkQueueUserDataNode->lUserGold += 10;
		}
		
	}
	else  /// 该用户本局输了
	{
		pkQueueUserDataNode->lUserGold += lGold;
	}
	
    int doubleScore = calcDoubleScore(pkQueueUserDataNode);;

	pkQueueUserDataNode->lUserScore            += lGold > 0 ? (pkQueueUserDataNode->lRoomId + 1) * MT_USER_WIN_SCORE * doubleScore : 0;

	pkQueueUserDataNode->lUserWinRate           = 100 * (pkQueueUserDataNode->lUserWinChess / (float)pkQueueUserDataNode->lUserAllChess);
	calculatePlayerLevel(pkQueueUserDataNode);

	return 1;
}

int mtServiceRoomRoundOver::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceRoomRoundOver::playUpdateQueueUserInfoAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserIdx, long lGold, int iQueueUserNextAiIdx)
{
	/// 该AI已经上传结果了
	if (1 == pkQueueDeskDataNode->plRoundOver[iQueueUserNextAiIdx])
	{
		return 1;
	}

	/// 更新个人上传一局结果分数标识
	pkQueueDeskDataNode->plRoundOver[iQueueUserNextAiIdx]                          = 1;
	/// 更新个人等待下一局标识
	pkQueueDeskDataNode->plIsWaitStart[iQueueUserNextAiIdx]                        = 0;

	/// AI的上家是地主
	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[iQueueUserIdx])
	{
		pkQueueDeskDataNode->plRoundOverGold[iQueueUserNextAiIdx]      = -lGold;
		playUpdateQueueUserInfo(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserNextAiIdx], -lGold);
	}
	else /// AI的上家是农民
	{
		/// AI是地主
		if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[iQueueUserNextAiIdx])
		{
			pkQueueDeskDataNode->plRoundOverGold[iQueueUserNextAiIdx]      = -lGold;
			playUpdateQueueUserInfo(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserNextAiIdx], -lGold);
		}
		else /// AI是农民
		{
			pkQueueDeskDataNode->plRoundOverGold[iQueueUserNextAiIdx]      = lGold;
			playUpdateQueueUserInfo(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserNextAiIdx], lGold);
		}
	}

	return 1;
}

int mtServiceRoomRoundOver::playUpdateQueueDesk(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		/// 把桌子上的所有人置为完成状态
		if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx])
		{
			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER;
		}

// 		/// 把替代ai置为完成状态
// 		if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]
// 		&& mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx]
// 		&& MT_QUEUE_USER_ID_AI_START > pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]->lUserId
// 		&& 0 < pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]->lUserId)
// 		{
// 			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER;
// 		}
	}

	pkQueueDeskDataNode->lDeskExit          = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER;

	return 1;
}

int mtServiceRoomRoundOver::runWaitStartRecvAi(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		/// 发送‘等待开始’ 消息
		//  当前ai用户
		if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]
		   && mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx])
		{
			playWaitStartRecvAi(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]);
		}
	}

	return 1;
}

int mtServiceRoomRoundOver::playWaitStartRecvAi(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	/// 响应建立三人桌子
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomroundover377");
	if (NULL != pkQueuePacketDataNode)
	{
		mtServiceRoomWaitStartRecv::DataRead* pkDataRead = (mtServiceRoomWaitStartRecv::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes           = sizeof(mtServiceRoomWaitStartRecv::DataRead);
		pkDataRead->lServiceType           = mtServiceMgr::E_SERVICE_ROOM_WAIT_START;

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNode;

		m_pkServiceMgr->run(&kDataRun);

		m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtServiceRoomRoundOver::exit()
{
	return	0;
}

int mtServiceRoomRoundOver::calculatePlayerLevel(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	if(pkQueueUserDataNode->lUserScore >= 100000000)
	{
		pkQueueUserDataNode->lUserLevel	= 14;
	}
	else if(pkQueueUserDataNode->lUserScore >= 50000000)
	{
		pkQueueUserDataNode->lUserLevel	= 13;
	}
	else if(pkQueueUserDataNode->lUserScore >= 30000000)
	{
		pkQueueUserDataNode->lUserLevel	= 12;
	}
	else if(pkQueueUserDataNode->lUserScore >= 15000000)
	{
		pkQueueUserDataNode->lUserLevel	= 11;
	}
	else if(pkQueueUserDataNode->lUserScore >= 10000000)
	{
		pkQueueUserDataNode->lUserLevel	= 10;
	}
	else if(pkQueueUserDataNode->lUserScore >= 5000000)
	{
		pkQueueUserDataNode->lUserLevel	= 9;
	}
	else if(pkQueueUserDataNode->lUserScore >= 2000000)
	{
		pkQueueUserDataNode->lUserLevel	= 8;
	}
	else if(pkQueueUserDataNode->lUserScore >= 1500000)
	{
		pkQueueUserDataNode->lUserLevel	= 7;
	}
	else if(pkQueueUserDataNode->lUserScore >= 800000)
	{
		pkQueueUserDataNode->lUserLevel	= 6;
	}
	else if(pkQueueUserDataNode->lUserScore >= 300000)
	{
		pkQueueUserDataNode->lUserLevel	= 5;
	}
	else if(pkQueueUserDataNode->lUserScore >= 100000)
	{
		pkQueueUserDataNode->lUserLevel	= 4;
	}
	else if(pkQueueUserDataNode->lUserScore >= 50000)
	{
		pkQueueUserDataNode->lUserLevel	= 3;
	}
	else if(pkQueueUserDataNode->lUserScore >= 10000)
	{
		pkQueueUserDataNode->lUserLevel	= 2;
	}
	else if(pkQueueUserDataNode->lUserScore >= 5000)
	{
		pkQueueUserDataNode->lUserLevel	= 1;
	}
	else
	{
		pkQueueUserDataNode->lUserLevel	= 0;
	}

	return 1;
}

int mtServiceRoomRoundOver::resetWaitStartStatus(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	if (NULL != pkQueueDeskDataNode)
	{
		if (0 == pkQueueDeskDataNode->plRoundOver[0]
		 && 0 == pkQueueDeskDataNode->plRoundOver[1]
		 && 0 == pkQueueDeskDataNode->plRoundOver[2])
		{
			int iQueueUserDataNodeIdx;
			for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
			{
				pkQueueDeskDataNode->plIsWaitStart[iQueueUserDataNodeIdx] = 0;

				/// 重置‘等待开始’ 消息
				if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx])
				{

					pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx]->lIsWaitStart = 0;
				}
			}
		}
	}
	
	return 1;
}

int mtServiceRoomRoundOver::playRoundOver(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType)
{
	if (NULL != pkQueueDeskDataNode)
	{
		/// 发送更新响应给桌子上的其他人员
		DataWrite  kDataWrite;
		memset(&kDataWrite, 0, sizeof(kDataWrite));
		kDataWrite.lStructBytes        = sizeof(kDataWrite);
		kDataWrite.lServiceType        = lServiceType;

		int		iUserIdx;
		mtQueueUser::DataNode* pkQueueUserDataNode;
		for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
		{
			pkQueueUserDataNode   = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];
			if (pkQueueUserDataNode && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
			{
				mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomroundover513");
				if (pkQueuePacketDataNode)
				{
					kDataWrite.kUserInfo.lUserId             = pkQueueUserDataNode->lUserId;
					kDataWrite.kUserInfo.lUserGold           = pkQueueUserDataNode->lUserGold;
					kDataWrite.kUserInfo.lUserLevel          = pkQueueUserDataNode->lUserLevel;
					kDataWrite.kUserInfo.lUserScore          = pkQueueUserDataNode->lUserScore;
					kDataWrite.kUserInfo.lUserAllChess       = pkQueueUserDataNode->lUserAllChess;
					kDataWrite.kUserInfo.lUserWinChess       = pkQueueUserDataNode->lUserWinChess;
					kDataWrite.kUserInfo.lUserWinRate        = pkQueueUserDataNode->lUserWinRate;
					kDataWrite.kUserInfo.lUserOffLineCount   = pkQueueUserDataNode->lUserOffLineCount;
					kDataWrite.kUserInfo.lIsWaitStart        = pkQueueDeskDataNode->plIsWaitStart[pkQueueUserDataNode->lUserDeskIndex];
					kDataWrite.kUserInfo.ulDeskId            = (unsigned long)pkQueueDeskDataNode;
					memcpy(kDataWrite.kUserInfo.plPropsCount,pkQueueUserDataNode->plPropsCount, sizeof(pkQueueUserDataNode->plPropsCount));
					memcpy(kDataWrite.kUserInfo.pcUserAccountNum, pkQueueUserDataNode->pcUserAccountNum, sizeof(kDataWrite.kUserInfo.pcUserAccountNum));
					memcpy(kDataWrite.kUserInfo.pcUserName, pkQueueUserDataNode->pcUserName, sizeof(pkQueueUserDataNode->pcUserName));
					memcpy(kDataWrite.kUserInfo.pcUserNickName, pkQueueUserDataNode->pcUserNickName, sizeof(kDataWrite.kUserInfo.pcUserNickName));

					pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
					memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
					mtPrint(&kDataWrite);
					m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
				}
			}
		}
	}

	return 1;
}

int mtServiceRoomRoundOver::calcDoubleScore(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	int doubleScore = 1;
	if(pkQueueUserDataNode->plPropsCount[0] > 0 ||pkQueueUserDataNode->plPropsCount[1] == -1 )//双倍经验卡
	{
		doubleScore *= 2;
	}
	if(pkQueueUserDataNode->plPropsCount[13] > 0 ||pkQueueUserDataNode->plPropsCount[14] > 0||pkQueueUserDataNode->plPropsCount[15] == -1)//VIP
	{
		doubleScore *= 2;
	}

	return doubleScore;
}
