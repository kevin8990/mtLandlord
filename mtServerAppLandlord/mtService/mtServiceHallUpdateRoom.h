#ifndef		__MT_SERVICE_HALL_UPDATE_ROOM_H
#define		__MT_SERVICE_HALL_UPDATE_ROOM_H
#include "mtQueueWork.h"
#include "mtService.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"

class mtServiceHallUpdateRoom	: public mtService
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long							lBatchId;				/// 批次号id
		long							lVendorId;				/// 厂商代码
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		mtQueueHall::DataSpace          pkSpace[2];   			/// 场信息
		long				            llength;                /// 系统消息的数组长度
		char				            pcSystemNotice[256];	/// 系统公告
		mtQueueHall::MessageInfo    	pkMessageInfo; 		    /// 系统消息

	};

	struct DataInit
	{
		long							lStructBytes;
		mtQueueHall *                   pkQueueHall;   			         /// 场信息
		mtSQLEnv*						pkSQLEnv;
	};

public:
	mtServiceHallUpdateRoom();
	virtual ~mtServiceHallUpdateRoom();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void*   getUserNodeAddr(long lUserId);
	void mtServiceHallUpdateRoom::parseMessage(char * message);
public:
	mtQueueHall   * m_pkQueueHall;   	    /// 大厅信息
	mtSQLEnv      * m_pkSQLEnv;

};

#endif	///	__MT_SERVICE_HALL_UPDATE_ROOM_H