#ifndef		__MT_SERVICE_CONNECT_H
#define		__MT_SERVICE_CONNECT_H
#include "mtQueueWork.h"
#include "mtService.h"
///#include "mtSQLQDBC.h"
#include "mtQueueList.h"
#include "mtQueueMgr.h"

/// 连接大厅服务
class mtServiceConnect	: public mtService
{

public:
// 	enum 
// 	{
// 		E_LOGIN_HALL_TYPE_BEG          = 0,
// 		E_LOGIN_HALL_TYPE_NO_ID        = E_LOGIN_HALL_TYPE_BEG,    /// 没Id登录(初始登录)
// 		E_LOGIN_HALL_TYPE_ID,                                      /// Id登录
// 		E_LOGIN_HALL_TYPE_ACCOUNT,                                 /// 账号登录 XL0000 游客， 其他为用户注册账号
// 		E_LOGIN_HALL_TYPE_PHONE,                                   /// 手机号码
// 		E_LOGIN_HALL_TYPE_QQ,                                      /// QQ号码
// 
// 		E_LOGIN_HALL_TYPE_END
// 	};

	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long 							plVersion[4];			/// 版本号 0修改基本bug， 1表示增加功能， 2表示重大修改
		long                            lUserId;                /// 用户id > 0
		long							lBatchId;				/// 批次号id
		long							lVendorId;				/// 厂商代码
		long                            lRegister;              /// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间
		long                            lUserRegisterDate;      /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lResult;                /// 登录大厅结果(0 -账号错误，1 -登录成功)
		mtQueueHall::UserInfo           kUserInfo;              /// 用户信息
		mtQueueHall::DataSpace          pkSpace[2];   			/// 场信息
	};

	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		/// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};

public:
	mtServiceConnect();
	virtual ~mtServiceConnect();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);
	void*   getUserNodeAddr(long lUserId);
	static  void calcContinueLoginHall(mtQueueHall::UserInfo* pkUserInfo);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);
	void   UpdateUserProp(long lUserId,long *plUserPropsCount);
public:
	mtQueuePacket *                   m_pkQueuePacket;
	mtQueueHall *                     m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv *		                  m_pkSQLEnv;
	long							  m_lBatchId;				/// 批次号id
	long							  m_lVendorId;				/// 厂商代码
};

#endif	///	__MT_SERVICE_CONNECT_H