#ifndef		__MT_THREAD_MONITOR_H
#define		__MT_THREAD_MONITOR_H
#include "mtThread.h"
#include "mtThreadWork.h"
#include "mtThreadConnect.h"
#include "mtThreadRoom.h"
#include "mtThreadServiceMonitor.h"

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
		long						      lStructBytes;
		long						      lThreadTotalWork;
		long						      lThreadTotalRoom;

		mtThreadRoom*				      pkThreadRom;
		mtThreadRoom::DataInit*		      pkThreadRomDataInit;

		mtThreadServiceMonitor::DataInit* pkThreadServiceMonitorDataInit;
		mtThreadServiceMonitor*           pkThreadServiceMonitor;

		mtThreadWork*				      pkThreadWork;		
		mtThreadWork::DataInit*		      pkThreadWorkDataInit;

		
	};
public:
	mtThreadMonitor();
	~mtThreadMonitor();

	int		init(DataInit*	pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	long					          m_lThreadTotalWork;
	long					          m_lThreadTotalRoom;

	mtThreadRoom*				      m_pkThreadRom;
	mtThreadRoom::DataInit*		      m_pkThreadRomDataInit;

	mtThreadServiceMonitor::DataInit  m_kThreadServiceMonitorDataInit;
	mtThreadServiceMonitor*           m_pkThreadServiceMonitor;

	mtThreadWork*				      m_pkThreadWork;
	mtThreadWork::DataInit*		      m_pkThreadWorkDataInit;


	HANDLE						      m_hThread;
};

#endif	///	__MT_THREAD_MONITOR_H