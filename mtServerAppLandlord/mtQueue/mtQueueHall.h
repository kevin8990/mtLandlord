#ifndef		__MT_QUEUE_HALL_H
#define		__MT_QUEUE_HALL_H
#include "mtPlatform.h"
#include "mtDSQueue.h"
class  mtSQLEnv;
class mtQueueHall
{
public:
	enum
	{
		E_HALL_USER_STATUS_BEG           = 0,
		E_HALL_USER_STATUS_OFFLINE       = E_HALL_USER_STATUS_BEG, /// 离线
		E_HALL_USER_STATUS_ONLINE,                                 /// 在线(在大厅或者房间)
		E_HALL_USER_STATUS_HALL,                                   /// 当前在大厅(在线)
		E_HALL_USER_STATUS_ROOM,                                   /// 当前在某个房间里面(在线)
		E_HALL_USER_STATUS_ROOM_FORCE_EXIT,                        /// 当前在某个房间里面(在线,用户在游戏已经开始时，强行退出)
		E_HALL_USER_STATUS_ROOM_NETWORK_EXIT,                      /// 当前在某个房间里面(在线,用户在游戏已经开始时，由于网络原因退出)

		E_HALL_USER_STATUS_END
	};
	enum MatchRoomStatus
	{
		OPEN_SOON   = 0,
		IS_OPEN     = 1,
		HAS_ENDED   = -1,
	};
	enum MatchRoomCurrentStatus
	{
		OPEN   = 1,
		CLOSE  = 0
	};
	enum UserRankStatus
	{
		NOT_END     = -1,
		NO_WINNERS  = 0,
		NO_AWARD    = 1,
		HAS_AWARD   = -2
	};
	struct PKRoomInfo	//比赛场房间数据
	{

		long                            lRoomId;                /// 房间id
		char                            pcRoomName[32];         /// 房间名称
		char							pcRoomlP[15];			/// 房间服务器的IP
		long 							lRoomPort;				/// 房间服务器的端口
		long                            lante;					/// 房间底分(筹码)
		long                            llowestchip;			/// 房间准入分(筹码)
		char							pcRoomImg[80];			//比赛图片
		std::tm							lStartTime;				//开始时间
		long							lActiveTime;			//活动时间	
		std::tm							lEndTime;				//结束时间
		long							lFee;					//包名费用
		long							lFeeType;				//费用类别金币或者人民币
		long							lInitChip;				//初始筹码
		char							pcPrizeName[20];		//奖品
		long							lAwardNum;				//奖励人数
		char							pcAwardcontent[30];		//奖励规则
		//long							lActiveStatus;			//(即将开放、进行中、活动结束)
		//long							lCurStatus;				//是否开启
	};

	struct UserPKInfo		//用户比赛数据
	{
		long							lUserId;			//用户ID
		long							lRoomId;			//比赛场房间ID
		char							pcNickName[12];     //昵称
		long							lChip;				//筹码
		long							lScore;				//积分
		char							pcMobile[12];		//联系电话
		long							lStatus;			//得奖状态：未总结,未得奖,未领奖,已领奖
	};

	struct MatchRoom
	{
		long 				lStructBytes;		/// 包大小
		long                lRoomId;            /// 房间id
		char                pcRoomName[32];     /// 房间名称
		unsigned long		ulRoomlP;		    /// 房间服务器的IP
		long 				lRoomPort;		    /// 房间服务器的端口
		long                lPersonNumber;      /// 房间当前人数
		long                lGoldBase;          /// 房间底分(金币)
		long                lGoldAdmittance;    /// 房间准入分(金币)
		long                lGoldDeduct;        /// 0每局扣除分(金币)号
		char				pcRoomImg[80];		/// 比赛图片
		long				lActiveTime[3];		/// 活动时间						
		long				lFee;			    /// 包名费用
		long				lFeeType;		    /// 费用类别金币或者人民币
		long                lIntiChip;          //初始筹码
		char				pcPrizeName[20];	/// 奖品
		char				pcAwardcontent[30];	/// 奖励规则
		long				lcurstatus;		    /// 是否开启(1开启，0关闭)
		long				lstatus;		    /// (0即将开放、1进行中、-1活动结束)
	};


	struct DataRoom
	{
		long 							lStructBytes;			/// 包大小
		long                            lSpaceId;               /// 场id
		long                            lRoomId;                /// 房间id
		char                            pcRoomName[32];         /// 房间名称
		unsigned long 					ulRoomlP;			    /// 房间服务器的IP
		long 					        lRoomPort;		        /// 房间服务器的端口
		long                            lPersonNumber;          /// 房间当前人数
		long                            lGoldBase;              /// 房间底分(金币)
		long                            lGoldAdmittance;        /// 房间准入分(金币)
		long                            lGoldDeduct;            /// 每局扣除分(金币)号
	};

	struct DataSpace
	{
		long 							lStructBytes;			/// 包大小
		long                            lSpaceId;               /// 场id
		char                            pcSpaceName[32];        /// 场名称
		DataRoom					    pkRoom[6];
	};

