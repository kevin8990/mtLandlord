#ifndef 	__MT_SERVICE_HALL_UPDATE_PLAY_INFO_H
#define 	__MT_SERVICE_HALL_UPDATE_PLAY_INFO_H
#include "mtService.h"
#include "mtQueueHall.h"

class 	mtServiceHallUpdatePlayInfo : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			    /// 包大小
		long                        	lServiceType;			    /// 服务类型
		long 							plReservation[2];		    /// 保留字段
		long                            lUserId;                    /// 用户id
		long                            lUserGold;                  /// 用户拥有金币数
		long 							lUserScore;				    /// 用户积分
		long                            lUserLevel;                 /// 用户等级
		long                            lUserAllChess;              /// 总局数
		long                            lUserWinChess;              /// 胜局数
		long                            lUserWinRate;               /// 胜率
		long                            lUserOffLineCount;          /// 掉线次数
		long                            plPropsCount[16];           /// 用户道具数目
		long							plUserChessPlayedToday[6];	/// 游戏大厅6个房间的今天玩的次数
		char                            pcUserNickName[12];         /// 用户昵称
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long							lResult;                /// 更新用户信息结果(1 -成功，0 -失败， -1 -用户账号错误， -2 -用户不在线)
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*		pkSQLEnv;
	};

public:
	mtServiceHallUpdatePlayInfo();

	virtual ~mtServiceHallUpdatePlayInfo();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();

	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);

	int     updateDataRank(mtQueueHall::DataRank* pkDataRank);


	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv*				         m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_HALL_UPDATE_PLAY_INFO_H

