#include "mtServiceCashLottery.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceCashLottery::~mtServiceCashLottery()
{

}

mtServiceCashLottery::mtServiceCashLottery()
{

}

int mtServiceCashLottery::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceCashLottery::run( void* pData )
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

int mtServiceCashLottery::exit()
{
	return 1;
}

int	mtServiceCashLottery::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceCashLottery::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceCashLottery::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceCashLottery::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	static  long needFeeCard[] = {30,48,90,168,30,48,94,1788,1800,198};

	mtQueueHall::LotteryInfo kLotteryInfo;
    
	m_pkSQLEnv->getLotteryinfo(kDataRead.lUserId,&kLotteryInfo);

	if(kLotteryInfo.lFeeCard < needFeeCard[kDataRead.lPrizeId])
	{
		kDataWrite.lResult  = 0;
	}
	else
	{
		kLotteryInfo.lFeeCard  -= needFeeCard[kDataRead.lPrizeId];
		kDataWrite.lFeeCard     = kLotteryInfo.lFeeCard;
		kDataWrite.lResult      = 1;

		SaveAwardInfo(kDataRead.lUserId,kDataRead.lPrizeId,kDataRead.pcPhone);

		m_pkSQLEnv->updateLotteryinfo(&kLotteryInfo);
	}

	kDataWrite.lPrizeId  = kDataRead.lPrizeId;
}

void mtServiceCashLottery::SaveAwardInfo(long lUserId,long lPrizeId,char *pcPhone)
{
	if(lPrizeId < Seven_hundred_and_twenty_thousand_gold || lPrizeId > yiqi_commemorate_silver)
	{
		MT_DEBUG_PTCL("\nmtServiceCashLottery::SaveAwardInfo,lPrizeId:%d",lPrizeId);
		return;
	}

	static  long cashGoldList[]      = {720000,1300000,2600000,5200000};

	mtQueueHall::AwardInfo   kAwardInfo;
	mtQueueHall::UserInfo    kUserInfo;

	memset(&kAwardInfo,0,sizeof(kAwardInfo));
	memset(&kUserInfo,0,sizeof(kUserInfo));

	kAwardInfo.lUserid  = lUserId;
	kAwardInfo.lPrize   = lPrizeId + MT_CASH_LOTTERY_BASE;
	strcpy(kAwardInfo.pcPhone,pcPhone);

	if(lPrizeId < thirty_rechargeable_card)
	{
		kAwardInfo.iIsUse = 1;
		m_pkSQLEnv->getUserInfo(lUserId,&kUserInfo);
		kUserInfo.lUserGold += cashGoldList[lPrizeId];
		m_pkSQLEnv->updateUserInfo(&kUserInfo);
	}
	else
	{
		kAwardInfo.iIsUse = 0;
	}

	 m_pkSQLEnv->saveAwardInfo(&kAwardInfo);
}