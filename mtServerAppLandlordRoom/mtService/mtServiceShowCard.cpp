#include "mtServiceShowCard.h"
#include "mtQueueDesk.h"
#include "mtPrint.h"

mtServiceShowCard::mtServiceShowCard():
	m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL)
{

}

mtServiceShowCard::~mtServiceShowCard()
{

}

int mtServiceShowCard::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	return	1;
}

int mtServiceShowCard::run( DataRun* pkDataRun )
{
	DataRead* pkDataRead                       = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode = pkDataRun->pkQueueUserDataNode;
	mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	mtPrint(pkDataRead);
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lServiceType      = pkDataRead->lServiceType;
	kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
	kDataWrite.lShowCard         = pkDataRead->lShowCard;
 	memcpy(kDataWrite.pcCard, pkDataRead->pcCard, sizeof(pkDataRead->pcCard));

	int		iUserIdx;
	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
	{
		//  响应明牌信息给所有真实用户
		if (pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
		 && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("serviceshowcard48");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
				memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				mtPrint(&kDataWrite);
				m_pkQueueUser->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
			}
		}
	}

	return 1;
}

int mtServiceShowCard::exit()
{
	return	1;
}

