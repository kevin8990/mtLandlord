#ifndef		__MT_THREAD_CONNECT_H
#define		__MT_THREAD_CONNECT_H
#include "mtThread.h"
#include "mtQueueList.h"
#include "mtQueueUser.h"

class	mtThreadConnect : public mtThread
{
	/// 类内部结构体
public:
	/// 初始化类内部异常处理类型
	enum 
	{
		E_EXCEPTION_BEG = 0,
		E_EXCEPTION_INIT_DATA_IS_NULL,
		E_EXCEPTION_SERVER_SOCKET,
		E_EXCEPTION_SERVER_BIND,
		E_EXCEPTION_SERVER_LISTEN,
		E_EXCEPTION_SERVER_IOCTLSOCKET_GET,
		E_EXCEPTION_SERVER_IOCTLSOCKET_SET,
		E_EXCEPTION__BEGINTHREAD,
		E_EXCEPTION_END
	};

	/// 初始化类结构体参数
	struct DataInit
	{
		long				lStructBytes;		/// 结构体的大小
		unsigned long		ulIpAddress;		/// IP 地址
		int					iPort;				/// 端口号
		int					iListenNum;			/// 监听数
		mtQueueUser*		pkQueueUser;
		mtQueuePacket*		pkQueuePacket;
	};

public:
	mtThreadConnect();
	virtual ~mtThreadConnect();

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int	run();
	virtual int exception();

	SOCKET			    m_iSocketServer;
	SOCKADDR_IN		    m_kSockAddrInServer;

	SOCKET			    m_uiSocketClient;
	SOCKADDR_IN		    m_kSockAddrInClient;

	HANDLE				m_hThread;	
	mtQueueUser*		m_pkQueueUser;
	mtQueuePacket*		m_pkQueuePacket;
};

#endif	///	__MT_THREAD_CONNECT_H