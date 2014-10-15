#pragma once 
//连接协议
#define  MT_NODE_PLAYER 3
class 	mtServiceRoomConnect
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
		long 					lStructBytes;			               /// 包大小
		long                    lServiceType;			               /// 服务类型
		long 					plReservation[2];		               /// 保留字段
		long 				    lResult;                               /// 连接房间服务器结果(0 -失败，1 -成功)
		/// 2 -该用户上一次强制从某房间退出(游戏还在进行中)
		/// 3 -该用户上一次强制从某房间退出(游戏已经结束)
		/// 4 -该用户由于网络异常从某房间退出(游戏还在进行中)
		/// 5 -该用户由于网络异常从某房间退出(游戏已经结束)
		/// 6 -该用户账号已经在其它地方登陆了
		/// 7 -该用户账号已经不在该房间了
		DataUserInfo            kUserInfo[2];                          /// 其他玩家信息(在lResult为3、5时，第一个位置表示用户最新的信息，更新完上一次)
		/// 以下字段只有在lResult为2、4时才会有效
		char					pcCardsBase[MT_NODE_PLAYER];           /// 每一局的底牌信息
		char					pcCardsTotal[MT_NODE_PLAYER][24];      /// 该用户的当前牌面信息
		char					pcCardsShow[24];                       /// 明牌用户的当前牌面信息
		char					pcCardsCur[24];                        /// 桌子上当前打出的牌信
		char					pcCardsOutTotal[MT_NODE_PLAYER][24];   /// 每个人的当前已经出牌信息
		long                    plPlayerLots[MT_NODE_PLAYER];          /// 每个玩家的出牌的手数
		long				    plScore[MT_NODE_PLAYER];	           /// 玩家叫的分数(初始值是-1, 0 - 不叫分, 0< - 叫的分数  )
		long					plDoubleScore[MT_NODE_PLAYER];         /// 玩家加倍分数(初始值是-1, 0 - 不加倍, 1 - 加倍  )
		long                    plShowCard[MT_NODE_PLAYER];            /// 是否明牌(1 -明牌，0 -不明牌)
		long					plBomb[MT_NODE_PLAYER];                /// 玩家出的炸弹数(初始值是0, 0 < -)
		long				    lRoundOver;	                           /// 玩家是否上传一局结果分数(初始值是0, 0 - 还没上传, 1 - 已经上传了  )
		long					lRocket;                               /// 桌子上是否已经产生了火箭牌(初始值是0, 0 -还没产生火箭，1 -已经产生了火箭)
		long					lCardType;                             /// 桌子上当前打出的牌类型
		long                    lUserNodeIdx;                          /// 该用户在桌子上的位置
		long                    lUserDeskIdxCardOut;                   /// 桌子上当前打出牌的用户在桌子上的位置(过牌不算)
		long                    lUserDeskIdxCardOutNext;               /// 桌子当前将要出牌的用户在桌子上的位置
		long 					lQueueUserDefaultLandlord;             /// 桌子上当前默认开始叫分的用户在桌子上的位置(初始值是-1, 0< -)
	};

	struct DataWrite
	{
		long 							lStructBytes;			/// °ü´óÐ¡
		long                        	lServiceType;			/// ·þÎñÀàÐÍ
		long 							plReservation[2];		/// ±£Áô×Ö¶Î
		long 							lSpaceId;				/// ³¡Id
		long 							lRoomId;				/// ·¿¼äId
		long                   			lRequestSameRoom;       /// ÊÇ·ñÇëÇó½øÈëÍ¬Ò»·¿¼ä(ÓÃ»§ÏëÒª½øÈëµÄ·¿¼äÊÇ·ñºÍ´«ÈëµÄ³¡id·¿¼äidÏàÍ¬)
		DataUserInfo					kUserInfo;				/// Íæ¼Ò¸öÈËÐÅÏ¢
	};
	mtServiceRoomConnect();
	~mtServiceRoomConnect();
	void init(long lUserId);
public:
	DataUserInfo m_kDataUserInfo;
	DataWrite m_kConnectDataWrite;


};
