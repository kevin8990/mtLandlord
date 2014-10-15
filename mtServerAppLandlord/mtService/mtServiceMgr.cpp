#include "mtServiceMgr.h"
#include "mtPlatform.h"
#include "mtServiceHallExit.h"
#include "mtServiceHallRegister.h"
#include "mtServiceHallUpdateRoom.h"
#include "mtServiceHallRank.h"
#include "mtServiceHallUpdatePlayInfo.h"
#include "mtServiceHallUpdateSocialInfo.h"
#include "mtServiceHallGiveGold.h"
#include "mtServiceHallHall2Room.h"
#include "mtServiceHallRoom2Hall.h"
#include "mtServiceHallPropBuy.h"
#include "mtServiceHallGoldBuy.h"
#include "mtServiceHallFeedBack.h"
#include "mtServiceHallGoldBuyStatus.h"
#include "mtServiceGetLotteryArg.h"
#include "mtServiceLottery.h"
#include "mtServiceSystemMessage.h"
#include "mtServiceGetTaskInfo.h"
#include "mtServiceGetTaskAward.h"
#include "mtServiceGetPetInfo.h"
#include "mtServiceFeedPet.h"
#include "mtServiceSpitGold.h"
#include "mtServiceCashLottery.h"

#include "mtServiceMatchGetRoomInfo.h"
#include "mtServiceMatchGetUserInfo.h"
#include "mtServiceMatchSignUp.h"
#include "mtServiceMatchGetRoomStatus.h"
#include "mtServiceMatchGetRank.h"
#include "mtServiceMatchAward.h"

mtServiceMgr::~mtServiceMgr()
{

}

mtServiceMgr::mtServiceMgr()
{

}

int mtServiceMgr::run(void* pData)
{
	DataHead kDataHead;
	memset(&kDataHead, 0 , sizeof(kDataHead));
	
	int iRet = -2;
	if ((iRet=runReadDataHead(*(SOCKET*)pData, &kDataHead)))
	{
		//if (E_SERVICE_HALL_END > kDataHead.lServiceType)
		if (E_SERVICE_HALL_END > kDataHead.lServiceType && kDataHead.lServiceType>=E_SERVICE_HALL_BEG)
		{
			   m_pkServiceList[kDataHead.lServiceType]->run(pData);
		}/*else{
			//SOCKET socket = *(SOCKET*)pData;
			closesocket(m_kQueueWorkNodeClient.kSocketClient);
			m_kQueueWorkNodeClient.kSocketClient = -1;
		}*/
	}
	//printf("\nrunReadDataHead [iRet:%d]",iRet);
	return 1;
}

int mtServiceMgr::exit()
{
	int iServiceIdx;
	for (iServiceIdx = E_SERVICE_HALL_BEG; iServiceIdx < E_SERVICE_HALL_END; iServiceIdx++)
	{
		if (NULL != m_pkServiceList[iServiceIdx])
		{
			m_pkServiceList[iServiceIdx]->exit();
			delete m_pkServiceList[iServiceIdx];
			m_pkServiceList[iServiceIdx] = NULL;
		}
	}

	return	MTE_ERROR_OK;
}

int mtServiceMgr::init( DataInit* pkDataInit )
{
	m_pkSQLEnv				= pkDataInit->pkSQLEnv;
	m_lSpinCount                   = pkDataInit->lSpinCount;
	m_pkQueueMgr                   = pkDataInit->pkQueueMgr;
	m_pcServiceExeDir              = pkDataInit->pcServiceExeDir;

	initService();

	return 1;
}

int mtServiceMgr::initService()
{
	initServiceLoginConnect();				//
	initServiceHallRegister();
	initServiceHallPropBuy();

	initServiceHallGoldBuy();
	initServiceHallGoldBuyStatus();
	initServiceHallRank();

	initServiceHallUpdatePlayInfo();
	initServiceHallUpdateSocialInfo();		//
	initServiceHallGiveGold();

	initServiceHallUpdateRoom();
	initServiceHallHall2Room();
	initServiceHallRoom2Hall();

	initServiceHallFeedBack();
	initServiceHallExit();
    initServiceGetLottertArg();
    initServiceLottery();
	initServiceSystemMessage();

	initServiceGetTaskInfo();
	initServiceGetTaskAward();
	initServiceGetPetInfo();
	initServiceFeedPet();
	initServiceSpitGold();
	iniServiceCashLottery();

	initServiceMatchGetRoomInfo();
	initServiceMatchGetUserInfo();
	initServiceMatchSignUp();
	initServiceMatchGetRoomStatus();
	initServiceMatchGetRank();
	initServiceMatchAward();

	return	1;
}

