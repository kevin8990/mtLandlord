#include "mtDefineDebug.h"
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
#define	MT_ARRAY_LENGTH(_array)	((sizeof(_array))/sizeof(_array[0]))


void  mtPrintTab(int iTab)
{
	MT_DEBUG_PTCL("\n");
	for (int iIdx=0;iIdx<iTab;++iIdx)
	{
		MT_DEBUG_PTCL("\t");
	}
}
void mtPrintString(char* pcFormat,char* pcSrc,int iByte,int iTab)
{
	int iLineCount=16;
	mtPrintTab(iTab);
	MT_DEBUG(PTCL,pcFormat);
	MT_DEBUG(PTCL,"\n");
	int iIdx=0;
	for(;iIdx<iByte;++iIdx)
	{
		MT_DEBUG(PTCL,"%.2x ",pcSrc[iIdx]);
		if(((iIdx+1)%iLineCount)==0)
		{
			for(int j=0;j<iLineCount;++j)
			{
				if (isprint((unsigned char)(pcSrc[iIdx-iLineCount+j+1]))) 
					MT_DEBUG_PTCL("%c",pcSrc[iIdx-iLineCount+j]);
				else
					printf(".");

			}
			MT_DEBUG_PTCL("\n");
		}
	}
	int imod=iIdx&iLineCount;
	for(int j=0;j<imod;++j)
	{
		if (isprint((unsigned char)(pcSrc[j]))) 
			MT_DEBUG_PTCL("%c",pcSrc[j]);
		else
			printf(".");

	}
	MT_DEBUG_PTCL("\n");
}
void mtLoopLongPrint(char* pcFormat,long* plSrc,int iByte,int iTab)
{
	for(int iIdx=0;iIdx<iByte;++iIdx)
	{
		mtPrintTab(iTab);
		MT_DEBUG(PTCL,pcFormat,iIdx,plSrc[iIdx]);
	}
}

int		mtPrint(mtListNode* pkmtListNode,int iTab)
{
	mtPrintTab(iTab);
	MT_DEBUG(PTCL,"mtListNode.mtListNode*:		%x",pkmtListNode->pPre		);
	mtPrintTab(iTab);
	MT_DEBUG(PTCL,"mtListNode.pSelf*:			%x",pkmtListNode->pSelf			);
	mtPrintTab(iTab);
	MT_DEBUG(PTCL,"mtListNode.pNext*:			%x",pkmtListNode->pNext				);
	return	0;
}

