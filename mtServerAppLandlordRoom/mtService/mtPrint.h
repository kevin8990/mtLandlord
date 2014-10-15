#pragma once
#include "mtDSList.h"
#include "mtQueueDesk.h"
#include "mtQueueHall.h"
#include "mtQueueMgr.h"
#include "mtQueuePacket.h"
#include "mtService.h"
#include "mtServiceMgr.h"
#include "mtServiceCardDeal.h"
#include "mtServiceCardOut.h"
#include "mtServiceConnect.h"
#include "mtServiceRoomChangeDesk.h"
#include "mtServiceRoomChatContext.h"
#include "mtServiceRoomChatIcon.h"
#include "mtServiceRoomChatShortCut.h"
#include "mtServiceRoomDeskThree.h"
#include "mtServiceRoomDeskTwo.h"
#include "mtServiceRoomDoubleScore.h"
#include "mtServiceRoomExit.h"
#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomHeartBeat.h"
#include "mtServiceRoomRequestCardsLeft.h"
#include "mtServiceRoomRoundOver.h"
#include "mtServiceRoomWaitStartRecv.h"
#include "mtServiceShowCard.h"

extern		int		mtPrint(mtListNode* pkmtListNode,int iTab = 0);
extern		int		mtPrint(mtQueueDesk::DataNode* pkDataNode,int iTab = 0);
extern		int		mtPrint(mtQueueDesk::DataInit* pkDataInit,int iTab = 0);
extern		int		mtPrint(mtQueueHall::DataRoom* pkDataRoom,int iTab = 0);	
extern		int		mtPrint(mtQueueHall::DataSpace* pkDataSpace,int iTab = 0);
extern		int		mtPrint(mtQueueHall::DataInit* pkDataInit,int iTab = 0);
extern		int		mtPrint(mtQueueMgr::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtQueueMgr::UserNodeReplaceAi* pkUserNodeReplaceAi,int iTab = 0);		
extern		int		mtPrint(mtQueuePacket::DataNode* pkDataNode,int iTab = 0);		
extern		int		mtPrint(mtQueuePacket::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtQueueUser::DataNode* pkDataNode,int iTab = 0);		
extern		int		mtPrint(mtQueueUser::DataInit* pkDataInit,int iTab = 0)	;	
extern		int		mtPrint(mtService::DataUserInfo* pkDataUserInfo,int iTab = 0);	
extern		int		mtPrint(mtService::DataRun* pkDataRun,int iTab = 0);		
extern		int		mtPrint(mtServiceCardDeal::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceCardDeal::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceCardOut::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceCardOut::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceConnect::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceConnect::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceMgr::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChangeDesk::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChangeDesk::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChangeDesk::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatContext::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatContext::DataRead* pkDataRead,int iTab = 0);
extern		int		mtPrint(mtServiceRoomChatContext::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatIcon::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatIcon::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatIcon::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatShortCut::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatShortCut::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomChatShortCut::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskThree::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskThree::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskThree::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskTwo::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskTwo::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDeskTwo::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDoubleScore::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDoubleScore::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomDoubleScore::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomExit::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomExit::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomExit::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomGrabLandlord::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomGrabLandlord::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomGrabLandlord::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomHeartBeat::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomHeartBeat::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomHeartBeat::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRequestCardsLeft::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRequestCardsLeft::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRequestCardsLeft::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRoundOver::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRoundOver::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomRoundOver::DataWrite* pkDataWrite,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomWaitStartRecv::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomWaitStartRecv::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceRoomWaitStartRecv::DataWrite* pkDataWrite,int iTab = 0);	
extern		int		mtPrint(mtServiceShowCard::DataInit* pkDataInit,int iTab = 0);		
extern		int		mtPrint(mtServiceShowCard::DataRead* pkDataRead,int iTab = 0);		
extern		int		mtPrint(mtServiceShowCard::DataWrite* pkDataWrite,int iTab = 0);		

