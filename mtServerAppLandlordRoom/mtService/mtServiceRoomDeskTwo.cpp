#include "mtServiceRoomDeskTwo.h"
#include "mtServiceRoomWaitStartRecv.h"
#include "mtServiceMgr.h"

mtServiceRoomDeskTwo::mtServiceRoomDeskTwo():
	m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL)
{

}

mtServiceRoomDeskTwo::~mtServiceRoomDeskTwo()
{

}

int mtServiceRoomDeskTwo::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	m_pkQueueUser           = pkDataInit->pkQueueUser;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkServiceMgr          = pkDataInit->pkServiceMgr;

	return	1;
}

int mtServiceRoomDeskTwo::run( DataRun* pkDataRun )
{
	mtQueueUser::DataNode* pkQueueUserDataNode  = pkDataRun->pkQueueUserDataNode;
	if (NULL == pkQueueUserDataNode)
	{
		m_pkQueuePacket->push(pkDataRun->pkQueuePacketDataNode);
		return 0;
	}

	mtQueueDesk::DataNode* pkQueueDeskDataNode  = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
	if (NULL == pkQueueDeskDataNode)
	{
		m_pkQueuePacket->push(pkDataRun->pkQueuePacketDataNode);
		return 1;
	}
	MT_DEBUG_PTCL("\n组成双人桌子![U1:%x,%d] [U2:%x,%d]",pkQueueDeskDataNode->pkQueueUserDataNode[0],
		                                             pkQueueDeskDataNode->pkQueueUserDataNode[0]->lUserId,
													 pkQueueDeskDataNode->pkQueueUserDataNode[1],
													 pkQueueDeskDataNode->pkQueueUserDataNode[1]->lUserId);
	/// 响应用户组桌子成功
	DataWrite kDataWrite;
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lResult           = 1;
	kDataWrite.lServiceType      = mtServiceMgr::E_SERVICE_ROOM_DESK_TWO;

	/// 响应第一个人
	long iPlayerIdx = 0;

	/// 是ai
	mtQueuePacket::DataNode* pkQueuePacketDataNode;
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iPlayerIdx])
	{
		playTwoDeskAi();
	}
	else /// 真实用户
	{
		pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("Desktwo62");
		setWriteData(pkQueueDeskDataNode->pkQueueUserDataNode[iPlayerIdx + 1], NULL, &kDataWrite);
		memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
		pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;

		m_pkQueueUser->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iPlayerIdx], pkQueuePacketDataNode);
	}

	/// 响应第二个人
	memset(&kDataWrite, 0, sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);
	kDataWrite.lResult           = 1;
	kDataWrite.lServiceType      = mtServiceMgr::E_SERVICE_ROOM_DESK_TWO;

	iPlayerIdx = 1;

	/// 是ai
	if (mtQueueDesk::E_PLAYER_MODE_AI == pkQueueDeskDataNode->plPlayerMode[iPlayerIdx])
	{
		playTwoDeskAi();
		/*Sleep(1000);*/
		///runTwoDeskWaitStartRecvAi(pkQueueDeskDataNode->pkQueueUserDataNode[iPlayerIdx]);
	}
	else /// 真实用户
	{
		pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("Desktwo87");
		setWriteData(NULL, pkQueueDeskDataNode->pkQueueUserDataNode[iPlayerIdx - 1], &kDataWrite);
		memcpy(pkQueuePacketDataNode->pcData, &kDataWrite, kDataWrite.lStructBytes);
		pkQueuePacketDataNode->kWSABuf.len  = kDataWrite.lStructBytes;

		m_pkQueueUser->DataWrite(pkQueueDeskDataNode->pkQueueUserDataNode[iPlayerIdx], pkQueuePacketDataNode);
	}

	m_pkQueuePacket->push(pkDataRun->pkQueuePacketDataNode);

	return	1;
}

int mtServiceRoomDeskTwo::exit()
{

	return	0;
}

