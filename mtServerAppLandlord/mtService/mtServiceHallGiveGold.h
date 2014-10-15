#ifndef 	__MT_SERVICE_HALL_GIVE_GOLD_H
#define 	__MT_SERVICE_HALL_GIVE_GOLD_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 每天中赠送金币服务(一天赠送几次，和联系天登录、完成任务等赠送不同)
class 	mtServiceHallGiveGold : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			    /// 包大小
		long                        	lServiceType;			    /// 服务类型
		long 							plReservation[2];		    /// 保留字段
		long                            lUserId;                    /// 用户id
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lGiveGoldFrequence;     /// 赠送分次数(>= 1成功，0 -失败，-1 -用户账号错误，-2 -用户不在线)
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};

public:
	mtServiceHallGiveGold();
	virtual ~mtServiceHallGiveGold();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv*		                 m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_HALL_GIVE_GOLD_H

