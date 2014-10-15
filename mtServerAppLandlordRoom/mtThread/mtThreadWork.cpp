#include "mtThreadWork.h"
#include "mtQueueUser.h"
#include "mtServiceRoomExit.h"

mtThreadWork::mtThreadWork() :
	m_hThread(INVALID_HANDLE_VALUE),
	m_ullTimeEnterRun(0)
{

}

mtThreadWork::~mtThreadWork()
{

}

int mtThreadWork::init( DataInit* pkInitData )
{	
	/// 初始化 m_iTimeEnterRun
	m_ullTimeEnterRun		= GetTickCount();

	m_hIOCP                 = pkInitData->hIOCP;
	m_pkQueueMgr            = pkInitData->pkQueueMgr;
	m_pkQueueUser           = pkInitData->pkQueueUser;
	m_pkQueuePacket         = pkInitData->pkQueuePacket;
	m_pkServiceMgr          = pkInitData->pkServiceMgr;

	/// 创建线程
	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__BEGINTHREAD);	

	m_eThreadState	= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadWork::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}

	m_ullTimeEnterRun		= 0;

	m_hIOCP                 = NULL;
	m_pkQueueUser           = NULL;
	m_pkQueuePacket         = NULL;
	m_pkServiceMgr          = NULL;
 
 	return	1;
}

void mtThreadWork::printfInfo(){
	SYSTEMTIME st;
	GetLocalTime(&st);		//得到本地时间/日期
	char buff[256];			//存放时间/日期信息的缓冲区
	sprintf(buff,"%u:%u:%u",st.wHour,st.wMinute,st.wSecond);

	printf("\nStart [D:%d-%d][U:%d-%d][N:%d-%d][%d:%d:%d:%d:%d:%d][%s] ThreadId:%d",
		m_pkQueueMgr->m_pkQueueDesk->getNodeNumRemain(),m_pkQueueMgr->m_pkQueueDesk->getNodeUseCount(),
		m_pkQueueUser->getNodeNumRemain(),m_pkQueueUser->getNodeUseCount(),
		m_pkQueuePacket->getNodeNumRemain(),m_pkQueuePacket->getNodeUseCount(),
	    m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[0].lOnlinePersonNumber,
		m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[1].lOnlinePersonNumber,
		m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[2].lOnlinePersonNumber,
		m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[3].lOnlinePersonNumber,
		m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[4].lOnlinePersonNumber,
		m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[5].lOnlinePersonNumber,buff,GetCurrentThreadId());


	printf("\n[One:%d:%d:%d][Two:%d:%d:%d][Three:%d:%d:%d][Four:%d:%d:%d][Five:%d:%d:%d][Six:%d:%d:%d]"
		,m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueThreeList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,1)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,1)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,1)->kQueueThreeList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,2)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,2)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,2)->kQueueThreeList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,3)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,3)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,3)->kQueueThreeList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,4)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,4)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,4)->kQueueThreeList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,5)->kQueueOneList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,5)->kQueueTwoList.getNodeNumUsed()
		,m_pkQueueMgr->getQueueHall()->getRoom(0,5)->kQueueThreeList.getNodeNumUsed());
}

