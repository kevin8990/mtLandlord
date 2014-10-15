#ifndef		__MT_SERVICE_H
#define		__MT_SERVICE_H
#include "mtQueueWork.h"
#include "mtQueuePacket.h"

//#include "mtSQLEnv.h"
class mtSQLEnv;

class mtService
{
public:
	mtService();
	virtual ~mtService();

	virtual int init(void* pData) = 0;
	virtual int	run(void* pData) = 0;
	virtual int exit() = 0;

	//int     getServiceDataDir(char* pcServiceDataDir, long lUserId);
};

#endif	///	__MT_SERVICE_H