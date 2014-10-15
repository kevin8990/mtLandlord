#include "mtServiceGetLotteryArg.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceGetLotteryArg::~mtServiceGetLotteryArg()
{
}

mtServiceGetLotteryArg::mtServiceGetLotteryArg()
{

}

int mtServiceGetLotteryArg::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceGetLotteryArg::run( void* pData )
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
				kDataWrite.lResult = -1;
			}
			else
			{
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
				{
					kDataWrite.lResult = -1;
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

int mtServiceGetLotteryArg::exit()
{
	return 1;
}

int	mtServiceGetLotteryArg::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceGetLotteryArg::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceGetLotteryArg::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void mtServiceGetLotteryArg::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::PrizeInfo kPrizeInfo;
	mtQueueHall::LotteryInfo kLotteryInfo;

	memset(&kLotteryInfo,0,sizeof(kLotteryInfo));

	if(m_pkSQLEnv->getPrizeInfo(&kPrizeInfo))//获取抽奖参数
	{
		kDataWrite.lNeedGold                = kPrizeInfo.lNeedGold;

		kDataWrite.lResult                  = 1;

		bool bVip = IsVip(kDataRead.lUserId);

		if(m_pkSQLEnv->getLotteryinfo(kDataRead.lUserId,&kLotteryInfo))
		{		
			//计算可抽奖次数
			calcSurplusLotteryFrequency(&kLotteryInfo,kPrizeInfo.lMaxTimes,kPrizeInfo.lVipTimes,bVip);

			kDataWrite.lMaxTimes               = bVip ? kPrizeInfo.lVipTimes : kPrizeInfo.lMaxTimes;
			kDataWrite.lUseTimes                = kLotteryInfo.lUseTimes;
			kDataWrite.lLuck                    = kLotteryInfo.lLuck;
			kDataWrite.lFeeCard                 = kLotteryInfo.lFeeCard;

			//跟新数据库可抽奖次数
			kLotteryInfo.lUserId = kDataRead.lUserId;
			m_pkSQLEnv->updateLotteryinfo(&kLotteryInfo);
		}
		else
		{
			calcSurplusLotteryFrequency(&kLotteryInfo,kPrizeInfo.lMaxTimes,kPrizeInfo.lVipTimes,bVip);

			kLotteryInfo.lUserId                = kDataRead.lUserId;
			kLotteryInfo.lUseTimes              = kLotteryInfo.lUseTimes;

			kDataWrite.lMaxTimes               = kLotteryInfo.lUseTimes;	
			kDataWrite.lUseTimes                = kLotteryInfo.lUseTimes;
			kDataWrite.lLuck                    = 0;
			kDataWrite.lFeeCard                 = 0;

			m_pkSQLEnv->saveLotteryinfo(&kLotteryInfo);
		}					 
	}
	else
	{
		kDataWrite.lResult                  = 0;
	}
}

void mtServiceGetLotteryArg::calcSurplusLotteryFrequency(mtQueueHall::LotteryInfo * pkLotteryInfo,long lMaxTimes,long lVipTimes,bool bVip)
{
	tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	
		if ( pkLotteryInfo->tUseTime.tm_year != kNow.tm_year
			||pkLotteryInfo->tUseTime.tm_mon != kNow.tm_mon
			||pkLotteryInfo->tUseTime.tm_mday !=kNow.tm_mday
			)
		{
			//重置剩余抽奖次数
			pkLotteryInfo->lUseTimes = bVip ? lVipTimes : lMaxTimes;
			//vip 的最大抽奖次数
		}
        pkLotteryInfo->tUseTime = kNow;
}

bool mtServiceGetLotteryArg::IsVip(long lUserId)
{
	mtQueueHall::UseTimeInfo  kTaskPrizeInfo;

	std::tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_FORVER + 1,&kTaskPrizeInfo))
	{
		return true;
	}
	else if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_THREE_MONTHS + 1,&kTaskPrizeInfo))
	{
		int iHours = 0;

		time_t nowTime = mktime(&kNow);
		time_t stopTime = mktime(&kTaskPrizeInfo.tUseTime);

		iHours = (stopTime - nowTime) / (60 * 60);

		if(iHours > 0)
		{
			return true;
		}
	}
	else if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_ONE_MONTHS + 1,&kTaskPrizeInfo))
	{
		int iHours = 0;

		time_t nowTime = mktime(&kNow);
		time_t stopTime = mktime(&kTaskPrizeInfo.tUseTime);

		iHours = (stopTime - nowTime) / (60 * 60);

		if(iHours > 0)
		{
			return true;
		}
	}

	return false;
}
