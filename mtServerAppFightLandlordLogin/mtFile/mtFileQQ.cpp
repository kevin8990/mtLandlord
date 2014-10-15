#include "mtFileQQ.h"

unsigned long	mtFileQQ::m_ulCreateFileBytes	= 0;

mtFileQQ::mtFileQQ()
{

}

mtFileQQ::~mtFileQQ()
{

}

int mtFileQQ::init( DataInit* pkDataInit )
{
	mtFile::init(pkDataInit);

	char*	pcFolder	= pkDataInit->pcFolder;
	m_pcFileName		= m_pcFullFileName;
	while (*pcFolder)
	{
		*m_pcFileName++	= *pcFolder++;
	}
	*m_pcFileName		= 0;
	FolderCreate(m_pcFullFileName);

	m_iFolderNameBytes	= m_pcFileName - m_pcFullFileName;
	m_iFileNameBytes	= sizeof(m_pcFullFileName) - m_iFolderNameBytes;

	m_iQQMax			= 10;				/// 0 (NULL,0) 10 (0-9)

	m_ulCreateFileBytes	= m_iQQMax * m_iQQMax * m_iQQMax * m_iQQMax * m_iQQMax * m_iQQMax * sizeof(DataFile);
	m_ulCreateFileBytes	= MT_BYTES_ALIGNED(m_ulCreateFileBytes, MT_BYTES_OF_SECTOR);

	for (int iNum = 0; iNum < 256; iNum++)
	{
		if (0 == iNum)
		{
			m_piNumQQ[iNum]	= 0;
		}
		else if (iNum >= '0' && iNum <= '9')
		{
			m_piNumQQ[iNum]	= iNum - '0';
		}
		else 
		{
			m_piNumQQ[iNum]	= -1;
		}
	}

	return	1;
}

char* mtFileQQ::getFileName( char* pcQQ )
{
	int		iIndex;
	for (iIndex = 0; iIndex < 12; iIndex++)
	{
		if (-1 == m_piNumQQ[pcQQ[iIndex]])
		{
			return NULL;
		}
	}
	int		iFolder0	= m_piNumQQ[pcQQ[9]] * m_iQQMax + m_piNumQQ[pcQQ[8]];
	int		iFile		= m_piNumQQ[pcQQ[7]] * m_iQQMax + m_piNumQQ[pcQQ[6]];
	sprintf_s(m_pcFileName, m_iFileNameBytes, "qq\\%d\\%d", 
		iFolder0, iFile);

	return	m_pcFullFileName;
}
