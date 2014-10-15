#ifndef		__MT_THREAD_MONITOR_H
#define		__MT_THREAD_MONITOR_H
#include "mtThread.h"
#include "mtThreadWork.h"
//#include "mtThreadListen.h"

class	mtThreadMonitor : public mtThread
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
		long			            lStructBytes;
		mtThreadWork           *	pkThreadWork;
		int				            iThreadWorkTotal;
		mtQueueHall::DataHall  *    pkDataHall;
		mtThreadWork::DataInit *	pkThreadWorkInitData;
		mtSQLEnv               *    pkSQLEnv;
	};
public:
	mtThreadMonitor();
	~mtThreadMonitor();

	int		init(DataInit*	pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	mtThreadWork           *	    m_pkThreadWork;
	int					            m_iThreadWorkTotal;
	uintptr_t				        m_uiThread;
	mtQueueHall::DataHall  *        m_pkDataHall;
	mtThreadWork::DataInit *		m_pkThreadWorkInitData;
	mtSQLEnv               *        m_pkSQLEnv;
};

#endif	///	__MT_THREAD_MONITOR_H