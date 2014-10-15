#include "mtQueueHall.h"

mtQueueHall::mtQueueHall() :
	m_lSpaceTotal(MT_NODE_QUEUE_SPACE)
{
	memset (m_pkSpaceList, 0, sizeof(m_pkSpaceList));
}

mtQueueHall::~mtQueueHall()
{

}

int mtQueueHall::init( void* pData )
{
	DataInit*	pkDataInit	= (DataInit*)pData;
	int			iSpinCount	= pkDataInit->iSpinCount;
	m_lSpaceTotal			= pkDataInit->lSpaceTotal;

	int		iSpaceIdx, iRoomIdx;

	for (iSpaceIdx = 0; iSpaceIdx < m_lSpaceTotal; iSpaceIdx++)
	{
		m_pkSpaceList[iSpaceIdx].lStructBytes	= sizeof(DataSpace);
		m_pkSpaceList[iSpaceIdx].lRomTotal		= pkDataInit->plRoomTotal[iSpaceIdx];
		m_pkSpaceList[iSpaceIdx].pkRoomList		= new DataRoom[m_pkSpaceList[iSpaceIdx].lRomTotal];
		for (iRoomIdx = 0; iRoomIdx < m_pkSpaceList[iSpaceIdx].lRomTotal; iRoomIdx++)
		{
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].lStructBytes	= sizeof(DataRoom);
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].lOnlinePersonNumber = 0;
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueOneList.init(iSpinCount);
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueTwoList.init(iSpinCount);
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueThreeList.init(iSpinCount);
			/// InitializeCriticalSectionAndSpinCount(&m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kCriticalSection, iSpinCount);
		}
	}

	return	1;
}

int mtQueueHall::exit()
{
	int		iSpaceIdx, iRoomIdx;
	for (iSpaceIdx = 0; iSpaceIdx < m_lSpaceTotal; iSpaceIdx++)
	{
		for (iRoomIdx = 0; iRoomIdx < m_pkSpaceList[iSpaceIdx].lRomTotal; iRoomIdx++)
		{
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].lStructBytes	= sizeof(DataRoom);
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueOneList.exit();
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueTwoList.exit();
			m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kQueueThreeList.exit();
			/// DeleteCriticalSection(&m_pkSpaceList[iSpaceIdx].pkRoomList[iRoomIdx].kCriticalSection);
		}

		delete [] m_pkSpaceList[iSpaceIdx].pkRoomList;
	}
	
	return	1;
}

