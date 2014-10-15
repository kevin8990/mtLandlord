#ifndef		__MT_SERVICE_MGR_H
#define		__MT_SERVICE_MGR_H

#include "mtService.h"
#include "mtPlatform.h"
#include "mtQueueUser.h"
#include "mtServiceConnect.h"
#include "mtQueueMgr.h"

class mtServiceMgr
{
public:
	enum EServiceId
	{
		E_SERVICE_ROOM_BEG	= 0,
		E_SERVICE_ROOM_CONNECT		= E_SERVICE_ROOM_BEG,	    /// 0连接房间服务器
		E_SERVICE_ROOM_DESK_TWO,							    /// 1双人桌
		E_SERVICE_ROOM_DESK_THREE,							    /// 2三人桌
		E_SERVICE_ROOM_WAIT_START,							    /// 3等待开始
		E_SERVICE_ROOM_CARD_DEAL,							    /// 4发牌
		E_SERVICE_ROOM_GRAD_LANDLORD,						    /// 5叫分
		E_SERVICE_ROOM_DOUBLE_SCORE,						    /// 6加倍
		E_SERVICE_ROOM_SHOW_CARD,						        /// 7明牌
		E_SERVICE_ROOM_CARD_OUT,						        /// 8出牌
		E_SERVICE_ROOM_REQUEST_CARDS_LEFT,                      /// 9请求其它各家手里的余牌信息
		E_SERVICE_ROOM_ROUND_OVER,                              /// 10一局结束(各家要上传分数)
		E_SERVICE_ROOM_CHANGE_DESK,                             /// 11换桌子
		E_SERVICE_ROOM_CHAT_SHORTCUT,						    /// 12快捷语
		E_SERVICE_ROOM_CHAT_ICON,							    /// 13表情
		E_SERVICE_ROOM_CHAT_CONTEXT,						    /// 14输入聊天内容
		E_SERVICE_ROOM_HEART_BEAT,                              /// 15房间心跳包服务
		E_SERVICE_ROOM_EXIT,								    /// 16退出房间
		E_SERVICE_ROOM_INCREMENT_GOLD,                          /// 17房间增加金币服务
		E_SERVICE_ROOM_END
	};

	struct	DataHead
	{
		long	lStructBytes;
		long	lServiceId;
		long	plReservation[2];
	};

	struct DataInit
	{
		long		            lStructBytes;
		int		                iSpinCount;
		HANDLE	                hIOCP;
		HANDLE					hEventManagerAi;
        mtQueueMgr*             pkQueueMgr;
	};

public:
	mtServiceMgr();
	virtual ~mtServiceMgr();

	int				init(DataInit* pkDataInit);
	virtual int		run(mtService::DataRun* pkDataRun);
	virtual int		exit();

private:
	int			initService();
	int			initServiceConnect();
	int			initServiceRoomDeskTwo();
	int			initServiceRoomDeskThree();
	int			initServiceRoomWaitStartRecv();
	int			initServiceRoomCardDeal();
	int         initServiceRoomGrabLandlord();
	int         initServiceRoomCardDoubleScore();
	int         initServiceRoomShowCard();
	int         initServiceRoomCardOut();
	int         initServiceRoomRequestCardsLeft();
	int         initServiceRoomRoundOver();
	int         initServiceRoomChangeDesk();
	int         initServiceRoomChatShortCut();
	int         initServiceRoomChatIcon();
	int         initServiceRoomChatContext();
	int         initServiceRoomHeartBeat();
	int         initServiceRoomIncrementGold();
	int         initServiceRoomExit();

private:
	int		                     m_iSpinCount;
	mtService*		             m_pkServiceList[E_SERVICE_ROOM_END];
	HANDLE	                     m_hIOCP;
	HANDLE					     m_hEventManagerAi;
	mtQueueMgr*                  m_pkQueueMgr;
};

#endif	///	__MT_SERVICE_MGR_H