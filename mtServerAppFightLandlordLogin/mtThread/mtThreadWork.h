#ifndef		__MT_THREAD_WORK_H
#define		__MT_THREAD_WORK_H
#include "mtPlatform.h"
#include "mtThread.h"

class	mtProtocol;
class	mtProtocolMgr;
class	mtSQLEnv;

class mtThreadWork : public mtThread
{
public:
	enum 
	{
		E_EXCEPTION_BEG = 0,
		E_EXCEPTION_CREATETHREAD,
		E_EXCEPTION_CREATEFILE_MGR,
		E_EXCEPTION_CREATESERVCIE_MGR,
		E_EXCEPTION_CREATE_EVENT,
		E_EXCEPTION_END
	};

	struct DataUser 
	{
		OVERLAPPED			kOverlapped;
		EOverLappedType		eOverlappedType;

		SOCKET				uiSocket;
		WSABUF				kWSABufRecv;
		char				pcDataRecv[1024];
		WSABUF				kWSABufSend;
		char				pcDataSend[1024];
		DWORD				closeSocketMilliSec;								//投递ms_lpfnDisconnectEx函数时的毫秒数
		BOOL				closeFlag;
		char*               pcAccount;
		unsigned long		ulId;
		mtThreadWork*		pkThreadWork;
	};

	struct DataInit
	{
		long				lStructBytes;
		HANDLE				hIOCP;
		SOCKET				uiSocketServer;
		unsigned long		ulTimeWaitMilliseconds;
		unsigned long		ulIdBeg;
		int					iTimeConnectLimitSencods;
		DataUser*			pkUserList;
		mtSQLEnv*			pkSQLEnv;
		int					iUserTotal;
		int					iThreadIdx;
		int					iThreadTotal;
		char*				pcFolder;
		CRITICAL_SECTION*	pkHallMaxIdCs;
		LPFN_ACCEPTEX		lpfnAcceptEx;
		LPFN_DISCONNECTEX	lpfnDisconnectEx;
	};

public:
	mtThreadWork();
	~mtThreadWork();
	
	int		init(DataInit* pkDataInit);
	int		exit();
	int		run();

	EThreadState		getThreadState();
	int					setThreadState(EThreadState	eThreadState, unsigned long ulMilliseconds);

	mtProtocolMgr*		getProtocolMgr();

	virtual int exception();
	static	DWORD WINAPI Run(LPVOID pData);

protected:
	static	int		ResetSocket(DataUser* pkDataUser);
	static	int		RunWSASend(DataUser* pkDataUser);

private:
	static	int		CompleteAcceptEx(DataUser* pkDataUser, mtThreadWork* pkThreadWork);
	static	int		CompleteWSARecv(DataUser* pkDataUser, mtThreadWork* pkThreadWork);
	static	int		CompleteWSASend(DataUser* pkDataUser, mtThreadWork* pkThreadWork);
	static	int		CompleteWSADisconnectEx(DataUser* pkDataUser, mtThreadWork* pkThreadWork);

	int		(*m_pkCompleteFunList[E_OLT_END])(DataUser* pkDataUser, mtThreadWork* pkThreadWork);
private:
	DataInit				m_kDataInit;

	static	HANDLE			ms_hIOCP;
	static	SOCKET			ms_uiSocketServer;
	static	LPFN_ACCEPTEX		ms_lpfnAcceptEx;
	static	LPFN_DISCONNECTEX	ms_lpfnDisconnectEx;



	DataUser*				m_pkUserList;
	int						m_iUserTotal;

	unsigned long			m_ulTimeWaitMilliseconds;
	int						m_iTimeConnectLimitSencods;
	
	static	DWORD			ms_dwLocalAddressLength;
	static	DWORD			ms_dwRemoteAddressLength;
	static	DWORD			ms_dwReceiveDataLength;

	int						m_iThreadIdx;
	int						m_iThreadTotal;
	EThreadState			m_eThreadState;
	HANDLE					m_hThread;
	HANDLE					m_hEvent;
	
	mtProtocolMgr*			m_pkProtocolMgr;
	CRITICAL_SECTION*	    m_pkHallMaxIdCs;

	OVERLAPPED_ENTRY		m_pkOverlappedEntryList[MAXIMUM_WAIT_OBJECTS];
	OVERLAPPED_ENTRY*		m_pkOverlappedEntry;

	unsigned long			m_ulOverlappedEntryTotal;
	unsigned long			m_ulOverlappedEntryCount;
	unsigned long			m_ulOverlappedEntryIdx;


	unsigned long			m_ulTransmitBytes;					//接收数据大小

	DataUser*				m_pkDataUser;
};

inline mtThread::EThreadState mtThreadWork::getThreadState()
{
	return				m_eThreadState;
}

inline mtProtocolMgr* mtThreadWork::getProtocolMgr()
{
	return m_pkProtocolMgr;
}
#endif	///	__MT_THREAD_WORK_H