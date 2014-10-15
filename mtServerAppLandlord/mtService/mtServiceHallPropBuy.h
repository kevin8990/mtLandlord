#ifndef 	__MT_SERVICE_HALL_PROP_BUY_H
#define 	__MT_SERVICE_HALL_PROP_BUY_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 购买道具服务
class 	mtServiceHallPropBuy : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long                            lPropIdx;               /// 道具索引
		long							lPropPrice;				/// 道具的价格，(旧版本花掉的金币)（新版本为元）
		long 							lPropBuyCount;	        /// 购买道具数量
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 购买道具结果
		long                            lusetime;               ///剩余使用小时
		long                            lPropIdx;               /// 道具索引                           
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
	mtServiceHallPropBuy();
	virtual ~mtServiceHallPropBuy();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	bool  IsVip(long lUserId);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv*		m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_HALL_PROP_BUY_H

