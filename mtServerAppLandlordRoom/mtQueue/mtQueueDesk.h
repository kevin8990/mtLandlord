#ifndef		__MT_QUEUE_DESK_H
#define		__MT_QUEUE_DESK_H
#include "mtPlatform.h"
#include "mtDSQueue.h"
#include "mtQueueUser.h"

class mtQueueDesk 
{
public:
	enum EPlayerType
	{
		E_PLAYER_TYPE_BEG		= 0,
		E_PLAYER_TYPE_LANDLORD	= E_PLAYER_TYPE_BEG,
		E_PLAYER_TYPE_FARMER,
		E_PLAYER_TYPE_END
	};

	enum EPlayerMode
	{
		E_PLAYER_MODE_BEG		= 0,
		E_PLAYER_MODE_USER	= E_PLAYER_MODE_BEG,
		E_PLAYER_MODE_AI,
		E_PLAYER_MODE_END
	};

	struct DataNode
	{
		long		                lStructBytes;
		mtQueueUser::DataNode*		pkQueueUserDataNode[MT_NODE_PLAYER];  /// 桌子上的人员信息
		char						pcCardsBase[MT_NODE_PLAYER];          /// 每一局的底牌信息
		char						pcCardsTotal[MT_NODE_PLAYER][24];     /// 每个人的当前牌面信息
		char						pcCardsCur[24];                       /// 桌子上当前打出的牌信息
		char						pcCardsOutAi[24];                     /// ai要打出的牌信息(只有ai能用到)
		char						pcCardsOutTotal[MT_NODE_PLAYER][24];  /// 每个人的当前已经出牌信息
		long						plPlayerMode[MT_NODE_PLAYER];	      /// 标识每个人是不是ai(初始值是0, 0 -是真实的人，1 -是ai)
		long                        plPlayerLots[MT_NODE_PLAYER];         /// 每个玩家的出牌的手数
		long						plPlayerType[MT_NODE_PLAYER];	      /// 每个人的类型(初始值是1， 0 - landlord, 1 - farmer)
		long 						plIsWaitStart[MT_NODE_PLAYER];        /// 是否已经‘等待开始’ 0还没有等待开始 1已经等待开始了
		long						plScore[MT_NODE_PLAYER];	          /// 玩家叫的分数(初始值是-1, 0 - 不叫分, 0< - 叫的分数  )
		long						plDoubleScore[MT_NODE_PLAYER];	      /// 玩家加倍分数(初始值是-1, 0 - 不加倍, 1 - 加倍  )
		long                        plShowCard[MT_NODE_PLAYER];           /// 是否明牌(初始值是-1, 1 -明牌，0 -不明牌)
		long						plRoundOver[MT_NODE_PLAYER];	      /// 玩家是否上传一局结果分数(初始值是0, 0 - 还没上传, 1 - 已经上传了  )
		long						plRoundOverGold[MT_NODE_PLAYER];	  /// 玩家上传一局结果分数(初始值是0, 0 < 表示上传分数)
		long					    plBomb[MT_NODE_PLAYER];               /// 玩家出的炸弹数(初始值是0, 0 < -)
		long                        lCardDeal;                            /// 桌子已经发牌了(初始值是0, 1 -已经发牌了)
		long					    lRocket;                              /// 桌子上是否已经产生了火箭牌(初始值是0, 0 -还没产生火箭，1 -已经产生了火箭)
		long                        lQueue;                               /// 桌子所在的队列(初始值0, 1 -单人队列，2 -两人队列，3 -三人队列)
		long						lCardType;                            /// 桌子上当前打出的牌类型
		long						lQueueUserDefaultLandlord;            /// 桌子上当前默认开始叫分的用户Id
		long                        lQueueUserDeskIdx;                    /// 桌子上当前打出牌的用户在桌子上的位置(过牌不算)
		long                        lUserDeskIdxCardOutNext;              /// 桌子当前将要出牌的用户在桌子上的位置
		long                        lDeskExit;                            /// 该桌子当前状态(0 -初始状态，1 -正在运行，2 -正在退出，3 -已经退出)
		ULONGLONG				    ullTimeEnterRun;                      /// 节点入链表时间
		CRITICAL_SECTION	        kCriticalSection;
		int					        iRetCirticalSection;
	};

	struct DataInit
	{
		long		lStructBytes;
		int			iSpinCount;
	};

public:
	mtQueueDesk();
	~mtQueueDesk();

	int				init(void* pData);
	int				exit(void);

	DataNode*		pop();
	int				push(DataNode* pkDataNode);

	DataNode*		getBaseAddrFromAddr(void* pData);
	int				getNodeNumRemain();
	int				getNodeNumTotal();

	long			getNodeUseCount();

	DataNode								m_pkNodeList[MT_NODE_QUEUE_DESK];
	mtDSQueue<DataNode*, MT_NODE_QUEUE_DESK>		m_pkIdQueue;
	int										m_iNodeTotal;
	int										m_iSpinCount;
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

inline mtQueueDesk::DataNode* mtQueueDesk::getBaseAddrFromAddr( void* pData )
{
	char*			pcAddrBase	= (char*)m_pkNodeList;
	char*			pcAddrSrc	= (char*)pData;
	return			(DataNode*)(pcAddrBase + 
		(((pcAddrSrc - pcAddrBase) / sizeof(DataNode)) * sizeof(DataNode)));
}

inline int mtQueueDesk::getNodeNumRemain()
{
	return	m_pkIdQueue.getNodeNumUsed();
}

inline int mtQueueDesk::getNodeNumTotal()
{
	return  m_iNodeTotal;
}

inline long	mtQueueDesk::getNodeUseCount()
{
	return m_pkIdQueue.getNodeUseCount();
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

inline mtQueueDesk::DataNode* mtQueueDesk::pop()
{
	DataNode*		pkDataNode;

	return	(m_pkIdQueue.pop(&pkDataNode) > 0) ? pkDataNode : NULL;	
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
inline int mtQueueDesk::push( DataNode* pkDataNode )
{
	return	(m_pkIdQueue.push(&pkDataNode) > 0) ? 1 : 0;
}

#endif	///	__MT_QUEUE_NODE_USER_H