int mtThreadWork::run()
{
	m_ullTimeEnterRun		                          = GetTickCount();
	BOOL                        bRet                  = FALSE;
	DWORD		                ulTransmitBytes       = 0;
	mtQueueUser::DataNode*		pkQueueUserDataNode   = NULL;
	mtQueuePacket::DataNode*	pkQueuePacketDataNode = NULL;

	printfInfo();
	
	bRet = GetQueuedCompletionStatus(m_hIOCP, &ulTransmitBytes, (PULONG_PTR)&pkQueueUserDataNode,
		(LPOVERLAPPED*)&pkQueuePacketDataNode, INFINITE);
 
	if (0 == ulTransmitBytes)	
	{
		mtServiceMgr::DataHead  * pkDataHead = (mtServiceMgr::DataHead *)pkQueuePacketDataNode->pcData;
		SYSTEMTIME curTime;
		GetLocalTime(&curTime);

		MT_DEBUG_PTCL("\nuser:%d recv data is zero![uid:%d][socket:%d][ServiceType:%d]",
			pkQueueUserDataNode->lUserId,
			pkQueueUserDataNode->uiSockets,
			pkDataHead->lServiceId);

		if (NULL != pkQueuePacketDataNode && NULL != pkQueueUserDataNode)
		{
			if (0 < pkQueueUserDataNode->lUserId && pkQueueUserDataNode->lUserId < MT_QUEUE_USER_ID_AI_START
			 && INVALID_SOCKET != pkQueueUserDataNode->uiSockets)
			{
				/// 删除 pkQueueUserDataNode
				exitQueueUserDataNode(pkQueueUserDataNode);
			}
		}
		else
		{
			MT_DEBUG_PTCL("\nzero error (%d) !!!\n",WSAGetLastError());
		}
	}
	else if (pkQueuePacketDataNode && mtQueuePacket::E_DATA_TYPE_READ == pkQueuePacketDataNode->eDataType) 
	{
		mtServiceMgr::DataHead  * pkDataHead = (mtServiceMgr::DataHead *)pkQueuePacketDataNode->pcData;

		MT_DEBUG_PTCL("\nCompletion recv data![uid:%d][ServiceType:%d] ThreadId:%d",pkQueueUserDataNode->lUserId,pkDataHead->lServiceId,GetCurrentThreadId());
		
		 m_pkQueueUser->removeDataNodePacket(pkQueueUserDataNode, pkQueuePacketDataNode);


		///投递读
		mtQueuePacket::DataNode*	pkDataNodePacket = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("threadwork161");
		if (NULL != pkDataNodePacket)
		{
			m_pkQueueUser->DataRead(pkQueueUserDataNode, pkDataNodePacket);
		}
		/// 游戏代码 读完成
		m_kServiceMgrDataRun.pkQueueUserDataNode   = pkQueueUserDataNode;
		m_kServiceMgrDataRun.pkQueuePacketDataNode = pkQueuePacketDataNode;
		m_pkServiceMgr->run(&m_kServiceMgrDataRun);
		
		m_pkQueuePacket->push(pkQueuePacketDataNode);
	}
	else if (pkQueuePacketDataNode && mtQueuePacket::E_DATA_TYPE_WRITE == pkQueuePacketDataNode->eDataType)	
	{
		mtServiceMgr::DataHead  * pkDataHead = (mtServiceMgr::DataHead *)pkQueuePacketDataNode->pcData;

		MT_DEBUG_PTCL("\nCompletion send data!!! [uid:%d][ServiceType::%d] ThreadId:%d",pkQueueUserDataNode->lUserId,pkDataHead->lServiceId,GetCurrentThreadId());

		m_pkQueueUser->removeDataNodePacket(pkQueueUserDataNode, pkQueuePacketDataNode);
		MT_DEBUG_PTCL("\nCompletion send data!UserId:%d,push:%x",pkQueueUserDataNode->lUserId,pkQueuePacketDataNode);
		m_pkQueuePacket->push(pkQueuePacketDataNode);
		
	}
	else if (MT_QUEUE_COMPLETION_STATUS_CUSTOM == ulTransmitBytes)
	{
		MT_DEBUG_PTCL("\nCustom data! [uid:%d]",pkQueueUserDataNode->lUserId);

		m_pkQueueUser->removeDataNodePacket(pkQueueUserDataNode, pkQueuePacketDataNode);
		/// 游戏代码

		m_kServiceMgrDataRun.pkQueueUserDataNode   = pkQueueUserDataNode;
		m_kServiceMgrDataRun.pkQueuePacketDataNode = pkQueuePacketDataNode;
		m_pkServiceMgr->run(&m_kServiceMgrDataRun);
		
		m_pkQueuePacket->push(pkQueuePacketDataNode);
	}
	else
	{
		MT_DEBUG_PTCL("\nelse data!!!  [bRet:%d][error:%d]",bRet,WSAGetLastError());
	}

	MT_DEBUG_PTCL("\nEnd!Threadid:%d ",GetCurrentThreadId());
 	
	return 0;
}

