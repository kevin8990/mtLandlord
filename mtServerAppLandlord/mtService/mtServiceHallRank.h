#ifndef 	__MT_SERVICE_HALL_RANK_H
#define 	__MT_SERVICE_HALL_RANK_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 排行榜服务
class 	mtServiceHallRank : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		mtQueueHall::DataRank           kRank[10];              /// 用户排名信息
	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
	};

public:
	mtServiceHallRank();
	virtual ~mtServiceHallRank();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
};

#endif 	/// __MT_SERVICE_HALL_RANK_H

