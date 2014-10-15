#pragma once
enum EServiceRoom
{
	E_SERVICE_ROOM_BEG	= 0,
	E_SERVICE_ROOM_CONNECT		= E_SERVICE_ROOM_BEG,	/// Á¬½Ó·¿¼ä·þÎñÆ÷
	E_SERVICE_ROOM_DESK_TWO,							/// Ë«ÈË×À
	E_SERVICE_ROOM_DESK_THREE,							/// ÈýÈË×À
	E_SERVICE_ROOM_WAIT_START,							/// µÈ´ý¿ªÊ¼
	E_SERVICE_ROOM_CARD_DEAL,							/// ·¢ÅÆ
	E_SERVICE_ROOM_GRAB_LANDLORD,						/// ½Ð·Ö
	E_SERVICE_ROOM_DOUBLE_SCORE,						/// ¼Ó±¶
	E_SERVICE_ROOM_SHOW_CARDS,							/// Ã÷ÅÆ
	E_SERVICE_ROOM_CARD_OUT,						    /// ³öÅÆ
	E_SERVICE_ROOM_REQUEST_CARDS_LEFT,					/// ÇëÇóÆäËû¼ÒÊÖÖÐµÄÓàÅÆ
	E_SERVICE_ROOM_ROUND_OVER,                          /// Ò»¾Ö½áÊø(¸÷¼ÒÒªÉÏ´«·ÖÊý)
	E_SERVICE_ROOM_CHANGE_DESK,							///»»×ù
	E_SERVICE_ROOM_CHAT_SHORTCUT,						/// ¿ì½ÝÓï
	E_SERVICE_ROOM_CHAT_ICON,							/// ±íÇé
	E_SERVICE_ROOM_CHAT_CONTEXT,						/// ÊäÈëÁÄÌìÄÚÈÝ
	E_SERVICE_ROOM_HEART_BEAT,							///±£³ÖÓÃ»§²»³¬Ê±µÄÐÄÌøÊý¾Ý
	E_SERVICE_ROOM_EXIT,								/// ÍË³ö·¿¼ä
	E_SERVICE_ROOM_FLEE,								///ÓÎÏ·ÌÓÅÜ
	E_SERVICE_ROOM_INCREMENT_GOLD,						/// ¹ºÂò½ð±Ò   Ôö¼ÓµÄÐ­Òé
	E_SERVICE_ROOM_END

};