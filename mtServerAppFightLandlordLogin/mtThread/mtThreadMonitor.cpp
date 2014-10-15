#include "mtThreadMonitor.h"
#include "mtThreadMgr.h"

#include "mtGlobal.h"

HANDLE	mtThreadMonitor::ms_hIOCP			= NULL;
SOCKET	mtThreadMonitor::ms_uiSocketServer	= INVALID_SOCKET;
LPFN_ACCEPTEX		mtThreadMonitor::ms_lpfnAcceptEx			= NULL;
LPFN_DISCONNECTEX	mtThreadMonitor::ms_lpfnDisconnectEx		= NULL;

mtThreadMonitor::mtThreadMonitor() :
	m_pkThreadMgr(NULL),
	m_ulTimeWaitMilliseconds(0),
	m_iTimeConnectLimitSencods(0),
	m_pkThreadWorkUserList(NULL),
	m_iThreadWorkUserTotal(0),
	m_pkThreadWorkList(NULL),
	m_iThreadWorkTotal(0),
	m_dwLocalAddressLength(0),
	m_dwRemoteAddressLength(0),
	m_dwReceiveDataLength(0)
{

}

mtThreadMonitor::~mtThreadMonitor()
{

}

int mtThreadMonitor::init( DataInit* pkDataInit )
{
	/// 初始化赋值
 	memcpy (&m_kDataInit, pkDataInit, sizeof(m_kDataInit));
	ms_hIOCP					= pkDataInit->hIOCP;
	ms_uiSocketServer			= pkDataInit->uiSocketServer;
	ms_lpfnAcceptEx				= pkDataInit->lpfnAcceptEx;
	ms_lpfnDisconnectEx			= pkDataInit->lpfnDisconnectEx;
	m_pkThreadMgr				= pkDataInit->pkThreadMgr;
	m_ulTimeWaitMilliseconds	= pkDataInit->ulTimeWaitMilliseconds;
	m_iTimeConnectLimitSencods	= pkDataInit->iTimeConnectLimitSencods;

	/// 内部获取赋值
	m_pkThreadWorkUserList		= m_pkThreadMgr->m_pkThreadWorkUserList;
	m_iThreadWorkUserTotal		= m_pkThreadMgr->m_iThreadWorkUserTotal;
	m_pkThreadWorkList			= m_pkThreadMgr->m_pkThreadWorkList;
	m_iThreadWorkTotal			= m_pkThreadMgr->m_iThreadWorkTotal;

	/// 初始化 AcceptEx 参数
	m_dwLocalAddressLength		= sizeof(SOCKADDR_IN) + 16;
	m_dwRemoteAddressLength		= sizeof(SOCKADDR_IN) + 16;
	m_dwReceiveDataLength		= sizeof(m_pkThreadWorkUserList->pcDataSend) 
		- m_dwLocalAddressLength - m_dwRemoteAddressLength;
  
	/// 初始化 Monitor 协议管理器
 	memset (&m_kProcotolMgrDataInit, 0, sizeof(m_kProcotolMgrDataInit));
	m_kProcotolMgrDataInit.lStructBytes		= sizeof(m_kProcotolMgrDataInit);
	m_kProcotolMgr.init(&m_kProcotolMgrDataInit);
 
// 	m_pkThreadMgr				= pkDataInit->pkThreadMgr;
// 
// // 	m_kDataInit.lStructBytes	= sizeof(m_kDataInit);
// // 	m_kDataInit.hIOCP			= m_hIOCP;
// // 	m_kDataInit.uiSocketServer  = m_uiSocketServer;
// // 	m_kDataInit.ulTimeWaitMilliseconds		= m_ulTimeWaitMilliseconds;
// // 	m_kDataInit.iTimeConnectLimitSencods	= m_iTimeConnectLimitSencods;
// // 	m_kDataInit.pkUserList		= m_pkThreadWorkUserList;
// // 	m_kDataInit.iUserTotal		= m_iThreadWorkUserTotal;
// // 	m_kDataInit.lpfnAcceptEx	= m_lpfnAcceptEx;
// // 	m_kDataInit.lpfnDisconnectEx			= m_lpfnDisconnectEx;
// // 	m_kDataInit.pkThreadMgr		= m_pkThreadMgr;
// 
// 	m_kDataInit.lStructBytes	= sizeof(m_kDataInit);
// 	m_kDataInit.pkThreadMgr		= m_pkThreadMgr;
// 
// 	m_kServiceMgr.init(&m_kDataInit);

	return	1;
}

int mtThreadMonitor::exit()
{
	printf ("Thread monitor 开始关闭\n");



	return	1;

}
 
int mtThreadMonitor::run()
{
	//Sleep(INFINITE);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (int iUserIdx = 0; iUserIdx < m_iThreadWorkUserTotal; iUserIdx++)
	{
		DWORD seconds;
		int len = sizeof(seconds);
		mtThreadWork::DataUser* pkDataUser = (mtThreadWork::DataUser*)(m_pkThreadWorkUserList+iUserIdx);
		int res = getsockopt(pkDataUser->uiSocket,SOL_SOCKET,SO_CONNECT_TIME,(char*)&seconds,&len);
		//处理连接不发数据的超时socket
		if(res == 0 && (seconds>=MT_SERVER_SOCKET_ACCPET_TIMEOUT && seconds<0xFFFFFFFF))
		{					
			
			MT_DEBUG(PTCL, "\ngetsockopt [res:%d] [seconds:%d] !\n",res,seconds);
			pkDataUser->closeSocketMilliSec = 0;
			pkDataUser->closeFlag = FALSE;
			int iRes = closesocket(pkDataUser->uiSocket);
			//pkDataUser->closeSocketMilliSec = 0;
			//pkDataUser->closeFlag = FALSE;
			MT_DEBUG(PTCL, "\nDELETE ERROR CLIENT [Socket:%d] [res:%d] [error:%d] !\n",pkDataUser->uiSocket,iRes,WSAGetLastError());
										//处理投递DisconnectEx函数，超时socket
		}
		else if(pkDataUser->closeSocketMilliSec != 0 
			&& (GetTickCount()-(pkDataUser->closeSocketMilliSec)) >= MT_SERVER_SOCKET_DISCONNECT_TIMEOUT)
		{

			DWORD sec = GetTickCount()-(pkDataUser->closeSocketMilliSec);

			pkDataUser->closeSocketMilliSec = 0;
			pkDataUser->closeFlag = FALSE;
			int iRes = closesocket(pkDataUser->uiSocket);
			//pkDataUser->closeSocketMilliSec = 0;
			//pkDataUser->closeFlag = FALSE;
			MT_DEBUG(PTCL, "\nDisconnectEx Timeout [Socket:%d] [seconds:%d] [res:%d] [error:%d] !\n",
				pkDataUser->uiSocket,sec,iRes,WSAGetLastError());
			
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return	0;
}
 
int mtThreadMonitor::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_IOCP_CREATE)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		return	0;
	}

	return	1;
}



// #endif