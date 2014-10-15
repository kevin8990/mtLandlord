#ifndef		__MT_QUEUE_SEND_PACK_AI_H
#define		__MT_QUEUE_SEND_PACK_AI_H
#include "mtPlatform.h"
#include "mtDSQueue.h"
#include "mtQueueUser.h"

class mtQueueSendPackAi
{
public:
	enum EDataType
	{
		E_DATA_TYPE_BEG	= 0,
		E_DATA_TYPE_NOT_USED = E_DATA_TYPE_BEG,
		E_DATA_TYPE_READ,
		E_DATA_TYPE_WRITE,
		E_DATA_TYPE_END
	};

	enum
	{
		E_SEND_TYPE_BEG	    = 0,
		E_SEND_TYPE_INIT    = E_DATA_TYPE_BEG,
		E_SEND_TYPE_SEND,
		E_SEND_TYPE_POST,
		E_SEND_TYPE_SEVICE,
		E_SEND_TYPE_END
	};

	struct DataNode
	{
		int                             iType;
		ULONGLONG				        ullTimeSend;                /// 发送时间
		mtQueueUser::DataNode*          pkQueueUserDataNode;
		mtQueuePacket::DataNode*        pkQueuePacketDataNode;
		struct DataNode*                pkNext;
	};

	struct DataInit
	{
		long		lStructBytes;
		int			iSpinCount;
	};

public:
	mtQueueSendPackAi();
	~mtQueueSendPackAi();

	int				init(void* pData);
	int				exit(void);

	DataNode*		pop();
	int      		pop(DataNode* pkDataNode, int iTListNum);
	int				push(DataNode* pkDataNode);
	int      		push(DataNode** pkDataNode, int iTListNum);

	void*			popInitDataNode();

	DataNode*		getBaseAddrFromAddr(void* pData);
	int				getNodeNumRemain();
	int				getNodeNumTotal();

	DataNode								    m_pkNodeList[MT_NODE_QUEUE_THREE];
	mtDSQueue<DataNode*, MT_NODE_QUEUE_THREE>	m_pkIdQueue;
	int										    m_iNodeTotal;
	int										    m_iSpinCount;
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
inline mtQueueSendPackAi::DataNode* mtQueueSendPackAi::getBaseAddrFromAddr( void* pData )
{
	char*			pcAddrBase	= (char*)m_pkNodeList;
	char*			pcAddrSrc	= (char*)pData;
	return			(DataNode*)(pcAddrBase + 
		(((pcAddrSrc - pcAddrBase) / sizeof(DataNode)) * sizeof(DataNode)));
}

inline int mtQueueSendPackAi::getNodeNumRemain()
{
	return	m_pkIdQueue.getNodeNumUsed();
}

inline int mtQueueSendPackAi::getNodeNumTotal()
{
	return  m_iNodeTotal;
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
inline mtQueueSendPackAi::DataNode* mtQueueSendPackAi::pop()
{
	DataNode*		pkDataNode;
	return	(m_pkIdQueue.pop(&pkDataNode) > 0) ? pkDataNode : NULL;	
}

inline int mtQueueSendPackAi::pop(DataNode*pkDataNode, int iTListNum)
{
	return	m_pkIdQueue.pop(&pkDataNode, iTListNum);
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
inline int mtQueueSendPackAi::push( DataNode* pkDataNode )
{
	return	(m_pkIdQueue.push(&pkDataNode) > 0) ? 1 : 0;
}

inline int mtQueueSendPackAi::push(DataNode** pkDataNode, int iTListNum)
{
	return	(m_pkIdQueue.push(pkDataNode, iTListNum) > 0) ? 1 : 0;
}

#endif	///	__MT_QUEUE_SEND_PACK_AI_H