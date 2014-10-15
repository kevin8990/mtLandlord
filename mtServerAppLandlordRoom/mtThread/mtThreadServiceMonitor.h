#ifndef		__MT_THREAD_SERVICE_MONITOR_H
#define		__MT_THREAD_SERVICE_MONITOR_H
#include "mtThread.h"
#include "mtThreadWork.h"

class mtServiceMonitorMgr;
class	mtThreadServiceMonitor : public mtThread
{
public:
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION__beginthread,
		EXCEPTION_CLIENT_socket,
		EXCEPTION_END
	};

	struct DataInit
	{
		long						lStructBytes;
		mtServiceMonitorMgr*        pkServiceMonitorMgr;
	};
public:
	mtThreadServiceMonitor();
	~mtThreadServiceMonitor();

	int		init(DataInit*	pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	HANDLE						m_hThread;
	mtServiceMonitorMgr*        m_pkServiceMonitorMgr;
};

#endif	///	__MT_THREAD_SERVICE_MONITOR_H
