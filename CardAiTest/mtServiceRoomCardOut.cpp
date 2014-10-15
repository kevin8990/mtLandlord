#include "mtServiceRoomCardOut.h"
#include "mtServiceRoomMgr.h"
#include <windows.h>

mtServiceRoomCardOut::mtServiceRoomCardOut()
{
	memset(&m_kCardOutDataWrite,0,sizeof(m_kCardOutDataWrite));
}

mtServiceRoomCardOut::~mtServiceRoomCardOut()
{

}

void mtServiceRoomCardOut::init(mteCardType CardType,char *pkCardSrc)
{
	m_kCardOutDataWrite.lCardType = CardType;
	m_kCardOutDataWrite.lServiceType = E_SERVICE_ROOM_CARD_OUT;
	m_kCardOutDataWrite.lStructBytes = sizeof(m_kCardOutDataWrite);
	memcpy(m_kCardOutDataWrite.pcCard,pkCardSrc,sizeof(char)*24);
	m_kCardOutDataWrite.plReservation[0] = 0;
	m_kCardOutDataWrite.plReservation[1] = 0;
}
