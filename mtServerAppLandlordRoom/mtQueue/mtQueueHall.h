#ifndef		__MT_QUEUE_HALL_H
#define		__MT_QUEUE_HALL_H
#include "mtQueueList.h"

class	mtQueueHall
{
public:
	enum ERoomQueueType
	{
		E_ROOM_QUEUE_TYPE_BEG	= 0,

		E_ROOM_QUEUE_TYPE_ONE,
		E_ROOM_QUEUE_TYPE_TWO,
		E_ROOM_QUEUE_TYPE_THREE,

		E_ROOM_QUEUE_TYPE_END
	};

	struct DataRoom
	{
		long				lStructBytes;
		long                lOnlinePersonNumber;
		mtQueueOneList		kQueueOneList;
		mtQueueTwoList		kQueueTwoList;
		mtQueueThreeList	kQueueThreeList;
		/// CRITICAL_SECTION	kCriticalSection;
	};

	struct DataSpace
	{
		long				lStructBytes;
		long				lRomTotal;
		DataRoom*			pkRoomList;
	};

	struct DataInit
	{
		long				lStructBytes;
		int					iSpinCount;
		long				lSpaceTotal;
		long				plRoomTotal[MT_NODE_QUEUE_SPACE];
	};

public:
	mtQueueHall();
	~mtQueueHall();

	int		init(void* pData);
	int		exit();

	DataSpace*			getSpace(int iSpaceIdx);
	DataRoom*			getRoom(int iSpaceIdx, int iRoomIdx);
	DataRoom*			getRoom(DataSpace* pkDataSpace, int iRoomIdx);
	void*				getQueue(int iSpaceIdx, int iRoomIdx, ERoomQueueType eQueueType);
	void*				getQueue(DataRoom* pkDataRoom, ERoomQueueType eQueueType);

	long				m_lSpaceTotal;
	DataSpace			m_pkSpaceList[MT_NODE_QUEUE_SPACE];
};

inline mtQueueHall::DataSpace*	mtQueueHall::getSpace(int iSpaceIdx)
{
	return	m_pkSpaceList+iSpaceIdx;
}

inline mtQueueHall::DataRoom* mtQueueHall::getRoom(int iSpaceIdx, int iRoomIdx)
{
	return	m_pkSpaceList[iSpaceIdx].pkRoomList + iRoomIdx;
}

inline mtQueueHall::DataRoom*	mtQueueHall::getRoom(DataSpace* pkDataSpace, int iRoomIdx)
{
	return	pkDataSpace->pkRoomList + iRoomIdx;
}

inline void* mtQueueHall::getQueue(DataRoom* pkDataRoom, ERoomQueueType eQueueType)
{
	switch (eQueueType)
	{
	case	E_ROOM_QUEUE_TYPE_ONE:
		return	(void*)(&(pkDataRoom->kQueueOneList));
	case	E_ROOM_QUEUE_TYPE_TWO:
		return	(void*)(&(pkDataRoom->kQueueTwoList));
	case	E_ROOM_QUEUE_TYPE_THREE:
		return	(void*)(&(pkDataRoom->kQueueThreeList));
	default:
		return	NULL;
	}
}

inline void* mtQueueHall::getQueue(int iSpaceIdx, int iRoomIdx, ERoomQueueType eQueueType)
{
	return	getQueue(getRoom(iSpaceIdx, iRoomIdx), eQueueType);
}

#endif	///	__MT_QUEUE_HALL_H