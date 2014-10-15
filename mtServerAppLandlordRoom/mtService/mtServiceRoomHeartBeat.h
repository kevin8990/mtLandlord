#ifndef 	__MT_SERVICE_ROOM_HEART_BEAT_H
#define 	__MT_SERVICE_ROOM_HEART_BEAT_H
#include "mtService.h"
#include "mtQueueMgr.h"

/// 用户心跳服务(每个真实用户客户端，都要每隔20秒钟发送一次心跳查询服务)
class 	mtServiceRoomHeartBeat : public mtService
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
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 网络可能有问题的用户id(0 -没有用户超时，0 < -超时用户的id)
	};
public:
	mtServiceRoomHeartBeat();
	virtual ~mtServiceRoomHeartBeat();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	mtQueueUser*	m_pkQueueUser;
	mtQueuePacket*	m_pkQueuePacket;
};

#endif 	/// __MT_SERVICE_ROOM_CHAT_ICON_H

