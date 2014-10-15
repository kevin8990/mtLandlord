#pragma once
#include "mtProtocol.h"

class mtProtocolMgr : public mtProtocol, public mtException
{
public:
	enum 
	{
		E_EXCEPTION_BEG	= 0,

		E_EXCEPTION_CREATE_PROTOCOL,

		E_EXCEPTION_END
	};
public:
	mtProtocolMgr();
	~mtProtocolMgr();

	int		init(DataInit* pkDataInit);
	int		exit();

	virtual	int	run(mtThreadWork::DataUser* pkDataUser);

	//mtProtocol*	getProtocolFromId(mtThreadWork::DataUser* pkDatauser);

	int		exception();
private:

// 	DataInit			m_kDataInit;
	mtProtocol*			m_pkProtocolList[E_PROTOCOL_END];

};