int		mtPrint(mtQueueDesk::DataNode* pkDataNode,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lStructBytes:		%d",pkDataNode->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.pkQueueUserDataNode:%x",pkDataNode->pkQueueUserDataNode			);
	mtPrintString("DataNode.pcCardsBase:",pkDataNode->pcCardsBase,MT_ARRAY_LENGTH(pkDataNode->pcCardsBase),iTab);
	mtPrintString("DataNode.pcCardsTotal[0]:",pkDataNode->pcCardsTotal[0],MT_ARRAY_LENGTH(pkDataNode->pcCardsTotal[0]),iTab);
	mtPrintString("DataNode.pcCardsTotal[1]:",pkDataNode->pcCardsTotal[1],MT_ARRAY_LENGTH(pkDataNode->pcCardsTotal[1]),iTab);
	mtPrintString("DataNode.pcCardsTotal[2]:",pkDataNode->pcCardsTotal[2],MT_ARRAY_LENGTH(pkDataNode->pcCardsTotal[2]),iTab);
	mtPrintString("DataNode.pcCardsCur:",pkDataNode->pcCardsCur,MT_ARRAY_LENGTH(pkDataNode->pcCardsCur),iTab);
	mtPrintString("DataNode.pcCardsOutAi:",pkDataNode->pcCardsOutAi,MT_ARRAY_LENGTH(pkDataNode->pcCardsOutAi),iTab);
	mtPrintString("DataNode.pcCardsOutTotal[0]:",pkDataNode->pcCardsOutTotal[0],MT_ARRAY_LENGTH(pkDataNode->pcCardsOutTotal[0]),iTab);
	mtPrintString("DataNode.pcCardsOutTotal[1]:",pkDataNode->pcCardsOutTotal[1],MT_ARRAY_LENGTH(pkDataNode->pcCardsOutTotal[1]),iTab);
	mtPrintString("DataNode.pcCardsOutTotal[2]:",pkDataNode->pcCardsOutTotal[2],MT_ARRAY_LENGTH(pkDataNode->pcCardsOutTotal[2]),iTab);
	mtLoopLongPrint("DataNode.plPlayerMode[%d]:		%d",pkDataNode->plPlayerMode	,MT_ARRAY_LENGTH(pkDataNode->plPlayerMode	),iTab);
	mtLoopLongPrint("DataNode.plPlayerType[%d]:		%d",pkDataNode->plPlayerType	,MT_ARRAY_LENGTH(pkDataNode->plPlayerType	),iTab);
	mtLoopLongPrint("DataNode.plIsWaitStart[%d]:	%d",pkDataNode->plIsWaitStart	,MT_ARRAY_LENGTH(pkDataNode->plIsWaitStart	),iTab);
	mtLoopLongPrint("DataNode.plScore[%d]:			%d",pkDataNode->plScore			,MT_ARRAY_LENGTH(pkDataNode->plScore		),iTab	);
	mtLoopLongPrint("DataNode.plDoubleScore[%d]:	%d",pkDataNode->plDoubleScore	,MT_ARRAY_LENGTH(pkDataNode->plDoubleScore	),iTab);
	mtLoopLongPrint("DataNode.plShowCard[%d]:		%d",pkDataNode->plShowCard		,MT_ARRAY_LENGTH(pkDataNode->plShowCard		),iTab);
	mtLoopLongPrint("DataNode.plRoundOver[%d]:		%d",pkDataNode->plRoundOver		,MT_ARRAY_LENGTH(pkDataNode->plRoundOver	),iTab);
	mtLoopLongPrint("DataNode.plBomb[%d]:			%d",pkDataNode->plBomb			,MT_ARRAY_LENGTH(pkDataNode->plBomb			),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lRocket:			%d",pkDataNode->lRocket		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lQueue:			%d",pkDataNode->lQueue                   );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lCardType:			%d",pkDataNode->lCardType                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lQueueUserDefaultLandlord:%d",pkDataNode->lQueueUserDefaultLandlord);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lQueueUserDeskIdx:	%d",pkDataNode->lQueueUserDeskIdx        );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserDeskIdxCardOutNext:%d",pkDataNode->lUserDeskIdxCardOutNext  );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lDeskExit:			%d",pkDataNode->lDeskExit                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.ullTimeEnterRun:	%lld",pkDataNode->ullTimeEnterRun          );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.iRetCirticalSection:%d",pkDataNode->iRetCirticalSection		);
	return	0;																	      
}		

int		mtPrint(mtQueueDesk::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",pkDataInit->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",pkDataInit->iSpinCount          );
	return 0;
}

int		mtPrint(mtQueueHall::DataRoom* pkDataRoom,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lStructBytes:		%d",pkDataRoom->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lOnlinePersonNumber:%d",pkDataRoom->lOnlinePersonNumber );
	return 0;
}

int		mtPrint(mtQueueHall::DataSpace* pkDataSpace,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSpace.lStructBytes:		%d",	pkDataSpace->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSpace.lRomTotal:		%d",	pkDataSpace->lRomTotal			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSpace.pkRoomList*:		%x",	pkDataSpace->pkRoomList			);
	return 0;
}

int		mtPrint(mtQueueHall::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",	pkDataInit->iSpinCount		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lSpaceTotal:		%x",	pkDataInit->lSpaceTotal		);
	mtLoopLongPrint("pkDataInit.plRoomTotal[%d]:	%d",pkDataInit->plRoomTotal	,MT_ARRAY_LENGTH(pkDataInit->plRoomTotal),iTab);
	return 0;
}

int		mtPrint(mtQueueMgr::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",	pkDataInit->iSpinCount		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lSpaceTotal:		%x",	pkDataInit->lSpaceTotal		);
	mtLoopLongPrint("pkDataInit.plRoomTotal[%d]:	%d",pkDataInit->plRoomTotal	,MT_ARRAY_LENGTH(pkDataInit->plRoomTotal),iTab);
	return 0;
}

int		mtPrint(mtQueueMgr::UserNodeReplaceAi* pkUserNodeReplaceAi,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.lStructBytes:		%d",	pkUserNodeReplaceAi->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.iReplaceAi: 		%d",	pkUserNodeReplaceAi->iReplaceAi 	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.iStatusExit:		%d",	pkUserNodeReplaceAi->iStatusExit	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.iStatusRun: 		%d",	pkUserNodeReplaceAi->iStatusRun 	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.iStatusRun: 		%d",	pkUserNodeReplaceAi->iStatusRun 	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserNodeReplaceAi.pkUserNode*: 		%x",	pkUserNodeReplaceAi->pkUserNode 	);
	
	return 0;
}

