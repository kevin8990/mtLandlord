#ifndef 	__MT_SERVICE_SPIT_GOLD_H
#define 	__MT_SERVICE_SPIT_GOLD_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 吐金币协议
class mtServiceSpitGold : public mtService
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
		long		lgold;			    // 吐出的金币
	};
	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};
	enum Result
	{
		E_RESULT_SUCESS                 = 1,
		E_RESULT_HAVED                  = -1,
		E_RESULT_ERROR                  = 0,
	};
public:
	mtServiceSpitGold();
	virtual ~mtServiceSpitGold();

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

#endif 	/// __MT_SERVICE_SPIT_GOLD_H

