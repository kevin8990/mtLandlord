#ifndef		__MT_THREAD_MONITOR_CONNECT_H
#define		__MT_THREAD_MONITOR_CONNECT_H
#include "mtThread.h"
#include "mtQueueWork.h"
#include "mtQueueHall.h"

/// 监听房间服务器来得连接
class mtServiceMonitorMgr;
class	mtThreadMonitorConnect : public mtThread
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
		long		             lStructBytes;		    /// 结构体的大小
		unsigned long	         ulIpAddress;		    /// IP 地址
		int		                 iPort;				    /// 端口号
		int		                 iListenNum;			/// 监听数
		mtServiceMonitorMgr*     pkServiceMonitorMgr;
		mtQueueHall::DataHall*   pkDataHall;   	        /// 场信息
		/// mtQueueHall::DataSpace*  pkSpace;   	        /// 场信息
	};

public:
	mtThreadMonitorConnect();
	virtual ~mtThreadMonitorConnect();

	int		init(DataInit* pkInitData);
	int		exit();

	virtual int run();
	virtual int exception();

	SOCKET			        m_iSocketServer;
	SOCKADDR_IN		        m_kSockAddrInServer;

	SOCKET			        m_iSocketClient;
	SOCKADDR_IN		        m_kSockAddrInClient;

// 	unsigned long	m_ulIoctlSocketFionbioFlagOld;
 	unsigned long	        m_ulIoctlSocketFionbioFlagCur;

	uintptr_t		        m_uiThread;

	mtServiceMonitorMgr*    m_pkServiceMonitorMgr;
	mtQueueHall::DataHall*  m_pkDataHall;   	                       /// 场信息
	/// mtQueueHall::DataSpace* m_pkSpace;   	        /// 场信息

};

#endif	///	__MT_THREAD_MONITOR_CONNECT_H
