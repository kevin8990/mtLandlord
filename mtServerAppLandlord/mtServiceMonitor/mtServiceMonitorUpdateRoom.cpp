#include "mtServiceMonitorUpdateRoom.h"

mtServiceMonitorUpdateRoom::~mtServiceMonitorUpdateRoom()
{
}

mtServiceMonitorUpdateRoom::mtServiceMonitorUpdateRoom()
{

}

int mtServiceMonitorUpdateRoom::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_pkSpace               = pkDataInit->pkSpace;

	return	1;
}


int mtServiceMonitorUpdateRoom::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	memset(&kDataRead, 0 , sizeof(kDataRead));

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		/// 更新房间信息
		int iRoomIdx;
		int iSpaceIdx;
		for (iSpaceIdx = 0; iSpaceIdx < 2; iSpaceIdx++)
		{
			for (iRoomIdx = 0; iRoomIdx < 6; iRoomIdx++)
			{
				m_pkSpace[iSpaceIdx].pkRoom[iRoomIdx].lPersonNumber  = kDataRead.plSpaceRoom[iSpaceIdx][iRoomIdx];
			}
		}

		OutPutPersonNumber();
	}

	return 1;
}

void mtServiceMonitorUpdateRoom::OutPutPersonNumber()
{
	SYSTEMTIME  sTime;
	GetLocalTime(&sTime);
	char buffer[50];
	sprintf(buffer,"%u/%u/%u %u:%u:%u",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);
	
	int iPersonCount = 0;
	for(int i = 0;i<6;i++){
		iPersonCount += m_pkSpace[0].pkRoom[i].lPersonNumber;
	}
	
	printf("\n\n场ID    房ID     人数（%d）    \
				  \n0       0        %d     \
				  \n0       1        %d     \
				  \n0       2        %d     \
				  \n0       3        %d     \
				  \n0       4        %d     \
				  \n0       5        %d      (%s)\n\n",   \
				  iPersonCount,\
				  m_pkSpace[0].pkRoom[0].lPersonNumber,m_pkSpace[0].pkRoom[1].lPersonNumber,m_pkSpace[0].pkRoom[2].lPersonNumber,  \
				  m_pkSpace[0].pkRoom[3].lPersonNumber,m_pkSpace[0].pkRoom[4].lPersonNumber,m_pkSpace[0].pkRoom[5].lPersonNumber,buffer   
				  );
}
//add by wsd end

int mtServiceMonitorUpdateRoom::exit()
{
	return 1;
}

int	mtServiceMonitorUpdateRoom::runRead(SOCKET socket, DataRead* pkDataRead)
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

