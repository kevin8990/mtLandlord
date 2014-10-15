#ifndef		__MT_THREAD_WORK_H
#define		__MT_THREAD_WORK_H
#include "mtThread.h"
#include "mtQueueList.h"
#include "mtQueueUser.h"
#include "mtServiceMgr.h"


class	mtThreadWork : public mtThread
{
public:
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION__BEGINTHREAD,
		EXCEPTION_END
	};

	struct	DataInit
	{
		long				    lStructBytes;
		HANDLE					hIOCP;
		mtQueueMgr*             pkQueueMgr;
		mtQueueUser*			pkQueueUser;
		mtQueuePacket*			pkQueuePacket;
		mtServiceMgr*           pkServiceMgr;
	};
public:
	mtThreadWork();
	~mtThreadWork();

	//////////////////////////////////
	void printfInfo();
	//////////////////////////////////

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();
	int     exitQueueUserDataNode(mtQueueUser::DataNode* pkQueueUserDataNode);
	int     playServiceRoomExit(mtQueueUser::DataNode* pkQueueUserDataNode);
	int		escape();
	HANDLE	getThread();
	
	ULONGLONG				m_ullTimeEnterRun;

	HANDLE					m_hThread;	
	HANDLE					m_hIOCP;
	mtQueueMgr*             m_pkQueueMgr;
	mtQueueUser*			m_pkQueueUser;
	mtQueuePacket*			m_pkQueuePacket;

	mtService::DataRun		m_kServiceMgrDataRun;
	mtServiceMgr*           m_pkServiceMgr;
};

inline int mtThreadWork::escape() 
{
	return	static_cast<int>(GetTickCount() - m_ullTimeEnterRun);
}

inline HANDLE mtThreadWork::getThread() 
{
	return	m_hThread;
}


#endif	///	__MT_THREAD_WORK_H