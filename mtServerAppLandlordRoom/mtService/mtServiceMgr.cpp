#include "mtServiceMgr.h"
#include "mtPlatform.h"
#include "mtServiceRoomDeskTwo.h"
#include "mtServiceRoomDeskThree.h"
#include "mtServiceCardDeal.h"
#include "mtServiceRoomWaitStartRecv.h"
#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomDoubleScore.h"
#include "mtServiceCardOut.h"
#include "mtServiceRoomExit.h"
#include "mtServiceRoomRoundOver.h"
#include "mtServiceRoomChatShortCut.h"
#include "mtServiceRoomChatIcon.h"
#include "mtServiceRoomChatContext.h"
#include "mtServiceRoomRequestCardsLeft.h"
#include "mtServiceRoomChangeDesk.h"
#include "mtServiceShowCard.h"
#include "mtServiceRoomHeartBeat.h"
#include "mtServiceRoomIncrementGold.h"

mtServiceMgr::~mtServiceMgr()
{

}

mtServiceMgr::mtServiceMgr()
{

}

int mtServiceMgr::run(mtService::DataRun* pkDataRun)
{
	DataHead*	                pkDataHead            = (DataHead*)pkDataRun->pkQueuePacketDataNode->pcData;

	if (pkDataHead->lServiceId >= E_SERVICE_ROOM_BEG && pkDataHead->lServiceId < E_SERVICE_ROOM_END)
	{
		m_pkServiceList[pkDataHead->lServiceId]->run(pkDataRun);
		return	1;
	}
	
 	return 0;
}

int mtServiceMgr::exit()
{
	int		iEventGameServiceIdx;
	for (iEventGameServiceIdx = E_SERVICE_ROOM_BEG; iEventGameServiceIdx < E_SERVICE_ROOM_END; iEventGameServiceIdx++)
	{
		if (NULL != m_pkServiceList[iEventGameServiceIdx])
		{
			m_pkServiceList[iEventGameServiceIdx]->exit();
			delete m_pkServiceList[iEventGameServiceIdx];
			m_pkServiceList[iEventGameServiceIdx] = NULL;
		}
	}

	return	1;
}

int mtServiceMgr::init( DataInit* pkDataInit )
{
	m_iSpinCount      = pkDataInit->iSpinCount;
	m_hIOCP           = pkDataInit->hIOCP;
	m_hEventManagerAi = pkDataInit->hEventManagerAi;
	m_pkQueueMgr      = pkDataInit->pkQueueMgr;

	initService();

	return 1;
}

int mtServiceMgr::initService()
{
	initServiceConnect();
	initServiceRoomDeskTwo();
	initServiceRoomDeskThree();
	initServiceRoomWaitStartRecv();
	initServiceRoomCardDeal();
	initServiceRoomGrabLandlord();
	initServiceRoomCardDoubleScore();
	initServiceRoomShowCard();
	initServiceRoomCardOut();
	initServiceRoomRequestCardsLeft();
	initServiceRoomRoundOver();
	initServiceRoomChangeDesk();
	initServiceRoomChatShortCut();
	initServiceRoomChatIcon();
	initServiceRoomChatContext();
	initServiceRoomHeartBeat();
	initServiceRoomIncrementGold();
	initServiceRoomExit();

	return	1;
}

