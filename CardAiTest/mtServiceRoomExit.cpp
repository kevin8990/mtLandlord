#include "mtServiceRoomExit.h"
#include "mtServiceRoomMgr.h"
#include <windows.h>

mtServiceRoomExit::mtServiceRoomExit()
{
	memset(&m_kExitDataWrite,0,sizeof(m_kExitDataWrite));
}

mtServiceRoomExit::~mtServiceRoomExit()
{

}

void mtServiceRoomExit::init()
{
	m_kExitDataWrite.lStatusExit = 1;
	m_kExitDataWrite.lServiceType = E_SERVICE_ROOM_EXIT;
	m_kExitDataWrite.lStructBytes = sizeof(m_kExitDataWrite);
    m_kExitDataWrite.plReservation[0] = 0;
	m_kExitDataWrite.plReservation[1] = 0;
}