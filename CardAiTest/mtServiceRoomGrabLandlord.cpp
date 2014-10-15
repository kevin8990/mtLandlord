#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomMgr.h"
#include <windows.h>

mtServiceRoomGrabLandlord::mtServiceRoomGrabLandlord()
{
	memset(&m_kGrablandlordDataWrite,0 ,sizeof(m_kGrablandlordDataWrite));
}

mtServiceRoomGrabLandlord::~mtServiceRoomGrabLandlord()
{

}

void mtServiceRoomGrabLandlord::init()
{
	m_kGrablandlordDataWrite.lScore = 3;
	m_kGrablandlordDataWrite.lServiceType = E_SERVICE_ROOM_GRAB_LANDLORD;
	m_kGrablandlordDataWrite.lStructBytes = sizeof(m_kGrablandlordDataWrite);
	m_kGrablandlordDataWrite.plReservation[0] = 0;
	m_kGrablandlordDataWrite.plReservation[1] = 0;
}