int mtServiceMgr::initServiceLoginConnect()
{
	mtServiceConnect::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
    kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	///kDataInit.pkSQLQDBC	      = m_pkSQLEnv->getQDBC(mtSQLEnv::E_DBC_CONNECT);
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_CONNECT]	= new mtServiceConnect;
	return m_pkServiceList[E_SERVICE_HALL_CONNECT]->init(&kDataInit);
}

int mtServiceMgr::initServiceHallRegister()
{
	mtServiceHallRegister::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;

	m_pkServiceList[E_SERVICE_HALL_REGISTER]	= new mtServiceHallRegister;
	return m_pkServiceList[E_SERVICE_HALL_REGISTER]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallPropBuy()
{
	mtServiceHallPropBuy::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
    kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_PROP_BUY]	= new mtServiceHallPropBuy;
	return m_pkServiceList[E_SERVICE_HALL_PROP_BUY]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallGoldBuy()
{
	mtServiceHallGoldBuy::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GOLD_BUY]	= new mtServiceHallGoldBuy;
	return m_pkServiceList[E_SERVICE_HALL_GOLD_BUY]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallGoldBuyStatus()
{
	mtServiceHallGoldBuyStatus::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;

	m_pkServiceList[E_SERVICE_HALL_GOLD_BUY_STATUS]	= new mtServiceHallGoldBuyStatus;
	return m_pkServiceList[E_SERVICE_HALL_GOLD_BUY_STATUS]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallRank()
{
	mtServiceHallRank::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;

	m_pkServiceList[E_SERVICE_HALL_RANKING]	= new mtServiceHallRank;
	return m_pkServiceList[E_SERVICE_HALL_RANKING]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallUpdatePlayInfo()
{
	mtServiceHallUpdatePlayInfo::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv			    = m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_UPDATE_PALY_INFO]	= new mtServiceHallUpdatePlayInfo;
	return m_pkServiceList[E_SERVICE_HALL_UPDATE_PALY_INFO]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallUpdateSocialInfo()
{
	mtServiceHallUpdateSocialInfo::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv			    = m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_UPDATE_SOCIAL_INFO]	= new mtServiceHallUpdateSocialInfo;
	return m_pkServiceList[E_SERVICE_HALL_UPDATE_SOCIAL_INFO]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallGiveGold()
{
	mtServiceHallGiveGold::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv            = m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GIVE_GOLD]	= new mtServiceHallGiveGold;
	return m_pkServiceList[E_SERVICE_HALL_GIVE_GOLD]->init(&kDataInit);
}

int mtServiceMgr::initServiceHallUpdateRoom()
{
	mtServiceHallUpdateRoom::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv            = m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_UPDATE_ROOM]	= new mtServiceHallUpdateRoom;
	return m_pkServiceList[E_SERVICE_HALL_UPDATE_ROOM]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallHall2Room()
{
	mtServiceHallHall2Room::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;

	m_pkServiceList[E_SERVICE_HALL_HALL_2_ROOM]	= new mtServiceHallHall2Room;
	return m_pkServiceList[E_SERVICE_HALL_HALL_2_ROOM]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallRoom2Hall()
{
	mtServiceHallRoom2Hall::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv            = m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_ROOM_2_HALL]	= new mtServiceHallRoom2Hall;
	return m_pkServiceList[E_SERVICE_HALL_ROOM_2_HALL]->init(&kDataInit);
}

int	mtServiceMgr::initServiceHallFeedBack()
{
	mtServiceHallFeedBack::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_FEED_BACK]	= new mtServiceHallFeedBack;
	return m_pkServiceList[E_SERVICE_HALL_FEED_BACK]->init(&kDataInit);
}

int mtServiceMgr::initServiceHallExit()
{
	mtServiceHallExit::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pcServiceExeDir     = m_pcServiceExeDir;
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkUserDataNodeList  = m_pkQueueMgr->m_pkHall->m_kDataHall.pkUserDataNodeList;

	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_EXIT]	= new mtServiceHallExit;
	return m_pkServiceList[E_SERVICE_HALL_EXIT]->init(&kDataInit);
}

int mtServiceMgr::initServiceGetLottertArg()
{
	mtServiceGetLotteryArg::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GETLOTTERYARG]	= new mtServiceGetLotteryArg;
	return m_pkServiceList[E_SERVICE_HALL_GETLOTTERYARG]->init(&kDataInit);
}