int		mtPrint(mtQueuePacket::DataNode* pkDataNode,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.eDataType:			%d",	pkDataNode->eDataType	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.ulTransmitBytes: 	%d",	pkDataNode->ulTransmitBytes 	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.ulFlag:			%d",	pkDataNode->ulFlag	);
	mtPrintString("DataNode.pcCardsBase:",pkDataNode->pcData,MT_ARRAY_LENGTH(pkDataNode->pcData),iTab);
	return 0;
}

int		mtPrint(mtQueuePacket::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.iSpinCount: 		%d",	pkDataInit->iSpinCount		);
	return 0;
}
int		mtPrint(mtQueueUser::DataNode* pkDataNode,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lStructBytes:		%d",pkDataNode->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lSpaceId:		 	%d",pkDataNode->lSpaceId		                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lRoomId:			%d",pkDataNode->lRoomId			                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserId:			%d",pkDataNode->lUserId				);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserIconId:      	%d",pkDataNode->lUserIconId                      );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserGold:        	%d",pkDataNode->lUserGold                        );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserSex:         	%d",pkDataNode->lUserSex         	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserLevel:       	%d",pkDataNode->lUserLevel                       );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserScore:		%d",pkDataNode->lUserScore		                 );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserAllChess:    	%d",pkDataNode->lUserAllChess    	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserWinChess:    	%d",pkDataNode->lUserWinChess                    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserWinRate:     	%d",pkDataNode->lUserWinRate                     );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserOffLineCount:	%d",pkDataNode->lUserOffLineCount	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lIsWaitStart:	 	%d",pkDataNode->lIsWaitStart	                 );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserDeskIndex:   	%d",pkDataNode->lUserDeskIndex                   );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserExit:        	%d",pkDataNode->lUserExit        	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.lUserIOCPStatus:  	%d",pkDataNode->lUserIOCPStatus                  );
	mtLoopLongPrint("DataNode.plPropsCount[%d]:  	%d",pkDataNode->plPropsCount,MT_ARRAY_LENGTH(pkDataNode->plPropsCount),iTab);
	mtPrintString("DataNode.pcUserAccountNum:",pkDataNode->pcUserAccountNum,MT_ARRAY_LENGTH(pkDataNode->pcUserAccountNum),iTab);
	mtPrintString("DataNode.pcUserName:",pkDataNode->pcUserName,MT_ARRAY_LENGTH(pkDataNode->pcUserName),iTab);
	mtPrintString("DataNode.pcUserNickName:",pkDataNode->pcUserNickName,MT_ARRAY_LENGTH(pkDataNode->pcUserNickName),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataNode.ullTimeEnterRun:  	%lld",pkDataNode->ullTimeEnterRun                  );
	
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataNode->pkListNode);++iIdx)
	{
		mtPrint(pkDataNode->pkListNode+iIdx,iTab+1);
	}
	return 0;
}

int		mtPrint(mtQueueUser::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",	pkDataInit->iSpinCount		);
	return 0;
}

int		mtPrint(mtService::DataUserInfo* pkDataUserInfo,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserId:           %d",pkDataUserInfo->lUserId				);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserIconId:       %d",pkDataUserInfo->lUserIconId                    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserSex:          %d",pkDataUserInfo->lUserSex                        );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserGold:         %d",pkDataUserInfo->lUserGold        	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserLevel:        %d",pkDataUserInfo->lUserLevel                       );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserScore:        %d",pkDataUserInfo->lUserScore			                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserAllChess:     %d",pkDataUserInfo->lUserAllChess    	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserWinChess:     %d",pkDataUserInfo->lUserWinChess                    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserWinRate:      %d",pkDataUserInfo->lUserWinRate                     );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lUserOffLineCount: %d",pkDataUserInfo->lUserOffLineCount	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lIsWaitStart:      %d",pkDataUserInfo->lIsWaitStart		                );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.lPlayerType:       %d",pkDataUserInfo->lPlayerType	                  );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.ulDeskId:          %d",pkDataUserInfo->ulDeskId         	);
	mtLoopLongPrint("DataUserInfo.plPropsCount[%d]:	%d",pkDataUserInfo->plPropsCount,MT_ARRAY_LENGTH(pkDataUserInfo->plPropsCount),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.pcUserAccountNum:  %s",pkDataUserInfo->pcUserAccountNum,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserAccountNum),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.pcUserName:        %s",pkDataUserInfo->pcUserName,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserName),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataUserInfo.pcUserNickName:    %s",pkDataUserInfo->pcUserNickName,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserNickName),iTab);
	//mtPrintString("DataUserInfo.pcUserAccountNum:",pkDataUserInfo->pcUserAccountNum,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserAccountNum),iTab);
	//mtPrintString("DataUserInfo.pcUserName:",pkDataUserInfo->pcUserName,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserName),iTab);
	//mtPrintString("DataUserInfo.pcUserNickName:",pkDataUserInfo->pcUserNickName,MT_ARRAY_LENGTH(pkDataUserInfo->pcUserNickName),iTab);
	
	return 0;
}

