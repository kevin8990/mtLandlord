#include "mtServiceMonitorUpdateSysNotice.h"

mtServiceMonitorUpdateSysNotice::~mtServiceMonitorUpdateSysNotice()
{
}

mtServiceMonitorUpdateSysNotice::mtServiceMonitorUpdateSysNotice()
{

}

int mtServiceMonitorUpdateSysNotice::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;

	return	1;
}


int mtServiceMonitorUpdateSysNotice::run( void* pData )
{
	SOCKET	   iSocket	= *(SOCKET*)pData;
	DataRead   kDataRead;
	memset(&kDataRead, 0 , sizeof(kDataRead));

	int		   iRet	= 0;
	if (runRead(iSocket, &kDataRead))
	{
		if (1 == kDataRead.lEffectStyle) /// 公告立即生效
		{
			memset(g_pcSystemNoticeNew, 0, sizeof(kDataRead.pcSystemNotice));
			memcpy(g_pcSystemNotice, kDataRead.pcSystemNotice, sizeof(kDataRead.pcSystemNotice));
		}
		else
		{
			memcpy(g_pcSystemNoticeNew, kDataRead.pcSystemNotice, sizeof(kDataRead.pcSystemNotice));
		}
	}

	return 1;
}

int mtServiceMonitorUpdateSysNotice::exit()
{
	return 1;
}

int	mtServiceMonitorUpdateSysNotice::runRead(SOCKET socket, DataRead* pkDataRead)
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

int	mtServiceMonitorUpdateSysNotice::runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes)
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

