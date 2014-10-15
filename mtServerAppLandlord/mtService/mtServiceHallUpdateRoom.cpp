#include "mtServiceHallUpdateRoom.h"
#include "mtServiceMgr.h"
#include "mtPrint.h"

mtServiceHallUpdateRoom::~mtServiceHallUpdateRoom()
{
}

mtServiceHallUpdateRoom::mtServiceHallUpdateRoom()
{

}

int mtServiceHallUpdateRoom::init( void* pData )
{
	DataInit*	pkDataInit	  = (DataInit*)pData;
	m_pkQueueHall             = pkDataInit->pkQueueHall;
	m_pkSQLEnv                = pkDataInit->pkSQLEnv;
	return	1;
}

int mtServiceHallUpdateRoom::run( void* pData)
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
		/// 更新房间信息
		if(1001 == kDataRead.lVendorId)
		    memcpy(kDataWrite.pkSpace, m_pkQueueHall->m_kDataHall.pkSpace + 2, sizeof(kDataWrite.pkSpace));
		else
			memcpy(kDataWrite.pkSpace, m_pkQueueHall->m_kDataHall.pkSpace, sizeof(kDataWrite.pkSpace));
		
		//系统消息和每日公告
		int amountMessage = m_pkSQLEnv->getSystemMessageList(&kDataWrite.pkMessageInfo);
		m_pkSQLEnv->getLastNoticeInfo(kDataWrite.pcSystemNotice);

		parseMessage(kDataWrite.pcSystemNotice);
		if(amountMessage >0)
		{
			kDataWrite.llength       = amountMessage;
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

int mtServiceHallUpdateRoom::exit()
{
	return 1;
}

int	mtServiceHallUpdateRoom::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallUpdateRoom::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallUpdateRoom::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void mtServiceHallUpdateRoom::parseMessage(char * message)
{
	while(*message != '\0')
	{
		if(*message == '\\')
		{
			*message = '\n';
		}
		message++;
	}
}
