#ifndef		__MT_THREAD_MANAGER_AI_H
#define		__MT_THREAD_MANAGER_AI_H
#include "mtThread.h"
#include "mtQueueMgr.h"
#include "mtServiceMgr.h"

class	mtThreadManagerAi : public mtThread
{
public:
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION__beginthread,
		EXCEPTION_END
	};

	struct DataInit
	{
		long				  lStructBytes;
		HANDLE	              hIOCP;
		HANDLE				  hEvent;
		mtQueueMgr*           pkQueueMgr;
		mtServiceMgr*	      pkServiceMgr;
	};
public:
	mtThreadManagerAi();
	~mtThreadManagerAi();

	int		init(DataInit*	pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	HANDLE				m_hThread;
	HANDLE	            m_hIOCP;
	HANDLE				m_hEvent;
	mtQueueMgr*         m_pkQueueMgr;
	mtServiceMgr*	    m_pkServiceMgr;
};

#endif	///	__MT_THREAD_MANAGER_AI_H