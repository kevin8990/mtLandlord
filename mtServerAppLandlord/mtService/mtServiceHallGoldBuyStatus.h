#ifndef 	__MT_SERVICE_HALL_GOLD_BUY_STATUS_H
#define 	__MT_SERVICE_HALL_GOLD_BUY_STATUS_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 新的游戏基地收费结构查询服务
class 	mtServiceHallGoldBuyStatus : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 查询充值状态结果(0 -失败，1 -充值状态已更新，-1 -用户账号错误，-2 -充值状态未更新，-3 -用户离线，-4 -用户充值失败)
		long                            lUserId;                /// 用户id
		long 							lGold;	                /// 充值的金币(只有在lResult等于1时才有效)
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
	};
public:
	mtServiceHallGoldBuyStatus();
	virtual ~mtServiceHallGoldBuyStatus();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	void*   getUserNodeAddr(long lUserId);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
};

#endif 	/// __MT_SERVICE_HALL_GOLD_BUY_STATUS_H