int mtThreadWork::exitQueueUserDataNode(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	if (-888 == pkQueueUserDataNode->lUserIOCPStatus)
	{
		return 1;
	}

	if(0 == pkQueueUserDataNode->lUserId)
	{
		MT_DEBUG_PTCL("\nError User Exit !!!\n");
		m_pkQueueUser->exitDataNode(pkQueueUserDataNode, m_pkQueuePacket);
		return 1;
	}

	pkQueueUserDataNode->lUserIOCPStatus = -888;
	if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RUNNING == pkQueueUserDataNode->lUserExit
	 || mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_CHANGE  == pkQueueUserDataNode->lUserExit
	 || mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER  == pkQueueUserDataNode->lUserExit)
	{
		long lUserId = pkQueueUserDataNode->lUserId;
		///pkQueueUserDataNode->lUserExit = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_EXITING;

		/// 先把处理桌子上的其他人员，如果其他两人都是ai，则直接释放桌子和其他节点信息
// 		mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkQueueUserDataNode->pkQueueDeskDataNode;
// 		if (NULL != pkQueueDeskDataNode)
// 		{
		if (1024 <= lUserId && lUserId < MT_QUEUE_USER_ID_AI_START)
		{
			MT_DEBUG_PTCL("\nExit QueueUserDataNode start node1 if1!Id:%d\n",lUserId);

			if (pkQueueUserDataNode == m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->pkUserNode)
			{
				/// 先做退出服务
				playServiceRoomExit(pkQueueUserDataNode);
			}
		}
		//}

		if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE != pkQueueUserDataNode->lUserExit)
		{

			MT_DEBUG_PTCL("\nexitQueueUserDataNode start node1 if2 !!!\n");

			pkQueueUserDataNode->lUserExit   = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
			MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 244",pkQueueUserDataNode);
			m_pkQueueUser->exitDataNode(pkQueueUserDataNode, m_pkQueuePacket);
			/// m_pkQueueUser->push(pkQueueUserDataNode);
		}

		if (1024 <= lUserId && lUserId < MT_QUEUE_USER_ID_AI_START)
		{

			MT_DEBUG_PTCL("\nexitQueueUserDataNode start node1 if3 !!!\n");

			if (pkQueueUserDataNode == m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->pkUserNode)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iStatusExit  = 0;
				m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iStatusRun   = 2;
				if (0 == m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iReplaceAi)
				{
					m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->pkUserNode  = NULL;
				}
			}
		}
	}
	else
	{
		MT_DEBUG_PTCL("\nexitQueueUserDataNode start node2 !!!\n");

		long lUserId = pkQueueUserDataNode->lUserId;
		if (mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE != pkQueueUserDataNode->lUserExit)
		{
			pkQueueUserDataNode->lUserExit   = mtQueueUser::E_SERVICE_ROOM_EXIT_STATUS_RELEASE;
			MT_DEBUG_PTCL("\nSet UserDataNode->lUserExit to RELEASE! %x 273",pkQueueUserDataNode);
			m_pkQueueUser->exitDataNode(pkQueueUserDataNode, m_pkQueuePacket);
			/// m_pkQueueUser->push(pkQueueUserDataNode);
		}

		if (1024 <= lUserId && lUserId < MT_QUEUE_USER_ID_AI_START)
		{
			if (pkQueueUserDataNode == m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->pkUserNode)
			{
				m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iStatusRun   = 2;
				if (0 == m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iReplaceAi)
				{
					m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->iStatusExit = 1;
					m_pkQueueMgr->getUserNodeReplaceAi(lUserId)->pkUserNode  = NULL;
				}
			}
		}
	}

	return 1;
}

int mtThreadWork::playServiceRoomExit(mtQueueUser::DataNode* pkQueueUserDataNode)
{
	mtQueuePacket::DataNode* pkQueuePacketDataNode = (mtQueuePacket::DataNode*)m_pkQueuePacket->popInitDataNode("threadwork280");

	if (pkQueuePacketDataNode)
	{
		mtServiceRoomExit::DataRead* pkDataRead = (mtServiceRoomExit::DataRead*)pkQueuePacketDataNode->pcData;

		pkDataRead->lStructBytes           = sizeof(mtServiceRoomExit::DataRead);
		pkDataRead->lServiceType           = mtServiceMgr::E_SERVICE_ROOM_EXIT;

		mtService::DataRun kDataRun;
		kDataRun.pkQueuePacketDataNode	= pkQueuePacketDataNode;
		kDataRun.pkQueueUserDataNode	= pkQueueUserDataNode;

		m_pkServiceMgr->run(&kDataRun);

		/////////////////////////////////////////////				回收假read packet包节点
		m_pkQueuePacket->push(pkQueuePacketDataNode);
	}

	return 1;
}

int mtThreadWork::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION__BEGINTHREAD)
	default:
		MT_DEBUG(7, "该异常没有被处理");
		return	0;
	}

	return	1;

}
