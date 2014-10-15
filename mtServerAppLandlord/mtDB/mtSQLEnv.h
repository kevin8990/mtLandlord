#ifndef			__MT_SQLENV_H_
#define			__MT_SQLENV_H_

#include "soci.h"
#include "soci-mysql.h"

#include "mtException.h"
#include "mtPlatform.h"

#include "mtServiceHallFeedBack.h"
#include "mtQueueHall.h"

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

public:
	mtSQLEnv(void);
	~mtSQLEnv(void);

	int		init(/*DataInit* pkDataInit*/);
	int		exit();
	int		run();

	virtual int exception();

	int		saveHallFeeBack(mtServiceHallFeedBack::DataRead* pkDataRead);		//反馈日志存储
	int		saveOnlinePerson(mtQueueHall::DataHall* m_kDataHall);
	int		savePayInfo(long userId,long point,long fee);						//保存充值金额

	int		getPrizeInfo(mtQueueHall::PrizeInfo* m_kPrizeInfo);						//获取抽奖参数
	int		getPrizeInfoList(mtQueueHall::PrizeInfo* m_kPrizeInfoList);				//获取抽奖列表

	int		getLotteryinfo(long userId,mtQueueHall::LotteryInfo* m_kLotteryInfo);		//用户抽奖信息获取
	int		saveLotteryinfo(mtQueueHall::LotteryInfo* m_kLotteryInfo);				//用户抽奖信息存储
	int		updateLotteryinfo(mtQueueHall::LotteryInfo* m_kLotteryInfo);				//用户抽奖信息更新

	int		getPigInfo(long userId,mtQueueHall::PigInfo* m_kPigInfo);					//获取金猪信息
	int		savePigInfo(mtQueueHall::PigInfo* m_kPigInfo);
	int		updatePigInfo(mtQueueHall::PigInfo* m_kPigInfo);

	int		getUserPropInfoList(long userId,mtQueueHall::UseTimeInfo* m_kPropInfoList,long* size);		//获取用户道具信息
	int		getUserPropInfo(long userId,long propId,mtQueueHall::UseTimeInfo* pkUserTimeInfo);
	int		saveUserPropInfo(long userId,long propId,std::tm stopTime);
	int		updateUserPropInfo(long userId,long propId,std::tm stopTime);

	int		getTaskInfo(long taskId,mtQueueHall::TaskInfo* m_kTaskInfo);
	int		getTaskInfoList(mtQueueHall::TaskInfo* m_kTaskInfoList);												//获取任务列表
	int		getUserTaskPrizeInfoList(long userId,mtQueueHall::UseTimeInfo* m_kTaskPrizeInfoList,long* size);	//获取任务领奖信息										//获取用户任务领奖信息
	int		getUserTaskPrizeInfo(long userId,long taskId,mtQueueHall::UseTimeInfo* m_kTaskPrizeInfo);		//获取任务领奖信息
	int		saveUserTaskPrizeInfo(long userId,long taskId);					//保存任务领奖信息
	int		updateUserTaskPrizeInfo(long userId,long taskId);					//更新任务领奖信息

	int		saveAwardInfo(mtQueueHall::AwardInfo* m_kAwardInfo);					//得奖信息保存

	int		getLastNoticeInfo(char* content);										//获取最新的公告
	int		saveSystemMessage(char* content);
	int		getSystemMessageList(mtQueueHall::MessageInfo* m_kMessageInfo);

	int		getUserInfo(long userId,mtQueueHall::UserInfo* pkUserInfo);		//用户信息获取
	int		saveUserInfo(mtQueueHall::UserInfo* pkUserInfo);					//用户信息存储
	int		updateUserInfo(mtQueueHall::UserInfo* pkUserInfo);					//用户信息更新
	
	int		getProject(long vendorId,long patchId,mtQueueHall::ProjectInfo* pkProjectInfo);
	
	int		getPKRoomList(mtQueueHall::PKRoomInfo* pkPKRoomInfoList,long size);				//拉取比赛场房间
	int		getPKRoom(long roomId,mtQueueHall::PKRoomInfo* pkPKRoomInfo);

	int		getUserPKInfo(long userId,long roomId,mtQueueHall::UserPKInfo* pkUserPKInfo);
	int		saveUserPKInfo(mtQueueHall::UserPKInfo* pkUserPKInfo);
	int		updateUserPKInfo(mtQueueHall::UserPKInfo* pkUserPKInfo);

	int		getPKRoomRankList(long roomId,mtQueueHall::DataRank* pkPKRoomRankList,long* size);
	int		getPKUserRankInfo(long userId,long roomId,mtQueueHall::DataRank* pkPKRoomRank);

	const char*	error();

private:
	connection_pool*	m_pkDBPool;						//线程池
	const char*			errorMsg;						//错误信息
	int					iPoolSize;
};

#endif