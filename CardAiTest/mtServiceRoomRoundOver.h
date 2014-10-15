#pragma  once 
class mtServiceRoomRoundOver
{
public:
	struct DataUserInfo
	{
		long 							lUserId;				/// ÓÃ»§Id
		long                            lUserIconId;            /// ÓÃ»§Í·Ïñid
		long                            lUserSex;               /// ÐÔ±ð (0--ÄÐ£¬1--Å®)
		long                            lUserGold;              /// ÓÃ»§ÓµÓÐ½ð±ÒÊý
		long                            lUserLevel;             /// ÓÃ»§µÈ¼¶
		long							lUserScore;				/// ÓÃ»§»ý·Ö
		long                            lUserAllChess;          /// ×Ü¾ÖÊý
		long                            lUserWinChess;          /// Ê¤¾ÖÊý
		long                            lUserWinRate;           /// Ê¤ÂÊ
		long                            lUserOffLineCount;      /// µôÏß´ÎÊý
		long 							lIsWaitStart;			/// ±ê¼ÇÊÇ·ñ¿ªÊ¼
		long							lPlayerType;	        /// Ã¿¸öÈËµÄÀàÐÍ(³õÊ¼ÖµÊÇ1£¬ 0 - landlord, 1 - farmer)
		unsigned long					ulDeskId;               /// ×À×Ó±êÊ¶
		long							lPropCount[10];
		char                            pcUserAccountNum[12];   /// ÓÃ»§ÕËºÅ
		char							pcUserName[12];
		char                            pcUserNickName[12];     /// ÓÃ»§êÇ³Æ

	};
	struct DataWrite
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long                            lGold;                  /// ±¾¾ÖÓ®È¡µÄ½ð±ÒÊý(¸ºÖµ±íÊ¾ÊäµÄ½ð±Ò)
	};

	struct DataRead
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		DataUserInfo             		kUserInfo;              /// Íæ¼ÒÒ»¾Ö¸üÐÂºóµÄÐÅÏ¢
	};

	mtServiceRoomRoundOver();
	~mtServiceRoomRoundOver();
	void init();
public:
	DataWrite m_kRoundOverDataWrite;
	

};






