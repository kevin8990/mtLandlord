#ifndef 	__MT_SERVICE_MATCH_SIGN_UP_H
#define 	__MT_SERVICE_MATCH_SIGN_UP_H
#include "mtService.h"
#include "mtQueueHall.h"
//比赛报名协议
class 	mtServiceMatchSignUp : public mtService
{
public:
	struct DataRead
	{
		long 				lStructBytes;			/// 包大小
		long                lServiceType;			/// 服务类型
		long 				plReservation[2];		/// 保留字段
		long                lUserId;                /// 用户id > 0
		long				lRoomId;                /// 房间id
		long				lFee;                   /// 费用
		long				lType;                  /// 类别（0：金币,1：花费）
	};

	struct DataWrite
	{
		long 				lStructBytes;			/// 包大小
		long                lServiceType;			/// 服务类型
		long 				plReservation[2];		/// 保留字段				
		long				chip;   			    /// 筹码
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
	mtServiceMatchSignUp();
	virtual ~mtServiceMatchSignUp();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void  * getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);

	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
};

#endif 	/// __MT_SERVICE_MATCH_SIGN_UP_H

