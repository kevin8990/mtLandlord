#pragma once 
#include "mtPlatform.h"
#include "mtFile.h"

class	mtFileAccount : public mtFile
{
public:
	struct	DataFile
	{
		unsigned long	ulId;
	};

public:
	mtFileAccount();
	~mtFileAccount();

	virtual	int				init(DataInit* pkDataInit);
	virtual char*			getFileName( char* pcAccount );
	virtual	unsigned long	getFileOffset( char* pcAccount );
	virtual unsigned long 	getCreateFileBytes();
	virtual unsigned long	getDataFileBytes();
	virtual unsigned long	getFolderBytes();
private:
	char	m_pcFullFileName[256];
	char*	m_pcFileName;
	int		m_iFileNameBytes;
	int		m_iFolderNameBytes;

	int						m_iAccountMax;
	static	unsigned long 	m_ulCreateFileBytes;
	int						m_piNumAccount[256];
};

inline unsigned long	mtFileAccount::getFolderBytes()
{
	return		m_iFolderNameBytes;
}

inline unsigned long	mtFileAccount::getDataFileBytes()
{
	return		sizeof(DataFile);
}

inline unsigned long 	mtFileAccount::getCreateFileBytes()
{
	return		m_ulCreateFileBytes;
}
