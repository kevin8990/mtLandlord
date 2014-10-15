#ifndef		__MT_THREAD_MGR_H
#define		__MT_THREAD_MGR_H

#include "mtDataStructQueue.h"
#include "mtQueueWork.h"
#include "mtThreadConnect.h"
#include "mtThreadWork.h"
#include "mtThreadMonitor.h"
#include "mtThreadMonitorConnect.h"
#include "mtServiceMonitorMgr.h"

class mtSQLEnv;

class	mtThreadMgr : public mtException
{
public:
	enum  
	{
		EXCEPTION_BEG	= 0,
		EXCEPTION_WSAStartup,
		EXCEPTION_QUEUE_WORK_NEW,
		EXCEPTION_THREAD_LISTEN_NEW,
		EXCEPTION_THREAD_WORK_NEW,
		EXCEPTION_THREAD_MONITOR_NEW,

		EXCEPTION_END
	};

	struct DataInit
	{
		long				      lStructBytes;
		mtServiceMgr*		      pkServiceMgr;
		mtServiceMonitorMgr*      pkServiceMonitorMgr;
		mtQueueHall::DataHall*    pkDataHall;   	         /// 场信息
		/// mtQueueHall::DataSpace*   pkSpace;   	         /// 场信息

		mtSQLEnv*		pkSQLEnv;
		
		int					      iThreadWorkTotal;		
		mtThreadConnect::DataInit kDataInitListen;
	};
public:
	mtThreadMgr();
	~mtThreadMgr();

	int		init(DataInit* pkDataInit);
	int		exit();
	int		run();

	virtual int exception();


	WSADATA					m_kWSAData;

	
	int						m_iThreadWorkTotal;
	mtThreadWork::DataInit	m_kThreadWorkInitData;

	mtQueueWorkList		    m_kQueueWorkList;
	mtThreadConnect			m_kThreadListen;
	mtThreadMonitor		    m_kThreadMonitor;
	mtThreadWork*			m_pkThreadWork;
	mtThreadMonitorConnect  m_kThreadMonitorConnect;
	mtServiceMgr*			m_pkServiceMgr;
	mtServiceMonitorMgr*    m_pkServiceMonitorMgr;
	mtQueueHall::DataHall*  m_pkDataHall;   	         /// 场信息
	/// mtQueueHall::DataSpace* m_pkSpace;   	         /// 场信息

};

#endif ///	__MT_THREAD_MGR_H