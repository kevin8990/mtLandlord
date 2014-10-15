#include "mtServiceConnect.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceConnect::~mtServiceConnect()
{
}

mtServiceConnect::mtServiceConnect()
{

}

int mtServiceConnect::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv               = pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceConnect::run( void* pData )
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

		kDataWrite.lStructBytes             = sizeof(kDataWrite);
		kDataWrite.lServiceType             = kDataRead.lServiceType;

		if (MT_SERVER_WORK_USER_ID_MIN <= kDataRead.lUserId && MT_SERVER_WORK_USER_ID_MAX > kDataRead.lUserId)
		{
			 /// 根据用户id，获得用户节点在内存的地址
				kDataWrite.lResult   = 0;
				mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
				if (!pkUserDataNode)
				{
					return 0;
				}
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine
					|| mtQueueHall::E_HALL_USER_STATUS_ONLINE == pkUserDataNode->lIsOnLine
					|| mtQueueHall::E_HALL_USER_STATUS_HALL == pkUserDataNode->lIsOnLine)
				{
					pkUserDataNode->lIsOnLine = mtQueueHall::E_HALL_USER_STATUS_HALL;
				}

				initDataWrite(kDataRead,kDataWrite);

				pkUserDataNode->lStructBytes      = sizeof(mtQueueHall::UserDataNode);
				pkUserDataNode->kUserInfo         = kDataWrite.kUserInfo;

			  MT_DEBUG_COLOR(4,FOREGROUND_GREEN | FOREGROUND_INTENSITY, "\nNew user(UserId = %d) login game hall......\n", kDataWrite.kUserInfo.lUserId);							
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
	return 1;
}

int mtServiceConnect::exit()
{
	return 1;
}

int	mtServiceConnect::runRead(SOCKET socket, DataRead* pkDataRead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, sizeof(DataRead), MSG_PEEK);

	if (0 > iReadBytesTotalHas || iReadBytesTotalHas < sizeof(pkDataRead->lStructBytes))
	{		
		return	0;
	}

	if (0 < pkDataRead->lStructBytes && iReadBytesTotalHas >= pkDataRead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, pkDataRead->lStructBytes, 0);
	}

	return (iReadBytesTotalHas < pkDataRead->lStructBytes) ? 0 : 1;
}

int	mtServiceConnect::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceConnect::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void mtServiceConnect::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	kDataWrite.lResult                  = 1;

	m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kDataWrite.kUserInfo);

	/// 计算设置用户是否连续天进行了登录(奖励分数用)
	calcContinueLoginHall(&kDataWrite.kUserInfo);

	/// 计算胜率
	if (0 >= kDataWrite.kUserInfo.lUserAllChess)
	{
		kDataWrite.kUserInfo.lUserAllChess     = 0;
		kDataWrite.kUserInfo.lUserWinChess     = 0;
		kDataWrite.kUserInfo.lUserWinRate      = 0;
	}
	else
	{
		kDataWrite.kUserInfo.lUserWinRate = 100 * (kDataWrite.kUserInfo.lUserWinChess / (float)(kDataWrite.kUserInfo.lUserAllChess));
	}

	m_pkSQLEnv->updateUserInfo(&kDataWrite.kUserInfo);

	/// 场房间信息
	if(1001 == kDataRead.lVendorId)
		memcpy(kDataWrite.pkSpace, m_pkQueueHall->m_kDataHall.pkSpace + 2, sizeof(kDataWrite.pkSpace));
	else
		memcpy(kDataWrite.pkSpace, m_pkQueueHall->m_kDataHall.pkSpace, sizeof(kDataWrite.pkSpace));

	UpdateUserProp(kDataRead.lUserId,kDataWrite.kUserInfo.plPropsCount);

}

void mtServiceConnect::UpdateUserProp(long lUserId,long *plUserPropsCount)
{
	std::tm tm;
	mtQueueHall::UseTimeInfo  kPropInfoList[MT_PROP_NUMBER];
	long    proNumber = 0;

	m_pkSQLEnv->getUserPropInfoList(lUserId,kPropInfoList,&proNumber);

	std::tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	for(int index = 0;index < proNumber;++index)
	{  
		//客户端收到的是道具的剩余使用小时
		if(kPropInfoList[index].tUseTime.tm_year == 0)//永久道具
		{
			plUserPropsCount[kPropInfoList[index].lId - 1] = -1;
		}
		else//限时道具
		{
			int iHours = 0;

			time_t nowTime = mktime(&kNow);

			time_t stopTime = mktime(&kPropInfoList[index].tUseTime);

			iHours = (stopTime - nowTime) / (60 * 60) + 1;

			plUserPropsCount[kPropInfoList[index].lId - 1] = iHours > 0 ? iHours : 0;
		} 
	}
}

void mtServiceConnect::calcContinueLoginHall(mtQueueHall::UserInfo* pkUserInfo)
{	
	tm  kNow;
	__time64_t kNowTime;
	
	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	tm  kPre = kNow;
	kPre.tm_mday = kPre.tm_mday - 1;
	if(mktime(&kPre) != ((time_t)-1))
	{
		long lLoginHallDateNew;
		unsigned short* pusYear = (unsigned short*)&lLoginHallDateNew;
		char* pcMonthDate       = (char*)&lLoginHallDateNew;

		pusYear[1]              = kPre.tm_year;
		pcMonthDate[1]          = kPre.tm_mon;
		pcMonthDate[0]          = kPre.tm_mday;

		/// 连续进行了登录
		if (pkUserInfo->lUserLoginHallDate == lLoginHallDateNew)
		{
			pusYear[1]                          = kNow.tm_year;
			pcMonthDate[1]                      = kNow.tm_mon;
			pcMonthDate[0]                      = kNow.tm_mday;
			pkUserInfo->lUserLoginHallDate      = lLoginHallDateNew;
			pkUserInfo->lUserContinueLoginDays++;
			pkUserInfo->lUserLoginFirstToday    = 1;

			/// 用户在每个房间的当天登录次数
			memset(pkUserInfo->plUserChessPalyed, 0, sizeof(pkUserInfo->plUserChessPalyed));
			pkUserInfo->lUserDayChess           = 0;
			pkUserInfo->lUserGiveGoldFrequence  = 0;
		}
		else /// 没有连续天登录，或者连续登录，但是已经计算过了
		{
			pusYear[1]              = kNow.tm_year;
			pcMonthDate[1]          = kNow.tm_mon;
			pcMonthDate[0]          = kNow.tm_mday;

			pkUserInfo->lUserLoginFirstToday    = 0;
			if (pkUserInfo->lUserLoginHallDate != lLoginHallDateNew) /// 没有连续天登录
			{
				/// 设置为今天为最近一次登录日期
				pkUserInfo->lUserLoginHallDate      = lLoginHallDateNew;
				pkUserInfo->lUserContinueLoginDays  = 1;
				pkUserInfo->lUserLoginFirstToday    = 1;

				/// 用户在每个房间的当天登录次数
				memset(pkUserInfo->plUserChessPalyed, 0, sizeof(pkUserInfo->plUserChessPalyed));
				pkUserInfo->lUserDayChess           = 0;
				pkUserInfo->lUserGiveGoldFrequence  = 0;
			}
		}
	}
}


