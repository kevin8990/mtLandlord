#include "mtServiceMatchGetRoomInfo.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceMatchGetRoomInfo::~mtServiceMatchGetRoomInfo()
{
}

mtServiceMatchGetRoomInfo::mtServiceMatchGetRoomInfo()
{

}

int mtServiceMatchGetRoomInfo::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		        = pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceMatchGetRoomInfo::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0 , sizeof(kDataWrite));
	memset(&kDataRead, 0 , sizeof(kDataRead));

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		mtPrint(&kDataRead);
		kDataWrite.lStructBytes            = sizeof(kDataWrite);
		kDataWrite.lServiceType            = kDataRead.lServiceType;

		if (MT_SERVER_WORK_USER_ID_MIN <= kDataRead.lUserId && MT_SERVER_WORK_USER_ID_MAX > kDataRead.lUserId)
		{
			// 根据用户id，获得用户节点在内存的地址
			mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
			if (!pkUserDataNode)
			{
				//kDataWrite.lResult = -1;
			}
			else
			{
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
				{
					//kDataWrite.lResult = -1;
					MT_DEBUG_ERROR(PTCL,"\nERROR:user(%d)is offline!Please login!",kDataRead.lUserId);
				}
				else
				{
					initDataWrite(kDataRead,kDataWrite);
				}
			}		
		}
		
		mtPrint(&kDataWrite);
		iRet = runWrite(iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3);
		if (iRet >= 1)
		{
			shutdown(iSocket, 1);
			closesocket(iSocket);
			*(SOCKET*)pData = INVALID_SOCKET;
		}
	}
	return 1;
}

int mtServiceMatchGetRoomInfo::exit()
{
	return 1;
}

int	mtServiceMatchGetRoomInfo::runRead(SOCKET socket, DataRead* pkDataRead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, sizeof(DataRead), MSG_PEEK);
	if (iReadBytesTotalHas < sizeof(pkDataRead->lStructBytes))
	{		
		return	0;
	}

	if (iReadBytesTotalHas >= pkDataRead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, pkDataRead->lStructBytes, 0);
	}

	return (iReadBytesTotalHas < pkDataRead->lStructBytes) ? 0 : 1;
}

int	mtServiceMatchGetRoomInfo::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
{
	int		iRet		= 0;
	int		iWriteTimes;

	for (iWriteTimes = 0;iWriteTimes < iTimes;iWriteTimes ++)
	{
		iRet	+= send(socket, pcBuffer + iRet, iBytes - iRet, flags);
		if (iRet >= iBytes)
		{
			return 1;
		}
	}

	return 0;
}

