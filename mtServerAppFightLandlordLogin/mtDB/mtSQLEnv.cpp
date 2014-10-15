#include "mtSQLEnv.h"

mtSQLEnv::mtSQLEnv(void)
{
}

mtSQLEnv::~mtSQLEnv(void)
{
}

namespace soci{
	template<>
	struct type_conversion<mtQueueUser::UserInfo>
	{
		typedef values base_type;

		static void from_base(values const & v,indicator & ind,mtQueueUser::UserInfo & userInfo)
		{
			userInfo.ulId = v.get<int>("id");					/// 用户id	0 表示失败
			sprintf(userInfo.pcVendorId,"%d",v.get<int>("vendorId"));					///厂商ID
			sprintf(userInfo.pcPatchId,"%d",v.get<int>("batchId"));						///批次ID
			strcpy(userInfo.pcCompanyId,v.get<std::string>("companyCode","").c_str());		///公司代码
			//userInfo.lUserIconId = v.get<int>("iconId");			/// 用户头像id
			strcpy(userInfo.pcAccount,v.get<std::string>("accountNum","").c_str());		/// 用户账号
			strcpy(userInfo.pcPhone,v.get<std::string>("phone","").c_str());				/// 手机号码
			strcpy(userInfo.pcQQ,v.get<std::string>("qq","").c_str());						/// QQ号码
			strcpy(userInfo.pcPwd,v.get<std::string>("pwd","").c_str());					/// 用户密码
			//strcpy(userInfo.pcUserName,v.get<std::string>("name","").c_str());					/// 用户姓名
			//strcpy(userInfo.pcUserNickName,v.get<std::string>("nickName","").c_str());			/// 用户昵称
			//strcpy(userInfo.pcUserLove,v.get<std::string>("love","").c_str());					/// 兴趣爱好
			//strcpy(userInfo.pcUserRemark,v.get<std::string>("remark","").c_str());				/// 备注说明
			
			//userInfo.lUserSex = v.get<int>("sex");						/// 性别 (0--男，1--女)
			//userInfo.lUserAge = v.get<int>("age");						/// 年龄
			//userInfo.lUserGold = v.get<int>("gold");					/// 用户拥有金币数
			//userInfo.lUserScore = v.get<int>("score");					/// 用户积分
			//userInfo.lUserLevel = v.get<int>("level");					/// 用户等级
			//userInfo.lUserAllChess = v.get<int>("allChess");			/// 总局数
			//userInfo.lUserWinChess = v.get<int>("winChess");			/// 胜局数
			//userInfo.lUserWinRate = v.get<int>("winRate");				/// 胜率
			//userInfo.lUserOffLineCount = v.get<int>("offLineCount");	/// 掉线次数
			//userInfo.lRegister = v.get<int>("iRegister");				/// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间

			std::tm kRegisterDate = v.get<std::tm>("registerDate");			/// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
			long lregisterHallDate;
			unsigned short* pusYear = (unsigned short*)&lregisterHallDate;
			char* pcMonthDate       = (char*)&lregisterHallDate;
			pusYear[1]                          = kRegisterDate.tm_year;
			pcMonthDate[1]                      = kRegisterDate.tm_mon;
			pcMonthDate[0]                      = kRegisterDate.tm_mday;
			userInfo.lUserRegisterDate = lregisterHallDate;
			
			/*std::tm kLoginDate = v.get<std::tm>("loginHallDate");		/// 用户最近一次登录的日期(XXXX-XX-XX)
			long lLoginHallDate;
			pusYear = (unsigned short*)&lLoginHallDate;
			pcMonthDate       = (char*)&lLoginHallDate;
			pusYear[1]                          = kLoginDate.tm_year;
			pcMonthDate[1]                      = kLoginDate.tm_mon;
			pcMonthDate[0]                      = kLoginDate.tm_mday;
			userInfo.lUserLoginHallDate = lLoginHallDate;*/

			//userInfo.lUserContinueLoginDays = v.get<int>("continueLoginDays");		/// 用户连续登录的天数
			//userInfo.lUserLoginFirstToday = v.get<int>("loginFirstToday");			/// 是否是今天的第一次登录(0 -不是今天第一次， 1 -是今天第一次)
			//userInfo.lUserGiveGoldFrequence = v.get<int>("giveGoldFrequence");		/// 每天赠送给用户分数的次数

			//char sPropsCount[50];
			//strcpy(sPropsCount,v.get<std::string>("propsCount","").c_str());		/// 用户道具数目
			//readNumSep(sPropsCount,",",userInfo.plPropsCount,10);

			//char sChessPalyed[30];
			//strcpy(sChessPalyed,v.get<std::string>("chessPalyed").c_str());		/// 用户当天在每个房间里玩游戏的局数
			//readNumSep(sChessPalyed,",",userInfo.plUserChessPalyed,6);
			
			ind = i_ok;
		}
		static void to_base(const mtQueueUser::UserInfo & userInfo,values &v,indicator & ind)
		{
			v.set("id",(int)userInfo.ulId);							/// 用户id	0 表示失败
			v.set("vendorId",atoi(userInfo.pcVendorId));						///厂商ID
			v.set("batchId",atoi(userInfo.pcPatchId));							///批次ID
			v.set("companyCode",(std::string)userInfo.pcCompanyId);			///公司代码
			//v.set("iconId",(int)userInfo.lUserIconId);					/// 用户头像id
			v.set("accountNum",(std::string)userInfo.pcAccount);		/// 用户账号
			v.set("phone",(std::string)userInfo.pcPhone); 					/// 手机号码
			v.set("qq",(std::string)userInfo.pcQQ); 						/// QQ号码
			v.set("pwd",(std::string)userInfo.pcPwd); 						/// 用户密码
			//v.set("name",(std::string)userInfo.pcUserName); 					/// 用户姓名
			v.set("nickName",(std::string)userInfo.pcNickName); 			/// 用户昵称
			//v.set("love",(std::string)userInfo.pcUserLove); 					/// 兴趣爱好
			//v.set("remark",(std::string)userInfo.pcUserRemark); 				/// 备注说明
			//v.set("sex",(int)userInfo.lUserSex);  						/// 性别 (0--男，1--女)
			//v.set("age",(int)userInfo.lUserAge);  						/// 年龄
			//v.set("gold",(int)userInfo.lUserGold);  					/// 用户拥有金币数
			//v.set("score",(int)userInfo.lUserScore);  					/// 用户积分
			//v.set("level",(int)userInfo.lUserLevel);  					/// 用户等级
			//v.set("allChess",(int)userInfo.lUserAllChess);  			/// 总局数
			//v.set("winChess",(int)userInfo.lUserWinChess);  			/// 胜局数
			//v.set("winRate",(int)userInfo.lUserWinRate);  				/// 胜率

			//v.set("offLineCount",(int)userInfo.lUserOffLineCount);  	/// 掉线次数
			//v.set("iRegister",(int)userInfo.lRegister);  				/// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间

			std::tm  kRegisterNow;
			__time64_t kRNowTime;
			time(&kRNowTime);
			_localtime64_s(&kRegisterNow, &kRNowTime);
			v.set("registerDate",kRegisterNow);			//userInfo.lUserRegisterDate		/// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
			v.set("loginHallDate",kRegisterNow);		//userInfo.lUserLoginHallDate		/// 用户最近一次登录的日期(XXXX-XX-XX)

			//v.set("continueLoginDays",(int)userInfo.lUserContinueLoginDays);		/// 用户连续登录的天数
			//v.set("loginFirstToday",(int)userInfo.lUserLoginFirstToday);			/// 是否是今天的第一次登录(0 -不是今天第一次， 1 -是今天第一次)
			//v.set("giveGoldFrequence",(int)userInfo.lUserGiveGoldFrequence); 		/// 每天赠送给用户分数的次数

			/*char pcValue[50];
			pcValue[49] = '\0';
			sprintf(pcValue,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",userInfo.plPropsCount[0],userInfo.plPropsCount[1],
				userInfo.plPropsCount[2],userInfo.plPropsCount[3],userInfo.plPropsCount[4],userInfo.plPropsCount[5],
				userInfo.plPropsCount[6],userInfo.plPropsCount[7],userInfo.plPropsCount[8],userInfo.plPropsCount[9]);
			pcValue[strlen(pcValue)] = '\0';
			v.set("propsCount",(std::string)pcValue);						/// 用户道具数目
			*/
			/*pcValue[49] = '\0';
			sprintf(pcValue,"%d,%d,%d,%d,%d,%d",userInfo.plUserChessPalyed[0],userInfo.plUserChessPalyed[1],
				userInfo.plUserChessPalyed[2],userInfo.plUserChessPalyed[3],userInfo.plUserChessPalyed[4],
				userInfo.plUserChessPalyed[5]);
			pcValue[strlen(pcValue)] = '\0';
			v.set("chessPalyed",(std::string)pcValue);						/// 用户当天在每个房间里玩游戏的局数
			*/
			ind = i_ok;
		}

