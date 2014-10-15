#include "mtFileAccount.h"

unsigned long 	mtFileAccount::m_ulCreateFileBytes	= 0;

mtFileAccount::mtFileAccount()
{

}

mtFileAccount::~mtFileAccount()
{

}

int mtFileAccount::init( DataInit* pkDataInit )
{
	/// 记录目录
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
	
	/// 建立创建数据
	m_iAccountMax		= 1 + 10 + 26;		/// 1 NULL, 10 (0-9), 26 (a-z)
	m_ulCreateFileBytes	= m_iAccountMax * m_iAccountMax * m_iAccountMax * m_iAccountMax * sizeof(DataFile);
	m_ulCreateFileBytes	= MT_BYTES_ALIGNED(m_ulCreateFileBytes, MT_BYTES_OF_SECTOR);

	/// 建立表
	int		iNum		= 0;
	for (iNum = 0; iNum < 256; iNum++)
	{
		if (0 == iNum)
		{
			m_piNumAccount[iNum]	= 0;
		}
		else if (iNum >= '0' && iNum <= '9')		//48-57
		{
			m_piNumAccount[iNum]	= iNum - '0' + 1;
		}
		else if (iNum >= 'a' && iNum <= 'z')		//97-122
		{
			m_piNumAccount[iNum]	= iNum - 'a' + 11;
		}
		else if (iNum >= 'A' && iNum <= 'Z')		//65-90
		{
			m_piNumAccount[iNum]	= iNum - 'A' + 11;
		}
		else 
		{
			m_piNumAccount[iNum]	= -1;
		}
		//printf("\n%d:%d ",iNum,m_piNumAccount[iNum]);
	}

	return	1;
}

char* mtFileAccount::getFileName( char* pcAccount )
{
	int		iIndex;
	for (iIndex = 0; iIndex < 12; iIndex++)
	{
		//printf("\n%d:%d:%d",iIndex,pcAccount[iIndex],m_piNumAccount[pcAccount[iIndex]]);
		if (-1 == m_piNumAccount[pcAccount[iIndex]])
		{
			return NULL;
		}
	}
	int		iFolder0	= m_piNumAccount[pcAccount[11]] * m_iAccountMax + m_piNumAccount[pcAccount[10]];
	int		iFolder1	= m_piNumAccount[pcAccount[9]] * m_iAccountMax + m_piNumAccount[pcAccount[8]];
	int		iFolder2	= m_piNumAccount[pcAccount[7]] * m_iAccountMax + m_piNumAccount[pcAccount[6]];
	int		iFile		= m_piNumAccount[pcAccount[5]] * m_iAccountMax + m_piNumAccount[pcAccount[4]];
	//指定长度m_iFileNameBytes，防止缓冲区溢出
	sprintf_s(m_pcFileName, m_iFileNameBytes, "account\\%d\\%d\\%d\\%d",
		iFolder0, iFolder1, iFolder2, iFile);

	return	m_pcFullFileName;
}

unsigned long mtFileAccount::getFileOffset( char* pcAccount )
{
	unsigned long ulOffset = 0;
	int		iIndex;
	for (iIndex = 3; iIndex >= 0; --iIndex)
	{
		ulOffset *= m_iAccountMax;
		ulOffset += m_piNumAccount[pcAccount[iIndex]];
	}

	return ulOffset * sizeof(DataFile);
}
