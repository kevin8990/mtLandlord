#ifndef 	__MT_SERVICE_HALL_FEED_BACK_H
#define 	__MT_SERVICE_HALL_FEED_BACK_H

#include "mtService.h"
#include "mtQueueHall.h"

/// 意见反馈服务
class 	mtServiceHallFeedBack : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			                    /// 包大小
		long                        	lServiceType;			                    /// 服务类型
		long 							plReservation[2];		                    /// 保留字段
		long                            lUserId;                                    /// 用户id
		char							pcContentFeedBack[MT_SYSTEM_FEED_BACK_LEN]; /// 用户反馈的信息
	};
	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;     			/// 用户反馈信息结果(>= 1成功，0 -失败，-1 -用户账号错误，-2 -用户不在线) 
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*		pkSQLEnv;
	};

public:
	mtServiceHallFeedBack();
	virtual ~mtServiceHallFeedBack();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息

	mtSQLEnv*		m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_HALL_GIVE_GOLD_H

