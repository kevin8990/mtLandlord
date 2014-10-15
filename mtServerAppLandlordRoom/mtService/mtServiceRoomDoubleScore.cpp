#include "mtServiceRoomDoubleScore.h"
#include "mtQueueDesk.h"

mtServiceRoomDoubleScore::mtServiceRoomDoubleScore():
	m_pkQueueMgr(NULL)
{

}

mtServiceRoomDoubleScore::~mtServiceRoomDoubleScore()
{

}

int mtServiceRoomDoubleScore::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueMgr            = pkDataInit->pkQueueMgr;
	return	1;
}

int mtServiceRoomDoubleScore::run( DataRun* pkDataRun )
{
	DataRead* pkDataRead                            = (DataRead*)pkDataRun->pkQueuePacketDataNode->pcData;
	mtQueueUser::DataNode* pkQueueUserDataNode      = pkDataRun->pkQueueUserDataNode;

	if (NULL == pkQueueUserDataNode)
	{
		return 1;
	}

	mtQueueDesk::DataNode* pkQueueDeskDataNode      = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	if (NULL == pkQueueDeskDataNode)
	{
		return 1;
	}

	/// 更新个人加倍分数
	int		iUserIdx;
	if (-1 != pkDataRead->lDoubleFlag) /// 真实人进行了加倍请求
	{
		pkQueueDeskDataNode->plDoubleScore[pkQueueUserDataNode->lUserDeskIndex]    = pkDataRead->lDoubleFlag;

		//  当前用户是真实人
		DataWrite  kDataWrite;
		memset(&kDataWrite, 0, sizeof(kDataWrite));
		kDataWrite.lStructBytes      = sizeof(kDataWrite);
		kDataWrite.lServiceType      = pkDataRead->lServiceType;
		kDataWrite.lUserId           = pkQueueUserDataNode->lUserId;
		kDataWrite.lDoubleFlag       = pkDataRead->lDoubleFlag;
		mtQueuePacket::DataNode* pkQueuePacketDataNode;

		for (iUserIdx = 0; iUserIdx < MT_NODE_PLAYER; iUserIdx++)
		{
			//  当前真实用户
			if (NULL != pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx]
			 && mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iUserIdx])
			{
				pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("doubleScore60");
				if (NULL != pkQueuePacketDataNode)
				{
					pkQueuePacketDataNode->kWSABuf.len    = kDataWrite.lStructBytes;
					memcpy (pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
					m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iUserIdx], pkQueuePacketDataNode);
				}
			}
		}
	}

	//  当前是真实用户
	if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkQueueUserDataNode->lUserDeskIndex])
	{
		/// 获得下家用户在桌子上的位置
		int iQueueUserNextIdx = getQueueUserNextQueueUser(pkQueueUserDataNode->lUserDeskIndex);

		//  下家是ai用户
		if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextIdx])
		{
			playDoubleScoreAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueUserNextIdx);

			/// 获得ai用户的下家在桌子上的位置
			iQueueUserNextIdx = getQueueUserNextQueueUser(iQueueUserNextIdx);

			//  ai下家还是是ai用户
			if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iQueueUserNextIdx])
			{
				playDoubleScoreAi(pkQueueDeskDataNode, pkDataRead->lServiceType, iQueueUserNextIdx);
			}
		}
	}

	return	1;
}

int mtServiceRoomDoubleScore::exit()
{
	return	1;
}

int mtServiceRoomDoubleScore::playDoubleScoreAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserIdx)
{
	/// 该ai已经叫过加倍了(可能是不加倍)
	if (0 <= pkQueueDeskDataNode->plDoubleScore[iQueueUserIdx])
	{
		return 1;
	}

	/// 更改自己为已经加倍状态
	pkQueueDeskDataNode->plDoubleScore[iQueueUserIdx] = 0;

	/// 获得桌子上的真实人员列表
	DataWrite kDataWrite;

	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lUserId           = pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserIdx]->lUserId;
	kDataWrite.lServiceType      = lServiceType;
	kDataWrite.lDoubleFlag       = 1;
	
	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	int iQueueUserDataNodeIdx;
	for (iQueueUserDataNodeIdx = 0; iQueueUserDataNodeIdx < MT_NODE_PLAYER; iQueueUserDataNodeIdx++)
	{
		/// 发送‘叫0分’ 消息
		//  当前真实用户
		if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[iQueueUserDataNodeIdx])
		{
			pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueueMgr->getQueuePacket()->popInitDataNode("doubleScore128");
			if (pkQueuePacketDataNode)
			{
				pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;
				memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
				m_pkQueueMgr->getQueueUser()->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iQueueUserDataNodeIdx], pkQueuePacketDataNode);
			}
		}
	}

	return 1;
}

int mtServiceRoomDoubleScore::getQueueUserNextQueueUser(int iQueueUserCurIdx)
{
	if (0 == iQueueUserCurIdx)
	{
		return iQueueUserCurIdx + 2;
	}

	return iQueueUserCurIdx - 1;
}

