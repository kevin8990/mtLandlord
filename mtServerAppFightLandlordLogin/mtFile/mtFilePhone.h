#pragma once
#include "mtPlatform.h"
#include "mtFile.h"

class	mtFilePhone : public mtFile
{
public:
	struct	DataFile
	{
		unsigned long	ulId;
	};

public:
	mtFilePhone();
	~mtFilePhone();

	virtual	int				init(DataInit* pkDataInit);
	virtual	char*			getFileName(char* pcPhone);
	virtual unsigned long	getFileOffset(char* pcPhone);
	virtual	unsigned long 	getCreateFileBytes();
	virtual	unsigned long	getDataFileBytes();
	virtual unsigned long	getFolderBytes();

protected:

	char	m_pcFullFileName[256];
	char*	m_pcFileName;
	int		m_iFileNameBytes;
	int		m_iFolderNameBytes;
	int		m_piNumPhone[256];
	int		m_iPhoneMax;

	static	unsigned long 	m_ulCreateFileBytes;
};

inline unsigned long	mtFilePhone::getFolderBytes()
{
	return		m_iFolderNameBytes;
}

inline unsigned long	mtFilePhone::getDataFileBytes()
{
	return		sizeof(DataFile);
}

inline unsigned long 	mtFilePhone::getCreateFileBytes()
{
	return		m_ulCreateFileBytes;
}


inline unsigned long mtFilePhone::getFileOffset( char* pcPhone )
{
	unsigned long ulOffset  = 0;
	int		iIndex;
	for (iIndex = 5; iIndex >= 0; --iIndex)
	{
		ulOffset *= m_iPhoneMax;
		ulOffset += m_piNumPhone[pcPhone[iIndex]];
	}

	return ulOffset * sizeof(DataFile);
}