		//拆解字符串函数
		static void readNumSep(const char* pcDir,const char *delim,long* count,int size){
			char pcDirTmp[50];
			strcpy(pcDirTmp, pcDir);
			char *p;
			for(int i=0;i<size;i++){
				if(i==0)
					p = strtok(pcDirTmp,delim);
				else
					p = strtok(NULL,delim);
				if(p)
					count[i] = atoi(p);
			}
		}
	};

	template<>
	struct type_conversion<mtQueueUser::ProjectInfo>
	{
		typedef values base_type;
		static void from_base(values const & v,indicator & ind,mtQueueUser::ProjectInfo & projectInfo)
		{
			sprintf(projectInfo.pcVendorId,"%d", v.get<int>("vid"));
			sprintf(projectInfo.pcPatchId,"%d", v.get<int>("pid"));
			projectInfo.lUpdateType = v.get<int>("update_type");
			strcpy(projectInfo.pcUpdateUrl,v.get<std::string>("update_url").c_str());	
			projectInfo.lUpdatePort = v.get<int>("update_port");
			projectInfo.lVersion3 = v.get<int>("update_v3");
			projectInfo.lVersion2 = v.get<int>("update_v2");
			projectInfo.lVersion1 = v.get<int>("update_v1");
			projectInfo.lVersion0 = v.get<int>("update_v0");

			ind = i_ok;
		}
		static void to_base(const mtQueueUser::ProjectInfo & projectInfo,values &v,indicator & ind)
		{

		}

	};
}


