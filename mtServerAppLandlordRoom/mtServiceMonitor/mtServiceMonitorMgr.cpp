#include "mtServiceMonitorMgr.h"
#include "mtPlatform.h"
#include "mtServiceMonitorUpdateRoom.h"

mtServiceMonitorMgr::~mtServiceMonitorMgr()
{

}

mtServiceMonitorMgr::mtServiceMonitorMgr()
{

}

int mtServiceMonitorMgr::run(mtServiceMonitor::DataRun* pkDataRun)
{
	return m_pkServiceMonitorList[pkDataRun->lServiceType]->run(pkDataRun);
}

int mtServiceMonitorMgr::exit()
{
	int		iEventGameServiceIdx;
	for (iEventGameServiceIdx = E_SERVICE_MONITOR_BEG; iEventGameServiceIdx < E_SERVICE_MONITOR_END; iEventGameServiceIdx++)
	{
		if (NULL != m_pkServiceMonitorList[iEventGameServiceIdx])
		{
			m_pkServiceMonitorList[iEventGameServiceIdx]->exit();
			delete m_pkServiceMonitorList[iEventGameServiceIdx];
			m_pkServiceMonitorList[iEventGameServiceIdx] = NULL;
		}
	}

	return	1;
}

int mtServiceMonitorMgr::init( DataInit* pkDataInit )
{
	m_pkQueueMgr = pkDataInit->pkQueueMgr;

	initService();

	return 1;
}


int	mtServiceMonitorMgr::runReadDataHead(SOCKET socket, DataHead* pkDataHead)
{	
	int	iReadBytesTotalHas	= recv(socket, (char*)pkDataHead, sizeof(DataHead), MSG_PEEK);
	if (0 > iReadBytesTotalHas || iReadBytesTotalHas < sizeof(pkDataHead->lStructBytes))
	{		
		return	0;
	}

	if (0 < pkDataHead->lStructBytes && iReadBytesTotalHas >= pkDataHead->lStructBytes) 
	{
		iReadBytesTotalHas	= recv(socket, (char*)pkDataHead, sizeof(DataHead), MSG_PEEK);
	}

	return (iReadBytesTotalHas < sizeof(DataHead)) ? 0 : 1;
}

int mtServiceMonitorMgr::initService()
{
	initServiceMonitorUpdateRoom();

	return	1;
}

int mtServiceMonitorMgr::initServiceMonitorUpdateRoom()
{
	mtServiceMonitorUpdateRoom::DataInit		kDataInit; 
	kDataInit.lStructBytes	= sizeof(kDataInit);
	kDataInit.pkQueueHall   = m_pkQueueMgr->getQueueHall();

	m_pkServiceMonitorList[E_SERVICE_MONITOR_UPDATE_ROOM]	= new mtServiceMonitorUpdateRoom;
	return m_pkServiceMonitorList[E_SERVICE_MONITOR_UPDATE_ROOM]->init(&kDataInit);
}

