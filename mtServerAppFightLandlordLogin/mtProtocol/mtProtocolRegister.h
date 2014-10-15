#ifndef		__MT_PROTOCOL_REGISTER_H
#define		__MT_PROTOCOL_REGISTER_H

#include "mtProtocol.h"

class	mtProtocolRegister : public mtProtocol
{
public:
	enum	EFileType 
	{
		E_REGISTER_TYPE_BEG	= 0,

		E_REGISTER_TYPE_ID	= E_REGISTER_TYPE_BEG,
		E_REGISTER_TYPE_ACCOUNT,
		E_REGISTER_TYPE_QQ,
		E_REGISTER_TYPE_PHONE,
		E_REGISTER_TYPE_TRY,

		E_REGISTER_TYPE_END
	};
	struct	DataSend
	{
		mtProtocol::DataSend	kDataHead;
		unsigned long			uUserId;                              /// 用户id
		char                    pcAccount[MT_BYTES_OF_ACCOUNT];       /// 用户账号
		char                    pcPwd[MT_BYTES_OF_PWD];               /// 注册账号密码(快速登录默认是000000)
		long                    lRegister;                            /// 是否已经注册(0 -未注册，1 -已注册)，未注册下个字段lUserRegisterDate表示首次快速登录的时间
		long                    lUserRegisterDate;                    /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
	};	

	struct	DataRecv 
	{
		mtProtocol::DataRecv	kDataHead;
		unsigned long           uUserId;                              /// 用户id(0 -表示无id注册，其他表示有id注册)
		long					lAccountType;
		char                    pcAccount[MT_BYTES_OF_ACCOUNT];
		char                    pcQQ[MT_BYTES_OF_QQ];
		char                    pcPhone[MT_BYTES_OF_PHONE];
		char                    pcPwd[MT_BYTES_OF_PWD];               /// 注册账号密码(快速登录默认是000000)
		char                    pcCompanyId[MT_BYTES_OF_COMPANY_ID];  /// 公司id
		char                    pcPatchId[MT_BYTES_OF_PATCH_ID];      /// 批次号id
		char                    pcVendorId[MT_BYTES_OF_VENDOR_ID];    /// 厂商代码
		char                    pcResevation[MT_BYTES_OF_RESERVATION];/// 扩展字段
	};

public:
	mtProtocolRegister();
	virtual ~mtProtocolRegister();

	int		init(DataInit* pkDataInit);
	virtual int	exit();

	virtual	int	run(mtThreadWork::DataUser* pkDataUser);

	unsigned long	getNewId();

	unsigned long	m_ulId;

};
#endif	///	__MT_PROTOCOL_REGISTER_H