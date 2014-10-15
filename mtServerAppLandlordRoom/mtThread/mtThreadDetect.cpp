#include "mtThreadDetect.h"

mtThreadDetect::mtThreadDetect():
	m_hThread(INVALID_HANDLE_VALUE),
    m_pkQueueUser(NULL),
	m_pkQueuePacket(NULL)
{

}

mtThreadDetect::~mtThreadDetect()
{

}

int mtThreadDetect::init(DataInit* pkInitData)
{
	m_pkQueueMgr    = pkInitData->pkQueueMgr;
	m_pkQueuePacket = pkInitData->pkQueuePacket;
	m_pkQueueUser   = pkInitData->pkQueueUser;

	m_hThread	= (HANDLE)_beginthread(CallBackRun, 0, (void*)this);
	MT_EXCEPTION_DEBUG (INVALID_HANDLE_VALUE == m_hThread, EXCEPTION__BEGINTHREAD);	

	m_eThreadState			= E_THREAD_STATE_RUN;

	return 1;
}

int mtThreadDetect::run()
{
//	MT_EXCEPTION_DEBUG(m_pkQueueUser->bHaveRepeat(),EXCEPTION__BEGINTHREAD);
	int deskRemain,deskUse;
	int UserRemain,UserUse;
	int packetRemain,packetUse;
	int onlinePersonNumber;
	int onelist,twolist,threelist;

	deskRemain   = m_pkQueueMgr->m_pkQueueDesk->getNodeNumRemain();
	deskUse      = m_pkQueueMgr->m_pkQueueDesk->getNodeUseCount(),

	UserRemain   = m_pkQueueUser->getNodeNumRemain();
	UserUse      = m_pkQueueUser->getNodeUseCount();

    packetRemain = m_pkQueuePacket->getNodeNumRemain();
	packetUse    = m_pkQueuePacket->getNodeUseCount();

	onelist      = m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueOneList.getNodeNumUsed();
    twolist		 = m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueTwoList.getNodeNumUsed();
    threelist    = m_pkQueueMgr->getQueueHall()->getRoom(0,0)->kQueueThreeList.getNodeNumUsed();

	onlinePersonNumber = m_pkQueueMgr->m_pkQueueHall->m_pkSpaceList[0].pkRoomList[0].lOnlinePersonNumber;

	if(m_pkQueueUser->bHaveRepeat())
	{
		//mtLog("\nHave repeat usernode!");
	}

	return 1;
}

int mtThreadDetect::exit()
{
	if (INVALID_HANDLE_VALUE != m_hThread)
	{
		TerminateThread(m_hThread, 0);
		m_hThread	= INVALID_HANDLE_VALUE;
	}

	m_pkQueuePacket	= NULL;
	m_pkQueueUser	= NULL;

	return 1;
}

int mtThreadDetect::exception()
{
	return 1;
}