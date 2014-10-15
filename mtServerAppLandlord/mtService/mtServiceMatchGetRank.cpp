#include "mtServiceMatchGetRank.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceMatchGetRank::~mtServiceMatchGetRank()
{
}

mtServiceMatchGetRank::mtServiceMatchGetRank()
{

}

int mtServiceMatchGetRank::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}


int mtServiceMatchGetRank::run( void* pData )
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

int mtServiceMatchGetRank::exit()
{
	return 1;
}

int	mtServiceMatchGetRank::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceMatchGetRank::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceMatchGetRank::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceMatchGetRank::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::DataRank   kDataRank;
	mtQueueHall::DataRank   pkDataRank[10];
	mtQueueHall::PKRoomInfo kPKRoomInfo;
	mtQueueHall::UserPKInfo  kUserPKInfo;
	long sizeRank = 0;

	memset(&kDataRank,0,sizeof(kDataRank));
	memset(pkDataRank,0,sizeof(pkDataRank));
	memset(&kPKRoomInfo,0,sizeof(kPKRoomInfo));
	memset(&kUserPKInfo,0,sizeof(kUserPKInfo));


    m_pkSQLEnv->getPKUserRankInfo(kDataRead.lUserId + MT_SERVER_WORK_USER_ID_BASE,kDataRead.lRoomId + 1,&kDataRank);
	m_pkSQLEnv->getPKRoomRankList(kDataRead.lRoomId + 1,pkDataRank,&sizeRank);
	m_pkSQLEnv->getPKRoom(kDataRead.lRoomId + 1,&kPKRoomInfo);
	int iExit = m_pkSQLEnv->getUserPKInfo(kDataRead.lUserId + MT_SERVER_WORK_USER_ID_BASE,kDataRead.lRoomId + 1,&kUserPKInfo);

	if(iExit == 1)
	{
		kDataWrite.lUserRank   = kDataRank.lUserLevel;
		kDataWrite.lUserScore  = kDataRank.lUserScore;
	}
	else
	{
		kDataWrite.lUserRank   = 0;
		kDataWrite.lUserScore  = 0;
	}

	for(int i = 0;i < sizeRank;i++)
	{
		kDataWrite.pkRank[i].lScore  = pkDataRank[i].lUserScore;
		memcpy(kDataWrite.pkRank[i].pcUserName,pkDataRank[i].pcUserNickName,sizeof(pkDataRank[i].pcUserNickName));
	}

	kDataWrite.lStatus    = getUserMatchStatus(kDataRead.lUserId,kDataRead.lRoomId + 1,kDataRank.lUserLevel,kPKRoomInfo.lAwardNum,kPKRoomInfo.lEndTime,kPKRoomInfo.lActiveTime);

	if(iExit)
	{
		kUserPKInfo.lStatus  = kDataWrite.lStatus;
		m_pkSQLEnv->updateUserPKInfo(&kUserPKInfo);
	}
}

long  mtServiceMatchGetRank:: getUserMatchStatus(long lUserid,long lRoomid,long lUserRank,long lawardNum,tm tmEnd,long ldayTime)
{
	mtQueueHall::UserPKInfo  kUserPKInfo;

	memset(&kUserPKInfo,0,sizeof(kUserPKInfo));

	int iRet = m_pkSQLEnv->getUserPKInfo(lUserid + MT_SERVER_WORK_USER_ID_BASE,lRoomid,&kUserPKInfo);

	bool bmatchEnd = bMatchEnd(tmEnd,ldayTime);


		if(bmatchEnd)
		{
			if(iRet == 1)
			{
				if(lUserRank > lawardNum)
				{
					return mtQueueHall::NO_WINNERS;
				}
				else
				{
					if(kUserPKInfo.lStatus == mtQueueHall::HAS_AWARD)
					{
						return mtQueueHall::HAS_AWARD;
					}
					else
					{						
						return mtQueueHall::NO_AWARD;
					}
				}
			}
			else
			{
				return mtQueueHall::NO_WINNERS;
			}

		}
		else
		{
            return  mtQueueHall::NOT_END;
		}
}

bool   mtServiceMatchGetRank::bMatchEnd(tm tmEnd,long ldayTime)
{
	time_t tNow;

	time(&tNow);

	time_t tEnd     = getEndTime(tmEnd,ldayTime);
   
	return  tNow > tEnd;
}

time_t mtServiceMatchGetRank::getEndTime(tm tmEnd,long ldayTime)
{
	int endhour = 23;
	while(endhour >= 0)
	{
		if((1 << endhour) & ldayTime)
		{
			break;
		}
		endhour--;
	}
	//tmEnd.tm_mday -=1;
	tmEnd.tm_hour  = 0;tmEnd.tm_min = 0;tmEnd.tm_sec = 0;
	
	time_t tEnd     = mktime(&tmEnd);
	tEnd += 60 * 60 * (endhour + 1);

	return tEnd;
}
