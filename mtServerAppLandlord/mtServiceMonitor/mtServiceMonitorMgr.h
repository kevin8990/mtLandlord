#ifndef		__MT_SERVICE_MONITOR_MGR_H
#define		__MT_SERVICE_MONITOR_MGR_H
#include "mtPlatform.h"
#include "mtQueueHall.h"
#include "mtQueueWork.h"

class mtServiceMonitor;
class mtServiceMonitorMgr
{
public:
	enum EServiceId
	{
		E_SERVICE_MONITOR_BEG	              = 0,
		E_SERVICE_MONITOR_UPDATE_ROOM         = E_SERVICE_MONITOR_BEG,  /// 房间信息定时更新
		E_SERVICE_MONITOR_UPDATE_SYS_NOTICE,                            /// 房间公告定时更新
		E_SERVICE_MONITOR_UPDATE_GOLD_BUY,                              /// 用户充值后，收费服务器通知给游戏服务器
		E_SERVICE_MONITOR_END
	};

	struct DataHead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
	};

	struct DataInit
	{
		long		             lStructBytes;
		mtQueueHall::DataHall*   pkDataHall;                    /// 场信息
		///mtQueueHall::DataSpace*  pkSpace;   	               /// 场信息
	};

	struct DataRun
	{
		long		     lStructBytes;
		mtQueueWorkNode* pQueueWorkNode;
	};

public:
	mtServiceMonitorMgr();
	virtual ~mtServiceMonitorMgr();

	int			init(DataInit* pkDataInit);
	virtual int	run(void* pData);
	virtual int exit();

	int	runReadDataHead(SOCKET socket, DataHead* pkDataHead);
private:
	int			initService();
	int			initServiceMonitorUpdateRoom();
	int			initServiceMonitorUpdateSysNotice();
	int			initServiceMonitorUpdateGoldBuy();

	mtQueueHall::DataHall*       m_pkDataHall;   	                       /// 场信息
	mtServiceMonitor*		     m_pkServiceList[E_SERVICE_MONITOR_END];

};

#endif	///	__MT_SERVICE_MONITOR_MGR_H
