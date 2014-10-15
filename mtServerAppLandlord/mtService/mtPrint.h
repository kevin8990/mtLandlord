#pragma once
#include "mtServiceConnect.h"
#include "mtServiceHallExit.h"
#include "mtServiceHallFeedBack.h"
#include "mtServiceHallGiveGold.h"
#include "mtServiceHallGoldBuy.h"
#include "mtServiceHallHall2Room.h"
#include "mtServiceHallPropBuy.h"
#include "mtServiceHallRank.h"
#include "mtServiceHallRegister.h"
#include "mtServiceHallRoom2Hall.h"
#include "mtServiceHallUpdatePlayInfo.h"
#include "mtServiceHallUpdateRoom.h"
#include "mtServiceHallUpdateSocialInfo.h"
#include "mtServiceGetLotteryArg.h"
#include "mtServiceLottery.h"
#include "mtServiceSystemMessage.h"

#include "mtServiceGetTaskInfo.h"
#include "mtServiceGetTaskAward.h"
#include "mtServiceGetPetInfo.h"
#include "mtServiceFeedPet.h"
#include "mtServiceSpitGold.h"
#include "mtServiceCashLottery.h"

#include "mtServiceMatchGetRoomInfo.h"
#include "mtServiceMatchGetUserInfo.h"
#include "mtServiceMatchSignUp.h"
#include "mtServiceMatchGetRoomStatus.h"
#include "mtServiceMatchGetRank.h"
#include "mtServiceMatchAward.h"

#include "mtServiceMgr.h"


extern int mtPrint(mtQueueHall::DataRoom* pkDataRoom,int iTab = 0);
extern int mtPrint(mtQueueHall::DataSpace* pkDataSpace,int iTab = 0);
extern int mtPrint(mtQueueHall::UserInfo* pkUserInfo,int iTab = 0);
extern int mtPrint(mtQueueHall::UserDataNode* pkUserDataNode,int iTab = 0);
extern int mtPrint(mtQueueHall* pkmtQueueHall,int iTab = 0);

extern int mtPrint(mtQueueHall::MatchRoom* pkDataRoom,int iTab = 0);

