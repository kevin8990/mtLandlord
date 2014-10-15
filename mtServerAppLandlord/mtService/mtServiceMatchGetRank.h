#ifndef 	__MT_SERVICE_MATCH_GET_GET_RANK_H
#define 	__MT_SERVICE_MATCH_GET_GET_RANK_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 比赛排名拉取协议
class 	mtServiceMatchGetRank : public mtService
{
public:
	struct DataRead
	{
		long 				lStructBytes;			/// 包大小
		long                lServiceType;			/// 服务类型
		long 				plReservation[2];		/// 保留字段
		long                lUserId;                	/// 用户id > 0
		long				lRoomId;
	};

	struct Rank
	{
		char                            pcUserName[32];       	 /// 昵称
		long                            lScore;   		 /// 积分	
	};

	struct DataWrite
	{
		long 				lStructBytes;			/// 包大小
		long                lServiceType;			/// 服务类型
		long 				plReservation[2];		/// 保留字段				
		long				lUserRank;              /// 用户排名
		long				lUserScore;             /// 用户积分
		long				lStatus;                /// -1: 未结束，0:未得奖，1:未领奖，-2:已领奖
		Rank				pkRank[10];   			/// 前十排名
	};



	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};
public:
	mtServiceMatchGetRank();
	virtual ~mtServiceMatchGetRank();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void  * getUserNodeAddr(long lUserId);
	void    initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	long    getUserMatchStatus(long lUserid,long lRoomid,long lUserRank,long lawardNum,tm tmEnd,long ldayTime);
	bool    bMatchEnd(tm tmEnd,long ldayTime);
	time_t  getEndTime(tm tmEnd,long ldayTime);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_MATCH_GET_GET_RANK_H

