#pragma once 
//开始协议
class 	mtServiceRoomWaitStart
{
public:
	struct DataWrite
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		
	};

	struct DataRead
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		
		long 				lUserId;
	};
	mtServiceRoomWaitStart();
	~mtServiceRoomWaitStart();
	void init();
public:
	DataWrite m_kWaitStartDataWrite;
};
