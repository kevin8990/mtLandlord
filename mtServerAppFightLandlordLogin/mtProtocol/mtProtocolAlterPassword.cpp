#include "mtProtocolAlterPassword.h"
#include "mtProtocolLogin.h"
#include "mtPrint.h"

#include "mtQueueUser.h"
#include "mtSQLEnv.h"

mtProtocolAlterPassword::mtProtocolAlterPassword()
{

}

mtProtocolAlterPassword::~mtProtocolAlterPassword()
{

}

int mtProtocolAlterPassword::init(DataInit* pkDataInit)
{
	mtProtocol::init(pkDataInit);
	m_lpfnDisconnectEx	= pkDataInit->lpfnDisconnectEx;

	return	0;
}

int mtProtocolAlterPassword::exit()
{
	return	0;
}

int mtProtocolAlterPassword::run( mtThreadWork::DataUser* pkDataUser )
{
	DataRecv*	pkDataRecv	= (DataRecv*)pkDataUser->pcDataRecv;
	DataSend*	pkDataSend	= (DataSend*)pkDataUser->pcDataSend;
	mtPrint(pkDataRecv);
	pkDataSend->kDataHead.lStructBytes	= sizeof(DataSend);
	pkDataSend->kDataHead.lProtocolId	= pkDataRecv->kDataHead.lProtocolId;
	pkDataSend->kDataHead.lKey			= pkDataRecv->kDataHead.lKey;
	pkDataUser->pcAccount				= pkDataRecv->pcAccount;

	long * id = (long *)pkDataRecv->pcAccount;
	sprintf(pkDataUser->pcAccount,"%d",*id);

	if(mtSQLEnv::DB_OPERATING_SUCCESS == m_pkSQLEnv->updatePassWord(pkDataUser->pcAccount,pkDataRecv->pcPwd,pkDataRecv->pcNewPwd))
	{
		pkDataSend->kDataHead.lResultId	= E_RESULT_SUCCESS;
	}
	else
	{
		pkDataSend->kDataHead.lResultId	= E_RESULT_FAIL_PWD_INVALID;
		MT_DEBUG_ERROR(PTCL, "\nERROR:user(UserId=%d) alter passworld fail!\n", pkDataUser->pcAccount);
		mtPrint(pkDataSend);
	}
	return E_RESULT_SUCCESS_NEED_SEND;
}
