#pragma once
#include <WinSock2.h>
#include "mtCardAI.h"
#include "mtNetWork.h"
#include "mtServiceRoomCardOut.h"
#include "mtServiceRoomConnect.h"
#include "mtServiceRoomGrabLandlord.h"
#include "mtServiceRoomMgr.h"
#include "mtServiceRoomWaitStart.h"
#include "mtServiceRoomRoundOver.h"
#include "mtServiceRoomCardDealRecv.h"
#include "mtServiceRoomDeskThree.h"
#include "mtServiceRoomExit.h"
#include "time.h"
#include "log.h"
#include <direct.h>
#pragma comment(lib, "ws2_32.lib")
#include<iostream>
using namespace std;

enum
{
	E_ERROR_SELECT		= -3,			/// select失败
	E_ERROR_SELECT_NONE	= -4,			/// 没有socket有消息
	E_ERROR_READ		= -5,			/// read失败
	E_ERROR_READ_BYTES	= -6,			/// read的字节数不对
	E_ERROR_WRITE		= -7,			/// 写失败
	E_ERROR_WRITE_BYTES	= -8,			/// 写的字节数不对
	E_ERROR_CLOSE		= -9,			/// close socket失败
	E_ERROR_NETWORK_CLOSE = -10,		/// 网络断掉
};

