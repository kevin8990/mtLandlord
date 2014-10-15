#include "mtServiceHallGoldBuyStatus.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
//#include "mtPrint.h"

mtServiceHallGoldBuyStatus::~mtServiceHallGoldBuyStatus()
{
}

mtServiceHallGoldBuyStatus::mtServiceHallGoldBuyStatus()
{

}

int mtServiceHallGoldBuyStatus::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkQueueHall           = pkDataInit->pkQueueHall;

	return	1;
}


int mtServiceHallGoldBuyStatus::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0 , sizeof(kDataWrite));
	memset(&kDataRead, 0 , sizeof(kDataRead));

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		///mtPrint(&kDataRead);
		kDataWrite.lStructBytes            = sizeof(kDataWrite);
		kDataWrite.lServiceType            = kDataRead.lServiceType;
		kDataWrite.lUserId                 = kDataRead.lUserId;

		/// 当前用户是否在线
		/// 根据用户id，获得用户节点在内存的地址
		mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
		if (!pkUserDataNode)
		{
			kDataWrite.lResult           = -1;
		}
		else
		{
			if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
			{
				kDataWrite.lResult       = -3;
			}
			else
			{
				/// 未收到收费服务器的更新状态
				if (0 == pkUserDataNode->lUserGoldBuyStatus)
				{
					kDataWrite.lResult   = -2;
				}
				else
				{
					kDataWrite.lResult   = 1;
					kDataWrite.lGold     = pkUserDataNode->lUserGoldBuy;
				}
			}
		}

		//mtPrint(&kDataWrite);
		iRet = runWrite(iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3);
		if (iRet >= 1)
		{
			if (1 == kDataWrite.lResult)
			{
				pkUserDataNode->lUserGoldBuyStatus  =  0;
				pkUserDataNode->kUserInfo.lUserGold += pkUserDataNode->lUserGoldBuy;
			}

			shutdown(iSocket, 1);
			closesocket(iSocket);
			*(SOCKET*)pData = INVALID_SOCKET;
		}
	}

	return 1;
}

int mtServiceHallGoldBuyStatus::exit()
{
	return 1;
}

int	mtServiceHallGoldBuyStatus::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallGoldBuyStatus::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallGoldBuyStatus::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}
