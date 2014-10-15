#ifndef		__MT_QUEUE_PACKET_H
#define		__MT_QUEUE_PACKET_H
#include "mtPlatform.h"
#include "mtDSQueue.h"

class mtQueuePacket
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

	struct DataNode
	{
		OVERLAPPED			kDataOverLapped;
		void*               pvService;
		long                lServiceType;			/// 服务类型
		void*               pvData;
		SOCKET	            iSocket;
		HANDLE              hFile;
		EDataType			eDataType;
		char				pcData[MT_BYTES_DATA_PACKET];
	};

	struct DataInit
	{
		long		lStructBytes;
		int			iSpinCount;
	};

public:
	mtQueuePacket();
	~mtQueuePacket();

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

	DataNode								        m_pkNodeList[MT_NODE_QUEUE_PACKET_MAX];
	mtDSQueue<DataNode*, MT_NODE_QUEUE_PACKET_MAX>	m_pkIdQueue;
	int										        m_iNodeTotal;
	int										        m_iSpinCount;
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
inline mtQueuePacket::DataNode* mtQueuePacket::getBaseAddrFromAddr( void* pData )
{
	char*			pcAddrBase	= (char*)m_pkNodeList;
	char*			pcAddrSrc	= (char*)pData;
	return			(DataNode*)(pcAddrBase + 
		(((pcAddrSrc - pcAddrBase) / sizeof(DataNode)) * sizeof(DataNode)));
}

inline int mtQueuePacket::getNodeNumRemain()
{
	return	m_pkIdQueue.getNodeNumUsed();
}

inline int mtQueuePacket::getNodeNumTotal()
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
inline mtQueuePacket::DataNode* mtQueuePacket::pop()
{
	DataNode*		pkDataNode;
	return	(m_pkIdQueue.pop(&pkDataNode) > 0) ? pkDataNode : NULL;	
}

inline int mtQueuePacket::pop(DataNode*pkDataNode, int iTListNum)
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
inline int mtQueuePacket::push( DataNode* pkDataNode )
{
	return	(m_pkIdQueue.push(&pkDataNode) > 0) ? 1 : 0;
}

inline int mtQueuePacket::push(DataNode** pkDataNode, int iTListNum)
{
	return	(m_pkIdQueue.push(pkDataNode, iTListNum) > 0) ? 1 : 0;
}

#endif	///	__MT_QUEUE_PACKET_H