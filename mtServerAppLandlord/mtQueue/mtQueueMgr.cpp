#include "mtQueueMgr.h"
#include "mtQueuePacket.h"

mtQueueMgr::mtQueueMgr() :
	m_pkQueuePacket(NULL)
{

}

mtQueueMgr::~mtQueueMgr()
{

}

int mtQueueMgr::init( void* pData )
{
	DataInit*	pkDataInit			    = (DataInit*)pData;
	int			iSpinCount			    = pkDataInit->iSpinCount;
	m_pcServiceExeDir                   = pkDataInit->pcServiceExeDir;

	m_pkQueuePacket					    = new mtQueuePacket;
	mtQueuePacket::DataInit	kQueuePacketDataInit;
	kQueuePacketDataInit.lStructBytes	= sizeof(kQueuePacketDataInit);
	kQueuePacketDataInit.iSpinCount		= iSpinCount;
	m_pkQueuePacket->init(&kQueuePacketDataInit);

	m_pkHall					        = new mtQueueHall;
	mtQueueHall::DataInit	kQueueHallDataInit;
	kQueueHallDataInit.lStructBytes	    = sizeof(kQueueHallDataInit);
	kQueueHallDataInit.iSpinCount		= iSpinCount;
	kQueueHallDataInit.pcServiceExeDir  = m_pcServiceExeDir;
	kQueueHallDataInit.pkSQLEnv         =pkDataInit->pkSQLEnv;
	m_pkHall->init(&kQueueHallDataInit);

	return	1;
}

int mtQueueMgr::exit()
{
	m_pkQueuePacket->exit();

	return	1;
}

