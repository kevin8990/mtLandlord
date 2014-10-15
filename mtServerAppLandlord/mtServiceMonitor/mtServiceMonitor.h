#ifndef		__MT_SERVICE_MONITOR_H
#define		__MT_SERVICE_MONITOR_H

class mtServiceMonitor
{
public:
	mtServiceMonitor();
	virtual ~mtServiceMonitor();

	virtual int init(void* pData) = 0;
	virtual int	run(void* pData) = 0;
	virtual int exit() = 0;

};

#endif	///	__MT_SERVICE_MONITOR_H
