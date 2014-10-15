#include "mtServiceConnect.h"
#include "mtQueueMgr.h"
#include "mtPrint.h"

mtServiceConnect::mtServiceConnect()
{

}

mtServiceConnect::~mtServiceConnect()
{

}

int mtServiceConnect::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueueHall			= pkDataInit->pkQueueHall;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;

	return	1;
}

int mtServiceConnect::run( DataRun* pkDataRun )
{
	mtQueueUser::DataNode*      pkQueueUserDataNode	  = pkDataRun->pkQueueUserDataNode;
	mtQueuePacket::DataNode*	pkQueuePacketDataNode1 = pkDataRun->pkQueuePacketDataNode;
	DataRead*                   pkDataRead            = (DataRead*)pkQueuePacketDataNode1->pcData;
	mtPrint(pkDataRead);
	pkQueueUserDataNode->lSpaceId                     = pkDataRead->lSpaceId;
	pkQueueUserDataNode->lRoomId                      = pkDataRead->lRoomId;
	pkQueueUserDataNode->lUserId                      = pkDataRead->kUserInfo.lUserId;
	pkQueueUserDataNode->lUserIconId                  = pkDataRead->kUserInfo.lUserIconId;
	pkQueueUserDataNode->lUserGold                    = pkDataRead->kUserInfo.lUserGold;
	pkQueueUserDataNode->lUserSex                     = pkDataRead->kUserInfo.lUserSex;
	pkQueueUserDataNode->lUserLevel                   = pkDataRead->kUserInfo.lUserLevel;


	pkQueueUserDataNode->lUserScore                   = pkDataRead->kUserInfo.lUserScore;
	pkQueueUserDataNode->lUserAllChess                = pkDataRead->kUserInfo.lUserAllChess;
	pkQueueUserDataNode->lUserWinChess                = pkDataRead->kUserInfo.lUserWinChess;
	pkQueueUserDataNode->lUserWinRate                 = pkDataRead->kUserInfo.lUserWinRate;
	pkQueueUserDataNode->pkQueueDeskDataNode          = NULL;
	//pkQueueUserDataNode->pkQueuePacketDataNodeHead    = NULL;
	pkQueueUserDataNode->lUserDeskIndex               = -1;
	pkQueueUserDataNode->lUserOffLineCount            = pkDataRead->kUserInfo.lUserOffLineCount;
	pkQueueUserDataNode->lIsWaitStart                 = 0;
	pkQueueUserDataNode->ullTimeEnterRun              = GetTickCount();
	memcpy(pkQueueUserDataNode->plPropsCount, pkDataRead->kUserInfo.plPropsCount, sizeof(pkDataRead->kUserInfo.plPropsCount));
	memcpy(pkQueueUserDataNode->pcUserAccountNum, pkDataRead->kUserInfo.pcUserAccountNum, sizeof(pkDataRead->kUserInfo.pcUserAccountNum));
	memcpy(pkQueueUserDataNode->pcUserName, pkDataRead->kUserInfo.pcUserName, sizeof(pkDataRead->kUserInfo.pcUserName));
	memcpy(pkQueueUserDataNode->pcUserNickName, pkDataRead->kUserInfo.pcUserNickName, sizeof(pkDataRead->kUserInfo.pcUserNickName));
	memset(pkQueueUserDataNode->pkListNode, 0, sizeof(pkQueueUserDataNode->pkListNode));

	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	memset(kDataWrite.pcCardsBase, MT_CARD_TERMINATE, sizeof(kDataWrite.pcCardsBase));
	memset(kDataWrite.pcCardsTotal, MT_CARD_TERMINATE, sizeof(kDataWrite.pcCardsTotal));
	memset(kDataWrite.pcCardsShow, MT_CARD_TERMINATE, sizeof(kDataWrite.pcCardsShow));
	memset(kDataWrite.pcCardsCur, MT_CARD_TERMINATE, sizeof(kDataWrite.pcCardsCur));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lResult           = 0;

	//mtLog("User %d Login\n",pkQueueUserDataNode->lUserId);
	////////////////////////////////////////////////////////////////////////////弹出新packetNode
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("serviceconnect67");


	/// 先判断这个用户是否被ai替代了
	if (MT_QUEUE_USER_ID_AI_START < pkQueueUserDataNode->lUserId || 1024 > pkQueueUserDataNode->lUserId)
	{
		pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
		memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
		mtPrint(&kDataWrite);
		m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
		return 1;
	}

	mtQueueMgr::UserNodeReplaceAi* pkUserNodeReplaceAi   = m_pkQueueMgr->getUserNodeReplaceAi(pkQueueUserDataNode->lUserId);
	mtQueueUser::DataNode* pkQueueUserDataNodeReplaceAi  = pkUserNodeReplaceAi->pkUserNode;

	if (NULL != pkUserNodeReplaceAi->pkUserNode && (1 == pkUserNodeReplaceAi->iReplaceAi)) /// 该用户有替代ai
	{
		mtQueueDesk::DataNode* pkQueueDeskReplaceAi          = (mtQueueDesk::DataNode*)pkQueueUserDataNodeReplaceAi->pkQueueDeskDataNode;
		/// 判断替代AI当前的游戏状态
		/// 替代AI正在进行游戏，可以恢复场景，由用户接替AI继续玩
		if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueUserDataNodeReplaceAi->lUserExit)
		{
			if (NULL != pkQueueDeskReplaceAi)
			{
				EnterCriticalSection(&pkQueueDeskReplaceAi->kCriticalSection);
			}
			
			if (NULL != pkUserNodeReplaceAi->pkUserNode 
				&& mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueUserDataNodeReplaceAi->lUserExit)
			{
				runRecoverScene(pkQueueUserDataNode, pkQueueUserDataNodeReplaceAi, &kDataWrite);
				kDataWrite.lResult                    = 4;
				if (pkUserNodeReplaceAi->iStatusExit)
				{
					kDataWrite.lResult                = 2;
				}

				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);

				/// 回收替代AI节点
				pkUserNodeReplaceAi->iStatusExit           = 0;
				pkUserNodeReplaceAi->iReplaceAi            = 0;
				pkUserNodeReplaceAi->iStatusRun            = 1;
				pkQueueUserDataNodeReplaceAi->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
				MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 117",pkQueueUserDataNodeReplaceAi);
				m_pkQueueMgr->getQueueUser()->push(pkUserNodeReplaceAi->pkUserNode);
				///pkUserNodeReplaceAi->pkUserNode  = NULL;
				pkUserNodeReplaceAi->pkUserNode  = pkQueueUserDataNode;
				mtQueueHall::DataRoom*	pkQueueHallDataRoom = m_pkQueueHall->getRoom(pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
				pkQueueHallDataRoom->lOnlinePersonNumber++;
				MT_DEBUG(4, "6User(UserId=%d) login room(Space=%d,Room=%d), count of user is(%d)......\n", pkQueueUserDataNode->lUserId, pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId, pkQueueHallDataRoom->lOnlinePersonNumber);
				if (pkQueueDeskReplaceAi)
				{
					LeaveCriticalSection(&pkQueueDeskReplaceAi->kCriticalSection);
				}

				return 1;
			}
		}
		else /// 游戏已经结束
		{
			/// 用户信息获取
			kDataWrite.kUserInfo[0]               = pkDataRead->kUserInfo;
			kDataWrite.kUserInfo[0].lUserGold     = pkQueueUserDataNodeReplaceAi->lUserGold;
			kDataWrite.kUserInfo[0].lUserLevel    = pkQueueUserDataNodeReplaceAi->lUserLevel;
			kDataWrite.kUserInfo[0].lUserScore    = pkQueueUserDataNodeReplaceAi->lUserScore;
			kDataWrite.kUserInfo[0].lUserAllChess = pkQueueUserDataNodeReplaceAi->lUserAllChess;
			kDataWrite.kUserInfo[0].lUserWinChess = pkQueueUserDataNodeReplaceAi->lUserWinChess;
			kDataWrite.kUserInfo[0].lUserWinRate  = pkQueueUserDataNodeReplaceAi->lUserWinRate;

			/// 用户是否请求再次进入一样的房间
			if (1 == pkDataRead->lRequestSameRoom) /// 用户请求再次进入一样的房间
			{
				pkQueueUserDataNode->lUserGold        = pkQueueUserDataNodeReplaceAi->lUserGold;
		
				pkQueueUserDataNode->lUserLevel       = pkQueueUserDataNodeReplaceAi->lUserLevel;
				pkQueueUserDataNode->lUserScore       = pkQueueUserDataNodeReplaceAi->lUserScore;
				pkQueueUserDataNode->lUserAllChess    = pkQueueUserDataNodeReplaceAi->lUserAllChess;
				pkQueueUserDataNode->lUserWinChess    = pkQueueUserDataNodeReplaceAi->lUserWinChess;
				pkQueueUserDataNode->lUserWinRate     = pkQueueUserDataNodeReplaceAi->lUserWinRate;
				kDataWrite.lResult                    = 5;
				pkUserNodeReplaceAi->iStatusRun       = 1;
				if (pkUserNodeReplaceAi->iStatusExit)
				{
					kDataWrite.lResult                = 3;
				}
			}
			else /// 用户请求再次进入别的房间
			{
				kDataWrite.lResult                    = 5;
				if (pkUserNodeReplaceAi->iStatusExit)
				{
					kDataWrite.lResult                = 3;
				}

				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);

				/// 回收替代AI节点
				pkUserNodeReplaceAi->iStatusExit = 0;
				pkUserNodeReplaceAi->iReplaceAi  = 0;
				pkUserNodeReplaceAi->iStatusRun  = 2;
				if (pkUserNodeReplaceAi->pkUserNode)
				{
					pkQueueUserDataNodeReplaceAi->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
					MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 241",pkQueueUserDataNodeReplaceAi);
					m_pkQueueMgr->getQueueUser()->push(pkQueueUserDataNodeReplaceAi);
				}

				pkUserNodeReplaceAi->pkUserNode  = NULL;
				return 1;
			}
		}
	}
	else///该用户没有替代AI
	{ 
		if (NULL != pkUserNodeReplaceAi->pkUserNode && 1 == pkUserNodeReplaceAi->iStatusRun)
		{
			if (MT_SERVICE_HEART_BEAT_TIME < (GetTickCount() - pkUserNodeReplaceAi->pkUserNode->ullTimeEnterRun))
			{
				/// 用户是否请求再次进入一样的房间
				//if (1 == pkDataRead->lRequestSameRoom) /// 用户请求再次进入一样的房间
				//{
				//	/// 释放用户节点
		
				//	mtQueuePacket::DataNode*	pkQueuePacketDataNodeHead = pkUserNodeReplaceAi->pkUserNode->pkQueuePacketDataNodeHead;
				//	mtQueuePacket::DataNode*	pkQueuePacketDataNodeDelete;

				//	while (pkQueuePacketDataNodeHead)
				//	{
				//		pkQueuePacketDataNodeDelete	= pkQueuePacketDataNodeHead;
				//		pkQueuePacketDataNodeHead		= pkQueuePacketDataNodeHead->pkNext;
				//		if (pkQueuePacketDataNodeDelete)
				//		{
				//			m_pkQueueMgr->getQueuePacket()->push(pkQueuePacketDataNodeDelete);
				//		}
				//	 }

				//	kDataWrite.lResult                    = 1;
				//	pkUserNodeReplaceAi->iStatusRun       = 1;
				//	MT_DEBUG(4, "(UserId=%d)!!!!!???!!!!!???!!!!!!???(Space=%d,Room=%d)......\n", pkUserNodeReplaceAi->pkUserNode->lUserId, pkUserNodeReplaceAi->pkUserNode->lSpaceId, pkUserNodeReplaceAi->pkUserNode->lRoomId);
				//}
				//else /// 用户请求再次进入别的房间
				//{
				//	pkUserNodeReplaceAi->iStatusExit      = 0;
				//	pkUserNodeReplaceAi->iReplaceAi       = 0;
				//	pkUserNodeReplaceAi->iStatusRun       = 2;
				//	pkUserNodeReplaceAi->pkUserNode       = NULL;

				//	kDataWrite.lResult                    = 7;
				//	pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				//	memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				//	mtPrint(&kDataWrite);
				//	m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);

				//	return 1;
				//}
				closesocket(pkQueueUserDataNodeReplaceAi->uiSockets);
			}
			//else
			//{
				kDataWrite.lResult                    = 6;
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);

				return 1;
			//}
		}
		else
		{
			/// 用户是否请求再次进入一样的房间
			if (1 == pkDataRead->lRequestSameRoom) /// 用户请求再次进入一样的房间
			{
				kDataWrite.lResult                    = 1;
				pkUserNodeReplaceAi->iStatusRun       = 1;
			}
			else /// 用户请求再次进入别的房间
			{
				pkUserNodeReplaceAi->iStatusExit      = 0;
				pkUserNodeReplaceAi->iReplaceAi       = 0;
				pkUserNodeReplaceAi->iStatusRun       = 2;
				pkUserNodeReplaceAi->pkUserNode       = NULL;

				kDataWrite.lResult                    = 7;
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);

				return 1;
			}
		}
	}

	/// 回收替代AI节点
	pkUserNodeReplaceAi->iStatusExit = 0;
	pkUserNodeReplaceAi->iReplaceAi  = 0;

	//if (pkUserNodeReplaceAi->pkUserNode)
	//{
	//	pkQueueUserDataNodeReplaceAi->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
	//	MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 338",pkQueueUserDataNodeReplaceAi);
	//	m_pkQueueMgr->getQueueUser()->push(pkQueueUserDataNodeReplaceAi);
	//}

	pkUserNodeReplaceAi->pkUserNode  = pkQueueUserDataNode;

	pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
	memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
	mtPrint(&kDataWrite);

	m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
	
	mtQueueHall::DataRoom*	pkQueueHallDataRoom = m_pkQueueHall->getRoom(pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
	pkQueueHallDataRoom->lOnlinePersonNumber++;
	pkQueueHallDataRoom->kQueueOneList.push(&pkQueueUserDataNode);

	MT_DEBUG(4,"\n\nUser(UserId=%d) login room(Space=%d,Room=%d), count of user is(%d)......\n", pkQueueUserDataNode->lUserId, pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId, pkQueueHallDataRoom->lOnlinePersonNumber);

	if (pkQueueUserDataNodeReplaceAi
		&& 0 != pkQueueUserDataNodeReplaceAi->uiSockets
		&& INVALID_SOCKET != pkQueueUserDataNodeReplaceAi->uiSockets)
	{
		pkQueueUserDataNodeReplaceAi->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
		MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 378",pkQueueUserDataNodeReplaceAi);
		closesocket(pkQueueUserDataNodeReplaceAi->uiSockets);
	}

	return	1;
}

