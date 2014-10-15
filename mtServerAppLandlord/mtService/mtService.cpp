#include "mtService.h"

mtService::~mtService()
{

}

mtService::mtService()
{

}

/*int mtService::getServiceDataDir(char* pcServiceDataDir, long lUserId)
{
	strcpy(pcServiceDataDir, g_pcServerDataDir);

	char* pcUseId    = (char*)&lUserId;
	sprintf(pcServiceDataDir + strlen(pcServiceDataDir), "%d\\%d\\%d\\%d\\", pcUseId[3], pcUseId[2], pcUseId[1], pcUseId[0]);

	return 1;
}*/
