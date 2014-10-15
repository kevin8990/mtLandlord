#ifndef		__MT_THREAD_H
#define		__MT_THREAD_H
#include "mtPlatform.h"
#include "mtException.h"

class	mtThread : public mtException
{
public:
	enum EControlMode
	{
		E_CONTROL_MODE_BEG	= 0,					
		E_CONTROL_MODE_NORMAL	= E_CONTROL_MODE_BEG,	/// 正常模式
		E_CONTROL_MODE_DEBUG,							/// 调试模式
		E_CONTROL_MODE_END
	};
	enum EThreadState
	{
		E_THREAD_STATE_BEG	= 0,
		E_THREAD_STATE_RUN	= E_THREAD_STATE_BEG,		/// 运行状态
		E_THREAD_STATE_STOP,							/// 停止状态，线程已经结束
		E_THREAD_STATE_PAUSE,							/// 暂停状态
		E_THREAD_STATE_TERMINATE,						/// 终止状态，可能处于停止过程中
		E_THREAD_STATE_END
	};
public:
	mtThread();
	virtual ~mtThread();

	virtual	int	run()		= 0;
	virtual int exception() = 0;

	EControlMode	getControlMode();
	EThreadState	getThreadState();

	int				setControlMode(EControlMode eControlMode);
	int				setThreadState(EThreadState eThreadState);

	static void	CallBackRun(void* pData);
	
protected:
	EControlMode			m_eControlMode;
	EThreadState			m_eThreadState;
};

inline mtThread::EControlMode mtThread::getControlMode() 
{
	return	m_eControlMode;
}

inline mtThread::EThreadState	mtThread::getThreadState()
{
	return	m_eThreadState;
}

inline int mtThread::setControlMode(EControlMode eControlMode) 
{
	if (eControlMode > E_CONTROL_MODE_BEG && eControlMode < E_CONTROL_MODE_END)
	{
		m_eControlMode	= eControlMode;
		return	1;
	}

	return	0;
}

inline int mtThread::setThreadState(EThreadState eThreadState)
{
	if (eThreadState > E_THREAD_STATE_BEG && eThreadState < E_THREAD_STATE_END)
	{
		m_eThreadState	= eThreadState;
		return	1;
	}

	return	0;

}

#endif	///	__MT_THREAD_H