#ifndef 	__MT_SERVICE_SYSTEM_MESSAGE_H
#define 	__MT_SERVICE_SYSTEM_MESSAGE_H
#include "mtService.h"
#include "mtQueueHall.h"

class 	mtServiceSystemMessage : public mtService
{
public:
	struct DataRead
	{
		long				lStructBytes;		/// 包大小
		long            	lServiceType;		/// 服务类型
		long				plReservation[2];	/// 保留字段
		long				lUserId;                /// 用户id
	};

	struct DataWrite
	{
		long				           lStructBytes;		/// 包大小
		long				           lServiceType;		/// 服务类型
		long				           plReservation[2];	/// 保留字段
		long                           lResult;
		long				           llength;             /// 系统消息的数组长度
		char				           pcSystemNotice[256];	/// 系统公告
		mtQueueHall::MessageInfo       pkMessageInfo; 
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
	mtServiceSystemMessage();
	virtual ~mtServiceSystemMessage();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void  * getUserNodeAddr(long lUserId);
	void parseMessage(char * message);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_SYSTEM_MESSAGE_H

