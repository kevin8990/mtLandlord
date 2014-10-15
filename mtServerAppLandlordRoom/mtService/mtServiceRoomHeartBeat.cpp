#include "mtServiceRoomHeartBeat.h"
#include "mtPrint.h"

mtServiceRoomHeartBeat::mtServiceRoomHeartBeat():
m_pkQueueUser(NULL),
m_pkQueuePacket(NULL)
{

}

mtServiceRoomHeartBeat::~mtServiceRoomHeartBeat()
{

}

int mtServiceRoomHeartBeat::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

///	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;

	return	1;
}

int mtServiceRoomHeartBeat::run( DataRun* pkDataRun )
{
	DataRead* pkDataRead                            = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode      = pkDataRun->pkQueueUserDataNode;
	mtPrint(pkDataRead);
	if (!pkQueueUserDataNode)
	{
		return 0;
	}

	pkQueueUserDataNode->ullTimeEnterRun            = GetTickCount();
///	mtQueueHall::DataRoom*	pkQueueHallDataRoom     = m_pkQueueHall->getRoom(pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
	mtQueueDesk::DataNode* pkQueueDeskDataNode      = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	MT_DEBUG(4, "\n(UserId=%d)~~~~~~~~ Heart Beat.", pkQueueUserDataNode->lUserId);
	if (!pkQueueDeskDataNode)
	{
///		SetEvent(pkQueueHallDataRoom->hEvent);
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	MT_DEBUG(4, "\n(UserId=%d)~~~~~~~~ Heart Beat (Space=%d,Room=%d)......", pkQueueUserDataNode->lUserId, pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes              = sizeof(kDataWrite);
	kDataWrite.lServiceType              = pkDataRead->lServiceType;
	kDataWrite.lUserId                   = 0;
	pkQueueUserDataNode->ullTimeEnterRun = GetTickCount();
	//SOCKET uiSocket                      = pkQueueUserDataNode->uiSockets;
	SOCKET uiSocket                       = INVALID_SOCKET;
	SOCKET uiSocketCardOutTimeOut        = INVALID_SOCKET;
		
	//  心跳信息为真实用户
	if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkQueueUserDataNode->lUserDeskIndex]
	&& pkQueueUserDataNode)
	{	
		long m = pkQueueDeskDataNode->plPlayerMode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext];
		void * p = pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext];
		/// 当前要将要出牌的用户是否心跳超时
		if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]
		 && pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext])
		{
			if (MT_SERVICE_HEART_BEAT_TIME < (GetTickCount() - pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]->ullTimeEnterRun))
			{
				kDataWrite.lUserId    = pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]->lUserId;
				uiSocket = pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]->uiSockets;
			}
			else /// 当前要将要出牌的用户未心跳超时
			{
				kDataWrite.lUserId    = 0;
			}		
				mtQueuePacket::DataNode* pkQueuePacketDataNode = NULL;
				pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("heartbeat78");
				if (pkQueuePacketDataNode)
				{
					pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
					memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
					mtPrint(&kDataWrite);
					m_pkQueueUser->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNode);
				}
			}
	}
	

	//判断其它哪些用户心跳超时
	for(int index = 0;index<MT_NODE_PLAYER;index++)
	{
		/*	MT_DEBUG(4, "\n(landrod=%d,outnext=%d,index=%d,UserId=%d Model=%d)~~~~~~~~ Heart Beat=%d.",
		pkQueueDeskDataNode->lQueueUserDefaultLandlord,
		pkQueueDeskDataNode->lUserDeskIdxCardOutNext,index, 
		pkQueueDeskDataNode->pkQueueUserDataNode[index]->lUserId,
		pkQueueDeskDataNode->plPlayerMode[index],
		(GetTickCount()-pkQueueDeskDataNode->pkQueueUserDataNode[index]-
		>ullTimeEnterRun));*/

		if(index != pkQueueUserDataNode->lUserDeskIndex)
		{
			if(pkQueueDeskDataNode->pkQueueUserDataNode[index] && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[index])
			{
				if(MT_SERVICE_HEART_BEAT_TIME < (GetTickCount()-pkQueueDeskDataNode->pkQueueUserDataNode[index]->ullTimeEnterRun))
				{
					MT_DEBUG_PTCL("\nClose TimeOut Heart Beat(UserId=%d)(Socket=%d)", pkQueueDeskDataNode->pkQueueUserDataNode[index]->lUserId,
						pkQueueDeskDataNode->pkQueueUserDataNode[index]->uiSockets);

					int iRet = closesocket(pkQueueDeskDataNode->pkQueueUserDataNode[index]->uiSockets);
					MT_DEBUG_PTCL("\nReturn:%d %d",iRet,WSAGetLastError());
				} 
			}
		}
	}

	/// 判断有没有人超时（抢地主或者出牌）
	if (pkQueueDeskDataNode && MT_SERVICE_USER_CARD_OUT_TIME < (GetTickCount() - pkQueueDeskDataNode->ullTimeEnterRun))
	{
		//超时出牌
		if (pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]
			&& mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext])
		{
			uiSocketCardOutTimeOut = pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]->uiSockets;
		}
	}
	
	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	if (kDataWrite.lUserId)
	{
		MT_DEBUG_PTCL("\nClose TimeOut Heart Beat(NextOut UserId=%d)(Socket=%d),time:%d", kDataWrite.lUserId,uiSocket,
			GetTickCount() - pkQueueDeskDataNode->pkQueueUserDataNode[pkQueueDeskDataNode->lUserDeskIdxCardOutNext]->ullTimeEnterRun);
		closesocket(uiSocket);
	}

	if (INVALID_SOCKET != uiSocketCardOutTimeOut)
	{
		MT_DEBUG_PTCL("\nClose TimeOut CARDOUT(UserId=%d)(Socket=%d) time:%d", kDataWrite.lUserId,uiSocketCardOutTimeOut,
			GetTickCount() - pkQueueDeskDataNode->ullTimeEnterRun);
		closesocket(uiSocketCardOutTimeOut);
	}

	///SetEvent(pkQueueHallDataRoom->hEvent);
	return	1;
}

int mtServiceRoomHeartBeat::exit()
{

	return	0;
}