int		mtPrint(mtService::DataRun* pkDataRun,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataRun->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUserDataNode*:%X",	pkDataRun->pkQueueUserDataNode		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacketDataNode*:%X",	pkDataRun->pkQueuePacketDataNode		);
	return 0;
}

int		mtPrint(mtServiceCardDeal::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lResult:			%d",	pkDataRead->lResult		);
	return 0;
}

int		mtPrint(mtServiceCardDeal::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lDefaultLandlordUserId:%d",	pkDataWrite->lDefaultLandlordUserId		);
	mtPrintString("DataRead.pcCards:",pkDataWrite->pcCards,MT_ARRAY_LENGTH(pkDataWrite->pcCards),iTab);
	mtPrintString("DataRead.pcBaseCards:",pkDataWrite->pcBaseCards,MT_ARRAY_LENGTH(pkDataWrite->pcBaseCards),iTab);
	return 0;
}
	
int		mtPrint(mtServiceCardOut::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lCardType:			%d",	pkDataRead->lCardType		);
	mtPrintString("DataRead.pcCard:",pkDataRead->pcCard,MT_ARRAY_LENGTH(pkDataRead->pcCard),iTab);
	return 0;
}

int		mtPrint(mtServiceCardOut::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lCardType:		%d",	pkDataWrite->lCardType		);
	mtPrintString("DataRead.pcCard:",pkDataWrite->pcCard,MT_ARRAY_LENGTH(pkDataWrite->pcCard),iTab);
	return 0;
}

int		mtPrint(mtServiceConnect::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lSpaceId:          %d",	pkDataRead->lSpaceId			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRequestSameRoom:  %d",	pkDataRead->lRequestSameRoom	);
	mtPrint(&pkDataRead->kUserInfo,iTab+1);
	return 0;
} 	

int		mtPrint(mtServiceConnect::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:     %d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:     %d",	pkDataWrite->lServiceType		);
	//mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:          %d", pkDataWrite->lResult		);
	return 0;
}

int		mtPrint(mtServiceMgr::DataHead* pkDataHead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataHead.lStructBytes:		%d",	pkDataHead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataHead.lServiceId:		%d",	pkDataHead->lServiceId		);
	mtLoopLongPrint("DataHead.plReservation[%d]:	%d",pkDataHead->plReservation,MT_ARRAY_LENGTH(pkDataHead->plReservation),iTab);
	return 0;
} 	

int		mtPrint(mtServiceMgr::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",	pkDataInit->iSpinCount		);
	return 0;
}

int		mtPrint(mtServiceRoomChangeDesk::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomChangeDesk::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomChangeDesk::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	return 0;
}

int		mtPrint(mtServiceRoomChatContext::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUser*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	return 0;
}

int		mtPrint(mtServiceRoomChatContext::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	//mtPrintString("DataRead.pcContext:",pkDataRead->pcContext,MT_ARRAY_LENGTH(pkDataRead->pcContext),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcContext:         %s",pkDataRead->pcContext);
	return 0;
}

