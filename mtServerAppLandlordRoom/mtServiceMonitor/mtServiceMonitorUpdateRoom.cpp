#include "mtServiceMonitorUpdateRoom.h"


mtServiceMonitorUpdateRoom::mtServiceMonitorUpdateRoom():
m_pkQueueHall(NULL)
{

}

mtServiceMonitorUpdateRoom::~mtServiceMonitorUpdateRoom()
{

}

int mtServiceMonitorUpdateRoom::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkQueueHall           = pkDataInit->pkQueueHall;

	return	1;
}

int mtServiceMonitorUpdateRoom::run( DataRun* pkDataRun )
{
	SOCKET	   iSocket	= pkDataRun->iSocket;
	DataWrite  kDataWrite;
	memset(&kDataWrite, 0 , sizeof(kDataWrite));

	int		   iRet	= 0;
	kDataWrite.lStructBytes            = sizeof(kDataWrite);
	kDataWrite.lServiceType            = pkDataRun->lServiceType;

	/// 更新房间信息
	int iRoomIdx;
	int iSpaceIdx;
	int lOnlinePersonNumber;
	for (iSpaceIdx = 0; iSpaceIdx < m_pkQueueHall->m_lSpaceTotal; iSpaceIdx++)
	{
		for (iRoomIdx = 0; iRoomIdx < m_pkQueueHall->m_pkSpaceList[iSpaceIdx].lRomTotal; iRoomIdx++)
		{
			lOnlinePersonNumber = m_pkQueueHall->m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].lOnlinePersonNumber;
			kDataWrite.plSpaceRoom[iSpaceIdx][iRoomIdx] = ((lOnlinePersonNumber >= 0) ? lOnlinePersonNumber : 0);
		}
	}

	iRet = runWrite(iSocket, (char*)&kDataWrite, kDataWrite.lStructBytes, 0, 3);

	return 1;
}

int	mtServiceMonitorUpdateRoom::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

int mtServiceMonitorUpdateRoom::exit()
{

	return	0;
}

