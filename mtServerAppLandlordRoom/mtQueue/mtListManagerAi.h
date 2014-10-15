#ifndef MT_LIST_MANAGER_AI_H
#define MT_LIST_MANAGER_AI_H
#include "mtQueueSendPackAi.h"
class mtListManagerAi
{
public:
	struct DataInit
	{
		long		             lStructBytes;
		int			             iSpinCount;
	};

public:
	mtListManagerAi();
	~mtListManagerAi();

	int	           init(void* pData);
	int	           exit(void);
	int	           getNodeNum();
	mtQueueSendPackAi::DataNode* getHead();
	mtQueueSendPackAi::DataNode* findNode(mtQueueSendPackAi::DataNode* pkNode);
	mtQueueSendPackAi::DataNode* push(mtQueueSendPackAi::DataNode* pkNode, int* piInsertHead);
	mtQueueSendPackAi::DataNode* popHead();
	int       pop(mtQueueSendPackAi::DataNode* pkNode);

	mtQueueSendPackAi::DataNode* m_pkListHead;
	int                          m_iDataNodeCount;
	int			                 m_iSpinCount;
	CRITICAL_SECTION		     m_kCirticalSection;
};

#endif //MT_LIST_MANAGER_AI_H