int		mtPrint(mtServiceRoomChatContext::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintString("DataWrite.pcContext:",pkDataWrite->pcContext,MT_ARRAY_LENGTH(pkDataWrite->pcContext),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomChatIcon::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUser*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	return 0;
}

int		mtPrint(mtServiceRoomChatIcon::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lIconIdx:			%d",	pkDataRead->lIconIdx	);
	return 0;
}

int		mtPrint(mtServiceRoomChatIcon::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lIconIdx:			%d",	pkDataWrite->lIconIdx		);
	
	return 0;
}

int		mtPrint(mtServiceRoomChatShortCut::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUser*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	return 0;
}

int		mtPrint(mtServiceRoomChatShortCut::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lShortCutIdx:		%d",	pkDataRead->lShortCutIdx	);
	return 0;
}

int		mtPrint(mtServiceRoomChatShortCut::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lShortCutIdx:		%d",	pkDataWrite->lShortCutIdx		);

	return 0;
}

int		mtPrint(mtServiceRoomDeskThree::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUser*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomDeskThree::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomDeskThree::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:			%d",	pkDataWrite->lResult		);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->kUserInfo);++iIdx)
	{
		mtPrint(pkDataWrite->kUserInfo+iIdx,iTab+1);
	}

	return 0;
}

int		mtPrint(mtServiceRoomDeskTwo::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueUser*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomDeskTwo::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomDeskTwo::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:			%d",	pkDataWrite->lResult		);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->kUserInfo);++iIdx)
	{
		mtPrint(pkDataWrite->kUserInfo+iIdx,iTab+1);
	}

	return 0;
}

int		mtPrint(mtServiceRoomDoubleScore::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomDoubleScore::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lDoubleFlag:		%d",	pkDataRead->lDoubleFlag		);
	return 0;
}

int		mtPrint(mtServiceRoomDoubleScore::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lDoubleFlag:		%d",	pkDataWrite->lDoubleFlag		);
	
	return 0;
}

int		mtPrint(mtServiceRoomExit::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomExit::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStatusExit:		%d",	pkDataRead->lStatusExit		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lCardType:			%d",	pkDataRead->lCardType		);
//	mtPrintString("DataRead.pcCard:",pkDataRead->pcCard,MT_ARRAY_LENGTH(pkDataRead->pcCard),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomExit::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStatusExit:		%d",	pkDataWrite->lStatusExit		);

	return 0;
}

int		mtPrint(mtServiceRoomGrabLandlord::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomGrabLandlord::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lScore:			%d",	pkDataRead->lScore		);

	return 0;
}

int		mtPrint(mtServiceRoomGrabLandlord::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:			%d",	pkDataWrite->lUserId		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lScore:			%d",	pkDataWrite->lScore		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserIdLandlord:	%d",	pkDataWrite->lUserIdLandlord		);

	return 0;
}

int		mtPrint(mtServiceRoomHeartBeat::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket		);
	return 0;
}

int		mtPrint(mtServiceRoomHeartBeat::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	//mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomHeartBeat::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
//	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:			%d",	pkDataWrite->lUserId		);
	return 0;
}

int		mtPrint(mtServiceRoomRequestCardsLeft::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomRequestCardsLeft::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomRequestCardsLeft::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintString("DataWrite.pcCardsLast:",pkDataWrite->pcCardsLast,MT_ARRAY_LENGTH(pkDataWrite->pcCardsLast),iTab);
	mtPrintString("DataWrite.pcCardsNext:",pkDataWrite->pcCardsNext,MT_ARRAY_LENGTH(pkDataWrite->pcCardsNext),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomRoundOver::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomRoundOver::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lGold:				%d",	pkDataRead->lGold	);
	return 0;
}

int		mtPrint(mtServiceRoomRoundOver::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrint(&pkDataWrite->kUserInfo,iTab+1);
	return 0;
}

int		mtPrint(mtServiceRoomWaitStartRecv::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkServiceMgr		);
	return 0;
}

int		mtPrint(mtServiceRoomWaitStartRecv::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;
}

int		mtPrint(mtServiceRoomWaitStartRecv::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId	);

	return 0;
}

int		mtPrint(mtServiceShowCard::DataInit* pkDataInit,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueUser		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkServiceMgr*:		%x",	pkDataInit->pkQueuePacket		);
	return 0;
}

int		mtPrint(mtServiceShowCard::DataRead* pkDataRead,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType		);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lShowCard:			%d",	pkDataRead->lShowCard	);
	mtPrintString("DataRead.pcCard:",pkDataRead->pcCard,MT_ARRAY_LENGTH(pkDataRead->pcCard),iTab);
	return 0;
}

int		mtPrint(mtServiceShowCard::DataWrite* pkDataWrite,int iTab)		
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType		);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lShowCard:		%d",	pkDataWrite->lShowCard	);
	mtPrintString("DataRead.pcCard:",pkDataWrite->pcCard,MT_ARRAY_LENGTH(pkDataWrite->pcCard),iTab);

	return 0;
}