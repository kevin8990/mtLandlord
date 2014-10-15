#ifndef 	__MT_SERVICE_LOTTERY_FEED_PET_H
#define 	__MT_SERVICE_LOTTERY_FEED_PET_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 喂养宠物协议
class 	mtServiceFeedPet : public mtService
{
public:
	struct DataRead
	{
		long		lStructBytes;		// 包大小
		long        lServiceType;	    // 服务类型
		long		plReservation[2];	// 保留字段
		long		lUserId;            // 用户id
	};
	struct DataWrite
	{
		long		lStructBytes;		// 包大小
		long		lServiceType;		// 服务类型
		long		plReservation[2];	// 保留字段
		long		lResult; 
		long		llevel;			    // 等级
		long		lexperience;		// 经验
		long		lmash;			    // 饲料 
		long		lnextexperience;	// 下一等级所需经验
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
	mtServiceFeedPet();
	virtual ~mtServiceFeedPet();

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

#endif 	/// __MT_SERVICE_LOTTERY_FEED_PET_H

