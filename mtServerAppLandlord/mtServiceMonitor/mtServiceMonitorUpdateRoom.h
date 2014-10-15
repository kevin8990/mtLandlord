#ifndef		__MT_SERVICE_HALL_UPDATE_ROOM_H
#define		__MT_SERVICE_HALL_UPDATE_ROOM_H
#include "mtQueueWork.h"
#include "mtServiceMonitor.h"
#include "mtQueueHall.h"

class mtServiceMonitorUpdateRoom : public mtServiceMonitor
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            plSpaceRoom[2][6];      /// 房间在线人数信息
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall::DataSpace*         pkSpace;   			         /// 场信息
	};

public:
	mtServiceMonitorUpdateRoom();
	virtual ~mtServiceMonitorUpdateRoom();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void	OutPutPersonNumber();

public:
	mtQueueHall::DataSpace*      m_pkSpace;   	    /// 场信息

};

#endif	///	__MT_SERVICE_HALL_UPDATE_ROOM_H