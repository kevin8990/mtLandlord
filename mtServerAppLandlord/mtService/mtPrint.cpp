#include "mtServiceConnect.h"
#include "mtServiceHallExit.h"
#include "mtServiceHallFeedBack.h"
#include "mtServiceHallGiveGold.h"
#include "mtServiceHallGoldBuy.h"
#include "mtServiceHallHall2Room.h"
#include "mtServiceHallPropBuy.h"
#include "mtServiceHallRank.h"
#include "mtServiceHallRegister.h"
#include "mtServiceHallRoom2Hall.h"
#include "mtServiceHallUpdatePlayInfo.h"
#include "mtServiceHallUpdateRoom.h"
#include "mtServiceHallUpdateSocialInfo.h"
#include "mtServiceGetLotteryArg.h"
#include "mtServiceLottery.h"
#include "mtServiceSystemMessage.h"
#include "mtServiceMgr.h"

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


#define	MT_ARRAY_LENGTH(_array)	((sizeof(_array))/sizeof(_array[0]))

static char * SeviceName[]={
"0:连接大厅","1:注册","2:购买道具","3:使用道具","4:购买金币","5:购买金币状态更新","6:获得排行榜",
"7:更新用户的账户信息","8:更新用户的基本资料信息","9:赠分","10:房间信息定时更新","11:用户请求从大厅进房间",
"12:用户请求从房间返回大厅","13:用户信息反馈","14:退出大厅","15:取抽奖参数","16:抽奖","17:系统消息","18:获取任务信息",
"19:获取任务奖励","20:获取宠物协议","21:喂养宠物","22:吐金币","23:兑奖"};

