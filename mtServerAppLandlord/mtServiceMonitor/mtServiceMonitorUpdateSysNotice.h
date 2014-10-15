#ifndef		__MT_SERVICE_HALL_UPDATE_SYS_NOTICE_H
#define		__MT_SERVICE_HALL_UPDATE_SYS_NOTICE_H
#include "mtPlatform.h"
#include "mtServiceMonitor.h"

class mtServiceMonitorUpdateSysNotice : public mtServiceMonitor
{

public:
	struct DataRead
	{
		long 							lStructBytes;			              /// 包大小
		long                        	lServiceType;			              /// 服务类型
		long 							plReservation[2];		              /// 保留字段
		long                            lEffectStyle;                         /// 公告的生效方式(0 -第二天生效，1 -立即)
		char                            pcSystemNotice[MT_SYSTEM_NOTICE_LEN];         
	};

	struct DataInit
	{
		long							lStructBytes;
	};

public:
	mtServiceMonitorUpdateSysNotice();
	virtual ~mtServiceMonitorUpdateSysNotice();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
};

#endif	///	__MT_SERVICE_HALL_UPDATE_SYS_NOTICE_H