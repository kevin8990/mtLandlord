#ifndef 	__MT_SERVICE_ROOM_CHAT_SHORT_CUT_H
#define 	__MT_SERVICE_ROOM_CHAT_SHORT_CUT_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 快捷语聊天服务
class 	mtServiceRoomChatShortCut : public mtService
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
		long                            lShortCutIdx;           /// 快捷语的id
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 发起聊天的用户id
		long                            lShortCutIdx;           /// 快捷语的id
	};
public:
	mtServiceRoomChatShortCut();
	virtual ~mtServiceRoomChatShortCut();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
};

#endif 	/// __MT_SERVICE_ROOM_CHAT_SHORT_CUT_H

