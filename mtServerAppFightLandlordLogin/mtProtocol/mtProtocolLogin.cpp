#include "mtProtocolLogin.h"
#include "mtPrint.h"
#include "mtGlobal.h"

#include "mtQueueUser.h"
#include "mtSQLEnv.h"

mtProtocolLogin::mtProtocolLogin()
{

}

mtProtocolLogin::~mtProtocolLogin()
{

}

int mtProtocolLogin::init(DataInit* pkDataInit)
{
	mtProtocol::init(pkDataInit);
	m_lpfnDisconnectEx	= pkDataInit->lpfnDisconnectEx;

	return	0;
}

int mtProtocolLogin::exit()
{
	return	0;
}

int mtProtocolLogin::run( mtThreadWork::DataUser* pkDataUser )
{
	DataRecv*	pkDataRecv	= (DataRecv*)pkDataUser->pcDataRecv;
	DataSend*	pkDataSend	= (DataSend*)pkDataUser->pcDataSend;
	mtPrint(pkDataRecv);
	memset(pkDataSend, 0, sizeof(DataSend));
	pkDataSend->kDataHead.lStructBytes	= sizeof(DataSend);
	pkDataSend->kDataHead.lProtocolId	= pkDataRecv->kDataHead.lProtocolId;
	pkDataSend->kDataHead.lKey			= pkDataRecv->kDataHead.lKey;
	pkDataUser->pcAccount				= pkDataRecv->pcAccount;

	if(pkDataRecv->lAccountType == E_ACCOUNT_TYPE_ID)
	{
		long * id = (long *)pkDataRecv->pcAccount;
		sprintf(pkDataRecv->pcAccount,"%d",*id);
	}	

	 mtQueueUser::UserInfo kUserInfo;

	if(m_pkSQLEnv->getUserInfo(pkDataRecv->pcAccount,pkDataRecv->lAccountType,&kUserInfo))
	{
		if(0 ==strcmp(kUserInfo.pcPwd , pkDataRecv->pcPwd))
		{
			pkDataSend->kDataHead.lResultId	= E_RESULT_SUCCESS;
			pkDataSend->ulId				= kUserInfo.ulId;
			pkDataSend->lUserRegisterDate   =kUserInfo.lUserRegisterDate;
			MT_DEBUG(PTCL, "\nuser(UserId=%d) game login server......\n", pkDataSend->ulId);
		}
		else
		{
			pkDataSend->kDataHead.lResultId	= E_RESULT_FAIL_ACCOUNT_INVALIDE;
			MT_DEBUG_ERROR(PTCL, "\nERROR:user(UserId=%d) game login server fail!Wrong password!\n", pkDataSend->ulId);
		}
	}
	else
	{
		pkDataSend->kDataHead.lResultId	= E_RESULT_FAIL_USER_NOT_EXIST;
	}
	return  E_RESULT_SUCCESS_NEED_SEND;
}