inline void  mtPrintTab(int iTab)
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
	//MT_DEBUG(PTCL,"[%s]",pcSrc);
	MT_DEBUG(PTCL,"[");
	for(int iTemp=0;iTemp<iByte&&pcSrc[iTemp]!='\0';++iTemp)
		MT_DEBUG(PTCL,"%c",pcSrc[iTemp]);
	MT_DEBUG(PTCL,"]");
	MT_DEBUG(PTCL,"\n");
	int iIdx=0;
	for(;iIdx<iByte;++iIdx)
	{
		MT_DEBUG(PTCL,"%.2x ",pcSrc[iIdx]);
		if(((iIdx+1)%iLineCount)==0)
		{
			MT_DEBUG_PTCL("[");
			for(int j=0;j<iLineCount;++j)
			{
				if (isprint((unsigned char)(pcSrc[iIdx-iLineCount+j+1]))) 
					MT_DEBUG_PTCL("%c",pcSrc[iIdx-iLineCount+j]);
				else
					printf(".");

			}
			MT_DEBUG_PTCL("]");
			MT_DEBUG_PTCL("\n");
		}
	}
	int imod=iIdx%iLineCount;
	if(imod!=0)
	{
		MT_DEBUG_PTCL("[");
		for(int j=0;j<imod;++j)
		{
			if (isprint((unsigned char)(pcSrc[j]))) 
				MT_DEBUG_PTCL("%c",pcSrc[j]);
			else
				printf(".");

		}
		MT_DEBUG_PTCL("]");
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

void mtLoopStringPrint(char* pcFormat,char* plSrc,int iByte,int iTab)
{
	for(int iIdx=0;iIdx<iByte;++iIdx)
	{
		mtPrintTab(iTab);
		MT_DEBUG(PTCL,pcFormat,iIdx,plSrc[iIdx]);
	}
}

std::string convertIPToString(unsigned long ip)
{
	char buffer[17];

	int res = sprintf(buffer, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	if (res < 0) {
		return std::string();
	}

	return buffer;
}

std::string convertTimeToString(long lTime)
{
	char buffer[11];

	int res = sprintf(buffer, "%d-%02d-%02d", ((lTime >> 16) & 0xFFFF) + 1900, ((lTime >> 8) & 0xFF) + 1, lTime & 0xFF);
	if (res < 0)
	{
		return std::string();
	}

	return buffer;
}

std::string converTimePeriodToString(long TimePeriod)
{
	std::string strRet;
	int offset = 0;
	char bufer[100];
	bool bBegin = true;
	char Placeholder[50];
	sprintf(Placeholder,"\t                          ");

	while(offset < 24)
	{
		if((1 << offset) & TimePeriod)
		{
			if(bBegin)
			{
				sprintf(bufer,"\n%s%02d:00 -- ",Placeholder,offset);
				strRet += bufer;
				bBegin =false;
			}

			if(!((1 << (offset + 1)) & TimePeriod))
			{
				sprintf(bufer,"%02d:00",(offset + 1) % 24);
				strRet += bufer;
				bBegin =true;
			}	
		}
		offset++;
	}
	return strRet;
}

std::string converActiveTimeToString(long *lActiveTime)
{
	std::string sRet;
	char buffer[100];

	int res = sprintf(buffer, "%s -- %s", convertTimeToString(lActiveTime[0]).c_str(),convertTimeToString(lActiveTime[1]).c_str());

	sRet += buffer;
	sRet += converTimePeriodToString(lActiveTime[2]);

	return sRet;
}

int	mtPrint(mtQueueHall::DataRoom* pkDataRoom,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lStructBytes:    %d",pkDataRoom->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lSpaceId:        %d",pkDataRoom->lSpaceId			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lRoomId:         %d",pkDataRoom->lRoomId			);
	
//	mtPrintString("DataRoom.pcRoomName:",pkDataRoom->pcRoomName,MT_ARRAY_LENGTH(pkDataRoom->pcRoomName),iTab);

	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.ulRoomlP:        %s",convertIPToString(pkDataRoom->ulRoomlP).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lRoomPort:       %d",ntohs(pkDataRoom->lRoomPort)	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lPersonNumber:   %d",pkDataRoom->lPersonNumber		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldBase:       %d",pkDataRoom->lGoldBase    		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldAdmittance  %d",pkDataRoom->lGoldAdmittance	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldDeduct:     %d",pkDataRoom->lGoldDeduct  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"-------------------------------------------");
	return	0;
}

int mtPrint(mtQueueHall::MatchRoom* pkDataRoom,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"-------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lStructBytes:    %d",pkDataRoom->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lRoomId:         %d",pkDataRoom->lRoomId			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.pcRoomName:      %s",pkDataRoom->pcRoomName			);

	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.ulRoomlP:        %s",convertIPToString(pkDataRoom->ulRoomlP).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lRoomPort:       %d",ntohs(pkDataRoom->lRoomPort)	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lPersonNumber:   %d",pkDataRoom->lPersonNumber		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldBase:       %d",pkDataRoom->lGoldBase    		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldAdmittance  %d",pkDataRoom->lGoldAdmittance	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lGoldDeduct:     %d",pkDataRoom->lGoldDeduct  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.pcRoomImg:       %s",pkDataRoom->pcRoomImg  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lActiveTime:     %s",converActiveTimeToString(pkDataRoom->lActiveTime).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lFee:            %d",pkDataRoom->lFee  		    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lFeeType:        %d",pkDataRoom->lFeeType  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lIntiChip:       %d",pkDataRoom->lIntiChip  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.pcPrizeName:     %s",pkDataRoom->pcPrizeName  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.pcAwardcontent:  %s",pkDataRoom->pcAwardcontent  	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lcurstatus:      %d",pkDataRoom->lcurstatus  		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRoom.lstatus:         %d",pkDataRoom->lstatus  		    );

	
	return	0;
}

int mtPrint(mtQueueHall::DataSpace* pkDataSpace,int iTab)
{
	//SetConsoleColor(FOREGROUND_RED|FOREGROUND_GREEN);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSpace.lStructBytes:      %d",pkDataSpace->lStructBytes		);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataSpace.lSpaceId:          %d",pkDataSpace->lSpaceId			);
	//mtPrintString("DataSpace.pcSpaceName:",pkDataSpace->pcSpaceName,MT_ARRAY_LENGTH(pkDataSpace->pcSpaceName),iTab		);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataSpace->pkRoom);++iIdx)
		mtPrint(&pkDataSpace->pkRoom[iIdx],iTab+1);
	//SetConsoleColor(FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED);
	return 0;
}

