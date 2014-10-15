#pragma once
#include "mtPlatform.h"
#include "mtFile.h"

class	mtFileQQ : public mtFile
{
public:
	struct	DataFile
	{
		unsigned long	ulId;
	};

public:
	mtFileQQ();
	~mtFileQQ();

	virtual	int				init(DataInit* pkDataInit);
	virtual	char*			getFileName(char* pcQQ);
	virtual unsigned long	getFileOffset(char* pcQQ);
	virtual	unsigned long 	getCreateFileBytes();
	virtual	unsigned long	getDataFileBytes();
	virtual unsigned long	getFolderBytes();

protected:

	char	m_pcFullFileName[256];
	char*	m_pcFileName;
	int		m_iFileNameBytes;
	int		m_iFolderNameBytes;
	int		m_piNumQQ[256];
	int		m_iQQMax;

	static	unsigned long 	m_ulCreateFileBytes;
};

inline unsigned long mtFileQQ::getFileOffset( char* pcQQ )
{
	unsigned long ulOffset  = 0;
	int		iIndex;
	for (iIndex = 5; iIndex >= 0; iIndex--)
	{
		ulOffset *= m_iQQMax;
		ulOffset += m_piNumQQ[pcQQ[iIndex]];
	}

	return ulOffset * sizeof(DataFile);
}

inline unsigned long mtFileQQ::getCreateFileBytes()
{
	return		m_ulCreateFileBytes;
}

inline unsigned long mtFileQQ::getDataFileBytes()
{
	return		sizeof(DataFile);
}

inline unsigned long mtFileQQ::getFolderBytes()
{
	return		m_iFolderNameBytes;
}
