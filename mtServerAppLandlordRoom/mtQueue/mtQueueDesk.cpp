#include "mtQueueDesk.h"


mtQueueDesk::mtQueueDesk()
{
	m_iNodeTotal	= MT_NODE_QUEUE_DESK;
}

mtQueueDesk::~mtQueueDesk()
{

}

int mtQueueDesk::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	m_iSpinCount			= pkDataInit->iSpinCount;

	memset (m_pkNodeList, 0, sizeof(m_pkNodeList));

	int		iRet			= m_pkIdQueue.init(m_iSpinCount);

	int			iNodeIdx;
	DataNode*	pkDataNode	= m_pkNodeList;
	for (iNodeIdx = 0; iNodeIdx < m_iNodeTotal; iNodeIdx++)
	{
		pkDataNode->lStructBytes	= sizeof(DataNode);
		if (!m_pkIdQueue.push(&pkDataNode))
		{
			return	0;
		}

		pkDataNode->iRetCirticalSection = InitializeCriticalSectionAndSpinCount(&(pkDataNode->kCriticalSection), m_iSpinCount);
		if (0 == pkDataNode->iRetCirticalSection)
		{
			return 0;
		}

		pkDataNode++;
	}

	return	iRet;
}

int mtQueueDesk::exit( void )
{
	int			iNodeIdx;
	DataNode*	pkDataNode	= m_pkNodeList;
	for (iNodeIdx = 0; iNodeIdx < m_iNodeTotal; iNodeIdx++)
	{
		if (0 != pkDataNode->iRetCirticalSection)
		{
			DeleteCriticalSection(&(pkDataNode->kCriticalSection));
			pkDataNode->iRetCirticalSection = 0;
		}
	}

	m_iNodeTotal	= 0;
	return m_pkIdQueue.exit();
}