int mtServiceRoomDeskTwo::setWriteData(mtQueueUser::DataNode* pkDataNode1, mtQueueUser::DataNode* pkDataNode2, DataWrite* pkDataWrite)
{
	if (pkDataNode1)
	{
		mtQueueDesk::DataNode* pQueueDeskDataNode = (mtQueueDesk::DataNode*)pkDataNode1->pkQueueDeskDataNode;

		pkDataWrite->kUserInfo[0].lUserId            = pkDataNode1->lUserId;
		pkDataWrite->kUserInfo[0].lUserIconId        = pkDataNode1->lUserIconId;
		pkDataWrite->kUserInfo[0].lUserSex           = pkDataNode1->lUserSex;
		pkDataWrite->kUserInfo[0].lUserGold          = pkDataNode1->lUserGold;
		pkDataWrite->kUserInfo[0].lUserLevel         = pkDataNode1->lUserLevel;
		pkDataWrite->kUserInfo[0].lUserScore         = pkDataNode1->lUserScore;
		pkDataWrite->kUserInfo[0].lUserAllChess      = pkDataNode1->lUserAllChess;
		pkDataWrite->kUserInfo[0].lUserWinChess      = pkDataNode1->lUserWinChess;
		pkDataWrite->kUserInfo[0].lUserWinRate       = pkDataNode1->lUserWinRate;
		pkDataWrite->kUserInfo[0].lUserOffLineCount  = pkDataNode1->lUserOffLineCount;
		pkDataWrite->kUserInfo[0].lIsWaitStart       = pQueueDeskDataNode->plIsWaitStart[pkDataNode1->lUserDeskIndex];
		pkDataWrite->kUserInfo[0].lPlayerType        = pQueueDeskDataNode->plPlayerType[pkDataNode1->lUserDeskIndex];
		pkDataWrite->kUserInfo[0].ulDeskId           = (unsigned long)pkDataNode1->pkQueueDeskDataNode;
		memcpy(pkDataWrite->kUserInfo[0].plPropsCount, pkDataNode1->plPropsCount, sizeof(pkDataNode1->plPropsCount));
		memcpy(pkDataWrite->kUserInfo[0].pcUserAccountNum, pkDataNode1->pcUserAccountNum, sizeof(pkDataNode1->pcUserAccountNum));
		memcpy(pkDataWrite->kUserInfo[0].pcUserName, pkDataNode1->pcUserName, sizeof(pkDataNode1->pcUserName));
		memcpy(pkDataWrite->kUserInfo[0].pcUserNickName, pkDataNode1->pcUserNickName, sizeof(pkDataNode1->pcUserNickName));
	}

	if (pkDataNode2)
	{
		mtQueueDesk::DataNode* pQueueDeskDataNode = (mtQueueDesk::DataNode*)pkDataNode2->pkQueueDeskDataNode;

		pkDataWrite->kUserInfo[1].lUserId            = pkDataNode2->lUserId;
		pkDataWrite->kUserInfo[1].lUserIconId        = pkDataNode2->lUserIconId;
		pkDataWrite->kUserInfo[1].lUserSex           = pkDataNode2->lUserSex;
		pkDataWrite->kUserInfo[1].lUserGold          = pkDataNode2->lUserGold;
		pkDataWrite->kUserInfo[1].lUserLevel         = pkDataNode2->lUserLevel;
		pkDataWrite->kUserInfo[1].lUserScore         = pkDataNode2->lUserScore;
		pkDataWrite->kUserInfo[1].lUserAllChess      = pkDataNode2->lUserAllChess;
		pkDataWrite->kUserInfo[1].lUserWinChess      = pkDataNode2->lUserWinChess;
		pkDataWrite->kUserInfo[1].lUserWinRate       = pkDataNode2->lUserWinRate;
		pkDataWrite->kUserInfo[1].lUserOffLineCount  = pkDataNode2->lUserOffLineCount;
		pkDataWrite->kUserInfo[1].lIsWaitStart       = pQueueDeskDataNode->plIsWaitStart[pkDataNode2->lUserDeskIndex];
		pkDataWrite->kUserInfo[1].lPlayerType        = pQueueDeskDataNode->plPlayerType[pkDataNode2->lUserDeskIndex];
		pkDataWrite->kUserInfo[1].ulDeskId           = (unsigned long)pkDataNode2->pkQueueDeskDataNode;
		memcpy(pkDataWrite->kUserInfo[1].plPropsCount, pkDataNode2->plPropsCount, sizeof(pkDataNode2->plPropsCount));
		memcpy(pkDataWrite->kUserInfo[1].pcUserAccountNum, pkDataNode2->pcUserAccountNum, sizeof(pkDataNode2->pcUserAccountNum));
		memcpy(pkDataWrite->kUserInfo[1].pcUserName, pkDataNode2->pcUserName, sizeof(pkDataNode2->pcUserName));
		memcpy(pkDataWrite->kUserInfo[1].pcUserNickName, pkDataNode2->pcUserNickName, sizeof(pkDataNode2->pcUserNickName));
	}

	return 1;
}

int mtServiceRoomDeskTwo::runTwoDeskWaitStartRecvAi(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	/// 响应建立三人桌子
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("Desktwo160");
	if (pkQueuePacketDataNode)
	{
		mtServiceRoomWaitStartRecv::DataRead* pkDataRead = (mtServiceRoomWaitStartRecv::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes           = sizeof(mtServiceRoomWaitStartRecv::DataRead);
		pkDataRead->lServiceType           = mtServiceMgr::E_SERVICE_ROOM_WAIT_START;

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	   = pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	   = pkQueueUserDataNode;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收packet包节点
		m_pkQueuePacket->push(pkQueuePacketDataNode);
	}

	return 1;
}

