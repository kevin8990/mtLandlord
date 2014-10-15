#include "mtServiceHallExit.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallExit::~mtServiceHallExit()
{
}

mtServiceHallExit::mtServiceHallExit()
{

}

int mtServiceHallExit::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueuePacket         = pkDataInit->pkQueuePacket;
	m_pkUserDataNodeList    = pkDataInit->pkUserDataNodeList;

	m_pkSQLEnv				= pkDataInit->pkSQLEnv;

	return	1;
}

int mtServiceHallExit::run( void* pData )
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

		if (MT_SERVER_WORK_USER_ID_MIN <= kDataRead.kUserInfo.lUserId && MT_SERVER_WORK_USER_ID_MAX > kDataRead.kUserInfo.lUserId)
		{
			// 根据用户id，获得用户节点在内存的地址
			mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.kUserInfo.lUserId);
			if (!pkUserDataNode)
			{
				kDataWrite.lResult = 0;
			}
			else
			{
				if (mtQueueHall::E_HALL_USER_STATUS_OFFLINE == pkUserDataNode->lIsOnLine)
				{
					kDataWrite.lResult = 3;
					pkUserDataNode->lSpaceId = -1;
					pkUserDataNode->lRoomId  = -1;
				}
				else
				{
					if (mtQueueHall::E_HALL_USER_STATUS_ONLINE == pkUserDataNode->lIsOnLine
						|| mtQueueHall::E_HALL_USER_STATUS_HALL   == pkUserDataNode->lIsOnLine) /// 用户当前在线
					{
						pkUserDataNode->lIsOnLine              = mtQueueHall::E_HALL_USER_STATUS_OFFLINE;
						pkUserDataNode->lSpaceId = -1;
						pkUserDataNode->lRoomId  = -1;
					}

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

int mtServiceHallExit::exit()
{
	return 1;
}

int	mtServiceHallExit::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallExit::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallExit::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkUserDataNodeList + lUserId;
	}
	return NULL;
}

void  mtServiceHallExit::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	m_pkSQLEnv->updateUserInfo(&kDataRead.kUserInfo);
	kDataWrite.lResult                  = 1;
	kDataWrite.lUserId                  = kDataRead.kUserInfo.lUserId;
}