int mtServiceMgr::initServiceLottery()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_LOTTERY]	= new mtServiceLottery();
	return m_pkServiceList[E_SERVICE_HALL_LOTTERY]->init(&kDataInit);

}

int mtServiceMgr::initServiceSystemMessage()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_SYSTEMMESSAGE]	= new mtServiceSystemMessage();
	return m_pkServiceList[E_SERVICE_HALL_SYSTEMMESSAGE]->init(&kDataInit);

}

int mtServiceMgr::initServiceGetTaskInfo()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GET_TASK_INFO]	= new mtServiceGetTaskInfo();
	return m_pkServiceList[E_SERVICE_HALL_GET_TASK_INFO]->init(&kDataInit);

}

int mtServiceMgr::initServiceGetTaskAward()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GET_TASK_AWARD]	= new mtServiceGetTaskAward();
	return m_pkServiceList[E_SERVICE_HALL_GET_TASK_AWARD]->init(&kDataInit);

}

int mtServiceMgr::initServiceGetPetInfo()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_GET_PET_INFO]	= new mtServiceGetPetInfo();
	return m_pkServiceList[E_SERVICE_HALL_GET_PET_INFO]->init(&kDataInit);

}

int mtServiceMgr::initServiceFeedPet()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_FEED_PET]	= new mtServiceFeedPet();
	return m_pkServiceList[E_SERVICE_HALL_FEED_PET]->init(&kDataInit);

}

int mtServiceMgr::initServiceSpitGold()
{
	mtServiceLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_SPIT_GOLD]	= new mtServiceSpitGold();
	return m_pkServiceList[E_SERVICE_HALL_SPIT_GOLD]->init(&kDataInit);

}

int mtServiceMgr::iniServiceCashLottery()
{
	mtServiceCashLottery::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_HALL_CASH_LOTTERY]	= new mtServiceCashLottery();
	return m_pkServiceList[E_SERVICE_HALL_CASH_LOTTERY]->init(&kDataInit);

}

int mtServiceMgr::initServiceMatchGetRoomInfo()
{
	mtServiceMatchGetRoomInfo::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_GET_ROOM_INFO]	= new mtServiceMatchGetRoomInfo();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_GET_ROOM_INFO]->init(&kDataInit);
}

int mtServiceMgr::initServiceMatchGetUserInfo()
{
	mtServiceMatchGetUserInfo::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_GET_USER_INFO]	= new mtServiceMatchGetUserInfo();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_GET_USER_INFO]->init(&kDataInit);
}

int mtServiceMgr::initServiceMatchSignUp()
{
	mtServiceMatchSignUp::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_SIGN_UP]	= new mtServiceMatchSignUp();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_SIGN_UP]->init(&kDataInit);
}

int mtServiceMgr::initServiceMatchGetRoomStatus()
{
	mtServiceMatchGetRoomStatus::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_GET_ROOM_STATUS]	= new mtServiceMatchGetRoomStatus();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_GET_ROOM_STATUS]->init(&kDataInit);
}

int mtServiceMgr::initServiceMatchGetRank()
{
	mtServiceMatchGetRank::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_GET_RANK]	= new mtServiceMatchGetRank();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_GET_RANK]->init(&kDataInit);
}

int mtServiceMgr::initServiceMatchAward()
{
	mtServiceMatchAward::DataInit		kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	kDataInit.pkQueuePacket       = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkQueueHall         = m_pkQueueMgr->m_pkHall;
	kDataInit.pkSQLEnv				= m_pkSQLEnv;

	m_pkServiceList[E_SERVICE_MATCH_HALL_AWARD]	= new mtServiceMatchAward();
	return m_pkServiceList[E_SERVICE_MATCH_HALL_AWARD]->init(&kDataInit);
}

int	mtServiceMgr::runReadDataHead(SOCKET socket, DataHead* pkDataHead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataHead, sizeof(DataHead), MSG_PEEK);

	if (0 > iReadBytesTotalHas || iReadBytesTotalHas < sizeof(pkDataHead->lStructBytes))
	{	
		return	0;
	}

	if (0 < pkDataHead->lStructBytes && iReadBytesTotalHas >= pkDataHead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataHead, sizeof(DataHead), MSG_PEEK);
	}
	return (iReadBytesTotalHas < sizeof(DataHead)) ? 0 : 1;
}