int mtPrint(mtQueueHall::UserInfo* pkUserInfo,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserId:                %d",   pkUserInfo->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserIconId:            %d",   pkUserInfo->lUserIconId	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserAccountNum:       %s",   pkUserInfo->pcUserAccountNum);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserNickName:         %s",   pkUserInfo->pcUserNickName);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserPwd:              %s",   pkUserInfo->pcUserPwd);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserLove:             %s",   pkUserInfo->pcUserLove);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserRemark:           %s",   pkUserInfo->pcUserRemark);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserPhone:            %s",   pkUserInfo->pcUserPhone);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.pcUserQQ:               %s",   pkUserInfo->pcUserQQ);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserSex:               %d",	pkUserInfo->lUserSex);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserAge:               %d",	pkUserInfo->lUserAge);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserGold:              %d",	pkUserInfo->lUserGold);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserScore:             %d",	pkUserInfo->lUserScore);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserLevel:             %d",	pkUserInfo->lUserLevel);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserPayCount:          %d",	pkUserInfo->lUserPayCount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserFirstCharge:       %d",	pkUserInfo->lUserFirstCharge);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserDayChess:          %d",	pkUserInfo->lUserDayChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserAllChess:          %d",	pkUserInfo->lUserAllChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserWinChess:          %d",	pkUserInfo->lUserWinChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserWinRate:           %d",	pkUserInfo->lUserWinRate);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserOffLineCount:      %d",	pkUserInfo->lUserOffLineCount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserRegisterDate:      %s",	/*pkUserInfo->lUserRegisterDate*/convertTimeToString(pkUserInfo->lUserRegisterDate).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserLoginHallDate:     %s",	/*pkUserInfo->lUserLoginHallDate*/convertTimeToString(pkUserInfo->lUserLoginHallDate).c_str());
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserContinueLoginDays: %d",   pkUserInfo->lUserContinueLoginDays);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserLoginFirstToday:   %d",   pkUserInfo->lUserLoginFirstToday);

	mtLoopLongPrint("UserInfo.plPropsCount[%2d]:      %2d",pkUserInfo->plPropsCount,MT_ARRAY_LENGTH(pkUserInfo->plPropsCount),iTab);
	//mtLoopLongPrint("UserInfo.plUserChessPalyed[%d]:%d",pkUserInfo->plUserChessPalyed,MT_ARRAY_LENGTH(pkUserInfo->plUserChessPalyed),iTab);

	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lUserGiveGoldFrequence: %d",pkUserInfo->lUserGiveGoldFrequence);
	
	return 0;
}

int mtPrint(mtQueueHall::UserDataNode* pkUserDataNode,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lStructBytes:		%d",	pkUserDataNode->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lIsOnLine:			%d",	pkUserDataNode->lIsOnLine	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lSpaceId:			%d",	pkUserDataNode->lSpaceId	);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"UserInfo.lRoomId:			%d",	pkUserDataNode->lRoomId		);
	mtPrint(&pkUserDataNode->kUserInfo,iTab+1);
	return 0;
}

int mtPrint(mtQueueHall::DataRank* pkDataRank,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"----------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRank.lUserId:           %d",	pkDataRank->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRank.lUserLevel:        %d",	pkDataRank->lUserLevel);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRank.lUserScore:        %d",	pkDataRank->lUserScore);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRank.pcUserNickName:    %s",	pkDataRank->pcUserNickName);
	//mtPrintString("DataRank.pcUserNickName:",pkDataRank->pcUserNickName,MT_ARRAY_LENGTH(pkDataRank->pcUserNickName),iTab);
	return 0;
}

int mtPrint(mtQueueHall::DataHall* pkDataHall,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataHall.lStructBytes:		%d",	pkDataHall->lStructBytes);
	mtPrint(pkDataHall->pkSpace,iTab+1);
	mtPrint(pkDataHall->pkDataRank,iTab+1);
	mtPrint(pkDataHall->pkUserDataNodeList,iTab+1);
	return 0;
}

int mtPrint(mtQueueHall::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.iSpinCount:		%d",	pkDataInit->iSpinCount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	return 0;
}

int mtPrint(mtQueueHall* pkmtQueueHall,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"mtQueueHall.m_iSpinCount:	%d",	pkmtQueueHall->m_iSpinCount);
	mtPrint(&pkmtQueueHall->m_kDataHall,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"mtQueueHall.m_pcServiceExeDir:%x",	pkmtQueueHall->m_pcServiceExeDir);
	///	mtPrintTab(iTab);MT_DEBUG(PTCL,"mtQueueHall.m_lMaxUserId:	%d",	pkmtQueueHall->m_lMaxUserId);
	return 0;
}

