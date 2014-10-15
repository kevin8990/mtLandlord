#include "mtThreadMonitor.h"

mtThreadMonitor::mtThreadMonitor() :
	m_pkThreadWork(NULL),
	m_iThreadWorkTotal(0),
	m_uiThread(0),
	m_pkThreadWorkInitData(NULL)
{

}

mtThreadMonitor::~mtThreadMonitor()
{

}

int mtThreadMonitor::run()
{
	//Sleep(1000*60);
	tm  kNow;
	__time64_t kNowTime;

	time(&kNowTime);
	_localtime64_s(&kNow, &kNowTime);

	if(0 == kNow.tm_min || 30 == kNow.tm_min)//整点或半点时间保存在线人数到数据库
	{
       m_pkSQLEnv->saveOnlinePerson(m_pkDataHall);
	   Sleep(1000 * 60 * 30);
	}
	else
	{
		Sleep(1000 * 60 * 1);
	}
	

	int		iThreadWorkIdx;

	/*for (iThreadWorkIdx = 0; iThreadWorkIdx < m_iThreadWorkTotal; iThreadWorkIdx++)
	{
		if (m_pkThreadWork[iThreadWorkIdx].escape() > MT_THREAD_WORK_TIME_VALID)
		{
			printf("\nmtThreadMonitor::run [iThreadWorkIdx:%d][Time:%d]",iThreadWorkIdx
				,m_pkThreadWork[iThreadWorkIdx].escape());
			//m_pkThreadWork[iThreadWorkIdx].exit();
			//m_pkThreadWork[iThreadWorkIdx].init(m_pkThreadWorkInitData);
		}

		Sleep(100);
		MT_DEBUG(4, "%s\t%p\t%p\n", typeid(*this).name(), this, m_pkThreadWork+iThreadWorkIdx);
	}*/


	for (iThreadWorkIdx = 0; iThreadWorkIdx < m_iThreadWorkTotal; iThreadWorkIdx++)
	{
		if(iThreadWorkIdx%4==0)
		{
			printf("\n");
		}
		MT_DEBUG_COLOR(0,FOREGROUND_GREEN,"[%d][%d-%d]\t",iThreadWorkIdx,m_pkThreadWork[iThreadWorkIdx].escape()/1000,m_pkThreadWork[iThreadWorkIdx].escape()%1000);
	}

	MT_DEBUG_COLOR(PTCL,FOREGROUND_GREEN,"\nm_pkQueueWorkList [count:%d]\n",m_pkThreadWork->getUserNum());

	
	return	1;
}

int mtThreadMonitor::exception()
{

	return	1;
}

int mtThreadMonitor::init( DataInit* pkInitData )
{
	m_pkThreadWork		    = pkInitData->pkThreadWork;
	m_iThreadWorkTotal	    = pkInitData->iThreadWorkTotal;
	m_pkThreadWorkInitData	= pkInitData->pkThreadWorkInitData;
	m_pkDataHall            = pkInitData->pkDataHall;
	m_pkSQLEnv              = pkInitData->pkSQLEnv;
	/// 创建线程
	m_uiThread	= _beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (-1L == m_uiThread, EXCEPTION__beginthread);	

	m_eThreadState			= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadMonitor::exit()
{
	m_iThreadWorkTotal	= 0;
	m_pkThreadWork		= NULL;
	m_pkThreadWorkInitData	= NULL;
	return	1;
}

