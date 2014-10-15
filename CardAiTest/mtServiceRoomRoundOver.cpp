#include "mtServiceRoomRoundOver.h"
#include "mtServiceRoomMgr.h"
#include <time.h>
#include <windows.h>

mtServiceRoomRoundOver::mtServiceRoomRoundOver()
{
	memset(&m_kRoundOverDataWrite,0,sizeof(m_kRoundOverDataWrite));

}

mtServiceRoomRoundOver::~mtServiceRoomRoundOver()
{

}

void mtServiceRoomRoundOver::init()
{
	m_kRoundOverDataWrite.lStructBytes = sizeof(m_kRoundOverDataWrite);
	m_kRoundOverDataWrite.lServiceType = E_SERVICE_ROOM_ROUND_OVER;
	m_kRoundOverDataWrite.plReservation[0] = 0;
	m_kRoundOverDataWrite.plReservation[1] = 0;
	int GoldNum = 0;
	srand( (unsigned)time( NULL ) );
	int i = 1 + rand()%10;
	if (i%2)
	{
		 GoldNum = 1000*(rand()%10);
	}
	else
	{
		GoldNum = -(1000*(rand()%10));
	}

	m_kRoundOverDataWrite.lGold = GoldNum;
	
	
}