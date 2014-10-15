#ifndef 	__MT_SERVICE_HALL_REGISTER_H
#define 	__MT_SERVICE_HALL_REGISTER_H
#include "mtQueueWork.h"
#include "mtService.h"

class mtQueuePacket;
class mtQueueHall;
class 	mtServiceHallRegister : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long 							lUserId;                /// 用户id(-1 -没有Id注册)
		long 							lUserSex;				/// 性别
		char 							pcUserAccountNum[12];	/// 账号
		char							pcUserPwd[12];			/// 密码
		char							pcUserPhoneNum[12];		/// 手机号码
		char							pcUserQQ[12];			/// QQ号码
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long 							lUserId;				/// 用户id, -1 表示注册失败， 成功 >= MT_SERVER_WORK_USER_ID_MIN
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueuePacket*                  pkQueuePacket;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
	};
public:
	mtServiceHallRegister();
	virtual ~mtServiceHallRegister();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);

	static  VOID CALLBACK FileIOCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息

};

#endif 	/// __MT_SERVICE_HALL_REGISTER_H