int mtPrint(mtServiceConnect::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%s",	SeviceName[pkDataRead->lServiceType]);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtLoopLongPrint("DataRead.plVersion[%d]:		%d",pkDataRead->plVersion,MT_ARRAY_LENGTH(pkDataRead->plVersion),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:               %d",pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRegister:             %d",pkDataRead->lRegister);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserRegisterDate:     %s",convertTimeToString(pkDataRead->lUserRegisterDate).c_str());
	return 0;
}

int mtPrint(mtServiceConnect::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrint(&pkDataWrite->kUserInfo,iTab+1);

	//for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->pkSpace);++iIdx)
	//{
	//	mtPrint(&pkDataWrite->pkSpace[iIdx],iTab+1);
	//}
	return 0;
}

int mtPrint(mtServiceConnect::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir:	%x",	pkDataInit->pcServiceExeDir);
	mtPrint(pkDataInit->pkQueueHall,iTab+1);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallExit::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrint(&pkDataRead->kUserInfo,iTab+1);
	return 0;

}

int mtPrint(mtServiceHallExit::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:              %d",	pkDataWrite->lUserId);
	return 0;
}

int mtPrint(mtServiceHallExit::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkUserDataNodeList*:%x",	pkDataInit->pkUserDataNodeList);
	return 0;
}

int mtPrint(mtServiceHallFeedBack::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:			%d",	pkDataRead->lUserId);
	mtPrintString("DataRead.pcContentFeedBack:",pkDataRead->pcContentFeedBack,MT_ARRAY_LENGTH(pkDataRead->pcContentFeedBack),iTab);
	return 0;

}

int mtPrint(mtServiceHallFeedBack::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:			%d",	pkDataWrite->lResult);
	return 0;
}

int mtPrint(mtServiceHallFeedBack::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallGiveGold::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:			%d",	pkDataRead->lUserId);
	return 0;

}

int mtPrint(mtServiceHallGiveGold::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lGiveGoldFrequence:%d",	pkDataWrite->lGiveGoldFrequence);
	return 0;
}

int mtPrint(mtServiceHallGiveGold::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallGoldBuy::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lGold:             %d",	pkDataRead->lGold);
	return 0;

}

int mtPrint(mtServiceHallGoldBuy::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	return 0;
}

int mtPrint(mtServiceHallGoldBuy::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallHall2Room::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lSpaceId:          %d",	pkDataRead->lSpaceId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	
	return 0;

}

int mtPrint(mtServiceHallHall2Room::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lSpaceId:             %d",	pkDataWrite->lSpaceId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lRoomId:              %d",	pkDataWrite->lRoomId);
	
	return 0;
}

int mtPrint(mtServiceHallHall2Room::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	return 0;
}

int mtPrint(mtServiceHallPropBuy::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lPropIdx:          %d",	pkDataRead->lPropIdx);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lPropPrice:        %d",pkDataRead->lPropPrice);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lPropBuyCount:     %d",pkDataRead->lPropBuyCount);
	return 0;

}

int mtPrint(mtServiceHallPropBuy::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);

	return 0;
}

int mtPrint(mtServiceHallPropBuy::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallRank::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;

}

int mtPrint(mtServiceHallRank::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->kRank);++iIdx)
	{
		mtPrint(&pkDataWrite->kRank[iIdx],iTab+1);
	}
	return 0;
}

int mtPrint(mtServiceHallRank::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallRegister::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:			%d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserSex:			%d",	pkDataRead->lUserSex);
	mtPrintString("DataRead.pcUserAccountNum:",pkDataRead->pcUserAccountNum,MT_ARRAY_LENGTH(pkDataRead->pcUserAccountNum),iTab);
	mtPrintString("DataRead.pcUserPwd:",pkDataRead->pcUserPwd		,MT_ARRAY_LENGTH(pkDataRead->pcUserPwd		),iTab);
	mtPrintString("DataRead.pcUserPhoneNum:",pkDataRead->pcUserPhoneNum	,MT_ARRAY_LENGTH(pkDataRead->pcUserPhoneNum	),iTab);
	mtPrintString("DataRead.pcUserQQ:",pkDataRead->pcUserQQ		,MT_ARRAY_LENGTH(pkDataRead->pcUserQQ		),iTab);

	return 0;

}