	struct UserInfo
	{
		long                            lUserId;                /// 用户id	0 表示失败
		//long							lBatchId;				/// 批次号id
		//long							lVendorId;				/// 厂商代码
		long                            lUserIconId;            /// 用户头像id
		long                            lUserSceneId;               ///场景ID
		char                            pcUserAccountNum[12];   /// 用户账号
		char                            pcUserName[12];         /// 用户姓名
		char                            pcUserNickName[12];     /// 用户昵称
		char                            pcUserPwd[12];          /// 用户密码
		char                            pcUserLove[32];         /// 兴趣爱好
		char                            pcUserRemark[64];       /// 备注说明
		char                            pcUserPhone[12];        /// 手机号码
		char                            pcUserQQ[12];           /// QQ号码
		long                            lUserSex;               /// 性别 (0--男，1--女)
		long                            lUserAge;               /// 年龄
		long                            lUserGold;              /// 用户拥有金币数
		long 							lUserScore;				/// 用户积分
		long                            lUserLevel;             /// 用户等级
		long							lUserPayCount;				/// 用户总充值
		long							lUserFirstCharge;			/// 用户首充奖励金币	
		long							lUserDayChess;				/// 当日局数
		long                            lUserAllChess;          /// 总局数
		long                            lUserWinChess;          /// 胜局数
		long                            lUserWinRate;           /// 胜率
		long                            lUserOffLineCount;      /// 掉线次数
		//long                            lIRegister;             /// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间
		long                            lUserRegisterDate;      /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
		long                            lUserLoginHallDate;     /// 用户最近一次登录的日期(XXXX-XX-XX)
		long                            lUserContinueLoginDays; /// 用户连续登录的天数
		long                            lUserLoginFirstToday;   /// 是否是今天的第一次登录(0 -不是今天第一次， 1 -是今天第一次)
		long                            plPropsCount[16];       /// 用户道具数目
		long                            plUserChessPalyed[6];   /// 用户当天在每个房间里玩游戏的局数
		long                            lUserGiveGoldFrequence; /// 每天赠送给用户分数的次数
	};

	struct ProjectInfo
	{
		long            lVendorId;				/// 厂商代码
		long            lBatchId;				/// 批次号id
		char			pcRoomServer[15];			/// 房间地址
		long			lRoomPort;					/// 房间端口
	};

	struct Message
	{
		char		content[80];
	};

	struct MessageInfo			//系统消息
	{
		Message		pkMessage[10];
	};

	struct TaskInfo
	{
		char							pcTaskName[20];		//任务名称
		long							ldemand;			//要求
		long							awardgold;			//奖励金币
		char							cType;				//类型 1为当日对战局数,2为总对战局数,3为总胜局数,4为总充值数
		char							cIsUse;				//是否已领取1已领取，0未领取
	};

	struct UseTimeInfo
	{
		long							lId;					///	ID
		std::tm							tUseTime;				/// 时间
	};

	struct PigInfo		//宠物猪
	{
		long                            lUserId; 
		long							lLevel;					/// 等级
		long							lexperience;			/// 经验
		long							lMash;					/// 剩余饲料
		std::tm							tUseTime;				/// 吐金币时间
	};

	struct PrizeInfo			//奖品信息
	{
		long                            lNeedGold;				/// 抽奖所需金币
		long                            lMaxTimes;				///	最大抽奖次数
		long							lVipTimes;				/// Vip最大的抽奖次数
		long                            lLuckPrize;				/// 幸运值满后的奖品
		long                            lProbability;			/// 抽奖阀值
		long                            lPrize;					/// 奖品序号
	};

	struct LotteryInfo			//用户抽奖信息
	{
		long                            lUserId; 
		long							lSumTimes;				/// 抽奖总次数
		long							lSumGold;				/// 抽花费总金币
		long							lUseTimes;				/// 可以抽奖次数
		long							lLuck;					/// 幸运值
		long                            lFeeCard;               ///奖劵
		std::tm							tUseTime;				/// 抽奖的时间
	};

	struct AwardInfo			//得奖信息
	{
		long                            lUserid;
		char                            pcPhone[MT_BYTES_OF_PHONE]; 
		long                            lPrize;					///	抽到的奖品
		long                            iIsUse;					/// 是否领取
	};

	struct UserDataNode
	{
		long							lStructBytes;
		long                            lIsOnLine;              /// 用户在线状态
		long                            lSpaceId;               /// 场id
		long                            lRoomId;                /// 房间id
		long                            lUserGoldBuy;           /// 用户购买的金币数
		long                            lUserGoldBuyStatus;     /// 用户购买金币的结果状态(0 -为更新，即还没收到收费服务器的状态更新，1 -已更新，即收到收费服务器的状态更新)
		UserInfo                        kUserInfo;              /// 用户信息
	};

	struct DataRank
	{
		long                            lUserId;                /// 用户id
		long                            lUserLevel;             /// 用户等级
		long 							lUserScore;				/// 用户积分
		char                            pcUserNickName[12];     /// 用户昵称
	};

	struct DataHall
	{
		long 							lStructBytes;			     /// 包大小
		DataSpace*                      pkSpace;   			         /// 场信息
		CRITICAL_SECTION	            kCriticalSectionDataRank;    /// 排行榜内存操作临界区
		DataRank*                       pkDataRank;                  /// 排行榜前10名信息
		UserDataNode*                   pkUserDataNodeList;          /// 用户信息
	};

	struct DataInit
	{
		long		lStructBytes;
		int			iSpinCount;
		char*       pcServiceExeDir;
		mtSQLEnv   *pkSQLEnv;
	};

public:
	mtQueueHall();

	~mtQueueHall();

	int    init(void* pData);
	int    exit(void);

	int    initSpaceRoom();
	int				             m_iSpinCount;
	mtQueueHall::DataHall        m_kDataHall;
	char*                        m_pcServiceExeDir;
	mtSQLEnv               *     m_pkSQLEnv;
	
};

#endif	///	__MT_QUEUE_HALL_H