#include "mtThread.h"

#define MT_THREAD_STATE_ERCORD(_eThreadState)	printf ("%s\t%p\t%s\n", MT_GET_NAME(_eThreadState), 	pkThread, typeid(*pkThread).name())

mtThread::mtThread() :
	m_eControlMode(E_CONTROL_MODE_NORMAL),
	m_eThreadState(E_THREAD_STATE_STOP)
{

}

void mtThread::CallBackRun( void* pData )
{
	mtThread*	pkThread = (mtThread*)pData;

	while (1)
	{
		SleepEx(5, TRUE);
MT_LABEL_CALLBACK_RUN_PAUSE:
		if (E_CONTROL_MODE_DEBUG == pkThread->getControlMode())
		{
			switch (pkThread->getThreadState())
			{
			case E_THREAD_STATE_RUN:
				{
					MT_THREAD_STATE_ERCORD(MT_GET_NAME(:E_THREAD_STATE_RUN));
				}
				break;
			case E_THREAD_STATE_PAUSE:
				{
					MT_THREAD_STATE_ERCORD(MT_GET_NAME(E_THREAD_STATE_PAUSE));
					goto	MT_LABEL_CALLBACK_RUN_PAUSE;
				}
				break;
			case E_THREAD_STATE_TERMINATE:
				{
					MT_THREAD_STATE_ERCORD(MT_GET_NAME(E_THREAD_STATE_TERMINATE));
					goto	MT_LABEL_CALLBACK_RUN_TERMINATE;
				}
				break;
			default:
				break;
			}
		}

		((mtThread*)pData)->run();
	}

MT_LABEL_CALLBACK_RUN_TERMINATE:
	pkThread->setThreadState(E_THREAD_STATE_STOP);
}

mtThread::~mtThread()
{

}
