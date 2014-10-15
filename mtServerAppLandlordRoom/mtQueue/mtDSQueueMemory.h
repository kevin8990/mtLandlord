#ifndef		__MT_DS_QUEUE_MEMORY_H
#define		__MT_DS_QUEUE_MEMORY_H

#include "mtDSQueue.h"


template<class _Tclass, int _iNodeNumTotal>
class mtDSQueueMemory
{
public:
	mtDSQueueMemory();
	~mtDSQueueMemory();

	int		init(int iSpinCount);
	int		exit(void);

	int		pop();
	int		push(int iNodeId);
	void*	getNodeFromId(int iNodeId);

	int		getIdFromAddr(void* pData);
	int		getNodeNumUsed();
	int		getNodeNumTotal();

private:
	_Tclass									m_pkNodeList[_iNodeNumTotal];
	mtDSQueue<int, _iNodeNumTotal>			m_pkIdQueue;
};

template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::getIdFromAddr( void* pData )
{
	char*			pcAddrBase	= (char*)m_pkNodeList;
	char*			pcAddrSrc	= (char*)pData;
	return	(pcAddrSrc - pcAddrBase) / sizeof(_Tclass);
}

template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::getNodeNumTotal()
{
	return	_iNodeNumTotal;
}

template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::exit( void )
{
	return	m_pkIdQueue.exit();
}

template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::init( int iSpinCount )
{
	int iRet = m_pkIdQueue.init(iSpinCount);

	int		iNodeidx;
	for (iNodeidx = 0; iNodeidx < _iNodeNumTotal; iNodeidx)
	{
		m_pkIdQueue.push(&iNodeidx);
	}
}

template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::getNodeNumUsed()
{
	return	m_pkIdQueue.getNodeNumUsed();
}

 /************************************************************************
  * @function:	getNodeFromId                                                     
  * @describe:	获取一个结点的地址
  * @param-out:	
  * @return:	
 		-成功			结点地址
 		-失败			NULL
  * @author:		fuke
  ************************************************************************
  */
template<class _Tclass, int _iNodeNumTotal>
void* mtDSQueueMemory<_Tclass, _iNodeNumTotal>::getNodeFromId( int iNodeId )
{
	if (iNodeId >= 0 && iNodeId < _iNodeNumTotal)
	{
		return	(void*)(m_pkNodeList + iNodeId);
	}

	return	NULL;
}

 /************************************************************************
  * @function:	push                                                     
  * @describe:	往队列中压入一个元素
  * @param-out:	
  * @return:	
 		-成功			>= 0
 		-队列为空		-1
  * @author:		fuke
  ************************************************************************
  */
template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::push( int iNodeId )
{
	return	(m_pkIdQueue.push(&iNodeId) > 0) ? 1 : 0;
}

 /************************************************************************
  * @function:	pop                                                     
  * @describe:	从队列中弹出一个元素
  * @param-out:	pT
  * @return:	
 		-成功			>= 0
 		-队列为空		-1
  * @author:		fuke
  ************************************************************************
  */
template<class _Tclass, int _iNodeNumTotal>
int mtDSQueueMemory<_Tclass, _iNodeNumTotal>::pop()
{
	int		iNodeId;
	return	(m_pkIdQueue.pop(&iNodeId) > 0) ? iNodeId : -1;	
}

template<class _Tclass, int _iNodeNumTotal>
mtDSQueueMemory<_Tclass, _iNodeNumTotal>::~mtDSQueueMemory()
{

}

template<class _Tclass, int _iNodeNumTotal>
mtDSQueueMemory<_Tclass, _iNodeNumTotal>::mtDSQueueMemory() 
{

}

#endif	///	__MT_DS_QUEUE_MEMORY_H