int mtPrint(mtServiceHallRegister::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:		%d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserId:			%d",	pkDataWrite->lUserId);

	return 0;
}

int mtPrint(mtServiceHallRegister::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallRoom2Hall::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lSpaceId:          %d",	pkDataRead->lSpaceId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStatusExit:       %d",	pkDataRead->lStatusExit);

	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserGold:         %d",	pkDataRead->lUserGold);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserScore:        %d",	pkDataRead->lUserScore);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserLevel:        %d",	pkDataRead->lUserLevel);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserDayChess:     %d",	pkDataRead->lUserDayChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserAllChess:     %d",	pkDataRead->lUserAllChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserWinChess:     %d",	pkDataRead->lUserWinChess);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserWinRate:      %d",	pkDataRead->lUserWinRate);

	return 0;

}

int mtPrint(mtServiceHallRoom2Hall::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);

	return 0;
}

int mtPrint(mtServiceHallRoom2Hall::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	return 0;
}

int mtPrint(mtServiceHallUpdatePlayInfo::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserGold:         %d",	pkDataRead->lUserGold);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserScore:        %d",	pkDataRead->lUserScore			);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserLevel:        %d",	pkDataRead->lUserLevel       );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserAllChess:     %d",	pkDataRead->lUserAllChess    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserWinChess:     %d",	pkDataRead->lUserWinChess    );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserWinRate:      %d",	pkDataRead->lUserWinRate     );
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserOffLineCount: %d",	pkDataRead->lUserOffLineCount);
	mtLoopLongPrint("DataRead.plPropsCount[%2d]:      %d",pkDataRead->plPropsCount,MT_ARRAY_LENGTH(pkDataRead->plPropsCount),iTab);
	//mtLoopLongPrint("DataRead.plUserChessPlayedToday[%d]:%d",pkDataRead->plUserChessPlayedToday,MT_ARRAY_LENGTH(pkDataRead->plUserChessPlayedToday),iTab);
	//mtPrintString("DataRead.pcUserNickName:",pkDataRead->pcUserNickName		,MT_ARRAY_LENGTH(pkDataRead->pcUserNickName		),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserNickName:    %s",	pkDataRead->pcUserNickName);
	return 0;

}

int mtPrint(mtServiceHallUpdatePlayInfo::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:  %d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);

	return 0;
}

int mtPrint(mtServiceHallUpdatePlayInfo::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceHallUpdateRoom::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lBatchId:          %d",	pkDataRead->lBatchId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lVendorId:         %d",	pkDataRead->lVendorId);

	return 0;

}

int mtPrint(mtServiceHallUpdateRoom::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab + 1);MT_DEBUG(PTCL,"***************************************************");
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->pkSpace);++iIdx)
		mtPrint(&pkDataWrite->pkSpace[iIdx],iTab+1);
	return 0;
}

int mtPrint(mtServiceHallUpdateRoom::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:			%x",	pkDataInit->pkQueueHall);
	return 0;
}

int mtPrint(mtServiceHallUpdateSocialInfo::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserIconId:       %d",	pkDataRead->lUserIconId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserSex:          %d",	pkDataRead->lUserSex);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserAge:          %d",	pkDataRead->lUserAge);

	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserNickName:    %s",	pkDataRead->pcUserNickName);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserPwd:         %s",	pkDataRead->pcUserPwd);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserLove:        %s",	pkDataRead->pcUserLove);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserRemark:      %s",	pkDataRead->pcUserRemark);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserPhone:       %s",	pkDataRead->pcUserPhone);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcUserQQ:          %s",	pkDataRead->pcUserQQ);
	//mtPrintString("DataRead.pcUserNickName:",pkDataRead->pcUserNickName	,MT_ARRAY_LENGTH(pkDataRead->pcUserNickName	),iTab);
	//mtPrintString("DataRead.pcUserPwd:",pkDataRead->pcUserPwd		,MT_ARRAY_LENGTH(pkDataRead->pcUserPwd		),iTab);
	//mtPrintString("DataRead.pcUserLove:",pkDataRead->pcUserLove		,MT_ARRAY_LENGTH(pkDataRead->pcUserLove		),iTab);
	//mtPrintString("DataRead.pcUserRemark:",pkDataRead->pcUserRemark	,MT_ARRAY_LENGTH(pkDataRead->pcUserRemark	),iTab);
	//mtPrintString("DataRead.pcUserPhone:",pkDataRead->pcUserPhone		,MT_ARRAY_LENGTH(pkDataRead->pcUserPhone	),iTab);
	//mtPrintString("DataRead.pcUserQQ:",pkDataRead->pcUserQQ		,MT_ARRAY_LENGTH(pkDataRead->pcUserQQ		),iTab);
	return 0;

}

