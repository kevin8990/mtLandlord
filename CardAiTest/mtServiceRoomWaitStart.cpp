#include "mtServiceRoomWaitStart.h"
#include "mtServiceRoomMgr.h"
#include <windows.h>

mtServiceRoomWaitStart::mtServiceRoomWaitStart()
{
	memset(&m_kWaitStartDataWrite,0,sizeof(m_kWaitStartDataWrite));
}

mtServiceRoomWaitStart::~mtServiceRoomWaitStart()
{

}

void mtServiceRoomWaitStart::init()
{
	m_kWaitStartDataWrite.lStructBytes = sizeof(m_kWaitStartDataWrite);
	m_kWaitStartDataWrite.lServiceType = E_SERVICE_ROOM_WAIT_START;
	memset(m_kWaitStartDataWrite.plReservation,0,sizeof(long)*2);

}