#include "mtProtocolConnect.h"
#include "mtThreadMgr.h"
#include "mtThreadWork.h"
#include "mtGlobal.h"
#include "mtPrint.h"
#include "mtSQLEnv.h"

mtProtocolConnect::mtProtocolConnect()
{
	
}

mtProtocolConnect::~mtProtocolConnect()
{

}

int mtProtocolConnect::init(DataInit* pkDataInit)
{
	mtProtocol::init(pkDataInit);
	return	0;
}

int mtProtocolConnect::exit()
{
	return	0;
}

int mtProtocolConnect::run( mtThreadWork::DataUser* pkDataUser )
{
	DataRecv*	pkDataRecv	= (DataRecv*)pkDataUser->pcDataRecv;
	DataSend*	pkDataSend	= (DataSend*)pkDataUser->pcDataSend;

	mtPrint(pkDataRecv);
	memset(pkDataSend, 0, sizeof(DataSend));
	pkDataSend->kDataHead.lStructBytes	= sizeof(DataSend);
	pkDataSend->kDataHead.lProtocolId	= pkDataRecv->kDataHead.lProtocolId;
	pkDataSend->kDataHead.lKey			= pkDataRecv->kDataHead.lKey;
	pkDataSend->kDataHead.lResultId		= E_RESULT_SUCCESS;
	pkDataSend->pkAddr[E_ADDR_TYPE_DOWNLOAD].ulIP	= g_ulDownloadServerIP;
	pkDataSend->pkAddr[E_ADDR_TYPE_DOWNLOAD].lPort	= g_lDownloadServerPort;
	pkDataSend->pkAddr[E_ADDR_TYPE_CHARGE].ulIP	    = g_ulChargeServerIP;
	pkDataSend->pkAddr[E_ADDR_TYPE_CHARGE].lPort	= g_lChargeServerPort;
	pkDataSend->pkAddr[E_ADDR_TYPE_HALL].ulIP		= g_ulHallServerIP;
	pkDataSend->pkAddr[E_ADDR_TYPE_HALL].lPort	    = g_lHallServerPort;

	/// 软件更新信息
	getUpdateInfo(pkDataUser);
	long  lVersion[4]            = { kProjectInfo.lVersion3, kProjectInfo.lVersion2, kProjectInfo.lVersion1, kProjectInfo.lVersion0 };
	memcpy(pkDataSend->lVersion, lVersion, sizeof(lVersion));
	updateVersion(pkDataRecv, pkDataSend);

	mtPrint(pkDataSend);
	return E_RESULT_SUCCESS_NEED_SEND;
}

int mtProtocolConnect::updateVersion(DataRecv* pkDataRecv, DataSend* pkDataSend)
{
	/// 版本判断,是否需要强制升级
	pkDataSend->lUpdateVersion = 0;
	if (kProjectInfo.lVersion3 > pkDataRecv->lVersion[3])
	{
		pkDataSend->lUpdateVersion = 1;
		return 1;
	}

	if (kProjectInfo.lVersion2 > pkDataRecv->lVersion[2])
	{
		pkDataSend->lUpdateVersion = 1;
		return 1;
	}

	if (kProjectInfo.lVersion1 > pkDataRecv->lVersion[1])
	{
		pkDataSend->lUpdateVersion = 1;
		return 1;
	}

	if (kProjectInfo.lVersion0 > pkDataRecv->lVersion[0])
	{
		pkDataSend->lUpdateVersion = 1;
		return 1;
	}

	return 0;
}

int mtProtocolConnect::getUpdateInfo(mtThreadWork::DataUser* pkDataUser)
{
	DataSend*	pkDataSend	= (DataSend*)pkDataUser->pcDataSend;
	DataRecv*   pkDataRecv  = (DataRecv *)pkDataUser->pcDataRecv;


	if(1 !=m_pkSQLEnv->getProject(pkDataRecv->pcVendorId,pkDataRecv->pcPatchId,&kProjectInfo))
		m_pkSQLEnv->getProject("1000","1",&kProjectInfo);

	 pkDataSend->lUpdateUrlType = kProjectInfo.lUpdateType;

	if (0 == pkDataSend->lUpdateUrlType)
	{
		MT_DEBUG(PTCL, "mtProtocolConnect  NO SOFTWARE PACK TO UPDATE !\n");
		return 1;
	}
	strcpy(pkDataSend->pcUpdateUrl,kProjectInfo.pcUpdateUrl);

	if (1 == pkDataSend->lUpdateUrlType) /// ip
	{
		if (0 == strcmp(pkDataSend->pcUpdateUrl, ""))
		{
			memset(pkDataSend->pcUpdateUrl, 0, sizeof(pkDataSend->pcUpdateUrl));
			pkDataSend->lUpdateUrlType      = 0;
			pkDataSend->lUpdateServerPort   = 0;
			MT_DEBUG(PTCL, "mtProtocol Connect UPDATE SOFTWARE PACK INFO DISPOSE ERROR !\n");
		}
		else
		{
			pkDataSend->lUpdateServerPort = kProjectInfo.lUpdatePort;
		}
	}
	else if (2 == pkDataSend->lUpdateUrlType) /// url
	{
		if (0 == strcmp(pkDataSend->pcUpdateUrl, ""))
		{
			memset(pkDataSend->pcUpdateUrl, 0, sizeof(pkDataSend->pcUpdateUrl));
			pkDataSend->lUpdateUrlType      = 0;
			pkDataSend->lUpdateServerPort   = 0;
			MT_DEBUG(PTCL, "mtProtocolConnect UPDATE SOFTWARE PACK INFO DISPOSE ERROR !\n");
		}
	}
	else
	{
		memset(pkDataSend->pcUpdateUrl, 0, sizeof(pkDataSend->pcUpdateUrl));
		pkDataSend->lUpdateUrlType      = 0;
		pkDataSend->lUpdateServerPort   = 0;
		MT_DEBUG(PTCL, "mtProtocolConnect UPDATE SOFTWARE PACK INFO DISPOSE ERROR !\n");
	}
	return 1;
}
