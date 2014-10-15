#ifndef		__MT_THREAD_CONNECT_H
#define		__MT_THREAD_CONNECT_H
#include "mtThread.h"
#include "mtQueueWork.h"

class	mtThreadConnect : public mtThread
{
	/// 类内部结构体
public:
	/// 初始化类内部异常处理类型
	enum 
	{
		EXCEPTION_BEG = 0,
		EXCEPTION_INIT_DATA_IS_NULL,
		EXCEPTION_SERVER_socket,
		EXCEPTION_SERVER_bind,
		EXCEPTION_SERVER_listen,
		EXCEPTION_SERVER_ioctlsocket_get,
		EXCEPTION_SERVER_ioctlsocket_set,
		EXCEPTION__beginthread,
		EXCEPTION_END
	};

	/// 初始化类结构体参数
	struct DataInit
	{
	public:
		int		            iStructBytes;		/// 结构体的大小
		unsigned long	    ulIpAddress;		/// IP 地址
		int		            iPort;				/// 端口号
		int		            iListenNum;			/// 监听数
		mtQueueWorkList*	pkQueueWorkList;	/// 工作队列
	};

public:
	mtThreadConnect();
	virtual ~mtThreadConnect();

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	SOCKET			m_iSocketServer;
	SOCKADDR_IN		m_kSockAddrInServer;

	SOCKET			m_iSocketClient;
	SOCKADDR_IN		m_kSockAddrInClient;

// 	unsigned long	m_ulIoctlSocketFionbioFlagOld;
 	unsigned long	m_ulIoctlSocketFionbioFlagCur;

	uintptr_t		m_uiThread;	
	mtQueueWorkList*	m_pkQueueWorkList;



};

#endif	///	__MT_THREAD_CONNECT_H