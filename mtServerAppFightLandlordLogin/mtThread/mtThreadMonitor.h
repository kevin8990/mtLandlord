#pragma once

#include "mtPlatform.h"
#include "mtThread.h"
#include "mtThreadWork.h"
#include "mtProtocolMonitorMgr.h"

class	mtThreadMgr;


class mtThreadMonitor : public mtThread
{
public:
	enum 
	{
		E_EXCEPTION_BEG = 0,
		E_EXCEPTION_IOCP_CREATE,
		E_EXCEPTION_END
	};
public:
	/*struct DataUser 
	{
		OVERLAPPED			kOverlapped;
		EOverLappedType		eOverlappedType;

		SOCKET				uiSocket;
		WSABUF				kWSABufRecv;
		char				pcDataRecv[1024];
		WSABUF				kWSABufSend;
		char				pcDataSend[1024];
	};*/

	struct DataInit
	{
		long				lStructBytes;
 		HANDLE				hIOCP;
 		SOCKET				uiSocketServer;
		LPFN_ACCEPTEX		lpfnAcceptEx;
		LPFN_DISCONNECTEX	lpfnDisconnectEx;

 		unsigned long		ulTimeWaitMilliseconds;
		int					iTimeConnectLimitSencods;

 		//mtThreadWork::DataUser*			pkUserList;
 		//int					iUserTotal;
 		
		mtThreadMgr*		pkThreadMgr;
	};

public:
	mtThreadMonitor();
	~mtThreadMonitor();

	int		init(DataInit* pkDataInit);
	int		exit();

	virtual int exception();

	int		run();

private:
 	DataInit					m_kDataInit;
 	static	HANDLE				ms_hIOCP;
 	static	SOCKET				ms_uiSocketServer;
 	static	LPFN_ACCEPTEX		ms_lpfnAcceptEx;
 	static	LPFN_DISCONNECTEX	ms_lpfnDisconnectEx;

	mtThreadMgr*				m_pkThreadMgr;
	unsigned long				m_ulTimeWaitMilliseconds;
	int							m_iTimeConnectLimitSencods;

	mtThreadWork::DataUser*		m_pkThreadWorkUserList;
	int							m_iThreadWorkUserTotal;
	mtThreadWork*				m_pkThreadWorkList;
	int							m_iThreadWorkTotal;

	DWORD						m_dwLocalAddressLength;
	DWORD						m_dwRemoteAddressLength;
	DWORD						m_dwReceiveDataLength;
 
	mtProcotolMonitorMgr::DataInit		m_kProcotolMgrDataInit;
	mtProcotolMonitorMgr		m_kProcotolMgr;
 	
};
