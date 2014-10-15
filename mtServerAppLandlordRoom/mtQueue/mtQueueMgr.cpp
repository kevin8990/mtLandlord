#include "mtQueueMgr.h"

mtQueueMgr::mtQueueMgr() :
	m_pkQueuePacket(NULL),
	m_pkQueueUser(NULL),
	m_pkQueueDesk(NULL),
	m_pkQueueHall(NULL)
{

}

mtQueueMgr::~mtQueueMgr()
{

}

int mtQueueMgr::init( void* pData )
{
	DataInit*	pkDataInit			= (DataInit*)pData;
	m_hIOCP							= pkDataInit->hIOCP;
	int			iSpinCount			= pkDataInit->iSpinCount;

	m_pkQueuePacket					= new mtQueuePacket;
	mtQueuePacket::DataInit	kQueuePacketDataInit;
	kQueuePacketDataInit.lStructBytes	= sizeof(kQueuePacketDataInit);
	kQueuePacketDataInit.iSpinCount		= iSpinCount;
	m_pkQueuePacket->init(&kQueuePacketDataInit);

	m_pkQueueUser					= new mtQueueUser;
	mtQueueUser::DataInit	kQueueUserDataInit;
	kQueueUserDataInit.lStructBytes	= sizeof(kQueueUserDataInit);
	kQueueUserDataInit.iSpinCount	= iSpinCount;
	kQueueUserDataInit.hIOCP		= m_hIOCP;
	m_pkQueueUser->init(&kQueueUserDataInit);

	m_pkQueueDesk					= new mtQueueDesk;
	mtQueueDesk::DataInit	kQueueDeskDataInit;
	kQueueDeskDataInit.lStructBytes	= sizeof(kQueueDeskDataInit);
	kQueueDeskDataInit.iSpinCount	= iSpinCount;
	m_pkQueueDesk->init(&kQueueDeskDataInit);

	m_pkQueueSendPackAi					= new mtQueueSendPackAi;
	mtQueueSendPackAi::DataInit	kQueueSendPackAiDataInit;
	kQueueSendPackAiDataInit.lStructBytes	= sizeof(kQueueSendPackAiDataInit);
	kQueueSendPackAiDataInit.iSpinCount	= iSpinCount;
	m_pkQueueSendPackAi->init(&kQueueSendPackAiDataInit);

	m_pkListManagerAi					= new mtListManagerAi;
	mtListManagerAi::DataInit	kQueueListManagerAiDataInit;
	kQueueListManagerAiDataInit.lStructBytes	= sizeof(kQueueListManagerAiDataInit);
	kQueueListManagerAiDataInit.iSpinCount	    = iSpinCount;
	m_pkListManagerAi->init(&kQueueListManagerAiDataInit);

	m_pkQueueHall					= new mtQueueHall;
	mtQueueHall::DataInit	kQueueHallDataInit;
	kQueueHallDataInit.lStructBytes	= sizeof(kQueueHallDataInit);
	kQueueHallDataInit.iSpinCount	= iSpinCount;
	kQueueHallDataInit.lSpaceTotal	= pkDataInit->lSpaceTotal;
	memcpy (kQueueHallDataInit.plRoomTotal, pkDataInit->plRoomTotal, sizeof(pkDataInit->plRoomTotal));
	m_pkQueueHall->init(&kQueueHallDataInit);

	memset(m_pkUserNodeReplaceAi, 0, sizeof(m_pkUserNodeReplaceAi));

	return	1;
}

int mtQueueMgr::exit()
{
	m_pkQueueHall->exit();
	m_pkQueueDesk->exit();
	m_pkQueueUser->exit();
	return	1;
}

