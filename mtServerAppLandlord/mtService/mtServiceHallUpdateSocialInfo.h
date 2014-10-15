#ifndef 	__MT_SERVICE_HALL_UPDATE_SOCIAL_INFO_H
#define 	__MT_SERVICE_HALL_UPDATE_SOCIAL_INFO_H
#include "mtService.h"
#include "mtQueueHall.h"

class 	mtServiceHallUpdateSocialInfo : public mtService
{
public:
	struct DataRead
	{
		long 							lStructBytes;			/// 包大小
		long                        	lServiceType;			/// 服务类型
		long 							plReservation[2];		/// 保留字段
		long                            lUserId;                /// 用户id
		long                            lUserIconId;            /// 用户头像id
		long                            lUserSceneId;               ///场景ID
		long                            lUserSex;               /// 性别 (0--男，1--女)
		long                            lUserAge;               /// 年龄
		char                            pcUserName[12];         /// 用户姓名
		char                            pcUserNickName[12];     /// 用户昵称
		char                            pcUserPwd[12];          /// 用户密码
		char                            pcUserLove[32];         /// 兴趣爱好
		char                            pcUserRemark[64];       /// 备注说明
		char                            pcUserPhone[12];        /// 手机号码
		char                            pcUserQQ[12];           /// QQ号码
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
	mtServiceHallUpdateSocialInfo();
	virtual ~mtServiceHallUpdateSocialInfo();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void*   getUserNodeAddr(long lUserId);
    void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);

	int     updateDataRank(mtQueueHall::DataRank* pkDataRank);
	void    UpdateUserInfo(mtQueuePacket::DataNode* pkQueuePacketDataNode);

	mtQueuePacket*                   m_pkQueuePacket;
	mtQueueHall*                     m_pkQueueHall;   		/// 大厅信息

	mtSQLEnv*		m_pkSQLEnv;





};

#endif 	/// __MT_SERVICE_HALL_UPDATE_SOCIAL_INFO_H

