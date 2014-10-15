#ifndef			__MT_SQLENV_H_
#define			__MT_SQLENV_H_

#include "soci.h"
#include "soci-mysql.h"

#include "mtException.h"
#include "mtDefine.h"
#include "mtGlobal.h"

#include "mtQueueUser.h"

using namespace soci;
using namespace std;

class mtSQLEnv : public mtException
{
public:
	enum  
	{
		EXCEPTION_BEG	= 0,
		EXCEPTION_DB_POOL_NEW,
		EXCEPTION_DB_CONNECT_NEW,

		EXCEPTION_END
	};

	enum DBResult
	{
		DB_OPERATING_BEG,
		DB_OPERATING_SUCCESS					= 1,
		DB_OPERATING_FAIL						= 0,
		DB_OPERATING_FAIL_PHONE_IS_EXIST		= -1,
		DB_OPERATING_FAIL_QQ_IS_EXIST			= -2,
		DB_OPERATING_FAIL_ALL_IS_EXIST			= -3,
		DB_OPERATING_FAIL_ERROR					= -4
	};

	/*struct UserInfo
	{
		long                            lUserId;                /// 用户id	0 表示失败
		char							pcVendorId[6];			/// 厂商id
		char							pcPatchId[4];			/// 批次号id
		char							pcCompanyId[22];		/// 公司代码
		//long                            lUserIconId;            /// 用户头像id
		char                            pcUserAccountNum[12];   /// 用户账号
		char                            pcUserPhone[12];        /// 手机号码
		char                            pcUserQQ[12];           /// QQ号码
		char                            pcUserNickName[12];     /// 用户昵称
		//char                            pcUserName[12];         /// 用户姓名
		char                            pcUserPwd[12];          /// 用户密码
		//char                            pcUserLove[32];         /// 兴趣爱好
		//char                            pcUserRemark[64];       /// 备注说明
		//long                            lUserSex;               /// 性别 (0--男，1--女)
		//long                            lUserAge;               /// 年龄
		//long                            lUserGold;              /// 用户拥有金币数
		//long 							  lUserScore;				/// 用户积分
		//long                            lUserLevel;             /// 用户等级
		//long                            lUserAllChess;          /// 总局数
		//long                            lUserWinChess;          /// 胜局数
		//long                            lUserWinRate;           /// 胜率
		//long                            lUserOffLineCount;      /// 掉线次数
		long                            lIRegister;             /// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间
		long                            lUserRegisterDate;      /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
		long                            lUserLoginHallDate;     /// 用户最近一次登录的日期(XXXX-XX-XX)
		//long                            lUserContinueLoginDays; /// 用户连续登录的天数
		//long                            lUserLoginFirstToday;   /// 是否是今天的第一次登录(0 -不是今天第一次， 1 -是今天第一次)
		//long                            plPropsCount[10];       /// 用户道具数目
		//long                            plUserChessPalyed[6];   /// 用户当天在每个房间里玩游戏的局数
		//long                            lUserGiveGoldFrequence; /// 每天赠送给用户分数的次数
	};*/

public:
	mtSQLEnv(void);
	~mtSQLEnv(void);

	int		init(/*DataInit* pkDataInit*/);
	int		exit();
	int		run();

	virtual int exception();

	//int		saveHallFeeBack(DataRead* pkDataRead);					//反馈日志存储

	int		getProject(char* vendorId,char* patchId,mtQueueUser::ProjectInfo* pkProjectInfo);

	int		getUserInfo(char* account,long type,mtQueueUser::UserInfo* pkUserInfo);		//用户信息获取
	int		saveUserInfo(mtQueueUser::UserInfo* pkUserInfo);					//用户信息存储

	int		bindingUserInfo(char* id,char* phone,char* qq);						//绑定其他账号
	int		isBingdingUserInfo(char* phone,char* qq);							//phone和qq是否绑定
	int		updatePassWord(char* id,char* userOldPwd,char* userNewPwd);		//修改密码

	int		getSuperName(char* superName);						//随机获取姓
	int		getLastName(char* lastName);						//随机获取名
	int     getName(char* name);

	const char*	error();

private:
	connection_pool*	m_pkDBPool;						//线程池
	const char*			errorMsg;						//错误信息
	int					iPoolSize;
	
};

#endif