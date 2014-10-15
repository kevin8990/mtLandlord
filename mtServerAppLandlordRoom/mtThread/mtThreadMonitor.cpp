#include "mtThreadMonitor.h"

mtThreadMonitor::mtThreadMonitor() :
	m_lThreadTotalWork(0),
	m_lThreadTotalRoom(0),
	m_pkThreadRom(NULL),
	m_pkThreadWork(NULL),
	m_pkThreadRomDataInit(NULL),
	m_pkThreadWorkDataInit(NULL),
	m_hThread(INVALID_HANDLE_VALUE)
{
}

mtThreadMonitor::~mtThreadMonitor()
{

}

int mtThreadMonitor::run()
{
//	long	lRomIdx;
// 	for (lRomIdx = 0; lRomIdx < m_lThreadTotalRoom; lRomIdx++)
// 	{
// 		if (m_pkThreadRom[lRomIdx].escape() > MT_THREAD_WORK_TIME_VALID)
// 		{
// 			m_pkThreadRom[lRomIdx].exit();
// 			m_pkThreadRom[lRomIdx].init(m_pkThreadRomDataInit);
// 		}
// 	}
// 
// 	for (lRomIdx = 0; lRomIdx < m_lThreadTotalWork; lRomIdx++)
// 	{
// 		if (m_pkThreadWork[lRomIdx].escape() > MT_THREAD_WORK_TIME_VALID)
// 		{
// 			m_pkThreadWork[lRomIdx].exit();
// 			m_pkThreadWork[lRomIdx].init(m_pkThreadWorkDataInit);
// 		}
// 	}

///	Sleep(MT_THREAD_WORK_TIME_VALID);

	return	1;
}

int mtThreadMonitor::exception()
{

	return	1;
}

int mtThreadMonitor::init( DataInit* pkInitData )
{
	m_lThreadTotalWork    		= pkInitData->lThreadTotalWork;
	m_lThreadTotalRoom          = pkInitData->lThreadTotalRoom;
	m_pkThreadRom				= pkInitData->pkThreadRom;
	m_pkThreadRomDataInit		= pkInitData->pkThreadRomDataInit;
	m_pkThreadWork				= pkInitData->pkThreadWork;
	m_pkThreadWorkDataInit		= pkInitData->pkThreadWorkDataInit;

	/// 创建线程
	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__beginthread);	

	m_eThreadState			= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadMonitor::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}

	m_lThreadTotalWork    	= 0;
	m_lThreadTotalRoom      = 0;

	m_pkThreadWorkDataInit	= NULL;
	m_pkThreadWork			= NULL;
	m_pkThreadRomDataInit	= NULL;
	m_pkThreadRom			= NULL;
	
	return	1;
}

