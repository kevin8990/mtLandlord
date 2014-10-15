#ifndef		__MT_SERVICE_HALL_EXIT_H
#define		__MT_SERVICE_HALL_EXIT_H
#include "mtQueueWork.h"
#include "mtService.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"
/// 退出大厅服务
class mtServiceHallExit	: public mtService
{

public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		mtQueueHall::UserInfo           kUserInfo;              /// 用户信息
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 用户退出大厅结果，0 -用户信息更新失败， 1 -用户信息更新成功，2 -传入用户信息错误，3 -传入用户当前不在线
		long                            lUserId;                /// 用户id
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueuePacket*                  pkQueuePacket;
		mtQueueHall::UserDataNode*      pkUserDataNodeList;       /// 用户信息

		mtSQLEnv*		pkSQLEnv;
	};

public:
	mtServiceHallExit();
	virtual ~mtServiceHallExit();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	int     runLoginHallNoId(DataWrite* pkDataWrite);
	int     runLoginHallOther(SOCKET iSocket, DataWrite* pkDataWrite);

	void*   getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
public:
	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall::UserDataNode*       m_pkUserDataNodeList;       /// 用户信息

	mtSQLEnv*		m_pkSQLEnv;
};

#endif	///	__MT_SERVICE_HALL_EXIT_H