#pragma once
#include "mtCardAi.h"
//出牌协议
class mtServiceRoomCardOut
{
public:
	struct DataWrite
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long							lCardType;              /// ³öÅÆÀàÐÍ
		char 							pcCard[24];             /// ³öÅÆÄÚÈÝ
	};

	struct DataRead
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long                            lUserId;                /// ·¢ÅÆÓÃ»§id
		long							lCardType;              /// ³öÅÆÀàÐÍ
		char 							pcCard[24];             /// ³öÅÆÄÚÈÝ
	};
	mtServiceRoomCardOut();
	~mtServiceRoomCardOut();
	void init(mteCardType CardType,char *pkCardSrc);
public:
	DataWrite m_kCardOutDataWrite;

};
