#include "mtServiceHallGoldBuy.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallGoldBuy::~mtServiceHallGoldBuy()
{
}

mtServiceHallGoldBuy::mtServiceHallGoldBuy()
{

}

int mtServiceHallGoldBuy::init(void* pData)
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceHallGoldBuy::run(void* pData)
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

int mtServiceHallGoldBuy::exit()
{
	return 1;
}

int	mtServiceHallGoldBuy::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallGoldBuy::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallGoldBuy::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceHallGoldBuy::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::UserInfo kUserInfo;

	m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

	if(kUserInfo.lUserPayCount == 0)//首次充值
	{
		kUserInfo.lUserFirstCharge = kDataRead.lGold;
	}

	kUserInfo.lUserGold     += kDataRead.lGold;
	kUserInfo.lUserPayCount += kDataRead.lMoney;

	m_pkSQLEnv->savePayInfo(atoi(kUserInfo.pcUserAccountNum),kDataRead.lPoint,kDataRead.lMoney);
	m_pkSQLEnv->updateUserInfo(&kUserInfo);

	kDataWrite.lResult  = 1;
	kDataWrite.lGold    = kDataRead.lGold;	
	kDataWrite.lMash    = givingMash(kDataRead.lUserId,kDataRead.lMoney);///冲金币，送饲料		
}

long mtServiceHallGoldBuy::givingMash(long  lUserId,long lMoney)
{
	long lMash = 0;
	switch(lMoney)
	{
		case 6:
		case 8:
			lMash = 1;
			break;;
		case 10:
			lMash = 2;
			break;
		case 20:
			lMash = 4;
			break;
		case 50:
			lMash = 11;
			break;
		case 100:
			lMash = 25;
			break;
		default:
			lMash = 0;
			break;
	}
	if(lMash != 0)
	{
		mtQueueHall::PigInfo kPigInfo;

		memset(&kPigInfo,0,sizeof(kPigInfo));

		if(m_pkSQLEnv->getPigInfo(lUserId,&kPigInfo))
		{
			kPigInfo.lMash += lMash;

			m_pkSQLEnv->updatePigInfo(&kPigInfo);
		}
		else
		{
			kPigInfo.lUserId  = lUserId;
			kPigInfo.lMash   += lMash;

			m_pkSQLEnv->savePigInfo(&kPigInfo);
		}

		//保存饲料到包裹表
		mtQueueHall::UseTimeInfo kUseTimeInfo;
		tm  kNow;

		memset(&kUseTimeInfo,0,sizeof(kUseTimeInfo));
		memset(&kNow,0,sizeof(kNow));

		if(m_pkSQLEnv->getUserPropInfo(lUserId,6,&kUseTimeInfo))
		{
			m_pkSQLEnv->updateUserPropInfo(lUserId,6,kNow);
		}
		else
		{
			m_pkSQLEnv->saveUserPropInfo(lUserId,6,kNow);
		}
	}

	return lMash;
}
