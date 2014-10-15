#ifndef 	__MT_SERVICE_MONITOR_UPDATE_ROOM_H
#define 	__MT_SERVICE_MONITOR_UPDATE_ROOM_H
#include "mtServiceMonitor.h"
#include "mtQueueMgr.h"

/// 房间上报信息给大厅服务
class 	mtServiceMonitorUpdateRoom : public mtServiceMonitor
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueHall*            pkQueueHall;
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            plSpaceRoom[2][6];      /// 房间在线人数信息
	};
public:
	mtServiceMonitorUpdateRoom();
	virtual ~mtServiceMonitorUpdateRoom();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int	runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

    mtQueueHall*             m_pkQueueHall;
};

#endif 	/// __MT_SERVICE_MONITOR_UPDATE_ROOM_H

