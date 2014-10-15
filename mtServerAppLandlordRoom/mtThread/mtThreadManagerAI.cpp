#include "mtThreadManagerAi.h"
#include "mtPlatform.h"
#include "mtService.h"

mtThreadManagerAi::mtThreadManagerAi() :
	m_hThread(INVALID_HANDLE_VALUE)
{
}

mtThreadManagerAi::~mtThreadManagerAi()
{

}

int mtThreadManagerAi::run()
{
	static DWORD dwMilliseconds = INFINITE;
	switch (WaitForSingleObject(m_hEvent, dwMilliseconds))
	{
	case WAIT_OBJECT_0:
	case WAIT_TIMEOUT:
		{
			/// 处理当前的发送包
			/// 获取第一个发送包
			LONGLONG llTimeTmp = 0;
			mtQueueSendPackAi::DataNode* pkDataNodeAi = m_pkQueueMgr->m_pkListManagerAi->popHead();
			if (pkDataNodeAi)
			{
				llTimeTmp = pkDataNodeAi->ullTimeSend - GetTickCount();
				if (0 < llTimeTmp)
				{
					int iInsertHead = 0;
					dwMilliseconds  = llTimeTmp;
					m_pkQueueMgr->m_pkListManagerAi->push(pkDataNodeAi, &iInsertHead);
					break;
				}
				else
				{
					dwMilliseconds  = INFINITE;
				}

				if (pkDataNodeAi->pkQueueUserDataNode && pkDataNodeAi->pkQueuePacketDataNode 
					&& pkDataNodeAi->pkQueueUserDataNode->pkQueueDeskDataNode)
				{
					mtQueueDesk::DataNode* pkQueueDeskDataNode = (mtQueueDesk::DataNode*)pkDataNodeAi->pkQueueUserDataNode->pkQueueDeskDataNode;
					if (mtQueueSendPackAi::E_SEND_TYPE_SEND == pkDataNodeAi->iType)
					{
						if (mtQueueDesk::E_PLAYER_MODE_USER == pkQueueDeskDataNode->plPlayerMode[pkDataNodeAi->pkQueueUserDataNode->lUserDeskIndex])
						{
							m_pkQueueMgr->getQueueUser()->DataWrite(pkDataNodeAi->pkQueueUserDataNode, pkDataNodeAi->pkQueuePacketDataNode);
						}
					}
					else if (mtQueueSendPackAi::E_SEND_TYPE_POST == pkDataNodeAi->iType)
					{
						if (m_pkQueueMgr->getQueueUser()->addDataNodePacket(pkDataNodeAi->pkQueueUserDataNode, pkDataNodeAi->pkQueuePacketDataNode))
						{
							PostQueuedCompletionStatus(m_hIOCP, MT_QUEUE_COMPLETION_STATUS_CUSTOM, (ULONG_PTR)pkDataNodeAi->pkQueueUserDataNode, &(pkDataNodeAi->pkQueuePacketDataNode->kDataOverLapped));
						}
						else
						{
							m_pkQueueMgr->getQueuePacket()->push(pkDataNodeAi->pkQueuePacketDataNode);
						}
					}
					else if (mtQueueSendPackAi::E_SEND_TYPE_SEVICE == pkDataNodeAi->iType)
					{
						mtService::DataRun kDataRun;
						kDataRun.pkQueuePacketDataNode	= pkDataNodeAi->pkQueuePacketDataNode;
						kDataRun.pkQueueUserDataNode	= pkDataNodeAi->pkQueueUserDataNode;

						mtServiceMgr::DataHead* pkDataHead = (mtServiceMgr::DataHead*)pkDataNodeAi->pkQueuePacketDataNode->pcData;

						m_pkServiceMgr->run(&kDataRun);

						/////////////////////////////////////////////				回收packet包节点
						m_pkQueueMgr->getQueuePacket()->push(pkDataNodeAi->pkQueuePacketDataNode);
					}
					else 
					{
						/////////////////////////////////////////////				回收packet包节点
						m_pkQueueMgr->getQueuePacket()->push(pkDataNodeAi->pkQueuePacketDataNode);
					}
				}
				else
				{
					m_pkQueueMgr->getQueuePacket()->push(pkDataNodeAi->pkQueuePacketDataNode);
					MT_DEBUG_PTCL("\nmtThreadManagerAi UserExit Recover NodeNum:%d",m_pkQueueMgr->getQueuePacket()->getNodeNumRemain());
				}

				/// 回收节点
				m_pkQueueMgr->m_pkQueueSendPackAi->push(pkDataNodeAi);
			}

			/// 处理预处理下一个发送包
			mtQueueSendPackAi::DataNode* pkDataNodeNextAi = m_pkQueueMgr->m_pkListManagerAi->getHead();
			if (pkDataNodeNextAi)
			{
				llTimeTmp           = pkDataNodeNextAi->ullTimeSend - GetTickCount();
				if (0 >= llTimeTmp)
				{
					dwMilliseconds  = 0;
				}
				else
				{
					dwMilliseconds  = llTimeTmp;
				}
			}
			else
			{
				dwMilliseconds = INFINITE;
			}
		}
		break;
	default:
		dwMilliseconds = INFINITE;
		break;
	}

	return	1;
}

int mtThreadManagerAi::exception()
{
	return	1;
}

int mtThreadManagerAi::init( DataInit* pkInitData )
{
	m_hIOCP        = pkInitData->hIOCP;
	m_hEvent       = pkInitData->hEvent;
	m_pkQueueMgr   = pkInitData->pkQueueMgr;
	m_pkServiceMgr = pkInitData->pkServiceMgr;

	/// 创建线程
	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__beginthread);	

	m_eThreadState			= E_THREAD_STATE_RUN;
	return	1;
}

int mtThreadManagerAi::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}
	
	return	1;
}