int mtPrint(mtServiceHallUpdateSocialInfo::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);

	return 0;
}

int mtPrint(mtServiceHallUpdateSocialInfo::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceGetLotteryArg::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
    
	return 0;

}

int mtPrint(mtServiceGetLotteryArg::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lNeedGold:            %d",	pkDataWrite->lNeedGold);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUseTimes:            %d",	pkDataWrite->lUseTimes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lMaxTimes:            %d",	pkDataWrite->lMaxTimes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lLuck:                %d",	pkDataWrite->lLuck);
   
	return 0;
}

int mtPrint(mtServiceGetLotteryArg::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceLottery::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lGold:             %d",	pkDataRead->lGold);

	return 0;

}

int mtPrint(mtServiceLottery::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lGold:                %d",	pkDataWrite->lGold);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lPrize:               %d",	pkDataWrite->lPrize);


	return 0;
}

int mtPrint(mtServiceLottery::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceSystemMessage::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;

}

int mtPrint(mtServiceSystemMessage::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.llength:              %d",	pkDataWrite->llength);
	//mtLoopStringPrint("DataWrite.pkMessageInfo[%d]:		%s",	pkDataWrite->pkMessageInfo.pkMessage->content,MT_ARRAY_LENGTH(pkDataWrite->pkMessageInfo.pkMessage),iTab);


	return 0;
}

int mtPrint(mtServiceSystemMessage::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceGetTaskInfo::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;
}

int mtPrint(mtServiceGetTaskInfo::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);

	return 0;
}

int mtPrint(mtServiceGetTaskInfo::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceGetTaskAward::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.ltaskId:           %d",	pkDataRead->ltaskId);

	return 0;
}

int mtPrint(mtServiceGetTaskAward::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.ltaskId:              %d",	pkDataWrite->ltaskId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lGold:                %d",	pkDataWrite->lGold);

	return 0;
}

int mtPrint(mtServiceGetTaskAward::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceGetPetInfo::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;
}

int mtPrint(mtServiceGetPetInfo::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.llevel:               %d",	pkDataWrite->llevel);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lexperience:          %d",	pkDataWrite->lexperience);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lnextexperience:      %d",	pkDataWrite->lnextexperience);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lmash:                %d",	pkDataWrite->lmash);

	return 0;
}

int mtPrint(mtServiceGetPetInfo::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceFeedPet::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;
}

int mtPrint(mtServiceFeedPet::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.llevel:               %d",	pkDataWrite->llevel);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lexperience:          %d",	pkDataWrite->lexperience);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lnextexperience:      %d",	pkDataWrite->lnextexperience);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lmash:                %d",	pkDataWrite->lmash);

	return 0;
}

int mtPrint(mtServiceFeedPet::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceSpitGold::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;
}

int mtPrint(mtServiceSpitGold::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lgold:                %d",	pkDataWrite->lgold);

	return 0;
}

int mtPrint(mtServiceSpitGold::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceCashLottery::DataRead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lPrizeId:          %d",	pkDataRead->lPrizeId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.pcPhone:           %s",	pkDataRead->pcPhone);

	return 0;
}

int mtPrint(mtServiceCashLottery::DataWrite* pkDataWrite,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lPrizeId:             %d",	pkDataWrite->lPrizeId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lFeeCard:             %d",	pkDataWrite->lFeeCard);

	return 0;
}

int mtPrint(mtServiceCashLottery::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchGetRoomInfo::DataRead* pkDataRead,int iTab = 0)  ///24拉取比赛房间信息协议
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);

	return 0;
}

int mtPrint(mtServiceMatchGetRoomInfo::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->pkRoom);++iIdx)
		mtPrint(&pkDataWrite->pkRoom[iIdx],iTab+1);


	return 0;
}

