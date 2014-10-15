 #include "mtThreadMgr.h"

mtThreadMgr::mtThreadMgr() :
	m_lThreadTotalWork(0),
	m_lThreadTotalRoom(0),
	m_pkThreadConnect(NULL),
	m_pkThreadRoom(NULL),
	m_pkThreadWork(NULL),
	m_pkThreadMonitor(NULL),
	m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL),
	m_pkServiceMgr(NULL),
	m_pkQueueMgr(NULL),
	m_hEventManagerAi(NULL)
{
	memset (&m_kWSAData, 0, sizeof(m_kWSAData));
	//memset (&m_kThreadConnect, 0, sizeof(m_kThreadConnect));
	memset (&m_kThreadWorkDataInit, 0, sizeof(m_kThreadWorkDataInit));
	//memset (&m_kThreadMonitor, 0, sizeof(m_kThreadMonitor));
	
}

mtThreadMgr::~mtThreadMgr()
{

}

int mtThreadMgr::init(DataInit* pkDataInit)
{
	/// 初始化网络模块动态链接库
	MT_EXCEPTION_DEBUG(NO_ERROR != WSAStartup(MAKEWORD(2,2), &m_kWSAData), EXCEPTION_WSAStartup);

	m_lThreadTotalWork              = pkDataInit->lThreadTotalWork;
	m_lThreadTotalRoom				= pkDataInit->lThreadTotalRoom;
	m_hIOCP                         = pkDataInit->hIOCP;
	m_hEventManagerAi               = pkDataInit->hEventManagerAi;
	m_pkServiceMgr                  = pkDataInit->pkServiceMgr;
	m_pkServiceMonitorMgr           = pkDataInit->pkServiceMonitorMgr;
	m_pkQueueMgr                    = pkDataInit->pkQueueMgr;

	m_pkQueueUser                   = m_pkQueueMgr->getQueueUser();
	m_pkQueuePacket                 = m_pkQueueMgr->getQueuePacket();


	/// 线程部分
	/// 连接线程
	m_pkThreadConnect   = new mtThreadConnect();
	m_pkThreadConnect->init(&pkDataInit->kThreadConnectDataInit);

	/// 工作线程
	m_pkThreadWork			               = new mtThreadWork[m_lThreadTotalWork];
	m_kThreadWorkDataInit.lStructBytes     = sizeof(m_kThreadWorkDataInit);
	m_kThreadWorkDataInit.hIOCP            = pkDataInit->hIOCP;
	m_kThreadWorkDataInit.pkQueueMgr       = m_pkQueueMgr;
	m_kThreadWorkDataInit.pkQueueUser      = m_pkQueueUser;
	m_kThreadWorkDataInit.pkQueuePacket    = m_pkQueuePacket;
	m_kThreadWorkDataInit.pkServiceMgr     = pkDataInit->pkServiceMgr;

	int iThreadWorkdIdx;
	for (iThreadWorkdIdx = 0; iThreadWorkdIdx < m_lThreadTotalWork; iThreadWorkdIdx++)
	{
		m_pkThreadWork[iThreadWorkdIdx].init(&m_kThreadWorkDataInit);
	}

	/// 房间线程
	Sleep(2000);
	m_pkThreadRoom			            = new mtThreadRoom[m_lThreadTotalRoom];
	m_kThreadRoomDataInit.lStructBytes  = sizeof(m_kThreadRoomDataInit);
	m_kThreadRoomDataInit.pkQueueMgr    = m_pkQueueMgr;
	m_kThreadRoomDataInit.pkServiceMgr  = m_pkServiceMgr;

	int		lThreadRoomIdx = 0;
	int lSpaceIdx;
	for (lSpaceIdx = 0; lSpaceIdx < m_pkQueueMgr->getQueueHall()->m_lSpaceTotal; lSpaceIdx++)
	{	//printf("\nm_lSpaceTotal:%d",m_pkQueueMgr->getQueueHall()->m_lSpaceTotal);
		int lRoomIdx;
		for (lRoomIdx = 0; lRoomIdx < m_pkQueueMgr->getQueueHall()->getSpace(lSpaceIdx)->lRomTotal; lRoomIdx++)
		{
			//printf(" lRomTotal:%d",m_pkQueueMgr->getQueueHall()->getSpace(lSpaceIdx)->lRomTotal);
			m_kThreadRoomDataInit.pkQueueHallDataRoom = m_pkQueueMgr->getQueueHall()->getRoom(lSpaceIdx, lRoomIdx);
			m_kThreadRoomDataInit.pkQueueHallDataRoom->lOnlinePersonNumber = 0;
			m_pkThreadRoom[lThreadRoomIdx++].init(&m_kThreadRoomDataInit);
		}
	}

	/// 定时更新房间信息到大厅线程
	m_kThreadServiceMonitorDataInit.lStructBytes	        = sizeof(m_kThreadServiceMonitorDataInit);
	m_kThreadServiceMonitorDataInit.pkServiceMonitorMgr	    = m_pkServiceMonitorMgr;
	m_pkThreadServiceMonitor                                = new mtThreadServiceMonitor();
	m_pkThreadServiceMonitor->init(&m_kThreadServiceMonitorDataInit);

	/// AI信息管理线程
	m_kThreadManagerAiDataInit.lStructBytes	           = sizeof(m_kThreadManagerAiDataInit);
	m_kThreadManagerAiDataInit.hIOCP                   = m_hIOCP;
	m_kThreadManagerAiDataInit.hEvent                  = m_hEventManagerAi;
	m_kThreadManagerAiDataInit.pkQueueMgr              = m_pkQueueMgr;
	m_kThreadManagerAiDataInit.pkServiceMgr            = m_pkServiceMgr;
	m_pkThreadManagerAi                                = new mtThreadManagerAi();
	m_pkThreadManagerAi->init(&m_kThreadManagerAiDataInit);

 	/// 监控线程
	mtThreadMonitor::DataInit	kDataInitThreadMonitor;
	kDataInitThreadMonitor.lStructBytes	                  = sizeof(kDataInitThreadMonitor);
	kDataInitThreadMonitor.lThreadTotalRoom	              = m_lThreadTotalRoom;
	kDataInitThreadMonitor.lThreadTotalWork               = m_lThreadTotalWork;
	kDataInitThreadMonitor.pkThreadWork	                  = m_pkThreadWork;
	kDataInitThreadMonitor.pkThreadWorkDataInit	          = &m_kThreadWorkDataInit;
	kDataInitThreadMonitor.pkThreadRom                    = m_pkThreadRoom;
	kDataInitThreadMonitor.pkThreadServiceMonitor         = m_pkThreadServiceMonitor;
	kDataInitThreadMonitor.pkThreadServiceMonitorDataInit = &m_kThreadServiceMonitorDataInit;
	//kDataInitThreadMonitor.pkThreadRomDataInit  = &m_kThreadRoomDataInit;
	m_pkThreadMonitor = new mtThreadMonitor();
 	m_pkThreadMonitor->init(&kDataInitThreadMonitor);

	///检测线程（测试）
	m_pkThreadDetectDataInit.lStructBytes     =  sizeof(m_pkThreadDetectDataInit);
	m_pkThreadDetectDataInit.pkQueueMgr       = m_pkQueueMgr;
	m_pkThreadDetectDataInit.pkQueuePacket    =  m_pkQueueMgr->getQueuePacket();
	m_pkThreadDetectDataInit.pkQueueUser      =  m_pkQueueMgr->getQueueUser();

	m_pkThreadDetect = new mtThreadDetect();
	m_pkThreadDetect->init(&m_pkThreadDetectDataInit);


	return	1;
}

