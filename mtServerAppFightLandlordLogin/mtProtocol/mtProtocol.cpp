#include "mtProtocol.h"

LPFN_DISCONNECTEX	mtProtocol::m_lpfnDisconnectEx	= NULL;

mtProtocol::mtProtocol()
{

}

mtProtocol::~mtProtocol()
{

}


int mtProtocol::exit()
{
	return	0;
}

int mtProtocol::init( DataInit* pkDataInit )
{
	m_lpfnDisconnectEx	= pkDataInit->lpfnDisconnectEx;
	m_pkSQLEnv			= pkDataInit->pkSQLEnv;
	m_pkHallMaxIdCs     = pkDataInit->pkHallMaxIdCs;
	return  0;
}

int mtProtocol::run( mtThreadWork::DataUser* pkDataUser )
{
	return  0;
}