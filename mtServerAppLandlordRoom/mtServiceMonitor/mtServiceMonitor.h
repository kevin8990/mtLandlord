#ifndef		__MT_SERVICE_MONITOR_H
#define		__MT_SERVICE_MONITOR_H
#include "mtPlatform.h"

class mtServiceMonitor
{
public:
	struct DataRun
	{
		long						lStructBytes;
		long                        lServiceType;			/// 服务类型
		SOCKET		                iSocket;
	};

	mtServiceMonitor();
	virtual ~mtServiceMonitor();

	virtual int init(void* pData) = 0;
	virtual int	run(DataRun* pkDataRun) = 0;
	virtual int exit() = 0;

};

#endif	///	__MT_SERVICE_MONITOR_H