int mtThreadMgr::exit()
{
	
 	m_pkThreadMonitor->exit();
	delete m_pkThreadMonitor;
	m_pkThreadMonitor = NULL;

 	m_pkThreadConnect->exit();
	delete m_pkThreadConnect;
	m_pkThreadConnect = NULL;

	if (NULL != m_pkThreadWork)
	{
		int		iThreadWorkIdx;
		for (iThreadWorkIdx = 0; iThreadWorkIdx < m_lThreadTotalWork; iThreadWorkIdx++)
		{
			m_pkThreadWork[iThreadWorkIdx].exit();
		}

		delete [] m_pkThreadWork;
		m_pkThreadWork =  NULL;
	}

	if (NULL != m_pkThreadRoom)
	{
		long	lThreadRoomIdx;
		for (lThreadRoomIdx = 0; lThreadRoomIdx < m_lThreadTotalRoom; lThreadRoomIdx++)
		{
			m_pkThreadRoom[lThreadRoomIdx].exit();
		}

		delete [] m_pkThreadRoom;
		m_pkThreadRoom =  NULL;
	}

	if (NULL != m_pkThreadServiceMonitor)
	{
		m_pkThreadServiceMonitor->exit();
		delete m_pkThreadServiceMonitor;
		m_pkThreadServiceMonitor = NULL;
	}

	if (NULL != m_pkThreadManagerAi)
	{
		m_pkThreadManagerAi->exit();
		delete m_pkThreadManagerAi;
		m_pkThreadManagerAi = NULL;
	}

	if (NULL != m_hEventManagerAi)
	{
		CloseHandle(m_hEventManagerAi);
		m_hEventManagerAi = NULL;
	}  

	if (0 != m_kWSAData.wVersion)
	{
		WSACleanup();
		m_kWSAData.wVersion	= 0;
	}

	return	1;
}

int mtThreadMgr::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION_WSAStartup)
		MT_EXCETPION_STRING(EXCEPTION_THREAD_LISTEN_NEW)
		MT_EXCETPION_STRING(EXCEPTION_THREAD_WORK_NEW)
	default:
		MT_DEBUG(7, "该异常没有被处理");
		return	0;
	}

	return	1;
}

int mtThreadMgr::run()
{
	return	1;
}

