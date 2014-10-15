#include "mtServiceRoomIncrementGold.h"
#include "mtServiceMgr.h"

mtServiceRoomIncrementGold::mtServiceRoomIncrementGold()
{

}

mtServiceRoomIncrementGold::~mtServiceRoomIncrementGold()
{

}

int mtServiceRoomIncrementGold::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	return	1;
}

int mtServiceRoomIncrementGold::run( DataRun* pkDataRun )
{
	mtQueueUser::DataNode*      pkQueueUserDataNode	  = pkDataRun->pkQueueUserDataNode;
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = pkDataRun->pkQueuePacketDataNode;
	DataRead*                   pkDataRead            = (DataRead*)pkQueuePacketDataNode->pcData;
	if (NULL == pkQueueUserDataNode)
	{
		MT_DEBUG(PTCL, "mtServiceRoomIncrementGold  USERNODE IS NULL !\n");
		return 1;
	}

	/// 发送玩家增加金币信息响应给所有人
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes         = sizeof(kDataWrite);
	kDataWrite.lServiceType         = pkDataRead->lServiceType;
	kDataWrite.lUserId              = pkQueueUserDataNode->lUserId;
	kDataWrite.lGold                = pkDataRead->lGold;
	pkQueueUserDataNode->lUserGold  += pkDataRead->lGold;

	mtQueuePacket::DataNode* pkQueuePacketDataNodeSendToUser = NULL;
	mtQueueDesk::DataNode* pkQueueDeskDataNode        = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	if (NULL == pkQueueDeskDataNode)
	{
		pkQueuePacketDataNodeSendToUser = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomIncrementgold46");
		if (pkQueuePacketDataNodeSendToUser)
		{
			pkQueuePacketDataNodeSendToUser->kWSABuf.len    = kDataWrite.lStructBytes;
			memcpy (pkQueuePacketDataNodeSendToUser->pcData, &kDataWrite, kDataWrite.lStructBytes);
			m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueUserDataNode, pkQueuePacketDataNodeSendToUser);
		}

		MT_DEBUG(PTCL, "mtServiceRoomIncrementGold  DESKNODE IS NULL !\n");
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	int		iUserIdx;
	for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
	{
		//  响应加分信息给所有真实用户
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
		&& mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
		{
			pkQueuePacketDataNodeSendToUser = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("roomIncrementgold66");
			if (pkQueuePacketDataNodeSendToUser)
			{
				pkQueuePacketDataNodeSendToUser->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNodeSendToUser->pcData, &kDataWrite, kDataWrite.lStructBytes);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNodeSendToUser);
			}
		}
	}

	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	return	1;
}

int mtServiceRoomIncrementGold::exit()
{

	return	0;
}

