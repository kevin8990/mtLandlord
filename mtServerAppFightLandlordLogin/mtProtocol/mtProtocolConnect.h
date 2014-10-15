#pragma once 
#include "mtProtocol.h"
#include "mtQueueUser.h"

class	mtProtocolConnect : public mtProtocol
{
public:
	enum	EAddrType
	{
		E_ADDR_TYPE_BEG			= 0,

		E_ADDR_TYPE_DOWNLOAD	= E_ADDR_TYPE_BEG,
		E_ADDR_TYPE_CHARGE,
		E_ADDR_TYPE_HALL,

		E_ADDR_TYPE_END			= 3
	};


	struct	DataSendAddr 
	{
		unsigned long	ulIP;
		long			lPort;
	};

	struct	DataSend
	{
		mtProtocol::DataSend	kDataHead;
		long					lVersion[4];
		long                    lUpdateVersion;                     /// 客户端是否强制升级(0 -不强制升级，由客户端决定，1 -强制升级，必须升级)
		long                    lUpdateUrlType;                     /// 0 -暂时没有软件更新信息 1 -是ip地址(127.0.0.1)，2 -url网址(http://www.dshine.com.cn/update/update.apk)
		long                    lUpdateServerPort;                  /// 软件更新服务器的端口(暂时未使用)
		char                    pcUpdateUrl[MT_APP_UPDATE_URL_LEN]; /// 软件更新服务器地址(http://www.dshine.com.cn/update/update.apk)
		DataSendAddr			pkAddr[E_ADDR_TYPE_END];
	};

	struct	DataRecv 
	{
		mtProtocol::DataRecv	kDataHead;
		long					lVersion[4];
		char                    pcPatchId[MT_BYTES_OF_PATCH_ID];      /// 批次号id
		char                    pcVendorId[MT_BYTES_OF_VENDOR_ID];    /// 厂商代码
	};

public:
	mtProtocolConnect();
	virtual ~mtProtocolConnect();

	int		init(DataInit* pkDataInit);
	virtual int	exit();

	virtual	int	run(mtThreadWork::DataUser* pkDataUser);
	int     updateVersion(DataRecv* pkDataRecv, DataSend* pkDataSend);
	int     getUpdateInfo(mtThreadWork::DataUser* pkDataUser);

	//SOCKET					m_uiSocker;
	mtQueueUser::ProjectInfo	kProjectInfo;
};