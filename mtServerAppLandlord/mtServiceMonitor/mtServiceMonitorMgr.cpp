#include "mtServiceMonitorMgr.h"
#include "mtServiceMonitor.h"
#include "mtServiceMonitorUpdateRoom.h"
#include "mtServiceMonitorUpdateSysNotice.h"
#include "mtServiceMonitorUpdateGoldBuy.h"

mtServiceMonitorMgr::~mtServiceMonitorMgr()
{

}

mtServiceMonitorMgr::mtServiceMonitorMgr()
{

}

int mtServiceMonitorMgr::run(void* pData)
{
	DataHead kDataHead;
	memset(&kDataHead, 0 , sizeof(kDataHead));
	if (runReadDataHead(*(SOCKET*)pData, &kDataHead))
	{
		if (E_SERVICE_MONITOR_END > kDataHead.lServiceType && kDataHead.lServiceType>=E_SERVICE_MONITOR_BEG){
			m_pkServiceList[kDataHead.lServiceType]->run(pData);
		}
	}
	else
	{
		return 0;
	}

	return 1;
}

int mtServiceMonitorMgr::exit()
{
	int iServiceIdx;
	for (iServiceIdx = E_SERVICE_MONITOR_BEG; iServiceIdx < E_SERVICE_MONITOR_END; iServiceIdx++)
	{
		if (NULL != m_pkServiceList[iServiceIdx])
		{
			m_pkServiceList[iServiceIdx]->exit();
			delete m_pkServiceList[iServiceIdx];
			m_pkServiceList[iServiceIdx] = NULL;
		}
	}

	return	MTE_ERROR_OK;
}

int mtServiceMonitorMgr::init( DataInit* pkDataInit )
{
	m_pkDataHall           = pkDataInit->pkDataHall;
    ///m_pkSpace           = pkDataInit->pkSpace;

	initService();

	return 1;
}

int mtServiceMonitorMgr::initService()
{
	initServiceMonitorUpdateRoom();
	initServiceMonitorUpdateSysNotice();
	initServiceMonitorUpdateGoldBuy();

	return	1;
}

int mtServiceMonitorMgr::initServiceMonitorUpdateRoom()
{
	mtServiceMonitorUpdateRoom::DataInit  kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkSpace             = m_pkDataHall->pkSpace;

	m_pkServiceList[E_SERVICE_MONITOR_UPDATE_ROOM]	= new mtServiceMonitorUpdateRoom;
	return m_pkServiceList[E_SERVICE_MONITOR_UPDATE_ROOM]->init(&kDataInit);
}

int	mtServiceMonitorMgr::initServiceMonitorUpdateSysNotice()
{
	mtServiceMonitorUpdateSysNotice::DataInit  kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);

	m_pkServiceList[E_SERVICE_MONITOR_UPDATE_SYS_NOTICE]	= new mtServiceMonitorUpdateSysNotice;
	return m_pkServiceList[E_SERVICE_MONITOR_UPDATE_SYS_NOTICE]->init(&kDataInit);
}

int	mtServiceMonitorMgr::initServiceMonitorUpdateGoldBuy()
{
	mtServiceMonitorUpdateGoldBuy::DataInit  kDataInit; 
	kDataInit.lStructBytes	      = sizeof(kDataInit);
	kDataInit.pkDataHall          = m_pkDataHall;

	m_pkServiceList[E_SERVICE_MONITOR_UPDATE_GOLD_BUY]	= new mtServiceMonitorUpdateGoldBuy;
	return m_pkServiceList[E_SERVICE_MONITOR_UPDATE_GOLD_BUY]->init(&kDataInit);
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
