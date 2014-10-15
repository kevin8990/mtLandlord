#include "mtServiceMonitorUpdateGoldBuy.h"

mtServiceMonitorUpdateGoldBuy::~mtServiceMonitorUpdateGoldBuy()
{
}

mtServiceMonitorUpdateGoldBuy::mtServiceMonitorUpdateGoldBuy()
{

}

int mtServiceMonitorUpdateGoldBuy::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkDataHall            = pkDataInit->pkDataHall;

	return	1;
}


int mtServiceMonitorUpdateGoldBuy::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	DataWrite  kDataWrite;
	memset(&kDataRead, 0 , sizeof(kDataRead));
	memset(&kDataWrite, 0 , sizeof(kDataWrite));
	kDataWrite.lStructBytes      = sizeof(kDataWrite);

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		kDataWrite.lServiceType      = kDataRead.lServiceType;
		memcpy(kDataWrite.pcKey, kDataRead.pcKey, sizeof(kDataWrite.pcKey));
		kDataWrite.lUserId				= kDataRead.lUserId;
		kDataWrite.lGoldBuy          = kDataRead.lGoldBuy;

		/// 根据用户id，获得用户节点在内存的地址
		mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);
		if (!pkUserDataNode)
		{
			kDataWrite.lResult       = -1;
		}
		else
		{
			if (0 >= kDataRead.lGoldBuy)
			{
				kDataWrite.lResult   = -2;
			}
			else
			{
				kDataWrite.lResult                             = 1;
				pkUserDataNode->lUserGoldBuy                   = kDataRead.lGoldBuy;
				pkUserDataNode->lUserGoldBuyStatus             = 1;
			}
		}
	}

	iRet = runWrite(iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3);
	if (iRet >= 1)
	{
		shutdown(iSocket, 1);
		closesocket(iSocket);
		*(SOCKET*)pData = INVALID_SOCKET;
	}

	return 1;
}

int mtServiceMonitorUpdateGoldBuy::exit()
{
	return 1;
}

int	mtServiceMonitorUpdateGoldBuy::runRead(SOCKET socket, DataRead* pkDataRead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, sizeof(DataRead), MSG_PEEK);
	if (0 > iReadBytesTotalHas || iReadBytesTotalHas < sizeof(pkDataRead->lStructBytes))
	{		
		return	0;
	}

	if (iReadBytesTotalHas >= pkDataRead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataRead, pkDataRead->lStructBytes, 0);
	}

	return (iReadBytesTotalHas < pkDataRead->lStructBytes) ? 0 : 1;
}

int	mtServiceMonitorUpdateGoldBuy::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceMonitorUpdateGoldBuy::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkDataHall->pkUserDataNodeList + lUserId;
	}

	return NULL;
}
