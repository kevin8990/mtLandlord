#ifndef		__MT_QUEUE_MGR_H
#define		__MT_QUEUE_MGR_H
#include "mtQueueList.h"
#include "mtQueueHall.h"

class mtQueuePacket;
class mtSQLEnv;
class mtQueueMgr 
{
public:
	struct DataInit
	{
		long	    lStructBytes;
		int		    iSpinCount;
		char*       pcServiceExeDir;
		mtSQLEnv *  pkSQLEnv;
	};
public:
	mtQueueMgr();
	~mtQueueMgr();

	int		init(void* pData);
	int		exit();

	mtQueuePacket*	getQueuePacket();

	mtQueuePacket*	m_pkQueuePacket;
	mtQueueHall*    m_pkHall;
	char*           m_pcServiceExeDir;
};

inline mtQueuePacket* mtQueueMgr::getQueuePacket()
{
	return	m_pkQueuePacket;
}

#endif /// __MT_QUEUE_MGR_H