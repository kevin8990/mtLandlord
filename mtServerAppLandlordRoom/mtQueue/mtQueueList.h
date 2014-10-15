#ifndef		__MT_QUEUE_LIST_H
#define		__MT_QUEUE_LIST_H

#include "mtDSQueue.h"
#include "mtDSQueueMemory.h"
#include "mtQueueUser.h"
#include "mtQueueDesk.h"
//#include "mtList.h"

typedef		mtDSQueue<mtQueueUser::DataNode*, MT_NODE_QUEUE_ONE>			mtQueueOneList;		/// NodeUserId
typedef		mtDSQueue<mtQueueDesk::DataNode*, MT_NODE_QUEUE_TWO>			mtQueueTwoList;		/// NodeDeskId
typedef		mtDSQueue<mtQueueDesk::DataNode*, MT_NODE_QUEUE_THREE>			mtQueueThreeList;	/// NodeDeskId

// typedef		mtList<mtQueueUser::DataNode*, MT_NODE_QUEUE_ONE>			mtQueueOneList;		/// NodeUserId
// typedef		mtList<mtQueueDesk::DataNode*, MT_NODE_QUEUE_TWO>			mtQueueTwoList;		/// NodeDeskId
// typedef		mtList<mtQueueDesk::DataNode*, MT_NODE_QUEUE_THREE>			mtQueueThreeList;	/// NodeDeskId

#endif	/// __MT_QUEUE_LIST_H