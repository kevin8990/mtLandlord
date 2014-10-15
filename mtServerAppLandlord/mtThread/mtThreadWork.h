#ifndef		__MT_THREAD_WORK_H
#define		__MT_THREAD_WORK_H
#include "mtThread.h"
#include "mtQueueWork.h"
//#include "mtServiceMgr.h"

///#include "mtSQLEnv.h"
#include "mtServiceMgr.h"

#define		READ_BUFF_BYTES					100
#define		WRITE_BUFF_BYTES				100	
#define		HALL_SERVER_IP					0xDA8AA8B4			/// 180.168.138.218
#define		HALL_SERVER_PORT				0x2364				/// 9060-9069
#define		CHARGE_SERVER_IP				0xDA8AA8B4			/// 180.168.138.218
#define		CHARGE_SERVER_PORT				0x2364				/// 9060-9069

#define		SEND_TIMES						3


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
		long				lStructBytes;
		mtQueueWorkList*	pkQueueWorkList;
		mtServiceMgr*		pkServiceMgr;
	};

	struct	DataRun 
	{
		long					lStructBytes;
		char					pcDataRead[MT_BYTES_THREAD_WORK_DATA_READ];
		long					lDataReadBytes;
		char					pcDataWrite[MT_BYTES_THREAD_WORK_DATA_WRITE];
		long					lDataWriteBytes;
	};

public:
	mtThreadWork();
	~mtThreadWork();

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	int		runRead(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	
	int		escape();
	HANDLE	getThread();
	int		dealWithLoginSocket(SOCKET socketClient);

	//////////////////////////////////////////////////
	int		getUserNum();

private:
	mtQueueWorkList*		m_pkQueueWorkList;
	HANDLE					m_hThread;
	ULONGLONG				m_ullTimeEnterRun;
	mtQueueWorkNode			m_kQueueWorkNodeClient;

	struct timeval		    m_kTimeVal;
	fd_set                  m_kReadSet;

	mtServiceMgr*			m_pkServiceMgr;

};

inline int mtThreadWork::getUserNum(){
	return m_pkQueueWorkList->m_iNodeNumUsed;
}

inline int mtThreadWork::escape() 
{
	return	static_cast<int>(GetTickCount() - m_ullTimeEnterRun);
}

inline HANDLE mtThreadWork::getThread() 
{
	return	m_hThread;
}


#endif	///	__MT_THREAD_WORK_H