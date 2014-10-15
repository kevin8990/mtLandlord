#ifndef			__MT_SERVER_H_
#define			__MT_SERVER_H_

#include "mtPlatform.h"
#include "mtException.h"
#include "mtSQLEnv.h"
#include "mtThreadMgr.h"


class mtServer : public mtException
{
public:
	enum   {				//异常代码枚举
		E_EXCEPTION_BEG	= 0,

		E_EXCEPTION_WSA_STARTUP,					//初始化socket库异常
		E_EXCEPTION_CLIENT_SOCKET,					//初始化socket异常
		E_EXCEPTION_WSAIOCTL_ACCEPTEX,
		E_EXCEPTION_WSAIOCTL_DISCONNECTEX,
		E_EXCEPTION_CLOSE_SOCKET,

		E_EXCEPTION_END,
	};
	struct DataInit {
		long				lStructBytes;			//DataInit结构体大小
		unsigned long		ulWorkIP;				//登录服务器IP
		unsigned short		usWorkPort;				//登陆服务器端口

		int					iUserListTotal;						//
		unsigned long		ulTimeWaitMilliseconds;				//
		int					iTimeConnectLimitSencods;			//
	};

public:
	mtServer();
	~mtServer();
	int				init(DataInit* pkDataInit);
	int				run();
	int				exit();
	virtual int		exception();

	int             initParams();
	int				initDataBase();
	int				initSocketFunction();
	int				initThreadMgr();
private:
	DataInit					m_kDataInit;						//
	WSADATA						m_kWSAData;
	int							m_iWSADataRet;						//WSAStartup返回值
	LPFN_ACCEPTEX				m_lpfnAcceptEx;						//AcceptEx函数指针
	LPFN_DISCONNECTEX			m_lpfnDisconnectEx;					//DisconnectEx函数指针
	
	mtThreadMgr					m_kThreadMgr;
	mtSQLEnv					m_kSQLEnv;							//数据库实例
};

#endif		/// __MT_SERVER_H_