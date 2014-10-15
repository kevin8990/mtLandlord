#include "mtServiceHallPropBuy.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallPropBuy::~mtServiceHallPropBuy()
{
}

mtServiceHallPropBuy::mtServiceHallPropBuy()
{

}

int mtServiceHallPropBuy::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceHallPropBuy::run( void* pData )
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
				kDataWrite.lResult = 0;
			}
			else
			{
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
				{
					kDataWrite.lResult = 0;
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

int mtServiceHallPropBuy::exit()
{
	return 1;
}

int	mtServiceHallPropBuy::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallPropBuy::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallPropBuy::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void mtServiceHallPropBuy::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	kDataWrite.lResult    = 1;
	kDataWrite.lPropIdx   = kDataRead.lPropIdx;

	mtQueueHall::UseTimeInfo kUseTimeInfo;

	tm  kNow;
	__time64_t kNowTime;

	memset(&kNow,0,sizeof(kNow));

	bool bVip = IsVip(kDataRead.lUserId);

	if(kDataRead.lPropIdx == 0 ||kDataRead.lPropIdx == 2||kDataRead.lPropIdx == 13||kDataRead.lPropIdx == 14)//限时道具
	{
		time(&kNowTime);
		_localtime64_s(&kNow, &kNowTime);

		int validTime  = 0;
		if(kDataRead.lPropIdx == 0)
		{
			validTime  = 3;
		}
		else if(kDataRead.lPropIdx == 2)
		{
			validTime  = 3;
		}
		else if (kDataRead.lPropIdx == 13)
		{
			validTime  = 30;
		}
		else
		{
			validTime  = 90;
		}

		time_t time1 = mktime(&kNow) + 8 * 60 * 60;//时区 8小时
		time1  += validTime * 24 *  60 * 60;
		tm * pt = gmtime(&time1); 


		if(m_pkSQLEnv->getUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,&kUseTimeInfo))
		{
			m_pkSQLEnv->updateUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,*pt);
		}
		else
		{
			m_pkSQLEnv->saveUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,*pt);
		}

		kDataWrite.lusetime    = validTime * 24;
	}
	else//永久道具
	{
		if(kDataRead.lPropIdx == 4 || kDataRead.lPropIdx == 5)//宠物或者宠物饲料
		{
			mtQueueHall::PigInfo kPigInfo;

			memset(&kPigInfo,0,sizeof(kPigInfo));

			if(m_pkSQLEnv->getPigInfo(kDataRead.lUserId,&kPigInfo))
			{
				kPigInfo.lMash += kDataRead.lPropIdx == 5 ? 1 : 0;

				if(kPigInfo.lLevel == 0 && kDataRead.lPropIdx == 4)
				{
					kPigInfo.lLevel = 1;
				}

				m_pkSQLEnv->updatePigInfo(&kPigInfo);
			}
			else
			{
				kPigInfo.lUserId  = kDataRead.lUserId;
				kPigInfo.lLevel = kDataRead.lPropIdx == 4 ? 1 : 0;
				kPigInfo.lMash += kDataRead.lPropIdx == 5 ? 1 : 0;

				m_pkSQLEnv->savePigInfo(&kPigInfo);
			}
		}

		if(!m_pkSQLEnv->getUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,&kUseTimeInfo))
		{
			m_pkSQLEnv->saveUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,kNow);
		}
		//else
		//{
		//	m_pkSQLEnv->updateUserPropInfo(kDataRead.lUserId,kDataRead.lPropIdx + 1,kNow);
		//}

		kDataWrite.lusetime    = -1;
	}

	if(kDataRead.lPropIdx >= 13 && kDataRead.lPropIdx <= 15)//vip道具额外奖励
	{
		mtQueueHall::PigInfo kPigInfo;

		memset(&kPigInfo,0,sizeof(kPigInfo));

		//奖励3个饲料
		if(m_pkSQLEnv->getPigInfo(kDataRead.lUserId,&kPigInfo))
		{
			kPigInfo.lMash += 3;

			m_pkSQLEnv->updatePigInfo(&kPigInfo);
		}
		else
		{
			kPigInfo.lUserId  = kDataRead.lUserId;
			kPigInfo.lMash   += 3;

			m_pkSQLEnv->savePigInfo(&kPigInfo);
		}
		//保存饲料到包裹表
		if(m_pkSQLEnv->getUserPropInfo(kDataRead.lUserId,6,&kUseTimeInfo))
		{
			m_pkSQLEnv->updateUserPropInfo(kDataRead.lUserId,6,kNow);
		}
		else
		{
			m_pkSQLEnv->saveUserPropInfo(kDataRead.lUserId,6,kNow);
		}

		//奖励38888个金币
		mtQueueHall::UserInfo   kUserInfo;

		m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

		kUserInfo.lUserGold += 38888;

		m_pkSQLEnv->updateUserInfo(&kUserInfo);

		//增加抽奖次数
		mtQueueHall::LotteryInfo kLotteryInfo;

		if(m_pkSQLEnv->getLotteryinfo(kDataRead.lUserId,&kLotteryInfo) && !bVip)
		{
			mtQueueHall::PrizeInfo kPrizeInfo;
			m_pkSQLEnv->getPrizeInfo(&kPrizeInfo);

			kLotteryInfo.lUseTimes += kPrizeInfo.lVipTimes - kPrizeInfo.lMaxTimes;
			m_pkSQLEnv->updateLotteryinfo(&kLotteryInfo);
		}
	}

	//用户信息更新
	mtQueueHall::UserInfo kUserInfo;

	m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

	if(kUserInfo.lUserPayCount == 0)//首次充值
	{
		kUserInfo.lUserFirstCharge = kDataRead.lPropPrice * 10000;
	}

	kUserInfo.lUserPayCount        += kDataRead.lPropPrice;

	m_pkSQLEnv->updateUserInfo(&kUserInfo);

	//保存用户付费信息
	m_pkSQLEnv->savePayInfo(kDataRead.lUserId + MT_SERVER_WORK_USER_ID_BASE,kDataRead.lPropIdx + 11,kDataRead.lPropPrice);
}

bool mtServiceHallPropBuy::IsVip(long lUserId)
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
