#ifndef		__MT_LIST_H
#define		__MT_LIST_H

#include "mtPlatform.h"
#include "mtException.h"
#include "mtDSList.h"

template<class _Tclass, int _iListNodeIdx>
class	mtList : public mtDSList
{
public:
	mtList();
	~mtList();

	int		init(int iSpinCount);
	int		exit(void);

	int			push( _Tclass* pkDataNode);
	int			push( _Tclass** pkDataNodeList, int iDataNodeNum);
	_Tclass*	pop();
	int			pop( _Tclass** pkDataNodeList, int iDataNodeNum);
	int			remove( _Tclass* pkDataNode);

protected:
	CRITICAL_SECTION	m_kCriticalSection;
	int					m_iRetCirticalSection;
	mtListNode*			m_pkListNodeList[10];
};

template<class _Tclass, int _iListNodeIdx>
int mtList<_Tclass, _iListNodeIdx>::exit( void )
{
	if (0 != m_iRetCirticalSection) 
	{
		DeleteCriticalSection(&m_kCriticalSection);
	}
	memset (&m_kCriticalSection, 0, sizeof(m_kCriticalSection));
	memset (m_pkListNodeList, 0, sizeof(m_pkListNodeList));
	m_iRetCirticalSection	= 0;

	return	1;
}

template<class _Tclass, int _iListNodeIdx>
int mtList<_Tclass, _iListNodeIdx>::init( int iSpinCount )
{
	memset (&m_kCriticalSection, 0, sizeof(m_kCriticalSection));
	m_iRetCirticalSection	= InitializeCriticalSectionAndSpinCount(&m_kCriticalSection, iSpinCount);
	MT_EXCEPTION_DEBUG(0 == m_iRetCirticalSection, E_EXCEPTION_INIT_CRITICAL_SECTION_FAILED);

	memset (m_pkListNodeList, 0, sizeof(m_pkListNodeList));

	return	1;
}

template<class _Tclass, int _iListNodeIdx>
int mtList<_Tclass, _iListNodeIdx>::pop( _Tclass** pkDataNodeList, int iDataNodeNum )
{
	EnterCriticalSection(&m_kCriticalSection);

	if (NULL == pkDataNodeList)
	{
		LeaveCriticalSection(&m_kCriticalSection);
		return	-1;
	}

	int		iRet	= pop(m_pkListNodeList, iDataNodeNum);

	if (iRet > 0)
	{
		int		iListNodeIdx;

		for (iListNodeIdx = 0; iListNodeIdx < iDataNodeNum, iListNodeIdx++)
		{
			pkDataNodeList[iListNodeIdx]	= (_Tclass*)m_pkListNodeList[iListNodeIdx]->pSelf;
			m_pkListNodeList[iListNodeIdx]->pSelf	= NULL;
		}
	}

	LeaveCriticalSection(&m_kCriticalSection);
	return	iRet;
	
}

template<class _Tclass, int _iListNodeIdx>
int mtList<_Tclass, _iListNodeIdx>::remove( _Tclass* pkDataNode )
{
	EnterCriticalSection(&m_kCriticalSection);
	int	iRet	= mtDSList::remove(pkDataNode->pkListNode + _iListNodeIdx);

	if (iRet > 0)
	{
		pkListNode->pSelf	= NULL;
	}

	LeaveCriticalSection(&m_kCriticalSection);
	return	iRet;
}

template<class _Tclass, int _iListNodeIdx>
_Tclass* mtList<_Tclass, _iListNodeIdx>::pop()
{
	EnterCriticalSection(&m_kCriticalSection);
	mtListNode*	pkListNode	= mtDSList::pop();

	if (NULL != pkListNode)
	{
		_Tclass* pkDataNode = (_Tclass*)(pkListNode->pSelf);
		pkListNode->pSelf	= NULL;
		LeaveCriticalSection(&m_kCriticalSection);
		return	pkDataNode;
	}
	
	LeaveCriticalSection(&m_kCriticalSection);
	return	NULL;
}

/************************************************************************
 * @function:	push                                                     
 * @describe:	向队列中压入一个元素
 * @param-in:	
		-pListNode		被压入的结点
		-pData			被压入的结点附带的信息
 * @return:	
		-成功				> 0
		-pkListNodeList为空			-1
		-队列结点已经插入过	-2
 * @author:		fuke
 ************************************************************************
 */
template<class _Tclass, int _iListNodeIdx>
int mtList<_Tclass, _iListNodeIdx>::push( _Tclass* pkDataNode )
{
	EnterCriticalSection(&m_kCriticalSection);
	if (NULL != pkDataNode->pkListNode[_iListNodeIdx].pSelf)
	{
		LeaveCriticalSection(&m_kCriticalSection);
		return	-2;
	}

	pkDataNode->pkListNode[_iListNodeIdx].pSelf	= pkDataNode;
	int	iRet	= mtDSList::push(&(pkDataNode->pkListNode[_iListNodeIdx]), pkDataNode);

	LeaveCriticalSection(&m_kCriticalSection);

	return	iRet;
}

template<class _Tclass, int _iListNodeIdx>
int	mtList<_Tclass, _iListNodeIdx>::push( _Tclass** pkDataNodeList, int iDataNodeNum)
{
	EnterCriticalSection(&m_kCriticalSection);
	if (NULL == pkDataNodeList)
	{
		LeaveCriticalSection(&m_kCriticalSection);
		return	-2;
	}

	int	iRet = -1;
	int		iDataNodeIdx;
	for (iDataNodeIdx = 0; iDataNodeIdx < iDataNodeNum; iDataNodeIdx++)
	{
		pkDataNodeList[iDataNodeIdx]->pkListNode[_iListNodeIdx].pSelf	= pkDataNodeList[iDataNodeIdx];
		iRet	= mtDSList::push(&(pkDataNodeList[iDataNodeIdx]->pkListNode[_iListNodeIdx]), pkDataNodeList[iDataNodeIdx]);
	}

	LeaveCriticalSection(&m_kCriticalSection);

	return	iRet;
}

template<class _Tclass, int _iListNodeIdx>
mtList<_Tclass, _iListNodeIdx>::~mtList()
{
	m_iRetCirticalSection	= 0;
	memset (&m_kCriticalSection, 0, sizeof(m_kCriticalSection));
	memset (m_pkListNodeList, 0, sizeof(m_pkListNodeList));
}

template<class _Tclass, int _iListNodeIdx>
mtList<_Tclass, _iListNodeIdx>::mtList() :
	mtDSList(),
	m_iRetCirticalSection(0)
{
	memset (&m_kCriticalSection, 0, sizeof(m_kCriticalSection));
	memset (m_pkListNodeList, 0, sizeof(m_pkListNodeList));
}



#endif	///	__MT_LIST_ONE_H