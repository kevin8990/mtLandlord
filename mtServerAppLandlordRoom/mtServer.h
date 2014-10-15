#ifndef		__MT_SERVER_H
#define		__MT_SERVER_H

#include "mtThreadMgr.h"
#include "mtServiceMonitorMgr.h"
class mtServer
{
public:
	mtServer(void);
	~mtServer(void);

	int		init();
	int		exit();

	int     initParams();
	int     initQueue();
	int		initService();
	int		initThread();

private:
	SYSTEM_INFO				           m_kSystemInfo;
	HANDLE			                   m_hIOCP;
	mtQueueMgr                         m_kQueueMgr;
	mtQueueMgr::DataInit               m_kQueueMgrDataInit;

	mtServiceMgr			           m_kServiceMgr;
	mtServiceMgr::DataInit	           m_kServiceMgrDataInit;

	mtServiceMonitorMgr		           m_kServiceMonitorMgr;
	mtServiceMonitorMgr::DataInit	   m_kServiceMonitorMgrDataInit;

	mtThreadMgr::DataInit	           m_kThreadMgrDataInit;
	mtThreadMgr				           m_kThreadMgr;

	HANDLE					           m_hEventManagerAi;

};

#endif ///	__MT_SERVER_H