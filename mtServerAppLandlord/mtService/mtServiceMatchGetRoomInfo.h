#ifndef 	__MT_SERVICE_MATCH_GET_ROOM_INFO_H
#define 	__MT_SERVICE_MATCH_GET_ROOM_INFO_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 24拉取比赛房间信息协议
class 	mtServiceMatchGetRoomInfo : public mtService
{
public:
	struct DataRead
	{
		long 				lStructBytes;		/// 包大小
		long                lServiceType;		/// 服务类型
		long 				plReservation[2];	/// 保留字段
		long                lUserId;            /// 用户id > 0
	};

	struct DataWrite
	{
		long 				      lStructBytes;		/// 包大小
		long                      lServiceType;		/// 服务类型
		long 				      plReservation[2];	/// 保留字段				
		mtQueueHall::MatchRoom    pkRoom[4];   		/// 房间信息
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
	mtServiceMatchGetRoomInfo();
	virtual ~mtServiceMatchGetRoomInfo();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void  * getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	void   getActiveTime(long *lActiveTime,tm &tmStart,tm &tmEndTime,long ldayTime);
	bool   bMatchOpen(long ldayTime);
	long   getMatchStatus(tm tmStart,tm tmEnd,long ldayTime);
	long   getBeginHour(long ldayTime);
	long   getEndHour(long ldayTime);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_MATCH_GET_ROOM_INFO_H

