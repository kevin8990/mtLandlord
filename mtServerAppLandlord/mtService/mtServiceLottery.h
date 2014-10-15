#ifndef 	__MT_SERVICE_LOTTERY_H
#define 	__MT_SERVICE_LOTTERY_H
#include "mtService.h"
#include "mtQueueHall.h"
/// 抽奖连接协议
class 	mtServiceLottery : public mtService
{
public:
	enum EPrizeId
	{
		E_PRIZE_FIVE_HUNDRED_GOLD = 1,      //500金币
		E_PRIZE_ONE_THOUSAND_GOLD   ,      //1000金币
		E_PRIZE_TWO_THOUSAND_GOLD ,        //2000金币
		E_PRIZE_FIFTY_THOUSAND_GOLD,       //5万金币                            
		E_PRIZE_ONE_FEE_CARD,              //1元话费券                            
		E_PRIZE_TWO_FEE_CARD,              //2元话费券                            
		E_PRIZE_THREE_FEE_CARD,            //3元话费券                  
		E_PRIZE_FOUR_FEE_CARD,             //5元话费券                           
		E_PRIZE_TEN_FEE_CARD,              //10元话费券          
        E_PRIZE_THIRTY_FEE_CARD,           //30元话费券        
	};
	enum ELotteryResult
	{
		E_RESULT_SUCCESS          = 1,    //成功
		E_RESULT_FAILURE          = 0,   //失败
		E_RESULT_ACCOUNT_ERROR    = -1,  //帐号错误
		E_RESULT_OFFLINE          = -2,  //用户不在线
		E_RESULT_GOLD_INADEQUATE  = -3,  // 金币不足 
		E_RESULT_NUMBER_FULL      = -4,  //次数已满
	};
	struct DataRead
	{
		long		lStructBytes;		// 包大小
		long        lServiceType;		// 服务类型
		long		plReservation[2];	// 保留字段
		long		lUserId;            // 用户id
		long		lGold;			    // 用户总金币
	};


	struct DataWrite
	{
		long		lStructBytes;		// 包大小
		long		lServiceType;		// 服务类型
		long		plReservation[2];	// 保留字段
		long		lResult; 
		long		lPrize;			    // 抽到的奖品
		long		lGold;			    // 目前金币 
		long        lLuck;
		long        lFeeCard;           //目前奖劵
	};
	struct DataInit
	{
		long							lStructBytes;
		char*                           pcServiceExeDir;
		mtQueueHall*                    pkQueueHall;   		// 大厅信息
		mtQueuePacket*                  pkQueuePacket;
		mtSQLEnv*						pkSQLEnv;
	};

public:
	mtServiceLottery();
	virtual ~mtServiceLottery();

	virtual int	init(void* pData);
	virtual int	run(void* pData);
	virtual int exit();
	int		runRead(SOCKET socket, DataRead* pkDataRead);
	int		runWrite(SOCKET socket, const char* pcBuffer, int iBytes,  int flags, int iTimes);

	void  * getUserNodeAddr(long lUserId);
	void   initDataWrite(DataRead &kDataRead,DataWrite &kDataWrite);

	long Lottery(mtQueueHall::PrizeInfo* pkPrizeInfoList);
	void SaveAwardInfo(long lPrize,mtQueueHall::UserInfo*   pkUserInfo,mtQueueHall::LotteryInfo * pkLotteryInfo);
	void GetMessage(char * message,char *name ,long PrepaidCard);

	mtQueuePacket           *                   m_pkQueuePacket;
	mtQueueHall             *                   m_pkQueueHall;   		/// 大厅信息
	mtSQLEnv                *		            m_pkSQLEnv;
	
};

#endif 	/// __MT_SERVICE_LOTTERY_H

