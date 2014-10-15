#ifndef		__MT_SERVICE_H
#define		__MT_SERVICE_H
#include "mtQueueUser.h"
#include "mtQueuePacket.h"

class mtService
{
public:
	struct DataUserInfo
	{
		long 							lUserId;				/// 用户Id
		long                            lUserIconId;            /// 用户头像id
		long                            lUserSex;               /// 性别 (0--男，1--女)
		long                            lUserGold;              /// 用户拥有金币数
		long                            lUserLevel;             /// 用户等级
		long 							lUserScore;				/// 用户积分
		long                            lUserAllChess;          /// 总局数
		long                            lUserWinChess;          /// 胜局数
		long                            lUserWinRate;           /// 胜率
		long                            lUserOffLineCount;      /// 掉线次数
		long 							lIsWaitStart;			/// 标记是否开始
		long							lPlayerType;	        /// 每个人的类型(初始值是1， 0 - landlord, 1 - farmer)
		unsigned long					ulDeskId;               /// 桌子标识
		long                            plPropsCount[16];       /// 用户道具数目
		char                            pcUserAccountNum[12];   /// 用户账号
		char                            pcUserName[12];         /// 用户姓名
		char                            pcUserNickName[12];     /// 用户昵称
	};

	struct DataRun
	{
		long						lStructBytes;
		mtQueueUser::DataNode*		pkQueueUserDataNode;
		mtQueuePacket::DataNode*	pkQueuePacketDataNode;
	};

	mtService();
	virtual ~mtService();

	virtual int init(void* pData) = 0;
	virtual int	run(DataRun* pkDataRun) = 0;
	virtual int exit() = 0;

};

#endif	///	__MT_SERVICE_H