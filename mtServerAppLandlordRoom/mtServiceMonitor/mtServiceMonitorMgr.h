#ifndef		__MT_SERVICE_MONITOR_MGR_H
#define		__MT_SERVICE_MONITOR_MGR_H

#include "mtServiceMonitor.h"
#include "mtPlatform.h"
#include "mtQueueMgr.h"

class mtServiceMonitorMgr
{
public:
	enum EServiceId
	{
		E_SERVICE_MONITOR_BEG	= 0,
		E_SERVICE_MONITOR_UPDATE_ROOM  = E_SERVICE_MONITOR_BEG,  /// 房间信息定时更新
		E_SERVICE_MONITOR_END
	};

	struct	DataHead
	{
		long	lStructBytes;
		long	lServiceType;
		long	plReservation[2];
	};

	struct DataInit
	{
		long		            lStructBytes;
        mtQueueMgr*             pkQueueMgr;
	};

public:
	mtServiceMonitorMgr();
	virtual ~mtServiceMonitorMgr();

	int				init(DataInit* pkDataInit);
	virtual int		run(mtServiceMonitor::DataRun* pkDataRun);
	virtual int		exit();
	int	runReadDataHead(SOCKET socket, DataHead* pkDataHead);
private:
	int			initService();
	int			initServiceMonitorUpdateRoom();

private:
	mtServiceMonitor*		     m_pkServiceMonitorList[E_SERVICE_MONITOR_END];
	mtQueueMgr*                  m_pkQueueMgr;
};

#endif	///	__MT_SERVICE_MONITOR_MGR_H