int mtServiceConnect::exit()
{
	return	0;
}

int mtServiceConnect::runRecoverScene(mtQueueUser::DataNode* pkQueueUserDataNode, mtQueueUser::DataNode* pkQueueUserDataNodeReplaceAi, DataWrite* pkDataWrite)
{
	mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkQueueUserDataNodeReplaceAi->pkQueueDeskDataNode;
	if (NULL != pkQueueDeskDataNode)
	{
		/// 恢复自己节点
		pkQueueUserDataNode->lIsWaitStart         = pkQueueUserDataNodeReplaceAi->lIsWaitStart;
		pkQueueUserDataNode->lUserDeskIndex       = pkQueueUserDataNodeReplaceAi->lUserDeskIndex;
		pkQueueUserDataNode->lUserExit            = pkQueueUserDataNodeReplaceAi->lUserExit;
		pkQueueUserDataNode->pkQueueDeskDataNode  = pkQueueUserDataNodeReplaceAi->pkQueueDeskDataNode;

		/// 其他成员信息
		int iUserNodePreIdx = getQueueUserPreQueueUser(pkQueueUserDataNodeReplaceAi->lUserDeskIndex);
		getUserInfo(pkQueueDeskDataNode, iUserNodePreIdx, pkDataWrite->kUserInfo);

		int iUserNodeNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNodeReplaceAi->lUserDeskIndex);
		getUserInfo(pkQueueDeskDataNode, iUserNodeNextIdx, pkDataWrite->kUserInfo + 1);

		/// 桌面信息
		int iUserNodeLandlordIdx = getQueueUserLandlord(pkQueueDeskDataNode);
		memcpy(pkDataWrite->pcCardsBase, pkQueueDeskDataNode->pcCardsBase, sizeof(pkQueueDeskDataNode->pcCardsBase));
		memcpy(pkDataWrite->pcCardsTotal, pkQueueDeskDataNode->pcCardsTotal, sizeof(pkDataWrite->pcCardsTotal));
		/// memcpy(pkDataWrite->pcCardsTotal, pkQueueDeskDataNode->pcCardsTotal[pkUserNodeReplaceAi->pkUserNode->lUserDeskIndex], sizeof(pkDataWrite->pcCardsTotal));
		memcpy(pkDataWrite->pcCardsShow, pkQueueDeskDataNode->pcCardsTotal[iUserNodeLandlordIdx], sizeof(pkDataWrite->pcCardsShow));
		memcpy(pkDataWrite->pcCardsCur, pkQueueDeskDataNode->pcCardsCur, sizeof(pkDataWrite->pcCardsCur));
		memcpy(pkDataWrite->pcCardsOutTotal, pkQueueDeskDataNode->pcCardsOutTotal, sizeof(pkDataWrite->pcCardsOutTotal));
		memcpy(pkDataWrite->plPlayerLots, pkQueueDeskDataNode->plPlayerLots, sizeof(pkQueueDeskDataNode->plPlayerLots));
		memcpy(pkDataWrite->plScore, pkQueueDeskDataNode->plScore, sizeof(pkQueueDeskDataNode->plScore));
		memcpy(pkDataWrite->plDoubleScore, pkQueueDeskDataNode->plDoubleScore, sizeof(pkQueueDeskDataNode->plDoubleScore));
		memcpy(pkDataWrite->plShowCard, pkQueueDeskDataNode->plShowCard, sizeof(pkQueueDeskDataNode->plShowCard));
		memcpy(pkDataWrite->plBomb, pkQueueDeskDataNode->plBomb, sizeof(pkQueueDeskDataNode->plBomb));
		pkDataWrite->lRoundOver               = pkQueueDeskDataNode->plRoundOver[pkQueueUserDataNodeReplaceAi->lUserDeskIndex];
		pkDataWrite->lRocket                  = pkQueueDeskDataNode->lRocket;
		pkDataWrite->lCardType                = pkQueueDeskDataNode->lCardType;
		pkDataWrite->lUserNodeIdx             = pkQueueUserDataNode->lUserDeskIndex;
		pkDataWrite->lUserDeskIdxCardOut      = pkQueueDeskDataNode->lQueueUserDeskIdx;
		pkDataWrite->lUserDeskIdxCardOutNext  = pkQueueDeskDataNode->lUserDeskIdxCardOutNext;
		pkDataWrite->lQueueUserDefaultLandlord= pkQueueDeskDataNode->lQueueUserDefaultLandlord;
// 		pkDataWrite->plLeftCardsNum[0]        = getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[0]);
// 		pkDataWrite->plLeftCardsNum[1]        = getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[1]);
// 		pkDataWrite->plLeftCardsNum[2]        = getQueueUserLeftCards(pkQueueDeskDataNode->pcCardsTotal[2]);

		pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueUserDataNode->lUserDeskIndex] = pkQueueUserDataNode;
		pkQueueDeskDataNode->plPlayerMode[pkQueueUserDataNode->lUserDeskIndex]        = mtQueueDesk::E_PLAYER_MODE_USER;

		return 1;
	}

	return 0;
}

