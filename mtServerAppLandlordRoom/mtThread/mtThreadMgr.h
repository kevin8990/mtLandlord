#ifndef		__MT_THREAD_MGR_H
#define		__MT_THREAD_MGR_H

#include "mtDSQueue.h"
#include "mtThreadConnect.h"
#include "mtThreadWork.h"
#include "mtThreadMonitor.h"
#include "mtThreadRoom.h"
#include "mtQueueMgr.h"
#include "mtThreadServiceMonitor.h"
#include "mtThreadManagerAI.h"
#include "mtThreadDetect.h"

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
		long						lStructBytes;
		long						lThreadTotalWork;
		long						lThreadTotalRoom;
		HANDLE					    hIOCP;
		HANDLE					    hEventManagerAi;
		mtServiceMgr*               pkServiceMgr;
		mtServiceMonitorMgr*        pkServiceMonitorMgr;
		mtQueueMgr*                 pkQueueMgr;
		mtThreadConnect::DataInit	kThreadConnectDataInit;
	};

public:
	mtThreadMgr();
	~mtThreadMgr();

	int		init(DataInit* pkDataInit);
	int		exit();
	int		run();

private:
	virtual int exception();

	WSADATA					          m_kWSAData;
	long					          m_lThreadTotalWork;
	long					          m_lThreadTotalRoom;

	/// 线程部分
	mtThreadConnect*                  m_pkThreadConnect;
	mtThreadRoom*			          m_pkThreadRoom;
	mtThreadRoom::DataInit            m_kThreadRoomDataInit;
	mtThreadWork*			          m_pkThreadWork;
	mtThreadWork::DataInit	          m_kThreadWorkDataInit;
	mtThreadMonitor*		          m_pkThreadMonitor;
	mtThreadServiceMonitor::DataInit  m_kThreadServiceMonitorDataInit;
	mtThreadServiceMonitor*           m_pkThreadServiceMonitor;
	mtThreadManagerAi::DataInit       m_kThreadManagerAiDataInit;
	mtThreadManagerAi*                m_pkThreadManagerAi;

	mtThreadDetect *                  m_pkThreadDetect;
	mtThreadDetect::DataInit          m_pkThreadDetectDataInit;

	HANDLE					          m_hIOCP;
	mtQueueUser*			          m_pkQueueUser;
	mtQueuePacket*			          m_pkQueuePacket;

	mtServiceMgr*                     m_pkServiceMgr;
	mtServiceMonitorMgr*              m_pkServiceMonitorMgr;
	mtQueueMgr*                       m_pkQueueMgr;

	HANDLE					          m_hEventManagerAi;
};

#endif ///	__MT_THREAD_MGR_H