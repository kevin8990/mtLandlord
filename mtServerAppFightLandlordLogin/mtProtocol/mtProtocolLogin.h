#pragma once 
#include "mtProtocol.h"

class mtFileMgr;

class	mtProtocolLogin : public mtProtocol
{
public:
	enum	EFileType 
	{
		E_ACCOUNT_TYPE_BEG	= 0,

		E_ACCOUNT_TYPE_ID	= E_ACCOUNT_TYPE_BEG,
		E_ACCOUNT_TYPE_ACCOUNT,
		E_ACCOUNT_TYPE_QQ,
		E_ACCOUNT_TYPE_PHONE,

		E_ACCOUNT_TYPE_END
	};

	struct	DataSend
	{
		mtProtocol::DataSend		kDataHead;
		unsigned long				ulId;
		long                        lRegister;             /// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间
		long                        lUserRegisterDate;     /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
	};	

	struct	DataRecv 
	{
		mtProtocol::DataRecv		kDataHead;
		long						lAccountType;
		char                        pcAccount[MT_BYTES_OF_ACCOUNT];
		char                        pcPwd[MT_BYTES_OF_PWD];
	};

public:
	mtProtocolLogin();
	virtual ~mtProtocolLogin();

	int		init(DataInit* pkDataInit);
	virtual int	exit();

	virtual	int	run(mtThreadWork::DataUser* pkDataUser);
};