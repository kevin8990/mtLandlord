#include "mtServiceRoomChatShortCut.h"
#include "mtPrint.h"

mtServiceRoomChatShortCut::mtServiceRoomChatShortCut():
m_pkQueueUser(NULL),
m_pkQueuePacket(NULL)
{

}

mtServiceRoomChatShortCut::~mtServiceRoomChatShortCut()
{

}

int mtServiceRoomChatShortCut::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;

	return	1;
}

int mtServiceRoomChatShortCut::run( DataRun* pkDataRun )
{
	DataRead* pkDataRead                            = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode      = pkDataRun->pkQueueUserDataNode;
	mtQueueDesk::DataNode* pkQueueDeskDataNode      = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	mtPrint(pkDataRead);
	///MT_DEBUG(4, "(UserId=%d)???????? Chat short cut.\n", pkQueueUserDataNode->lUserId);
	if (!pkQueueDeskDataNode)
	{
		return 1;
	}

	EnterCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	///MT_DEBUG(4, "(UserId=%d)???????? Chat short cut (Space=%d,Room=%d)......\n", pkQueueUserDataNode->lUserId, pkQueueUserDataNode->lSpaceId, pkQueueUserDataNode->lRoomId);
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
	kDataWrite.lShortCutIdx      = pkDataRead->lShortCutIdx;

	int		iUserIdx;
	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
	{
		//  响应打牌信息给所有真实用户
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
		 && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("charshortcut55");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
			 }
		}
	}

	LeaveCriticalSection(&pkQueueDeskDataNode->kCriticalSection);
	return	1;
}

int mtServiceRoomChatShortCut::exit()
{

	return	0;
}

