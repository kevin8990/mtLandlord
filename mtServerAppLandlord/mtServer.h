#ifndef			__MT_SERVER_H_
#define			__MT_SERVER_H_
#include "mtSQLEnv.h"
#include "mtThreadMgr.h"
//#include <sysinfoapi.h>

class mtServer
{
public:
	mtServer();
	~mtServer();

public:
	int		init();
	int		exit();

	int		initDataBase();
	int     initQueue();
	int		initService();
	int		initThread();

	int     initParams();
	
	SYSTEM_INFO				        m_kSystemInfo;
	mtQueueMgr                      m_kQueueMgr;
	mtQueueMgr::DataInit            m_kQueueMgrDataInit;

	mtSQLEnv						m_kSQLEnv;

	mtServiceMgr			        m_kServiceMgr;
	mtServiceMgr::DataInit	        m_kServiceMgrDataInit;

	mtServiceMonitorMgr	            m_kServiceMonitorMgr;
	mtServiceMonitorMgr::DataInit	m_kServiceMonitorMgrDataInit;

	mtThreadMgr				        m_kThreadMgr;
	mtThreadMgr::DataInit	        m_kThreadMgrDataInit;
};


#endif		/// __MT_SERVER_H_