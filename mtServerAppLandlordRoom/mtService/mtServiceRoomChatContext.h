#ifndef 	__MT_SERVICE_ROOM_CHAT_CONTEXT_H
#define 	__MT_SERVICE_ROOM_CHAT_CONTEXT_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 输入内容聊天服务
class 	mtServiceRoomChatContext : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		mtQueueUser*	        pkQueueUser;
		mtQueuePacket*	        pkQueuePacket;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		char                            pcContext[32];          /// 输入的聊天内容
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 发起聊天的用户id
		char                            pcContext[32];          /// 输入的聊天内容
	};
public:
	mtServiceRoomChatContext();
	virtual ~mtServiceRoomChatContext();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
};

#endif 	/// __MT_SERVICE_ROOM_CHAT_CONTEXT_H

