#ifndef		__MT_SERVICE_HALL_UPDATE_GOLD_BUY_H
#define		__MT_SERVICE_HALL_UPDATE_GOLD_BUY_H
#include "mtQueueWork.h"
#include "mtServiceMonitor.h"
#include "mtQueueHall.h"

class mtServiceMonitorUpdateGoldBuy : public mtServiceMonitor
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 用户用户购买金币结果(0 -失败，1 -成功)
		char                            pcKey[24];              /// key值
		long                            lUserId;                /// 用户id
		long                            lGoldBuy;               /// 购买的金币数(>0,其他值都是错误)
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 通知购买金币结果(0 -错误，1 -成功， -1 -用户id错误， -2, -购买金币数错误)
		char                            pcKey[24];              /// key值
		long                            lUserId;                /// 用户id
		long                            lGoldBuy;               /// 购买的金币数(>0,其他值都是错误)
		
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall::DataHall*          pkDataHall;   	        /// 场信息
		//mtQueueHall::DataSpace*         pkSpace;   			    /// 场信息
	};

public:
	mtServiceMonitorUpdateGoldBuy();
	virtual ~mtServiceMonitorUpdateGoldBuy();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void* getUserNodeAddr(long lUserId);
public:
	mtQueueHall::DataHall*       m_pkDataHall;   	            /// 场信息

};

#endif	///	__MT_SERVICE_HALL_UPDATE_GOLD_BUY_H