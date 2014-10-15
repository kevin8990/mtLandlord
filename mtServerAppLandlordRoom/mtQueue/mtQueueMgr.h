#ifndef		__MT_QUEUE_MGR_H
#define		__MT_QUEUE_MGR_H
#include "mtQueueUser.h"
#include "mtQueueDesk.h"
#include "mtQueueList.h"
#include "mtQueueHall.h"
#include "mtQueueSendPackAi.h"
#include "mtListManagerAi.h"

class mtQueueMgr 
{
public:
	struct DataInit
	{
		long	lStructBytes;
		int		iSpinCount;
		HANDLE	hIOCP;
		long	lSpaceTotal;
		long	plRoomTotal[MT_NODE_QUEUE_SPACE];
	};
public:
	struct UserNodeReplaceAi
	{
		long	lStructBytes;
		int     iReplaceAi;                    /// 是否为替代用户的ai(0 -不是替代，1 -是替代)
		int     iStatusExit;                   /// 退出方式(初始值0,1 -用户退出，0 -网络原因)
		int     iStatusRun;                    /// 在房间状态(初始值0,1 -正在房间中，2 -已经退出房间)
		mtQueueUser::DataNode*   pkUserNode;
	};

	mtQueueMgr();
	~mtQueueMgr();

	int		init(void* pData);
	int		exit();

	mtQueuePacket*	    getQueuePacket();
	mtQueueUser*	    getQueueUser();
	mtQueueDesk*	    getQueueDesk();
	mtQueueHall*	    getQueueHall();
	UserNodeReplaceAi*	getUserNodeReplaceAi(long lUserId);

	HANDLE			   m_hIOCP;

	mtQueuePacket*	   m_pkQueuePacket;
	mtQueueUser*	   m_pkQueueUser;
	mtQueueDesk*	   m_pkQueueDesk;
	mtQueueSendPackAi* m_pkQueueSendPackAi;
	mtListManagerAi*   m_pkListManagerAi;
	mtQueueHall*	   m_pkQueueHall;
	UserNodeReplaceAi  m_pkUserNodeReplaceAi[MT_QUEUE_USER_ID_AI_START];
};

inline mtQueuePacket* mtQueueMgr::getQueuePacket()
{
	return	m_pkQueuePacket;
}

inline mtQueueUser*	mtQueueMgr::getQueueUser()
{
	return	m_pkQueueUser;
}
inline mtQueueDesk*	mtQueueMgr::getQueueDesk()
{
	return	m_pkQueueDesk;
}

inline mtQueueHall*	mtQueueMgr::getQueueHall()
{
	return	m_pkQueueHall;
}

inline mtQueueMgr::UserNodeReplaceAi* mtQueueMgr::getUserNodeReplaceAi(long lUserId)
{
	if (MT_QUEUE_USER_ID_AI_START < lUserId || 1024 > lUserId)
	{
		return NULL;
	}

	return &m_pkUserNodeReplaceAi[lUserId];
}

#endif /// __MT_QUEUE_MGR_H