int mtSQLEnv::init(/*DataInit* pkDataInit*/)
{
	iPoolSize = g_DataBasePoolSize;

	MT_EXCEPTION_DEBUG(NULL != (m_pkDBPool = new connection_pool(iPoolSize)), EXCEPTION_DB_POOL_NEW);

	int		iConnectIdx;
	char conStr[256]  = {0};
	sprintf(conStr,"mysql://host=%s db=%s user=%s password='%s' charset=%s",g_DataBaseServer,
		g_DataBaseName,g_DataBaseUserName,g_DataBasePassWord,g_DataBaseCODE);
	//MT_DEBUG(8,"\n%s",conStr);
	try
	{
		for(iConnectIdx = 0; iConnectIdx<iPoolSize; iConnectIdx++){
			session &sessionConn = m_pkDBPool->at(iConnectIdx);
			sessionConn.open(conStr);
			//MT_DEBUG(8,"\ninit connection :%d",sessionConn.get_backend());
		}
		MT_DEBUG(PTCL, "init DBPool [%d] success!\n",(iConnectIdx));
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from Connect DB",e.what());
		errorMsg = e.what();
		MT_EXCEPTION_DEBUG(0, EXCEPTION_DB_CONNECT_NEW);
	}

	return 1;
}

/*int	mtSQLEnv::saveHallFeeBack(mtServiceHallFeedBack::DataRead* pkDataRead)
{
	try
	{
		session sql(*m_pkDBPool);
		sql << "insert into feedback(userid,content,create_at) values(:userid,:content,now())",
			use((pkDataRead->lUserId+MT_SERVER_WORK_USER_ID_BASE)),use((string)pkDataRead->pcContentFeedBack);
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from saveHallFeeBack",e.what());
		errorMsg = e.what();
		return 0;
	}

	return 1;
}*/

