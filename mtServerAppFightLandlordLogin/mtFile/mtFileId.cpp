#include "mtFileId.h"

unsigned long 	mtFileId::m_ulCreateFileBytes	= 0;

mtFileId::mtFileId()
{

}

mtFileId::~mtFileId()
{

}

int mtFileId::init( DataInit* pkDataInit )
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

	m_ulCreateFileBytes	= (1<<8) * (1<<8) * sizeof(DataFile);
	m_ulCreateFileBytes	= MT_BYTES_ALIGNED(m_ulCreateFileBytes, MT_BYTES_OF_SECTOR);

	return	1;
}

char* mtFileId::getFileName( char* pcAccount )
{
	unsigned char*	pucId = (unsigned char*)pcAccount;

	sprintf_s(m_pcFileName, m_iFileNameBytes, "id\\%d\\%d", pucId[3], pucId[2]);

	return	m_pcFullFileName;
}
