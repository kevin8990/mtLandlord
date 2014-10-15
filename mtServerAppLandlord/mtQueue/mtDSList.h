#ifndef		__MT_DS_LIST_H
#define		__MT_DS_LIST_H

#include "mtPlatform.h"
#include "mtException.h"

struct mtListNode
{
	mtListNode*		pPre;
	void*			pSelf;
	mtListNode*		pNext;
};

class	mtDSList : public mtException
{
public:
	enum 
	{
		E_EXCEPTION_BEG	= 0,
		E_EXCEPTION_INIT_CRITICAL_SECTION_FAILED,
		E_EXCEPTION_END
	};
public:
	mtDSList();
	~mtDSList();

	int		init(int iSpinCount);
	int		exit(void);

	int			push(mtListNode* pListNode, void* pData);
	mtListNode*	pop();
	int			pop(mtListNode** pkListNodeList, int iListNodeNum);
	int			remove(mtListNode* pkListNode);

	int			getNodeNumUsed();

	virtual int exception();

	mtListNode*			m_pkListNodeHead;
	mtListNode*			m_pkListNodeTail;
	int					m_iNodeNumUsed;
};

inline int mtDSList::getNodeNumUsed()
{
	return	m_iNodeNumUsed;
}



#endif	///	__MT_DS_LIST_H