void* mtServiceMatchGetRoomInfo::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceMatchGetRoomInfo::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::PKRoomInfo          pPKRoomInfo[4];

	memset(pPKRoomInfo,0,sizeof(pPKRoomInfo));

	m_pkSQLEnv->getPKRoomList(pPKRoomInfo,4);

	for(int i = 0;i < 4;i++)
	{
		kDataWrite.pkRoom[i].lStructBytes    = sizeof(mtQueueHall::MatchRoom);
		kDataWrite.pkRoom[i].lRoomId	     = pPKRoomInfo[i].lRoomId;
		memcpy(kDataWrite.pkRoom[i].pcRoomName,pPKRoomInfo[i].pcRoomName,sizeof(pPKRoomInfo[i].pcRoomName));;
		kDataWrite.pkRoom[i].ulRoomlP        = inet_addr(pPKRoomInfo[i].pcRoomlP);
		kDataWrite.pkRoom[i].lRoomPort       = htons(pPKRoomInfo[i].lRoomPort);
		kDataWrite.pkRoom[i].lPersonNumber   = 100;	
		kDataWrite.pkRoom[i].lGoldBase    	 = pPKRoomInfo[i].lante;	
		kDataWrite.pkRoom[i].lGoldAdmittance = pPKRoomInfo[i].llowestchip;	
		kDataWrite.pkRoom[i].lGoldDeduct  	 = 100;
		memcpy(kDataWrite.pkRoom[i].pcRoomImg,pPKRoomInfo[i].pcRoomImg,sizeof(pPKRoomInfo[i].pcRoomImg));	
		getActiveTime(kDataWrite.pkRoom[i].lActiveTime, pPKRoomInfo[i].lStartTime, pPKRoomInfo[i].lEndTime, pPKRoomInfo[i].lActiveTime);
		kDataWrite.pkRoom[i].lFee            = pPKRoomInfo[i].lFee;
		kDataWrite.pkRoom[i].lFeeType        = pPKRoomInfo[i].lFeeType; 
		kDataWrite.pkRoom[i].lIntiChip       = pPKRoomInfo[i].lInitChip; 
		memcpy(kDataWrite.pkRoom[i].pcPrizeName,pPKRoomInfo[i].pcPrizeName,sizeof(pPKRoomInfo[i].pcPrizeName));
		memcpy(kDataWrite.pkRoom[i].pcAwardcontent,pPKRoomInfo[i].pcAwardcontent,sizeof(pPKRoomInfo[i].pcAwardcontent));
		kDataWrite.pkRoom[i].lstatus         = getMatchStatus(pPKRoomInfo[i].lStartTime,pPKRoomInfo[i].lEndTime,pPKRoomInfo[i].lActiveTime);
		kDataWrite.pkRoom[i].lcurstatus  	 = (kDataWrite.pkRoom[i].lstatus == 1 && bMatchOpen(pPKRoomInfo[i].lActiveTime)) ? mtQueueHall::OPEN : mtQueueHall::CLOSE;
	}
}

void   mtServiceMatchGetRoomInfo::getActiveTime(long *lActiveTime,tm &tmStart,tm &tmEndTime,long ldayTime)
{
	lActiveTime[0] = (tmStart.tm_year << 16) + (tmStart.tm_mon << 8) + tmStart.tm_mday;
	lActiveTime[1] = (tmEndTime.tm_year << 16) + (tmEndTime.tm_mon << 8) + tmEndTime.tm_mday;
	lActiveTime[2] = ldayTime;
}

long   mtServiceMatchGetRoomInfo::getMatchStatus(tm tmStart,tm tmEnd,long ldayTime)
{
	time_t tNow;
	tm     tmNowTime;

	time(&tNow);
	_localtime64_s(&tmNowTime, &tNow);

	tmStart.tm_hour   = 0;tmStart.tm_min    = 0;tmStart.tm_sec    = 0;
	tmEnd.tm_hour   = 0;tmEnd.tm_min    = 0;tmEnd.tm_sec    = 0;

	time_t tStart   = mktime(&tmStart) + getBeginHour(ldayTime) * 60 * 60;
	time_t tEnd     = mktime(&tmEnd) + getEndHour(ldayTime) * 60 * 60;

	if(tNow < tStart)
	{
		return mtQueueHall::OPEN_SOON;
	}
	else if(tNow > tEnd)
	{
		return mtQueueHall::HAS_ENDED;
	}
	else 
	{
		return mtQueueHall::IS_OPEN;
	}
}

bool   mtServiceMatchGetRoomInfo::bMatchOpen(long ldayTime)
{
	time_t tNow;
	tm     tmNowTime;


	time(&tNow);
	_localtime64_s(&tmNowTime, &tNow);

	if((1 << tmNowTime.tm_hour) & ldayTime)
	{
		return true;
	}

	return false;
}

long   mtServiceMatchGetRoomInfo::getBeginHour(long ldayTime)
{
	long lBeginHour = 0;

	while(lBeginHour < 24)
	{
		if((1 << lBeginHour) & ldayTime)
		{
			break;
		}
		lBeginHour++;
	}
	return lBeginHour;
}

long   mtServiceMatchGetRoomInfo::getEndHour(long ldayTime)
{
	long lEndHour = 23;

	while(lEndHour >= 0 )
	{
		if((1 << lEndHour) & ldayTime)
		{
			break;
		}
		lEndHour--;
	}
	return lEndHour + 1;
}