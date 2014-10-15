#include "mtProtocolRegister.h"
#include "mtProtocolMgr.h"
#include "mtPrint.h"
#include "mtGlobal.h"

#include "mtQueueUser.h"

#include "mtSQLEnv.h"

 mtProtocolRegister::mtProtocolRegister()
 {
 	
 }
 
 mtProtocolRegister::~mtProtocolRegister()
 {
 
 }
 
 int mtProtocolRegister::init(DataInit* pkDataInit)
 {
	mtProtocol::init(pkDataInit);
	m_ulId				= pkDataInit->ulIdBeg;
 	return	0;
 }
 
 int mtProtocolRegister::exit()
 {
 	return	0;
 }

 int mtProtocolRegister::run( mtThreadWork::DataUser* pkDataUser )
 {
	 DataRecv*	pkDataRecv	            = (DataRecv*)pkDataUser->pcDataRecv;
	 DataSend*	pkDataSend	            = (DataSend*)pkDataUser->pcDataSend;
	 mtPrint(pkDataRecv);
	 memset(pkDataSend, 0, sizeof(DataSend));
	 pkDataSend->kDataHead.lStructBytes	= sizeof(DataSend);
	 pkDataSend->kDataHead.lProtocolId	= pkDataRecv->kDataHead.lProtocolId;
	 pkDataSend->kDataHead.lKey			= pkDataRecv->kDataHead.lKey;

	 switch (pkDataRecv->lAccountType)
	 {
     case   E_REGISTER_TYPE_QQ:
	 case   E_REGISTER_TYPE_PHONE:
		 {
			 if (MT_SERVER_WORK_USER_ID_MIN <= pkDataRecv->uUserId && MT_SERVER_WORK_USER_ID_MAX >= pkDataRecv->uUserId)
			 {
				 pkDataUser->ulId                             = pkDataRecv->uUserId;
				 pkDataSend->uUserId                          = pkDataUser->ulId;
				
				//检查QQ或Phone是否已经注册
				  if(mtSQLEnv::DB_OPERATING_SUCCESS != m_pkSQLEnv->isBingdingUserInfo(pkDataRecv->pcPhone,pkDataRecv->pcQQ))
				  {
					  pkDataSend->kDataHead.lResultId  = E_RESULT_FAIL_USER_IS_EXIST;
					  mtPrint(pkDataSend);
					  MT_DEBUG_ERROR(PTCL, "\nERROR:user(UserId=%d) register fail!The QQ or Phone is exist!\n", pkDataUser->pcAccount);
					  return E_RESULT_SUCCESS_NEED_SEND;
				  }
				 else
				 {
					 pkDataUser->pcAccount = (char*)&pkDataRecv->uUserId;
					 sprintf(pkDataUser->pcAccount,"%d",pkDataRecv->uUserId);

					 if(mtSQLEnv::DB_OPERATING_SUCCESS == m_pkSQLEnv->bindingUserInfo(pkDataUser->pcAccount,pkDataRecv->pcPhone,pkDataRecv->pcQQ))//绑定QQ或者phone
					 {
						 mtQueueUser::UserInfo kUserInfo;

						if(mtSQLEnv::DB_OPERATING_SUCCESS == m_pkSQLEnv->getUserInfo(pkDataUser->pcAccount,mtQueueUser::E_ACCOUNT_TYPE_ID,&kUserInfo))//获取用户信息，发回客户端
						{
							pkDataSend->kDataHead.lResultId	= E_RESULT_SUCCESS;

							pkDataSend->uUserId			    = kUserInfo.ulId;
							memcpy(pkDataSend->pcAccount, kUserInfo.pcAccount, sizeof(kUserInfo.pcAccount));
							memcpy(pkDataSend->pcPwd, kUserInfo.pcPwd, sizeof(kUserInfo.pcPwd));						
							pkDataSend->lUserRegisterDate   =kUserInfo.lUserRegisterDate;

							MT_DEBUG(4, "new user(UserId=%d) game login server......\n", pkDataSend->uUserId);

							return E_RESULT_SUCCESS_NEED_SEND;
						}
					 }
				 }			 
			 }

				 pkDataSend->kDataHead.lResultId  = E_RESULT_FAIL;
				 mtPrint(pkDataSend);
				 return E_RESULT_SUCCESS_NEED_SEND;
			 
		 }
		 break;
		 
	 case	E_REGISTER_TYPE_ID:
		 {
			 pkDataUser->ulId					          = getNewId();
			 if (0 == pkDataUser->ulId)
			 {
				 pkDataSend->kDataHead.lResultId  = E_RESULT_FAIL;
				 mtPrint(pkDataSend);
				 return E_RESULT_SUCCESS_NEED_SEND;
			 }
			 sprintf(pkDataSend->pcAccount, "%d", (MT_SERVER_WORK_USER_ID_BASE + pkDataUser->ulId));
			 strcpy(pkDataSend->pcPwd, MT_DEFAULT_PWD_RAPID_REGISTER);
			 pkDataSend->lRegister                        = 0;
			 unsigned short* pusYear                       = (unsigned short*)&(pkDataSend->lUserRegisterDate);
			 char* pcRegisterDate                         = (char*)&(pkDataSend->lUserRegisterDate);
			 SYSTEMTIME kSysTime; 
			 GetLocalTime(&kSysTime);
			 pusYear[1]                                   = kSysTime.wYear;
			 pcRegisterDate[1]                            = kSysTime.wMonth;
			 pcRegisterDate[0]                            = kSysTime.wDay;

			 pkDataUser->pcAccount				          = pkDataSend->pcAccount;

			 mtQueueUser::UserInfo kUserInfo;

			 kUserInfo.ulId = pkDataUser->ulId;
			 strcpy(kUserInfo.pcAccount,            pkDataSend->pcAccount);
			 strcpy(kUserInfo.pcQQ,                 pkDataRecv->pcQQ);
			 strcpy(kUserInfo.pcPhone,pkDataRecv->pcPhone);
			 strcpy(kUserInfo.pcPwd,                pkDataSend->pcPwd);
			 strcpy(kUserInfo.pcCompanyId,          pkDataRecv->pcCompanyId);
			 strcpy(kUserInfo.pcPatchId,            pkDataRecv->pcPatchId);
			 strcpy(kUserInfo.pcVendorId,           pkDataRecv->pcVendorId);
			 strcpy(kUserInfo.pcResevation,         pkDataRecv->pcResevation);
			 m_pkSQLEnv->getName(kUserInfo.pcNickName);
			 kUserInfo.lUserRegisterDate = pkDataSend->lUserRegisterDate;

			 if(mtSQLEnv::DB_OPERATING_SUCCESS == m_pkSQLEnv->saveUserInfo(&kUserInfo))
			 {
				 pkDataSend->uUserId			    = pkDataUser->ulId;
				 pkDataSend->kDataHead.lResultId	= E_RESULT_SUCCESS;
				 MT_DEBUG(4, "new user(UserId=%d) game login server......\n", pkDataSend->uUserId);
			 }
			 else
			 {
			   pkDataSend->kDataHead.lResultId	= E_RESULT_FAIL;
			 }
			 mtPrint(pkDataSend);
			 return E_RESULT_SUCCESS_NEED_SEND;
		 }
		 break;
	 default:
		 {
			 pkDataSend->kDataHead.lResultId  = E_RESULT_FAIL_ACCOUNT_INVALIDE;
			 mtPrint(pkDataSend);
			 return E_RESULT_SUCCESS_NEED_SEND;
		 }
		 break;
	 }

	 return	E_RESULT_SUCCESS;
 }

 unsigned long	mtProtocolRegister::getNewId()
 {
	 unsigned long ulUserIdNew = 0;
	 char   pcAppLandlordExeDir[MT_APP_FILE_PATH_LEN] = {'\0'};
	 strcpy(pcAppLandlordExeDir, g_pcServiceExeDir);
	 strcpy(pcAppLandlordExeDir + strlen(pcAppLandlordExeDir), MT_APP_ID_FILE_NAME);

	 /// 需要分配用户id
	 EnterCriticalSection(m_pkHallMaxIdCs);
	 /// 创建用户文件
	 FILE* pkFile = NULL;
	 pkFile = fopen(pcAppLandlordExeDir, "rb+");
	 if (NULL == pkFile)
	 {	 //起始1024
		 MT_DEBUG_PTCL("\nID file not found error");
		 LeaveCriticalSection(m_pkHallMaxIdCs);
		 return 0;
	 }

	 unsigned long ulUserId = 0;
	 if (1 != fread(&ulUserId, 4, 1, pkFile))
	 {
		 ulUserId = 0;
		 fclose(pkFile);
		 LeaveCriticalSection(m_pkHallMaxIdCs);
		 return 0;
	 }

	 ulUserIdNew = ulUserId;
	 ulUserId++;
	 fseek(pkFile, 0, SEEK_SET);							//文件指针指向文件的开头
	 fwrite(&ulUserId, sizeof(ulUserId), 1, pkFile);
	 fflush(pkFile);
	 fclose(pkFile);
	 LeaveCriticalSection(m_pkHallMaxIdCs);

	 return ulUserIdNew;
	 /// return	m_ulId++;
 }
