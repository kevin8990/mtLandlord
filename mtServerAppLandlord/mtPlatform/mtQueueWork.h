#ifndef		__MT_QUEUE_WORK_H
#define		__MT_QUEUE_WORK_H

#include "mtPlatform.h"
#include "mtDataStructQueue.h"

struct	mtQueueWorkNode
{
	SOCKET		kSocketClient;				//客户端套接字
	DWORD		timeSec;					//时间(毫秒)
	BOOL		bIsAccept;					//连接或接收数据的标志
};

typedef	mtDataStructQueue<mtQueueWorkNode, MT_QUEUE_WORK_SIZE>	mtQueueWorkList;

#endif	/// __MT_QUEUE_WORK_H