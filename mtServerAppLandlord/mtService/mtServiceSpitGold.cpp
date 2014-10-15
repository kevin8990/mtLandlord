#include "mtServiceSpitGold.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceSpitGold::~mtServiceSpitGold()
{
}

mtServiceSpitGold::mtServiceSpitGold()
{

}

int mtServiceSpitGold::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;
	m_pkSQLEnv		        = pkDataInit->pkSQLEnv;

	return	1;
}


int mtServiceSpitGold::run( void* pData )
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

int mtServiceSpitGold::exit()
{
	return 1;
}

int	mtServiceSpitGold::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceSpitGold::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceSpitGold::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceSpitGold::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::PigInfo  kPigInfo;

	std::tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	if(m_pkSQLEnv->getPigInfo(kDataRead.lUserId,&kPigInfo) && kPigInfo.lLevel > 0)
	{
		if ( kPigInfo.tUseTime.tm_year != kNow.tm_year
			||kPigInfo.tUseTime.tm_mon != kNow.tm_mon
			||kPigInfo.tUseTime.tm_mday !=kNow.tm_mday
			)
		{
			kDataWrite.lgold   = kPigInfo.lLevel * 10000;
			kDataWrite.lResult = E_RESULT_SUCESS;
		}
		else
		{
			kDataWrite.lResult = E_RESULT_HAVED;
		}
	}
	else
	{
		kDataWrite.lResult = E_RESULT_ERROR;
	}

	if(kDataWrite.lResult == E_RESULT_SUCESS)
	{
		mtQueueHall::UserInfo   kUserInfo;

		m_pkSQLEnv->getUserInfo(kDataRead.lUserId,&kUserInfo);
		kUserInfo.lUserGold   += kDataWrite.lgold;

		m_pkSQLEnv->updateUserInfo(&kUserInfo);


		kPigInfo.tUseTime      = kNow;
		m_pkSQLEnv->updatePigInfo(&kPigInfo);
	}
}

