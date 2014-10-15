#pragma once 
//抢地主协议
class mtServiceRoomGrabLandlord
{

public:
	struct DataWrite
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long							lScore;
	};

	struct DataRead
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long							lUserId;
		long							lScore;
		long                            lUserIdLandlord;        /// µØÖ÷ÓÃ»§id(-1 µ±Ç°»¹Ã»ÓÐµØÖ÷, 0< ±êÊ¶Íæ¼ÒÓÃ»§id)

	};

	mtServiceRoomGrabLandlord();
	~mtServiceRoomGrabLandlord();
	void init();
	DataWrite m_kGrablandlordDataWrite;
};
