#ifndef 	__MT_SERVICE_LOTTERY_GET_TASK_AWARD_H
#define 	__MT_SERVICE_LOTTERY_GET_TASK_AWARD_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 领取任务奖励协议
class 	mtServiceGetTaskAward : public mtService
{
public:
	struct DataRead
	{
		long		lStructBytes;		// 包大小
		long        lServiceType;	    // 服务类型
		long		plReservation[2];	// 保留字段
		long		lUserId;            // 用户id
		long		ltaskId;		    // 领取奖励的任务ID
	};


	struct DataWrite
	{
		long		lStructBytes;		// 包大小
		long		lServiceType;		// 服务类型
		long		plReservation[2];	// 保留字段
		long		lResult; 
		long        ltaskId;             //任务id
		long		lGold;			    // 奖励金币(目前金币) 
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};
	enum TaskType
	{
		E_TASK_TYPE_DAILY_TASK            = 1,
		E_TASK_TYPE_TOTAL_FIGHT            = 2,
		E_TASK_TYPE_TOTAL_WIN              = 3,
		E_TASK_TYPE_TOTAL_PAY              = 4,
	};
	enum Result
	{
		E_RESULT_SUCCESS                    = 1,//成功
		E_RESULT_ERROR                      = 0,//错误
		E_RESULT_MISS_DEMAND                = -1,//未达到要求
		E_RESULT_HAVE_GET                   = -2,//已经领取
	};
public:
	mtServiceGetTaskAward();
	virtual ~mtServiceGetTaskAward();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void  * getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	bool  IsVip(long lUserId);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall; // 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_LOTTERY_GET_TASK_AWARD_H

