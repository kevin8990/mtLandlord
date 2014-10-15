#include "mtServiceGetTaskAward.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceGetTaskAward::~mtServiceGetTaskAward()
{
}

mtServiceGetTaskAward::mtServiceGetTaskAward()
{

}

int mtServiceGetTaskAward::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceGetTaskAward::run( void* pData )
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
		kDataWrite.ltaskId                 = kDataRead.ltaskId;
		kDataWrite.lResult                 = E_RESULT_MISS_DEMAND;

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

int mtServiceGetTaskAward::exit()
{
	return 1;
}

int	mtServiceGetTaskAward::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceGetTaskAward::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceGetTaskAward::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void mtServiceGetTaskAward::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	if(kDataRead.ltaskId < 0 || kDataRead.ltaskId > MT_TASK_NUMBER)
	{
		kDataWrite.lResult = E_RESULT_ERROR;
	}
	else
	{
		mtQueueHall::TaskInfo	 kTaskInfo;
		mtQueueHall::UserInfo    kUserInfo;

		memset(&kTaskInfo,0,sizeof(kTaskInfo));
		memset(&kUserInfo,0,sizeof(kUserInfo));

		m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);

		//判断是否达到要求
		if(kDataRead.ltaskId == MT_TASK_NUMBER)//首冲奖励
		{
			if(kUserInfo.lUserFirstCharge != 0)
			{
				kDataWrite.lGold           = kUserInfo.lUserFirstCharge; 
				kDataWrite.lResult         = E_RESULT_SUCCESS;
			}
		}
		else
		{			
			m_pkSQLEnv->getTaskInfo(kDataRead.ltaskId + 1,&kTaskInfo);

			kDataWrite.lGold      = kTaskInfo.awardgold;

			//VIP领取每日登录奖励，再加3000金币
			if(IsVip(kDataRead.lUserId) && kDataRead.ltaskId == 0)
			{
				kDataWrite.lGold   +=      MT_VIP_AWARD;
			}

			switch(kTaskInfo.cType)
			{
			case E_TASK_TYPE_DAILY_TASK:
				{
					if(kUserInfo.lUserDayChess >= kTaskInfo.ldemand ||kDataRead.ltaskId == 0)
					{
						kDataWrite.lResult = E_RESULT_SUCCESS;
					}
					break;
				}
			case E_TASK_TYPE_TOTAL_FIGHT:
				{
					if(kUserInfo.lUserAllChess >= kTaskInfo.ldemand)
					{
						kDataWrite.lResult = E_RESULT_SUCCESS;
					}
					break;
				}
			case E_TASK_TYPE_TOTAL_WIN:
				{
					if(kUserInfo.lUserWinChess >= kTaskInfo.ldemand)
					{
						kDataWrite.lResult = E_RESULT_SUCCESS;
					}
					break;
				}
			case E_TASK_TYPE_TOTAL_PAY:
				{
					if(kUserInfo.lUserPayCount >= kTaskInfo.ldemand)
					{
						kDataWrite.lResult = E_RESULT_SUCCESS;
					}
					break;
				}
			default:
				{
					MT_DEBUG_PTCL("\n任务奖励领取失败!");
				}
			}
		}

		//判断是否已经领取
		mtQueueHall::UseTimeInfo kUseTimeInfo;
		if(m_pkSQLEnv->getUserTaskPrizeInfo(kDataRead.lUserId,kDataRead.ltaskId + 1,&kUseTimeInfo)
			|| kDataRead.ltaskId == MT_TASK_NUMBER)
		{
			if(kTaskInfo.cType == E_TASK_TYPE_DAILY_TASK)
			{
				tm  kNow;
				__time64_t kNowTime;

				time(&kNowTime);
				_localtime64_s(&kNow, &kNowTime);

				if( kUseTimeInfo.tUseTime.tm_year == kNow.tm_year
					&& kUseTimeInfo.tUseTime.tm_mon  == kNow.tm_mon
					&& kUseTimeInfo.tUseTime.tm_yday == kNow.tm_yday)
				{
					kDataWrite.lResult = E_RESULT_HAVE_GET;
				}						
			}
			else
			{
				if(kDataRead.ltaskId == MT_TASK_NUMBER)//首冲奖励
				{
					if(kUserInfo.lUserPayCount != 0 && kUserInfo.lUserFirstCharge == -1)
					{
						kDataWrite.lResult = E_RESULT_HAVE_GET;
					}							
				}
				else
				{
					kDataWrite.lResult = E_RESULT_HAVE_GET;
				}
			}					
		}

		//保存领奖结果
		if(kDataWrite.lResult == E_RESULT_SUCCESS)
		{
			if(kTaskInfo.cType == E_TASK_TYPE_DAILY_TASK)
			{
				mtQueueHall::UseTimeInfo kTaskPrizeInfo;
				if(m_pkSQLEnv->getUserTaskPrizeInfo(kDataRead.lUserId,kDataRead.ltaskId + 1,&kTaskPrizeInfo))
				{
					m_pkSQLEnv->updateUserTaskPrizeInfo(kDataRead.lUserId,kDataRead.ltaskId + 1);
				}
				else
				{
					m_pkSQLEnv->saveUserTaskPrizeInfo(kDataRead.lUserId,kDataRead.ltaskId + 1);
				}
			}
			else
			{
				if(kDataRead.ltaskId == MT_TASK_NUMBER)//首冲奖励5包饲料
				{
					mtQueueHall::PigInfo  kPigInfo;

					memset(&kPigInfo,0,sizeof(kPigInfo));

					if(m_pkSQLEnv->getPigInfo(kDataRead.lUserId,&kPigInfo))
					{
						kPigInfo.lMash   += 5;
						m_pkSQLEnv->updatePigInfo(&kPigInfo);
					}
					else
					{
						kPigInfo.lUserId  = kDataRead.lUserId;
						kPigInfo.lMash   += 5;
						m_pkSQLEnv->savePigInfo(&kPigInfo);
					}

					//保存饲料到包裹表
					mtQueueHall::UseTimeInfo kUseTimeInfo;
					tm  kNow;

					memset(&kUseTimeInfo,0,sizeof(kUseTimeInfo));
					memset(&kNow,0,sizeof(kNow));

					if(m_pkSQLEnv->getUserPropInfo(kDataRead.lUserId,6,&kUseTimeInfo))
					{
						m_pkSQLEnv->updateUserPropInfo(kDataRead.lUserId,6,kNow);
					}
					else
					{
						m_pkSQLEnv->saveUserPropInfo(kDataRead.lUserId,6,kNow);
					}

					kUserInfo.lUserFirstCharge = -1; 
				}
				else
				{
					m_pkSQLEnv->saveUserTaskPrizeInfo(kDataRead.lUserId,kDataRead.ltaskId + 1);						
				}
			}
			kUserInfo.lUserGold   += kDataWrite.lGold; 
			m_pkSQLEnv->updateUserInfo(&kUserInfo);
		}	
	}
}

bool mtServiceGetTaskAward::IsVip(long lUserId)
{
	mtQueueHall::UseTimeInfo  kTaskPrizeInfo;

	std::tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_FORVER,&kTaskPrizeInfo))
	{
		return true;
	}
	else if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_THREE_MONTHS,&kTaskPrizeInfo))
	{
		int iHours = ((kTaskPrizeInfo.tUseTime.tm_year * 365 + kTaskPrizeInfo.tUseTime.tm_yday)
			- (kNow.tm_year * 365 + kNow.tm_yday)) * 24 + (kTaskPrizeInfo.tUseTime.tm_hour - kNow.tm_hour);
		if(iHours > 0)
		{
			return true;
		}
	}
	else if(m_pkSQLEnv->getUserPropInfo(lUserId,MT_VIP_ONE_MONTHS,&kTaskPrizeInfo))
	{
		int iHours = ((kTaskPrizeInfo.tUseTime.tm_year * 365 + kTaskPrizeInfo.tUseTime.tm_yday)
			- (kNow.tm_year * 365 + kNow.tm_yday)) * 24 + (kTaskPrizeInfo.tUseTime.tm_hour - kNow.tm_hour);
		if(iHours > 0)
		{
			return true;
		}
	}

	return false;
}
