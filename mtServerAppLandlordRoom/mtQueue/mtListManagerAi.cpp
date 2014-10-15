#include "mtListManagerAi.h"


mtListManagerAi::mtListManagerAi() :
	m_pkListHead(NULL),
	m_iDataNodeCount(0),
	m_iSpinCount(0)
{
}

mtListManagerAi::~mtListManagerAi()
{

}

int mtListManagerAi::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_iSpinCount			= pkDataInit->iSpinCount;
	if (!InitializeCriticalSectionAndSpinCount(&m_kCirticalSection, m_iSpinCount))
	{
		return	0;
	}

	m_pkListHead     = NULL;
	m_iDataNodeCount = 0;
	return	1;
}

int mtListManagerAi::exit( void )
{
	DeleteCriticalSection(&m_kCirticalSection);
	m_iDataNodeCount	= 0;
	m_pkListHead        = NULL;

	return 1;
}

int	mtListManagerAi::getNodeNum()
{
	return m_iDataNodeCount;
}

mtQueueSendPackAi::DataNode* mtListManagerAi::getHead()
{
	return m_pkListHead;
}

mtQueueSendPackAi::DataNode* mtListManagerAi::popHead()
{
	mtQueueSendPackAi::DataNode* pkNodeRet = NULL;
	EnterCriticalSection(&m_kCirticalSection);
	if (m_pkListHead)
	{
		m_iDataNodeCount--;
		pkNodeRet         = m_pkListHead;
		m_pkListHead      = m_pkListHead->pkNext;
		pkNodeRet->pkNext = NULL;
	}
	LeaveCriticalSection(&m_kCirticalSection);

	return pkNodeRet;
}

mtQueueSendPackAi::DataNode* mtListManagerAi::findNode(mtQueueSendPackAi::DataNode* pkNode)
{
	mtQueueSendPackAi::DataNode* pkNodeRet = NULL;
	mtQueueSendPackAi::DataNode* pkNodeTmp = m_pkListHead;
	while (pkNodeTmp)
	{
		if (pkNode == pkNodeTmp)
		{
			pkNodeRet = pkNodeTmp;
			break;
		}

		pkNodeTmp = pkNodeTmp->pkNext;
	}

	return pkNodeRet;
}

mtQueueSendPackAi::DataNode* mtListManagerAi::push(mtQueueSendPackAi::DataNode* pkNode, int* piInsertHead)
{
	*piInsertHead       = 0;
	if (!pkNode)
	{
		return NULL;
	}

	EnterCriticalSection(&m_kCirticalSection);
	mtQueueSendPackAi::DataNode* pkNodeTmp = m_pkListHead;
	mtQueueSendPackAi::DataNode* pkNodePre = m_pkListHead;
	pkNode->pkNext      = NULL;
	m_iDataNodeCount++;
	/// 查找插入位置
	while (pkNodeTmp)
	{
		if (pkNodeTmp->ullTimeSend > pkNode->ullTimeSend)
		{
			break;
		}

		pkNodePre = pkNodeTmp;
		pkNodeTmp = pkNodeTmp->pkNext;
	}

	if (pkNodePre == m_pkListHead)
	{
		if (NULL == m_pkListHead)
		{
			*piInsertHead  = 1;
			m_pkListHead   = pkNode;
			LeaveCriticalSection(&m_kCirticalSection);
			return pkNode;
		}
	}

	pkNode->pkNext    = pkNodePre->pkNext;
	pkNodePre->pkNext = pkNode;
	LeaveCriticalSection(&m_kCirticalSection);

	return pkNode;
}

int mtListManagerAi::pop(mtQueueSendPackAi::DataNode* pkNode)
{
	if (!pkNode)
	{
		return 0;
	}

	EnterCriticalSection(&m_kCirticalSection);
	mtQueueSendPackAi::DataNode* pkNodeTmp = m_pkListHead;
	mtQueueSendPackAi::DataNode* pkNodePre = m_pkListHead;
	/// 查找插入位置
	while (pkNodeTmp)
	{
		if (pkNodeTmp == pkNode)
		{
			m_iDataNodeCount--;
			if (pkNodeTmp == m_pkListHead)
			{
				m_pkListHead   = pkNodeTmp->pkNext;
				pkNode->pkNext = NULL;
			}
			else
			{
				pkNodePre->pkNext = pkNodeTmp->pkNext;
				pkNode->pkNext    = NULL;
			}

			LeaveCriticalSection(&m_kCirticalSection);
			return 1;
		}

		pkNodePre = pkNodeTmp;
		pkNodeTmp = pkNodeTmp->pkNext;
	}

	LeaveCriticalSection(&m_kCirticalSection);
	return 0;
}
