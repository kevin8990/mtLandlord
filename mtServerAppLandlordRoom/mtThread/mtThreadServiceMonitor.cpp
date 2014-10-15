#include "mtThreadServiceMonitor.h"
#include "mtServiceMonitor.h"
#include "mtServiceMonitorMgr.h"

mtThreadServiceMonitor::mtThreadServiceMonitor():
	m_hThread(INVALID_HANDLE_VALUE)
{
}

mtThreadServiceMonitor::~mtThreadServiceMonitor()
{

}

int mtThreadServiceMonitor::run()
{
	/// 每隔5分钟上报一次房间信息(当前房间在线人数)
    Sleep(MT_THREAD_SERVICE_MONITOR_TIME);

	/// 连接大厅服务器
	SOCKET	iClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	MT_EXCEPTION_DEBUG (INVALID_SOCKET == iClientSocket, EXCEPTION_CLIENT_socket);


	SOCKADDR_IN	kClientSockAddrIn;
	memset (&kClientSockAddrIn, 0, sizeof(kClientSockAddrIn));
	kClientSockAddrIn.sin_family		= AF_INET;
	kClientSockAddrIn.sin_port			= g_lHallMonitorServerPort;  /// 26659
	kClientSockAddrIn.sin_addr.s_addr	= g_ulHallMonitorServerIP;   /// 192.168.1.28 - 0x1c01a8c0;//180.168.138.218 - 0xDA8AA8B4 

	if (0 != connect(iClientSocket, (SOCKADDR*)&kClientSockAddrIn, sizeof(kClientSockAddrIn)))
	{
		shutdown(iClientSocket, 2);
		closesocket(iClientSocket);
		return 0;
	}

	/// 上报房间信息给大厅
	mtServiceMonitor::DataRun kDataRun;
	kDataRun.lStructBytes    = sizeof(kDataRun);
	kDataRun.iSocket         = iClientSocket;
	kDataRun.lServiceType    = mtServiceMonitorMgr::E_SERVICE_MONITOR_UPDATE_ROOM;
	m_pkServiceMonitorMgr->run(&kDataRun);

	Sleep(3000);
	shutdown(iClientSocket, 1);
	closesocket(iClientSocket);

	return	1;
}

int mtThreadServiceMonitor::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION__beginthread)
		MT_EXCETPION_STRING(EXCEPTION_CLIENT_socket)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		break;
	}

	return	1;
}

int mtThreadServiceMonitor::init( DataInit* pkInitData )
{
	m_pkServiceMonitorMgr     = pkInitData->pkServiceMonitorMgr;

	/// 创建线程
	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__beginthread);	

	m_eThreadState			= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadServiceMonitor::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}
	
	return	1;
}

