#ifndef 	__MT_SERVICE_HALL_GOLD_BUY_H
#define 	__MT_SERVICE_HALL_GOLD_BUY_H
#include "mtService.h"
#include "mtQueueHall.h"
/// wap收费，购买金币服务
class 	mtServiceHallGoldBuy : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long							lPoint;					/// 
		long 							lMoney;	                /// 支付费用
		long 							lGold;	                /// 金币购买
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 购买道具结果(>= 1成功，0 -失败，-1 -用户账号错误，-2 -用户不在线)
		long 							lGold;	                /// 金币购买
		long                            lMash;                  ///赠送的饲料
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};
public:
	mtServiceHallGoldBuy();
	virtual ~mtServiceHallGoldBuy();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	long    givingMash(long  lUserId,long lMoney);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv*		                 m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_HALL_GOLD_BUY_H

