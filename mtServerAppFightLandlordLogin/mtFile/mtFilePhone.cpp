#include "mtFilePhone.h"

unsigned long 	mtFilePhone::m_ulCreateFileBytes	= 0;

mtFilePhone::mtFilePhone()
{

}

mtFilePhone::~mtFilePhone()
{

}

int mtFilePhone::init( DataInit* pkDataInit )
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

	for (int iNum = 0; iNum < 256; iNum++)
	{
		if (0 == iNum)
		{
			m_piNumPhone[iNum]	= 0;
		}
		else if (iNum >= '0' && iNum <= '9')
		{
			m_piNumPhone[iNum]	= iNum - '0';
		}
		else 
		{
			m_piNumPhone[iNum]	= -1;
		}
	}
	m_iPhoneMax		= 10;				/// 10 (0-9)

	m_ulCreateFileBytes	= m_iPhoneMax * m_iPhoneMax * m_iPhoneMax * m_iPhoneMax * m_iPhoneMax * m_iPhoneMax * sizeof(DataFile);
	m_ulCreateFileBytes	= MT_BYTES_ALIGNED(m_ulCreateFileBytes, MT_BYTES_OF_SECTOR);

	return	1;
}

char* mtFilePhone::getFileName( char* pcPhone )
{
	int		iIndex;
	for (iIndex = 0; iIndex < 12; iIndex++)
	{
		if (-1 == m_piNumPhone[pcPhone[iIndex]])
		{
			return NULL;
		}
	}
	int		iFolder0	= m_piNumPhone[pcPhone[9]] * m_iPhoneMax + m_piNumPhone[pcPhone[8]];
	int		iFile		= m_piNumPhone[pcPhone[7]] * m_iPhoneMax + m_piNumPhone[pcPhone[6]];
	sprintf_s(m_pcFileName, m_iFileNameBytes, "phone\\%d\\%d", 
		iFolder0, iFile);

	return	m_pcFullFileName;
}