int	mtSQLEnv::getProject(char* vendorId,char* patchId,mtQueueUser::ProjectInfo* pkProjectInfo)
{
	try
	{
		session sql(*m_pkDBPool);
		indicator ind = i_truncated;
		sql << "select * from project where vid = :vendorId and pid = :patchId limit 1",
			use(atoi(vendorId)),use(atoi(patchId)),into(*pkProjectInfo,ind);

		if(ind != i_ok)
			return DB_OPERATING_FAIL;
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from getProject",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}

//获取用户信息
int	mtSQLEnv::getUserInfo(char* account,long type,mtQueueUser::UserInfo* pkUserInfo)
{
	try
	{
		session sql(*m_pkDBPool);
		indicator ind = i_truncated;
		if(mtQueueUser::E_ACCOUNT_TYPE_ID == type)
		{
			sql << "select * from userinfo where id = :account limit 1",use((std::string)account),into(*pkUserInfo,ind);
		}
		else if(mtQueueUser::E_ACCOUNT_TYPE_ACCOUNT == type)
		{
			sql << "select * from userinfo where accountNum = :account limit 1",use((std::string)account),into(*pkUserInfo,ind);
		}
		else if(mtQueueUser::E_ACCOUNT_TYPE_PHONE == type)
		{
			sql << "select * from userinfo where phone = :account limit 1",use((std::string)account),into(*pkUserInfo,ind);
		}
		else if(mtQueueUser::E_ACCOUNT_TYPE_QQ == type)
		{
			sql << "select * from userinfo where qq = :account limit 1",use((std::string)account),into(*pkUserInfo,ind);
		}

		if(ind != i_ok)
			return DB_OPERATING_FAIL;
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from getUserInfo",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}

//保存用户信息
int mtSQLEnv::saveUserInfo(mtQueueUser::UserInfo* pkUserInfo)
{
	try
	{
		session sql(*m_pkDBPool);
		indicator ind = i_truncated;
		sql << "insert into userinfo(id,vendorId,batchId,companyCode,iconId,sceneId,accountNum,phone,qq,pwd,name,nickName,love,remark,sex,age,"\
			"gold,score,level,payCount,firstCharge,dayChess,allChess,winChess,winRate,offLineCount,registerDate,loginHallDate,continueLoginDays,loginFirstToday," \
			"giveGoldFrequence,chessPalyed) values(:id,:vendorId,:batchId,:companyCode,0,0,:accountNum,:phone,:qq,:pwd,'',:nickName,'',''," \
			"0,0,1000,0,0,0,0,0,0,0,0,0,now(),'1970-01-01 00:00:00',1,0,0,'0,0,0,0,0,0')",use(*pkUserInfo,ind);

		if(ind != i_ok)
			return DB_OPERATING_FAIL;
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from saveUserInfo",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}
//绑定其他账号
int mtSQLEnv::bindingUserInfo(char* id,char* phone,char* qq)
{
	try
	{
		session sql(*m_pkDBPool);
		//indicator ind = i_truncated;
		sql << "update userinfo set phone=:phone,qq=:qq where id=:id",
			use((std::string)phone),use((std::string)qq),use((std::string)id);

		//if(ind != i_ok)
			//return DB_OPERATING_FAIL;
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from bindingUserInfo",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}
int mtSQLEnv::isBingdingUserInfo(char* phone,char* qq)
{
	try
	{
		session sql(*m_pkDBPool);
		int count = 0;
		indicator ind = i_truncated;
		if(strcmp(phone,"") != 0 && strcmp(qq,"") !=0)
		{
			sql << "select count(*) from userinfo where phone=:phone or qq=:qq; ",
				use((std::string)phone),use((std::string)qq),into(count,ind);
			if(ind == i_ok){
				if(count > 0)
					return DB_OPERATING_FAIL_ALL_IS_EXIST;
			}
		}
		else if(strcmp(phone,"") != 0)
		{	
			sql << "select count(*) from userinfo where phone=:phone; ",use((std::string)phone),into(count,ind);

			if(ind == i_ok){
				if(count > 0)
					return DB_OPERATING_FAIL_PHONE_IS_EXIST;
			}
		}else if(strcmp(qq,"") !=0 )
		{
			sql << "select count(*) from userinfo where and qq=:qq;",use((std::string)qq),into(count,ind);
			if(ind == i_ok){
				if(count > 0)
					return DB_OPERATING_FAIL_QQ_IS_EXIST;
			}
		}
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from isBingdingUserInfo",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}
//修改用户密码
int mtSQLEnv::updatePassWord(char* id,char* userOldPwd,char* userNewPwd)
{
	try
	{
		session sql(*m_pkDBPool);
		//indicator ind = i_truncated;
		sql << "update userinfo set pwd=:userNewPwd where id=:id and pwd=:userOldPwd",
			use((std::string)userNewPwd),use((std::string)id),use((std::string)userOldPwd);

		//if(ind != i_ok)
			//return DB_OPERATING_FAIL;
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from updatePassWord",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_SUCCESS;
}

int mtSQLEnv::getSuperName(char* sName)
{
	try
	{
		session sql(*m_pkDBPool);
		string supername;
		indicator ind = i_truncated;
		sql << "SELECT surname FROM surname ORDER BY rand() LIMIT 1; ",into(supername,ind);
		if(ind == i_ok){
			strcpy(sName ,supername.c_str());
			return DB_OPERATING_SUCCESS;
		}
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from getSuperName",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_FAIL;
}
int mtSQLEnv::getLastName(char* lastName)
{
	try
	{
		session sql(*m_pkDBPool);
		string name;
		indicator ind = i_truncated;
		sql << "SELECT name FROM lastname ORDER BY rand() LIMIT 1; ",into(name,ind);
		if(ind == i_ok){
			strcpy(lastName ,name.c_str());
			return DB_OPERATING_SUCCESS;
		}
	}
	catch (std::exception const &e)
	{
		MT_DEBUG_PTCL("\nSQLERROR: [%s] from getLastName",e.what());
		errorMsg = e.what();
		return DB_OPERATING_FAIL_ERROR;
	}

	return DB_OPERATING_FAIL;
}

int mtSQLEnv::getName(char* name)
{
	char supername[8],lastname[8];
	getSuperName(supername);
	getLastName(lastname);
	sprintf(name,"%s%s",supername,lastname);
	return 1;
}

const char* mtSQLEnv::error()
{
	return this->errorMsg;
}

int mtSQLEnv::exit()
{
	if (NULL != m_pkDBPool)
	{
		int		iConnectIdx;
		for(iConnectIdx = 0; iConnectIdx<iPoolSize; iConnectIdx++){
			session &sessionConn = m_pkDBPool->at(iConnectIdx);
			sessionConn.close();
		}
		delete m_pkDBPool;
		m_pkDBPool = NULL;
	}
	return 1;
}

int mtSQLEnv::exception()
{
	switch (m_iExceptionId)
	{
		MT_EXCETPION_STRING(EXCEPTION_DB_POOL_NEW)
		MT_EXCETPION_STRING(EXCEPTION_DB_CONNECT_NEW)
		
	default:
		MT_DEBUG_PTCL( "该异常没有被处理");
		return	0;
	}

	return	1;
}

int mtSQLEnv::run()
{
	return	1;
}