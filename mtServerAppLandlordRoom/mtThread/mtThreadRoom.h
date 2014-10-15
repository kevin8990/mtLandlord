#ifndef		___MT_THREAD_ROOM_H
#define		___MT_THREAD_ROOM_H
#include "mtThread.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"
#include "mtServiceMgr.h"

class	mtThreadRoom : public mtThread
{
public:
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION__BEGINTHREAD,
		EXCEPTION_END
	};

	struct	DataInit
	{
		long				    lStructBytes;
		mtQueueMgr*             pkQueueMgr;
		mtServiceMgr*			pkServiceMgr;
		mtQueueHall::DataRoom*  pkQueueHallDataRoom;
	};


public:
	mtThreadRoom();
	~mtThreadRoom();

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	int runQueueOneList();
	int runQueueTwoList();
	int runQueueThreeList();
	
	int runQueueTwoListDesk(mtQueueHall::DataRoom*  pkQueueHallDataRoom, mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueuePacket::DataNode* pkQueuePacketDataNode);
	int runQueueThreeListDesk(mtQueueHall::DataRoom*  pkQueueHallDataRoom, mtQueueDesk::DataNode* pkQueueDeskDataNode, mtQueuePacket::DataNode* pkQueuePacketDataNode);
	
	int runFilterUserExitOneList(mtQueueHall::DataRoom* pkQueueHallDataRoom, mtQueueUser::DataNode** pkQueueUserDataNodes, int iUserCount);
	int runFilterUserExitTwoList(mtQueueHall::DataRoom* pkQueueHallDataRoom, mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int runFilterUserExitThreeList(mtQueueHall::DataRoom* pkQueueHallDataRoom, mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int runReFilterUserExitThreeList(mtQueueHall::DataRoom* pkQueueHallDataRoom, mtQueueDesk::DataNode* pkQueueDeskDataNode);
	
	int escape();
	int initQueueUserDataNodeAi(mtQueueUser::DataNode* pkQueueUserDataNodeAi,long lRoomId);
	int runWaitStartRecvAi(mtQueueUser::DataNode* pkQueueUserDataNode);
	int runStartCardDeal(mtQueueUser::DataNode* pkQueueUserDataNode);

	int runRoomHelpTwoList(mtQueueHall::DataRoom*  pkQueueHallDataRoom);
	//int runRoomHelpThreeList(mtQueueHall::DataRoom*  pkQueueHallDataRoom);
	int runHeartBeat(mtQueueUser::DataNode* pkQueueUserDataNode);

	timeval					m_kTimeVal;

	fd_set					m_kFdSet;
	ULONGLONG				m_ullTimeEnterRun;

	HANDLE					m_hThread;

	mtQueueMgr*             m_pkQueueMgr;
	mtQueueHall::DataRoom*  m_pkQueueHallDataRoom;

	mtServiceMgr*			m_pkServiceMgr;
};

inline int mtThreadRoom::escape() 
{
	return	static_cast<int>(GetTickCount() - m_ullTimeEnterRun);
}

#endif	///	___MT_THREAD_ROOM_H