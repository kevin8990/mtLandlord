#ifndef		__MT_SERVICE_MGR_H
#define		__MT_SERVICE_MGR_H

#include "mtService.h"
#include "mtPlatform.h"
#include "mtQueueWork.h"
#include "mtSQLEnv.h"
#include "mtServiceConnect.h"

class mtServiceMgr
{
public:
	enum EServiceId
	{
		E_SERVICE_HALL_BEG	               = 0,
		E_SERVICE_HALL_CONNECT             = E_SERVICE_HALL_BEG,
		E_SERVICE_HALL_REGISTER ,
		E_SERVICE_HALL_PROP_BUY,                                      /// 2购买道具
		E_SERVICE_HALL_PROP_USE,                                      /// 3使用道具
		E_SERVICE_HALL_GOLD_BUY,                                      /// 4购买金币
		E_SERVICE_HALL_GOLD_BUY_STATUS,                               /// 5购买金币状态更新
		E_SERVICE_HALL_RANKING,                                       /// 6获得排行榜
		E_SERVICE_HALL_UPDATE_PALY_INFO,                              /// 7更新用户的账户信息(包括排行榜)
		E_SERVICE_HALL_UPDATE_SOCIAL_INFO,                            /// 8更新用户的基本资料信息
		E_SERVICE_HALL_GIVE_GOLD,                                     /// 9赠分
		E_SERVICE_HALL_UPDATE_ROOM,                                   /// 10房间信息定时更新
		E_SERVICE_HALL_HALL_2_ROOM,                                   /// 11用户请求从大厅进房间
		E_SERVICE_HALL_ROOM_2_HALL,                                   /// 12用户请求从房间返回大厅
		E_SERVICE_HALL_FEED_BACK,                                     /// 13用户信息反馈
		E_SERVICE_HALL_EXIT,								          /// 14退出大厅
		E_SERVICE_HALL_GETLOTTERYARG,								  /// 15取抽奖参数
		E_SERVICE_HALL_LOTTERY,										  /// 16抽奖
		E_SERVICE_HALL_SYSTEMMESSAGE,								  /// 17系统消息
		//新增协议
		E_SERVICE_HALL_GET_TASK_INFO,                                  /// 18获取任务信息
		E_SERVICE_HALL_GET_TASK_AWARD,                                 /// 19获取任务奖励
		E_SERVICE_HALL_GET_PET_INFO,                                   /// 20获取宠物协议
		E_SERVICE_HALL_FEED_PET,                                       /// 21喂养宠物
		E_SERVICE_HALL_SPIT_GOLD,                                      /// 22吐金币
		E_SERVICE_HALL_CASH_LOTTERY,                                   /// 23兑奖
		//新增协议(比赛场14-8-18)
		E_SERVICE_MATCH_HALL_GET_ROOM_INFO,                            /// 24拉取比赛房间信息协议
		E_SERVICE_MATCH_HALL_GET_USER_INFO,                            /// 25拉取用户比赛场数据协议
		E_SERVICE_MATCH_HALL_SIGN_UP,                                  /// 26比赛报名协议.
		E_SERVICE_MATCH_HALL_GET_ROOM_STATUS,                          /// 27拉取房间状态协议
		E_SERVICE_MATCH_HALL_GET_RANK,                                 /// 28比赛排名拉取协议
		E_SERVICE_MATCH_HALL_AWARD,                                    /// 29比赛领奖协议

		E_SERVICE_HALL_END
	};

	struct DataHead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
	};

	struct DataInit
	{
		long		           lStructBytes;
		long                   lSpinCount;
		mtQueueMgr*            pkQueueMgr;
		char*                  pcServiceExeDir;
		mtSQLEnv*		       pkSQLEnv;
	};

	struct DataRun
	{
		long		     lStructBytes;
		mtQueueWorkNode* pQueueWorkNode;
	};

public:
	mtServiceMgr();
	virtual ~mtServiceMgr();

	int			init(DataInit* pkDataInit);
	virtual int	run(void* pData);
	virtual int exit();

	int	runReadDataHead(SOCKET socket, DataHead* pkDataHead);
private:
	int			initService();
	int			initServiceLoginConnect();
	int			initServiceHallRegister();
	int			initServiceHallPropBuy();
	int			initServiceHallGoldBuy();
	int			initServiceHallGoldBuyStatus();
	int			initServiceHallRank();
	int			initServiceHallUpdatePlayInfo();
	int			initServiceHallUpdateSocialInfo();
	int			initServiceHallGiveGold();
	int			initServiceHallUpdateRoom();
	int			initServiceHallHall2Room();
	int			initServiceHallRoom2Hall();
	int			initServiceHallFeedBack();
	int			initServiceHallExit();
	int         initServiceGetLottertArg();
	int         initServiceLottery();
	int         initServiceSystemMessage();

	int         initServiceGetTaskInfo();
	int         initServiceGetTaskAward();
	int         initServiceGetPetInfo();
	int         initServiceFeedPet();
	int         initServiceSpitGold();
	int         iniServiceCashLottery();

	int         initServiceMatchGetRoomInfo();
	int         initServiceMatchGetUserInfo();
	int         initServiceMatchSignUp();
	int         initServiceMatchGetRoomStatus();
	int         initServiceMatchGetRank();
	int         initServiceMatchAward();

	long                         m_lSpinCount;
	mtQueueMgr*                  m_pkQueueMgr;
	char*                        m_pcServiceExeDir;
	mtSQLEnv       *		     m_pkSQLEnv;
	mtService*		             m_pkServiceList[E_SERVICE_HALL_END];

};

#endif	///	__MT_SERVICE_MGR_H