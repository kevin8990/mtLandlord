#pragma once

#include "mtFile.h"

class	mtFileMgr 
{

public:
	mtFileMgr();
	~mtFileMgr();

public:
	int		init(mtFile::DataInit* pkDataInit);

	int		runRead( mtThreadWork::DataUser* pkDataUser );
	int		runWrite( mtThreadWork::DataUser* pkDataUser );

	int		completeLockFileEx(mtThreadWork::DataUser* pkDataUser);
	int		completeWriteFile(mtThreadWork::DataUser* pkDataUser, unsigned long ulCompleteTransfer);
	int		completeReadFile(mtThreadWork::DataUser* pkDataUser, unsigned long ulCompleteTransfer);

	mtFile*		m_pkFileList[mtFile::E_FILE_END];

	mtFile::DataInit		m_kDataInit;
};