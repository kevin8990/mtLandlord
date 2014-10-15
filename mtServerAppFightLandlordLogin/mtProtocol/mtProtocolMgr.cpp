#include "mtProtocolMgr.h"
#include "mtException.h"
#include "mtThreadWork.h"
#include "mtProtocolRegister.h"
#include "mtProtocolLogin.h"
#include "mtProtocolConnect.h"
#include "mtProtocolAlterPassword.h"

mtProtocolMgr::mtProtocolMgr()
{

}

mtProtocolMgr::~mtProtocolMgr()
{
	for (int iProtocolIdx = 0; iProtocolIdx < mtProtocol::E_PROTOCOL_END; ++iProtocolIdx)
	{
		if (m_pkProtocolList[iProtocolIdx] != NULL)
		{
			delete m_pkProtocolList[iProtocolIdx];
			m_pkProtocolList[iProtocolIdx] = NULL;
		}
	}
}

int mtProtocolMgr::init( DataInit* pkDataInit )
{
	memset(m_pkProtocolList, 0, sizeof(m_pkProtocolList));
	m_pkProtocolList[E_PROTOCOL_REGISTER]	= new mtProtocolRegister;
	MT_EXCEPTION_DEBUG(NULL != m_pkProtocolList[E_PROTOCOL_REGISTER], E_EXCEPTION_CREATE_PROTOCOL);
	m_pkProtocolList[E_PROTOCOL_REGISTER]->init(pkDataInit);
	m_pkProtocolList[E_PROTOCOL_LOGIN]	= new mtProtocolLogin;
	MT_EXCEPTION_DEBUG(NULL != m_pkProtocolList[E_PROTOCOL_LOGIN], E_EXCEPTION_CREATE_PROTOCOL);
	m_pkProtocolList[E_PROTOCOL_LOGIN]->init(pkDataInit);
	m_pkProtocolList[E_PROTOCOL_CONNECT]	= new mtProtocolConnect;
	MT_EXCEPTION_DEBUG(NULL != m_pkProtocolList[E_PROTOCOL_CONNECT], E_EXCEPTION_CREATE_PROTOCOL);
	m_pkProtocolList[E_PROTOCOL_CONNECT]->init(pkDataInit);
	m_pkProtocolList[E_PROTOCOL_ALTER_PASSWORD]	= new mtProtocolAlterPassword;
	MT_EXCEPTION_DEBUG(NULL != m_pkProtocolList[E_PROTOCOL_ALTER_PASSWORD], E_EXCEPTION_CREATE_PROTOCOL);
	m_pkProtocolList[E_PROTOCOL_ALTER_PASSWORD]->init(pkDataInit);

	return 1;
}

int mtProtocolMgr::exit()
{
	for (int iProtocolIdx = 0; iProtocolIdx < mtProtocol::E_PROTOCOL_END; ++iProtocolIdx)
	{
		m_pkProtocolList[iProtocolIdx]->exit();
	}

	return 0;
}

int mtProtocolMgr::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(E_EXCEPTION_CREATE_PROTOCOL)
	default:
		MT_DEBUG(7, "该错误没有被处理!\n");
		break;
	}

	return	1;
}

/*mtProtocol* mtProtocolMgr::getProtocolFromId( mtThreadWork::DataUser*	pkDatauser )
{
	mtProtocol::DataRecv*	pkDataRecv	= (mtProtocol::DataRecv*)pkDatauser->pcDataRecv;

	if (pkDataRecv->lProtocolId >= E_PROTOCOL_BEG && pkDataRecv->lProtocolId < E_PROTOCOL_END)
	{
		return	m_pkProtocolList[pkDataRecv->lProtocolId];
	}

	return	NULL;
}*/

int mtProtocolMgr::run( mtThreadWork::DataUser* pkDataUser )
{
	mtProtocol::DataRecv*	pkDataRecv	= (mtProtocol::DataRecv*)pkDataUser->pcDataRecv;
	
	if (pkDataRecv->lProtocolId >= E_PROTOCOL_BEG && pkDataRecv->lProtocolId < E_PROTOCOL_END){
		return  m_pkProtocolList[pkDataRecv->lProtocolId]->run(pkDataUser);
	}else{
		return 0;
	}
}
