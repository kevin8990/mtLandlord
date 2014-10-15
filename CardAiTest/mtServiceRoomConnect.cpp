#include "mtServiceRoomConnect.h"
#include "mtServiceRoomMgr.h"
#include <windows.h>
#include <iostream>
using namespace std;

mtServiceRoomConnect::mtServiceRoomConnect()
{
	memset(&m_kConnectDataWrite,0,sizeof(m_kConnectDataWrite));
	memset(&m_kDataUserInfo,0,sizeof(m_kDataUserInfo));

}

mtServiceRoomConnect::~mtServiceRoomConnect()
{

}

void mtServiceRoomConnect::init(long lUserId)
{
	
	m_kDataUserInfo.lUserId = lUserId;
	m_kDataUserInfo.lUserGold = 10000000;
	m_kDataUserInfo.lIsWaitStart = 0;
	m_kDataUserInfo.lPlayerType = 1;
	m_kDataUserInfo.ulDeskId = 0;
	sprintf(m_kDataUserInfo.pcUserNickName,"ai%d",lUserId);


	m_kConnectDataWrite.lStructBytes = sizeof(m_kConnectDataWrite);
	m_kConnectDataWrite.lServiceType = E_SERVICE_ROOM_CONNECT;
	memset(m_kConnectDataWrite.plReservation,0,sizeof(long)*2);
	m_kConnectDataWrite.lRoomId = 0;
	m_kConnectDataWrite.lSpaceId = 0;
	m_kConnectDataWrite.lRequestSameRoom = 1;
	memcpy(&m_kConnectDataWrite.kUserInfo,&m_kDataUserInfo,sizeof(m_kDataUserInfo));

	

}