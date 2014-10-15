#ifndef 	__MT_SERVICE_ROOM_WAIT_START_RECV_H
#define 	__MT_SERVICE_ROOM_WAIT_START_RECV_H
#include "mtService.h"
#include "mtQueueMgr.h"
#include "mtServiceMgr.h"

/// 用户点击"开始"，开始等待服务
class 	mtServiceRoomWaitStartRecv : public mtService
{
public:
	struct DataInit
	{
		long				    lStructBytes;
		HANDLE	                hIOCP;
		mtQueueMgr*             pkQueueMgr;
		mtServiceMgr*	        pkServiceMgr;
	};

	struct DataRead
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		/// 保留字段
	};

	struct DataWrite
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		/// 保留字段
		long 				lUserId;
	};
public:
	mtServiceRoomWaitStartRecv();
	virtual ~mtServiceRoomWaitStartRecv();

	virtual int init(void* pData);
	virtual int run(DataRun* pkDataRun);
	virtual int exit();

	int getQueueUserNextQueueUser(int iQueueUserCurIdx);
	 
	/// ai’开始等待‘服务响应
	int playWaitStartRecvAi(mtQueueDesk::DataNode* pkQueueDeskDataNode, long lServiceType, int iQueueUserIdx);
	int playReplaceAiExitRoom(mtQueueDesk::DataNode* pkQueueDeskDataNode);
	int playRoundOver(mtQueueUser::DataNode* pkQueueUserDataNode);

	HANDLE	                m_hIOCP;
	mtQueueMgr*             m_pkQueueMgr;
	mtServiceMgr*	        m_pkServiceMgr;
};

#endif 	/// __MT_SERVICE_ROOM_WAIT_START_RECV_H

