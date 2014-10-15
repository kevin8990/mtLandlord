#pragma once
#include "mtPlatform.h"

class	mtThread : public mtException
{
public:
	enum EOverLappedType
	{
		E_OLT_BEG			= 0,

		E_OLT_ACCEPTEX		= E_OLT_BEG,
		E_OLT_WSARECV,
		E_OLT_WSASEND,
		E_OLT_WSADISCONNECTEX,

		E_OLT_LOCKFILEEx,
//		E_OLT_CREATEFILE,
		E_OLT_READFILE,
		E_OLT_WRITEFILE,

		E_OLT_END
	};

	enum	EThreadState
	{
		E_THREAD_STATE_BEG	= 0,
		E_THREAD_STATE_STOP	= E_THREAD_STATE_BEG,		/// Õ£÷π
		E_THREAD_STATE_RUNNING,							/// ‘À––						/// Ω· ¯
//		E_THREAD_STATE_CONTROL,
		E_THREAD_STATE_SUSPEND,
		E_THREAD_STATE_END
	};
	/*
	struct DataUser 
	{
		OVERLAPPED			kOverLapped;
		EOverLappedType		eOverLappedType;

		SOCKET				uiSocket;
		WSABUF				kWSABufRecv;
		char				pcDataRecv[1024];
		WSABUF				kWSABufSend;
		char				pcDataSend[1024];
	};
	*/
};