extern int mtPrint(mtQueueHall::DataRank* pkDataRank,int iTab = 0);
extern int mtPrint(mtQueueHall::DataHall* pkDataHall,int iTab = 0);
extern int mtPrint(mtQueueHall::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceConnect::DataRead* pkDataRead,int iTab = 0);  ///0连接大厅
extern int mtPrint(mtServiceConnect::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceConnect::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallRegister::DataRead* pkDataRead,int iTab = 0);
extern int mtPrint(mtServiceHallRegister::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallRegister::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallPropBuy::DataRead* pkDataRead,int iTab = 0);  /// 2购买道具
extern int mtPrint(mtServiceHallPropBuy::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallPropBuy::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallGoldBuy::DataRead* pkDataRead,int iTab = 0); /// 4购买金币
extern int mtPrint(mtServiceHallGoldBuy::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallGoldBuy::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallRank::DataRead* pkDataRead,int iTab = 0); /// 6获得排行榜
extern int mtPrint(mtServiceHallRank::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallRank::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallGiveGold::DataRead* pkDataRead,int iTab = 0); /// 9赠分
extern int mtPrint(mtServiceHallGiveGold::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallGiveGold::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallUpdatePlayInfo::DataRead* pkDataRead,int iTab = 0);/// 7更新用户的账户信息(包括排行榜)
extern int mtPrint(mtServiceHallUpdatePlayInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallUpdatePlayInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallUpdateSocialInfo::DataRead* pkDataRead,int iTab = 0);/// 8更新用户的基本资料信息
extern int mtPrint(mtServiceHallUpdateSocialInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallUpdateSocialInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallUpdateRoom::DataRead* pkDataRead,int iTab = 0); /// 10房间信息定时更新
extern int mtPrint(mtServiceHallUpdateRoom::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallUpdateRoom::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallHall2Room::DataRead* pkDataRead,int iTab = 0);/// 11用户请求从大厅进房间
extern int mtPrint(mtServiceHallHall2Room::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallHall2Room::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallRoom2Hall::DataRead* pkDataRead,int iTab = 0); /// 12用户请求从房间返回大厅
extern int mtPrint(mtServiceHallRoom2Hall::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallRoom2Hall::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallFeedBack::DataRead* pkDataRead,int iTab = 0);  /// 13用户信息反馈
extern int mtPrint(mtServiceHallFeedBack::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallFeedBack::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceHallExit::DataRead* pkDataRead,int iTab = 0); /// 14退出大厅
extern int mtPrint(mtServiceHallExit::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceHallExit::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceGetLotteryArg::DataRead* pkDataRead,int iTab = 0);/// 15取抽奖参数
extern int mtPrint(mtServiceGetLotteryArg::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceGetLotteryArg::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceLottery::DataRead* pkDataRead,int iTab = 0);/// 16抽奖
extern int mtPrint(mtServiceLottery::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceLottery::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceSystemMessage::DataRead* pkDataRead,int iTab = 0); /// 17系统消息
extern int mtPrint(mtServiceSystemMessage::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceSystemMessage::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceGetTaskInfo::DataRead* pkDataRead,int iTab = 0);///18获取任务信息
extern int mtPrint(mtServiceGetTaskInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceGetTaskInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceGetTaskAward::DataRead* pkDataRead,int iTab = 0);///19获取任务奖励
extern int mtPrint(mtServiceGetTaskAward::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceGetTaskAward::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceGetPetInfo::DataRead* pkDataRead,int iTab = 0);  ///20获取宠物协议
extern int mtPrint(mtServiceGetPetInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceGetPetInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceFeedPet::DataRead* pkDataRead,int iTab = 0);  ///21喂养宠物
extern int mtPrint(mtServiceFeedPet::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceFeedPet::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceSpitGold::DataRead* pkDataRead,int iTab = 0);  ///22吐金币
extern int mtPrint(mtServiceSpitGold::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceSpitGold::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceCashLottery::DataRead* pkDataRead,int iTab = 0);  ///23兑奖
extern int mtPrint(mtServiceCashLottery::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceCashLottery::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchGetRoomInfo::DataRead* pkDataRead,int iTab = 0);  ///24拉取比赛房间信息协议
extern int mtPrint(mtServiceMatchGetRoomInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchGetRoomInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchGetUserInfo::DataRead* pkDataRead,int iTab = 0);  /// 25拉取用户比赛场数据协议
extern int mtPrint(mtServiceMatchGetUserInfo::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchGetUserInfo::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchSignUp::DataRead* pkDataRead,int iTab = 0);  /// 26比赛报名协议.
extern int mtPrint(mtServiceMatchSignUp::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchSignUp::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchGetRoomStatus::DataRead* pkDataRead,int iTab = 0);  /// 27拉取房间状态协议
extern int mtPrint(mtServiceMatchGetRoomStatus::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchGetRoomStatus::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchGetRank::DataRead* pkDataRead,int iTab = 0);  /// 28比赛排名拉取协议
extern int mtPrint(mtServiceMatchGetRank::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchGetRank::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMatchAward::DataRead* pkDataRead,int iTab = 0);  /// 29比赛领奖协议
extern int mtPrint(mtServiceMatchAward::DataWrite* pkDataWrite,int iTab = 0);
extern int mtPrint(mtServiceMatchAward::DataInit* pkDataInit,int iTab = 0);

extern int mtPrint(mtServiceMgr::DataHead* pkDataRead,int iTab = 0);   
extern int mtPrint(mtServiceMgr::DataInit* pkDataInit,int iTab = 0);
extern int mtPrint(mtServiceMgr::DataRun* pkDataRun,int iTab = 0);



