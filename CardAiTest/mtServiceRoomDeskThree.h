#pragma  once 
class mtServiceRoomDeskThree
{
public:
	struct DataUserInfo
	{
		long 							lUserId;				
		long                            lUserIconId;            
		long                            lUserSex;               
		long                            lUserGold;              
		long                            lUserLevel;             
		long							lUserScore;				
		long                            lUserAllChess;          
		long                            lUserWinChess;          
		long                            lUserWinRate;           
		long                            lUserOffLineCount;     
		long 							lIsWaitStart;			
		long							lPlayerType;	        
		unsigned long					ulDeskId;              
		long							plPropsCount[16];		
		char                            pcUserAccountNum[12];   
		char							pcUserName[12];
		char                            pcUserNickName[12];    

	};

	struct DataRead
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long                            lResult;                /// ×é×À×Ó³É¹¦·ñ 0Ê§°Ü£¬1³É¹¦
		DataUserInfo             		kUserInfo[2];           /// ÆäËûÍæ¼ÒÐÅÏ¢
	};
};