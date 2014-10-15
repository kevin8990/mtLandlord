#pragma once 
class 	mtServiceRoomExit
{
public:
	struct DataWrite
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		
		long                lStatusExit;           
		long				lCardType;             
		char 				pcCard[24];            

	};

	struct DataRead
	{
		long 				lStructBytes;
		long 				lServiceType;
		long 				plReservation[2];		
		long 				lUserId;
		long                lTypexit;           
	};
	mtServiceRoomExit();
	~mtServiceRoomExit();
	void init();
public:
	DataWrite m_kExitDataWrite;
};
