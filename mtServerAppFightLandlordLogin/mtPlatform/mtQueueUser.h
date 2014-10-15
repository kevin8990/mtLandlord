#ifndef		__MT_QUEUE_USER_H
#define		__MT_QUEUE_USER_H

class mtQueueUser 
{
public:

	enum	EACCOUNTType 
	{
		E_ACCOUNT_TYPE_BEG	= 0,

		E_ACCOUNT_TYPE_ID	= E_ACCOUNT_TYPE_BEG,
		E_ACCOUNT_TYPE_ACCOUNT,
		E_ACCOUNT_TYPE_QQ,
		E_ACCOUNT_TYPE_PHONE,

		E_ACCOUNT_TYPE_END
	};

	struct	UserInfo
	{
		unsigned long	ulId;
		char	        pcAccount[MT_BYTES_OF_ACCOUNT];
		char			pcNickName[MT_BYTES_OF_ACCOUNT];
		char            pcQQ[MT_BYTES_OF_QQ];
		char            pcPhone[MT_BYTES_OF_PHONE];
		char            pcPwd[MT_BYTES_OF_PWD];
		char            pcCompanyId[MT_BYTES_OF_COMPANY_ID];   /// 公司id
		char            pcPatchId[MT_BYTES_OF_PATCH_ID];       /// 批次号id
		char            pcVendorId[MT_BYTES_OF_VENDOR_ID];     /// 厂商代码
		char            pcResevation[MT_BYTES_OF_RESERVATION]; /// 扩展字段
		long            lUserRegisterDate;                     /// 注册日期高2字节表示年，次底字节表示月，低字节表示日(XXXX-XX-XX)
	};

	struct ProjectInfo
	{
		char            pcVendorId[MT_BYTES_OF_VENDOR_ID];     /// 厂商代码
		char            pcPatchId[MT_BYTES_OF_PATCH_ID];       /// 批次号id
		char            pcUpdateUrl[MT_APP_UPDATE_URL_LEN];
		long			lUpdateType;
		long			lUpdatePort;
		long			lVersion3;
		long			lVersion2;
		long			lVersion1;
		long			lVersion0;
	};

};


#endif	///	__MT_QUEUE_NODE_USER_H