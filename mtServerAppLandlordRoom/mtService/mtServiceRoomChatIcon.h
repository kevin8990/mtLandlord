#ifndef 	__MT_SERVICE_ROOM_CHAT_ICON_H
#define 	__MT_SERVICE_ROOM_CHAT_ICON_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 表情聊天服务
class 	mtServiceRoomChatIcon : public mtService
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
		long                            lIconIdx;               /// 表情的id
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 发起聊天的用户id
		long                            lIconIdx;               /// 表情的id
	};
public:
	mtServiceRoomChatIcon();
	virtual ~mtServiceRoomChatIcon();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
};

#endif 	/// __MT_SERVICE_ROOM_CHAT_ICON_H