int mtServiceConnect::getUserInfo(mtQueueDesk::DataNode* pkQueueDeskDataNode, int iUserNodeIdx, DataUserInfo* pkUserInfo)
{
	pkUserInfo->lUserId                   = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserId;
	pkUserInfo->lUserIconId               = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserIconId;
	pkUserInfo->lUserSex                  = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserSex;
	pkUserInfo->lUserGold                 = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserGold;
	pkUserInfo->lUserLevel                = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserLevel;
	pkUserInfo->lUserScore                = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserScore;
	pkUserInfo->lUserAllChess             = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserAllChess;
	pkUserInfo->lUserWinChess             = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserWinChess;
	pkUserInfo->lUserWinRate              = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserWinRate;
	pkUserInfo->lUserOffLineCount         = pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->lUserOffLineCount;
	pkUserInfo->lIsWaitStart              = pkQueueDeskDataNode->plIsWaitStart[iUserNodeIdx];
	pkUserInfo->lPlayerType               = pkQueueDeskDataNode->plPlayerType[iUserNodeIdx];
	pkUserInfo->ulDeskId                  = (unsigned long)pkQueueDeskDataNode;
	memcpy(pkUserInfo->pcUserAccountNum, pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->pcUserAccountNum, sizeof(pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->pcUserAccountNum));
	memcpy(pkUserInfo->pcUserNickName, pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->pcUserNickName, sizeof(pkQueueDeskDataNode->pkQueueUserDataNode[iUserNodeIdx]->pcUserNickName));

	return 1;
}

int mtServiceConnect::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceConnect::getQueueUserPreQueueUser(int iQueueUserCurIdx)
{
	if (2 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx - 2;
	}

	return iQueueUserCurIdx + 1;
}

int mtServiceConnect::getQueueUserLandlord(mtQueueDesk::DataNode* pkQueueDeskDataNode)
{
	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[0])
	{
		return 0;
	}

	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[1])
	{
		return 1;
	}

	if (mtQueueDesk::E_PLAYER_TYPE_LANDLORD == pkQueueDeskDataNode->plPlayerType[2])
	{
		return 2;
	}

	return 0;
}

int mtServiceConnect::getQueueUserLeftCards(char* pcCard)
{
	int iCardNum = 0;
	while(pcCard[iCardNum] != MT_CARD_TERMINATE)
	{
		iCardNum++;
	}

	return iCardNum;
}
