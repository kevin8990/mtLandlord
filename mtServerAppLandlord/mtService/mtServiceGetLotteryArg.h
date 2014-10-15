#ifndef 	__MT_SERVICE_LOTTERY_GETLOTTERYARG_H
#define 	__MT_SERVICE_LOTTERY_GETLOTTERYARG_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 抽奖连接协议
class 	mtServiceGetLotteryArg : public mtService
{
public:
	struct DataRead
	{
		long		lStructBytes;			// 包大小
		long        lServiceType;			// 服务类型
		long		plReservation[2];		// 保留字段
		long		lUserId;                // 用户id
	};

	struct DataWrite
	{
		long		lStructBytes;		// 包大小
		long		lServiceType;		// 服务类型
		long		plReservation[2];	// 保留字段
		long        lResult;			// 0：失败，服务器配置错误 1：成功
		long		lNeedGold;	        // 抽奖金币
		long		lUseTimes;			// 可用次数
		long		lMaxTimes;			// 最大抽奖次数
		long		lLuck;				// 幸运值
		long        lFeeCard;           // 奖劵
	};
	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};
public:
	mtServiceGetLotteryArg();
	virtual ~mtServiceGetLotteryArg();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void * getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	void   calcSurplusLotteryFrequency(mtQueueHall::LotteryInfo * pkLotteryInfo,long lMaxTimes,long lVipTimes,bool bVip);
	bool   IsVip(long lUserId);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_LOTTERY_GETLOTTERYARG_H

