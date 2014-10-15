#include "mtServiceRoomChangeDesk.h"
#include "mtServiceMgr.h"
#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomDoubleScore.h"
#include "mtPrint.h"

mtServiceRoomChangeDesk::mtServiceRoomChangeDesk()
{

}

mtServiceRoomChangeDesk::~mtServiceRoomChangeDesk()
{

}

int mtServiceRoomChangeDesk::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;

	return	1;
}

int mtServiceRoomChangeDesk::run( DataRun* pkDataRun )
{
	DataRead* pkDataRead                              = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode*      pkQueueUserDataNode	  = pkDataRun->pkQueueUserDataNode;
	mtPrint(pkDataRead);
	if (NULL == pkQueueUserDataNode)
	{
		return 0;
	}

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
		pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("changdesk53");
		if (NULL != pkQueuePacketDataNode)
		{
			pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
			memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
			mtPrint(&kDataWrite);
			m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
		}

		playRoomChangeDeskOne(pkQueueUserDataNode, pkQueueHallDataRoom);

		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	int		iUserIdx;
	mtQueueUser::DataNode*      pkQueueUserDataNodeTmp;

	for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
	{
		pkQueueUserDataNodeTmp = pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx];

		if (NULL != pkQueueUserDataNodeTmp && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("changedesk75");
			if (NULL != pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNodeTmp, pkQueuePacketDataNode);
			}
		}
	}

	/// 该用户在两人桌子队列
	if (2 == pkQueueDeskDataNode->lQueue)
	{
		runRoomChangeDeskTwo(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueHallDataRoom);
	}
	else if(3 == pkQueueDeskDataNode->lQueue) /// 用户在三人桌子
	{
		runRoomChangeDeskThree(pkQueueDeskDataNode, pkQueueUserDataNode, pkQueueHallDataRoom);
	}
	else /// 用户在单人队列
	{
	}

	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	return	1;
}

int mtServiceRoomChangeDesk::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

int mtServiceRoomChangeDesk::getQueueUserPreQueueUser(int iQueueUserCurIdx)
{
	if (2 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx - 2;
	}

	return iQueueUserCurIdx + 1;
}

int mtServiceRoomChangeDesk::playRoomChangeDeskOne(mtQueueUser::DataNode* pkQueueUserDataNode, mtQueueHall::DataRoom* pkQueueHallDataRoom)
{
	if (NULL != pkQueueUserDataNode && NULL == pkQueueUserDataNode->pkQueueDeskDataNode)
	{
		if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER == pkQueueUserDataNode->lUserExit
			&& MT_QUEUE_USER_ID_AI_START > pkQueueUserDataNode->lUserId)
		{
			pkQueueUserDataNode->lUserExit  = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING;
			pkQueueUserDataNode->ullTimeEnterRun  = GetTickCount();
			pkQueueHallDataRoom->kQueueOneList.push(&pkQueueUserDataNode);

			return 1;
		}
	}

	return 0;
}

int mtServiceRoomChangeDesk::runRoomChangeDeskTwo(mtQueueDesk::DataNode* pkQueueDeskDataNode,
	                                              mtQueueUser::DataNode* pkQueueUserDataNode,
											      mtQueueHall::DataRoom* pkQueueHallDataRoom)
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
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserDeskNextIdx])
	{
		/// 回收ai节点
		pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 从两人桌子队列，移除桌子
		pkQueueDeskDataNode->lDeskExit  = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 把这个人重新放入到单人队列等待重新组桌子
		pkQueueUserDataNode->ullTimeEnterRun     = GetTickCount();
		pkQueueUserDataNode->lIsWaitStart        = 0;
	}
	else /// 从新回到单人队列
	{
		/// 从两人桌子队列，移除桌子
		pkQueueDeskDataNode->lDeskExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		pkQueueUserDataNode->ullTimeEnterRun     = GetTickCount();
		pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->ullTimeEnterRun     = GetTickCount();
	}

	return 1;
}

int mtServiceRoomChangeDesk::runRoomChangeDeskThree(mtQueueDesk::DataNode* pkQueueDeskDataNode, 
	                                                mtQueueUser::DataNode* pkQueueUserDataNode,
												    mtQueueHall::DataRoom* pkQueueHallDataRoom)
{
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
			/// 回收第一个ai节点
			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextIdx]->lUserExit     = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 回收第二个ai节点
			pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDeskNextNextIdx]->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 从三人桌子队列，移除桌子
			pkQueueDeskDataNode->lDeskExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

			/// 把这个人重新放入到单人队列 等待重新组桌子
			pkQueueUserDataNode->lIsWaitStart                                              = 0;
			pkQueueUserDataNode->ullTimeEnterRun                                           = GetTickCount();
		}
		else /// 需要从新回到两人桌子
		{
			/// 把这个人重新放入到单人队列 等待重新组桌子
			pkQueueUserDataNode->lUserExit                              = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_CHANGE;
			pkQueueUserDataNode->ullTimeEnterRun                        = GetTickCount();
			pkQueueUserDataNode->lIsWaitStart                           = 0;
		}
	}
	else
	{
		/// 把这个人重新放入到单人队列 等待重新组桌子
		pkQueueUserDataNode->lUserExit                              = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_CHANGE;
		pkQueueUserDataNode->ullTimeEnterRun                        = GetTickCount();
		pkQueueUserDataNode->lIsWaitStart                           = 0;
	}

	return 1;
}

int mtServiceRoomChangeDesk::exit()
{

	return	0;
}

