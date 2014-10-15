#pragma once 
class mtServiceRoomCardDealRecv
{
public:
	struct DataWrite
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long							lResult;

	};

	struct DataRead
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î

		long 							lDefaultLandlordUserId;	/// Ä¬ÈÏ¿ªÊ¼½Ð·ÖµÄÍæ¼Ò
		char               				pcCards[17];            /// Ã¿¸öÈË17ÕÅÅÆ
		char                			pcBaseCards[7];         /// µ×ÅÆ3ÕÅ

	};

	mtServiceRoomCardDealRecv();
	~mtServiceRoomCardDealRecv();
	void DealWithCardRecv(DataRead *pkDataRead);

};
