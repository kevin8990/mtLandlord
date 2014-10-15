#include "mtServiceHallHall2Room.h"
#include "mtServiceMgr.h"
#include "mtQueuePacket.h"
#include "mtPrint.h"

mtServiceHallHall2Room::~mtServiceHallHall2Room()
{
}

mtServiceHallHall2Room::mtServiceHallHall2Room()
{

}

int mtServiceHallHall2Room::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueueHall           = pkDataInit->pkQueueHall;

	return	1;
}

int mtServiceHallHall2Room::run( void* pData )
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

int mtServiceHallHall2Room::exit()
{
	return 1;
}

int	mtServiceHallHall2Room::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceHallHall2Room::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

void* mtServiceHallHall2Room::getUserNodeAddr(long lUserId)
{
	if (MT_SERVER_WORK_USER_ID_MIN <= lUserId && MT_SERVER_WORK_USER_ID_MAX > lUserId)
	{
		return m_pkQueueHall->m_kDataHall.pkUserDataNodeList + lUserId;
	}

	return NULL;
}

void   mtServiceHallHall2Room::initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite)
{
	mtQueueHall::UserDataNode* pkUserDataNode = (mtQueueHall::UserDataNode*)getUserNodeAddr(kDataRead.lUserId);

	if (mtQueueHall::E_HALL_USER_STATUS_ROOM == pkUserDataNode->lIsOnLine) /// 用户当前在房间里(不能重复登录)
	{
		kDataWrite.lSpaceId        = pkUserDataNode->lSpaceId;
		kDataWrite.lRoomId         = pkUserDataNode->lRoomId;
		kDataWrite.lResult         = mtQueueHall::E_HALL_USER_STATUS_ROOM;
	}
	else if (mtQueueHall::E_HALL_USER_STATUS_ROOM_FORCE_EXIT == pkUserDataNode->lIsOnLine) /// 用户当前在房间里(由于上次是强行退出某房间)
	{
		kDataWrite.lSpaceId        = pkUserDataNode->lSpaceId;
		kDataWrite.lRoomId         = pkUserDataNode->lRoomId;
		kDataWrite.lResult         = mtQueueHall::E_HALL_USER_STATUS_ROOM_FORCE_EXIT;
	}
	else if (mtQueueHall::E_HALL_USER_STATUS_ROOM_NETWORK_EXIT == pkUserDataNode->lIsOnLine) /// 用户当前在房间里(由于上次是网络原因退出某房间)
	{
		kDataWrite.lSpaceId        = pkUserDataNode->lSpaceId;
		kDataWrite.lRoomId         = pkUserDataNode->lRoomId;
		kDataWrite.lResult         = mtQueueHall::E_HALL_USER_STATUS_ROOM_NETWORK_EXIT;
	}
	else
	{
		pkUserDataNode->lIsOnLine  = mtQueueHall::E_HALL_USER_STATUS_ROOM;
		pkUserDataNode->lSpaceId   = kDataRead.lSpaceId;
		pkUserDataNode->lRoomId    = kDataRead.lRoomId;

		kDataWrite.lSpaceId        = pkUserDataNode->lSpaceId;
		kDataWrite.lRoomId         = pkUserDataNode->lRoomId;
		kDataWrite.lResult         = 1;
	}
}