int mtPrint(mtServiceMatchGetRoomInfo::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchGetUserInfo::DataRead* pkDataRead,int iTab = 0)  /// 25拉取用户比赛场数据协议
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);

	return 0;
}

int mtPrint(mtServiceMatchGetUserInfo::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lflag:                %d",	pkDataWrite->lflag);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.chip:                 %d",	pkDataWrite->chip);

	return 0;
}

int mtPrint(mtServiceMatchGetUserInfo::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchSignUp::DataRead* pkDataRead,int iTab = 0)  /// 26比赛报名协议.
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lFee:              %d",	pkDataRead->lFee);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lType:             %d",	pkDataRead->lType);

	return 0;
}

int mtPrint(mtServiceMatchSignUp::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.chip:                 %d",	pkDataWrite->chip);


	return 0;
}

int mtPrint(mtServiceMatchSignUp::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchGetRoomStatus::DataRead* pkDataRead,int iTab = 0)  /// 27拉取房间状态协议
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);

	return 0;
}

int mtPrint(mtServiceMatchGetRoomStatus::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStatus:              %d",	pkDataWrite->lStatus);


	return 0;
}

int mtPrint(mtServiceMatchGetRoomStatus::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchGetRank::DataRead* pkDataRead,int iTab = 0)  /// 28比赛排名拉取协议
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);

	return 0;
}

int mtPrint(mtServiceMatchGetRank::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserRank:            %d",	pkDataWrite->lUserRank);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lUserScore:           %d",	pkDataWrite->lUserScore);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStatus:              %d",	pkDataWrite->lStatus);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.pkRank:               ");
for(int iIdx=0;iIdx<MT_ARRAY_LENGTH(pkDataWrite->pkRank);++iIdx)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"             UserName:          %s",	pkDataWrite->pkRank[iIdx].pcUserName);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"             Score:             %d",	pkDataWrite->pkRank[iIdx].lScore);
}


	return 0;
}

int mtPrint(mtServiceMatchGetRank::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMatchAward::DataRead* pkDataRead,int iTab = 0)  /// 29比赛领奖协议
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:      %d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:      %d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:  %d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lUserId:           %d",	pkDataRead->lUserId);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lRoomId:           %d",	pkDataRead->lRoomId);

	return 0;
}

int mtPrint(mtServiceMatchAward::DataWrite* pkDataWrite,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG_COLOR(PTCL,FOREGROUND_RED|FOREGROUND_GREEN,"--------------------------------------------");
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:         %d",	pkDataWrite->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lServiceType:         %d",	pkDataWrite->lServiceType);
	mtLoopLongPrint("DataWrite.plReservation[%d]:	%d",pkDataWrite->plReservation,MT_ARRAY_LENGTH(pkDataWrite->plReservation),iTab);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lResult:              %d",	pkDataWrite->lResult);

	return 0;
}

int mtPrint(mtServiceMatchAward::DataInit* pkDataInit,int iTab = 0)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueHall*:		%x",	pkDataInit->pkQueueHall);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueuePacket*:	%x",	pkDataInit->pkQueuePacket);
	return 0;
}

int mtPrint(mtServiceMgr::DataHead* pkDataRead,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lStructBytes:		%d",	pkDataRead->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataRead.lServiceType:		%d",	pkDataRead->lServiceType);
	mtLoopLongPrint("DataRead.plReservation[%d]:	%d",pkDataRead->plReservation,MT_ARRAY_LENGTH(pkDataRead->plReservation),iTab);
	return 0;

}

int mtPrint(mtServiceMgr::DataInit* pkDataInit,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lStructBytes:		%d",	pkDataInit->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.lSpinCount:		%d",	pkDataInit->lSpinCount);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pkQueueMgr*:		%x",	pkDataInit->pkQueueMgr);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pcServiceExeDir*:	%x",	pkDataInit->pcServiceExeDir);
	return 0;
}

int mtPrint(mtServiceMgr::DataRun* pkDataRun,int iTab)
{
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataWrite.lStructBytes:		%d",	pkDataRun->lStructBytes);
	mtPrintTab(iTab);MT_DEBUG(PTCL,"DataInit.pQueueWorkNode*:	%x",	pkDataRun->pQueueWorkNode);
	return 0;
}