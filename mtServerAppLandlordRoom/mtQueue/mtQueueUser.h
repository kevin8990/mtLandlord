#ifndef		__MT_QUEUE_USER_H
#define		__MT_QUEUE_USER_H
#include "mtPlatform.h"
#include "mtDSQueue.h"
#include "mtQueuePacket.h"
#include "mtDSList.h"
// 
// struct mtListNode
// {
// 	void*	pPre;
// 	void*	pSelf;
// 	void*	pNext;
// };

class mtQueueUser 
{
public:
	enum 
	{
		E_LIST_NODE_BEG		= 0,
		E_LIST_NODE_HEART	= E_LIST_NODE_BEG,
		E_LIST_NODE_ONE,
		E_LIST_NODE_END
	};

	enum
	{
		E_SERVICE_ROOM_EXIT_STATUS_BEG         = 0,
		E_SERVICE_ROOM_EXIT_STATUS_INIT        = E_SERVICE_ROOM_EXIT_STATUS_BEG,
		E_SERVICE_ROOM_EXIT_STATUS_RUNNING,
		E_SERVICE_ROOM_EXIT_STATUS_CHANGE,
		E_SERVICE_ROOM_EXIT_STATUS_ROUND_OVER,
		E_SERVICE_ROOM_EXIT_STATUS_EXITING,
		E_SERVICE_ROOM_EXIT_STATUS_EXITED,
		E_SERVICE_ROOM_EXIT_STATUS_RELEASE,

		E_SERVICE_ROOM_EXIT_STATUS_END
	};

	struct DataNode
	{
		long							lStructBytes;
		SOCKET							uiSockets;
		long 							lSpaceId;				        /// 场Id
		long 							lRoomId;				        /// 房间Id
		long 							lUserId;				        /// 用户Id
		long                            lUserIconId;                    /// 用户头像id
		long                            lUserGold;                      /// 用户拥有金币数
		long                            lUserSex;                       /// 性别 (0--男，1--女)
		long                            lUserLevel;                     /// 用户等级
		long 							lUserScore;				        /// 用户积分
		long                            lUserAllChess;                  /// 总局数
		long                            lUserWinChess;                  /// 胜局数
		long                            lUserWinRate;                   /// 胜率
		long                            lUserOffLineCount;              /// 掉线次数
		long 							lIsWaitStart;			        /// 是否已经‘等待开始’ 0还没有等待开始 1已经等待开始了
		long                            lUserDeskIndex;                 /// 人员在桌子上的顺序,初始值是-1,(0,1,2)
		long                            lUserExit;                      /// 该用户当前状态(0 -初始状态，1 -正在运行，2 -正在退出，3 -已经退出)
		long                            lUserIOCPStatus;                /// 
		long                            plPropsCount[16];               /// 用户道具数目
		char                            pcUserAccountNum[12];           /// 用户账号
		char                            pcUserName[12];                 /// 用户姓名
		char                            pcUserNickName[12];             /// 用户昵称
		ULONGLONG				        ullTimeEnterRun;                /// 节点入链表时间
 		void*							pkQueueDeskDataNode;		    /// mtQueueDesk::DataNode*
 		mtQueuePacket::DataNode*		pkQueuePacketDataNodeHead;
		mtListNode						pkListNode[E_LIST_NODE_END];
	};

	struct DataInit
	{
		long		lStructBytes;
		int			iSpinCount;
		HANDLE		hIOCP;
	};

public:
	mtQueueUser();
	~mtQueueUser();

	int				init(void* pData);
	int				exit(void);

	bool            bHaveRepeat();

	DataNode*		pop();
	int				push(DataNode* pkDataNode);

	void*			popInitDataNode(SOCKET uiSocket);

	DataNode*		getBaseAddrFromAddr(void* pData);
	int				getNodeNumRemain();
	int				getNodeNumTotal();

	long			getNodeUseCount();


	int				initDataNode(DataNode* pkDataNode, SOCKET uiSocket);
	int				exitDataNode(DataNode* pkQueueUserDataNode, mtQueuePacket* pkQueuePacket);

	int				addDataNodePacket(DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket);
	int				removeDataNodePacket(DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket);

	int				DataRead(DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket);
	int				DataWrite(DataNode* pkDataNodeUser, mtQueuePacket::DataNode* pkDataNodePacket);
	long            getNumberOfReadPacket(DataNode* pkDataNodeUser);
	void printUserPacket(DataNode* pkDataNodeUser);
private:
	DataNode										m_pkNodeList[MT_NODE_QUEUE_USER];
	mtDSQueue<DataNode*, MT_NODE_QUEUE_USER>		m_pkIdQueue;
	int												m_iNodeTotal;
	int												m_iSpinCount;
	HANDLE											m_hIOCP;
	CRITICAL_SECTION								m_kCirticalSection;

};

 /************************************************************************
  * @function:	getIdFromAddr                                                     
  * @describe:	根据结点算出 iNodeIdx
  * @param-in:	
		-pData		输入地址
  * @return:		结点下标
  * @author:		fuke
  ************************************************************************
  */
inline mtQueueUser::DataNode* mtQueueUser::getBaseAddrFromAddr( void* pData )
{
	char*			pcAddrBase	= (char*)m_pkNodeList;
	char*			pcAddrSrc	= (char*)pData;
	return			(DataNode*)(pcAddrBase + 
		(((pcAddrSrc - pcAddrBase) / sizeof(DataNode)) * sizeof(DataNode)));
}

inline int mtQueueUser::getNodeNumRemain()
{
	return	m_pkIdQueue.getNodeNumUsed();
}

inline int mtQueueUser::getNodeNumTotal()
{
	return  m_iNodeTotal;
}

inline long	mtQueueUser::getNodeUseCount()
{
	return m_pkIdQueue.getNodeUseCount();
}

inline bool mtQueueUser::bHaveRepeat()
{
	return m_pkIdQueue.bHaveRepeat();
}

 /************************************************************************
  * @function:	pop                                                     
  * @describe:	从队列中弹出一个元素
  * @param-out:	pT
  * @return:	
 		-成功			非NULL
 		-队列为空		NULL
  * @author:		fuke
  ************************************************************************
  */
inline mtQueueUser::DataNode* mtQueueUser::pop()
{
	DataNode*		pkDataNode;
	//return	(m_pkIdQueue.pop(&pkDataNode) > 0) ? pkDataNode : NULL;	
	if(m_pkIdQueue.pop(&pkDataNode) > 0)
	{
		pkDataNode->pkQueueDeskDataNode = NULL;				//桌子节点清空
		pkDataNode->pkQueuePacketDataNodeHead = NULL;			//包头节点设置
		return pkDataNode;
	}
	return NULL;
}

 /************************************************************************
  * @function:	push                                                     
  * @describe:	往队列中压入一个元素
  * @param-out:	
  * @return:	
 		-成功			1
 		-队列为空		0
  * @author:		fuke
  ************************************************************************
  */
inline int mtQueueUser::push( DataNode* pkDataNode )
{
	return	(m_pkIdQueue.push(&pkDataNode) > 0) ? 1 : 0;
}



#endif	///	__MT_QUEUE_NODE_USER_H