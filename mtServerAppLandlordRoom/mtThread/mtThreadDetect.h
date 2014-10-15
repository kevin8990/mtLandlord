#ifndef __MT_THREAD_DETECT_H
#define __MT_THREAD_DETECT_H

#include "mtThread.h"
#include "mtQueueList.h"
#include "mtQueueUser.h"
#include "mtQueueMgr.h"

class mtThreadDetect :public mtThread
{
public:
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION__BEGINTHREAD,
		EXCEPTION_END
	};

	struct DataInit
	{
		long				lStructBytes;		/// 结构体的大小
		mtQueueMgr*         pkQueueMgr;
		mtQueueUser*		pkQueueUser;
		mtQueuePacket*		pkQueuePacket;
	};

	mtThreadDetect();
	~mtThreadDetect();


	int		init(DataInit* pkInitData);
	int		exit();

	virtual int	run();
	virtual int exception();

	HANDLE				m_hThread;	
	mtQueueMgr *        m_pkQueueMgr;
	mtQueueUser*		m_pkQueueUser;
	mtQueuePacket*		m_pkQueuePacket;
};

#endif