int mtServiceMgr::initServiceConnect()
{
	mtServiceConnect::DataInit		kDataInit; 
	kDataInit.lStructBytes	= sizeof(kDataInit);
	kDataInit.pkQueueMgr    = m_pkQueueMgr;
	kDataInit.pkQueueUser	= m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueueHall	= m_pkQueueMgr->getQueueHall();
	kDataInit.pkQueuePacket = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_CONNECT]	= new mtServiceConnect;
	return m_pkServiceList[E_SERVICE_ROOM_CONNECT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomDeskTwo()
{
	mtServiceRoomDeskTwo::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkServiceMgr   = this;

	m_pkServiceList[E_SERVICE_ROOM_DESK_TWO]	= new mtServiceRoomDeskTwo;
	return m_pkServiceList[E_SERVICE_ROOM_DESK_TWO]->init(&kDataInit);
}

int	mtServiceMgr::initServiceRoomDeskThree()
{
	mtServiceRoomDeskThree::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkServiceMgr   = this;

	m_pkServiceList[E_SERVICE_ROOM_DESK_THREE]	= new mtServiceRoomDeskThree;
	return m_pkServiceList[E_SERVICE_ROOM_DESK_THREE]->init(&kDataInit);
}

int	mtServiceMgr::initServiceRoomWaitStartRecv()
{
	mtServiceRoomWaitStartRecv::DataInit kDataInit; 
	kDataInit.lStructBytes	= sizeof(kDataInit);
	kDataInit.pkQueueMgr    = m_pkQueueMgr;
	kDataInit.pkServiceMgr  = this;
	kDataInit.hIOCP         = m_hIOCP;

	m_pkServiceList[E_SERVICE_ROOM_WAIT_START]	= new mtServiceRoomWaitStartRecv;
	return m_pkServiceList[E_SERVICE_ROOM_WAIT_START]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomCardDeal()
{
	mtServiceCardDeal::DataInit		kDataInit; 
	kDataInit.lStructBytes	   = sizeof(kDataInit);
	kDataInit.pkQueueMgr       = m_pkQueueMgr;
	kDataInit.pkQueueUser      = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket    = m_pkQueueMgr->getQueuePacket();
	kDataInit.pkServiceMgr     = this;
	kDataInit.hEventManagerAi  = m_hEventManagerAi;

	m_pkServiceList[E_SERVICE_ROOM_CARD_DEAL]	= new mtServiceCardDeal;
	return m_pkServiceList[E_SERVICE_ROOM_CARD_DEAL]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomGrabLandlord()
{
	mtServiceRoomGrabLandlord::DataInit kDataInit; 
	kDataInit.lStructBytes	   = sizeof(kDataInit);
	kDataInit.hIOCP            = m_hIOCP;
	kDataInit.pkQueueMgr       = m_pkQueueMgr;
	kDataInit.pkServiceMgr     = this;
	kDataInit.hEventManagerAi  = m_hEventManagerAi;

	m_pkServiceList[E_SERVICE_ROOM_GRAD_LANDLORD]	= new mtServiceRoomGrabLandlord;
	return m_pkServiceList[E_SERVICE_ROOM_GRAD_LANDLORD]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomCardDoubleScore()
{
	mtServiceRoomDoubleScore::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueMgr     = m_pkQueueMgr;

	m_pkServiceList[E_SERVICE_ROOM_DOUBLE_SCORE]	= new mtServiceRoomDoubleScore;
	return m_pkServiceList[E_SERVICE_ROOM_DOUBLE_SCORE]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomShowCard()
{
	mtServiceShowCard::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_SHOW_CARD]	= new mtServiceShowCard;
	return m_pkServiceList[E_SERVICE_ROOM_SHOW_CARD]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomCardOut()
{
	mtServiceCardOut::DataInit		kDataInit; 
	kDataInit.lStructBytes	   = sizeof(kDataInit);
	kDataInit.pkQueueMgr       = m_pkQueueMgr;
	kDataInit.pkQueueUser      = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket    = m_pkQueueMgr->getQueuePacket();
	kDataInit.hEventManagerAi  = m_hEventManagerAi;

	m_pkServiceList[E_SERVICE_ROOM_CARD_OUT]	= new mtServiceCardOut;
	return m_pkServiceList[E_SERVICE_ROOM_CARD_OUT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomRequestCardsLeft()
{
	mtServiceRoomRequestCardsLeft::DataInit   kDataInit; 
	kDataInit.lStructBytes	     = sizeof(kDataInit);
	kDataInit.pkQueueMgr         = m_pkQueueMgr;

	m_pkServiceList[E_SERVICE_ROOM_REQUEST_CARDS_LEFT]	= new mtServiceRoomRequestCardsLeft;
	return m_pkServiceList[E_SERVICE_ROOM_REQUEST_CARDS_LEFT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomRoundOver()
{
	mtServiceRoomRoundOver::DataInit   kDataInit; 
	kDataInit.lStructBytes	     = sizeof(kDataInit);
	kDataInit.pkQueueMgr         = m_pkQueueMgr;
	kDataInit.pkServiceMgr       = this;

	m_pkServiceList[E_SERVICE_ROOM_ROUND_OVER]	= new mtServiceRoomRoundOver;
	return m_pkServiceList[E_SERVICE_ROOM_ROUND_OVER]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomChangeDesk()
{
	mtServiceRoomChangeDesk::DataInit   kDataInit; 
	kDataInit.lStructBytes	     = sizeof(kDataInit);
	kDataInit.pkQueueMgr         = m_pkQueueMgr;

	m_pkServiceList[E_SERVICE_ROOM_CHANGE_DESK]	= new mtServiceRoomChangeDesk;
	return m_pkServiceList[E_SERVICE_ROOM_CHANGE_DESK]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomChatShortCut()
{
	mtServiceRoomChatShortCut::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_CHAT_SHORTCUT]	= new mtServiceRoomChatShortCut;
	return m_pkServiceList[E_SERVICE_ROOM_CHAT_SHORTCUT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomChatIcon()
{
	mtServiceRoomChatIcon::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_CHAT_ICON]	= new mtServiceRoomChatIcon;
	return m_pkServiceList[E_SERVICE_ROOM_CHAT_ICON]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomChatContext()
{
	mtServiceRoomChatContext::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_CHAT_CONTEXT]	= new mtServiceRoomChatContext;
	return m_pkServiceList[E_SERVICE_ROOM_CHAT_CONTEXT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomHeartBeat()
{
	mtServiceRoomHeartBeat::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
///	kDataInit.pkQueueHall    = m_pkQueueMgr->getQueueHall();
	kDataInit.pkQueueUser    = m_pkQueueMgr->getQueueUser();
	kDataInit.pkQueuePacket  = m_pkQueueMgr->getQueuePacket();

	m_pkServiceList[E_SERVICE_ROOM_HEART_BEAT]	= new mtServiceRoomHeartBeat;
	return m_pkServiceList[E_SERVICE_ROOM_HEART_BEAT]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomIncrementGold()
{
	mtServiceRoomIncrementGold::DataInit		kDataInit; 
	kDataInit.lStructBytes	 = sizeof(kDataInit);
	kDataInit.pkQueueMgr     = m_pkQueueMgr;

	m_pkServiceList[E_SERVICE_ROOM_INCREMENT_GOLD]	= new mtServiceRoomIncrementGold;
	return m_pkServiceList[E_SERVICE_ROOM_INCREMENT_GOLD]->init(&kDataInit);
}

int mtServiceMgr::initServiceRoomExit()
{
	mtServiceRoomExit::DataInit   kDataInit; 
	kDataInit.lStructBytes	     = sizeof(kDataInit);
	kDataInit.pkQueueMgr         = m_pkQueueMgr;
	kDataInit.pkServiceMgr       = this;

	m_pkServiceList[E_SERVICE_ROOM_EXIT]	= new mtServiceRoomExit;
	return m_pkServiceList[E_SERVICE_ROOM_EXIT]->init(&kDataInit);
}

