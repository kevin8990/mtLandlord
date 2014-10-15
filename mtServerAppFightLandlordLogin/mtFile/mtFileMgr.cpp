#include "mtFileMgr.h"
#include "mtFileAccount.h"
#include "mtFileQQ.h"
#include "mtFilePhone.h"
#include "mtFileId.h"
#include "mtProtocol.h"

mtFileMgr::mtFileMgr()
{

}

mtFileMgr::~mtFileMgr()
{

}

int	mtFileMgr::init(mtFile::DataInit* pkDataInit)
{
	memcpy(&m_kDataInit, pkDataInit, sizeof(m_kDataInit));       
	m_pkFileList[mtFile::E_FILE_ACCOUNT]	= new mtFileAccount;
	m_pkFileList[mtFile::E_FILE_ACCOUNT]->init(pkDataInit);
	m_pkFileList[mtFile::E_FILE_QQ]			= new mtFileQQ;
	m_pkFileList[mtFile::E_FILE_QQ]->init(pkDataInit);
	m_pkFileList[mtFile::E_FILE_PHONE]		= new mtFilePhone;
	m_pkFileList[mtFile::E_FILE_PHONE]->init(pkDataInit);
	m_pkFileList[mtFile::E_FILE_ID]			= new mtFileId;
	m_pkFileList[mtFile::E_FILE_ID]->init(pkDataInit);

	return	1;
}

int mtFileMgr::runRead( mtThreadWork::DataUser* pkDataUser )
{
	if (pkDataUser->iFileType >= mtFile::E_FILE_BEG 
		&& pkDataUser->iFileType < mtFile::E_FILE_END)
	{
		return	m_pkFileList[pkDataUser->iFileType]->runRead(pkDataUser);
	}
	return	0;
}

int mtFileMgr::runWrite( mtThreadWork::DataUser* pkDataUser )
{
	if (pkDataUser->iFileType >= mtFile::E_FILE_BEG 
		&& pkDataUser->iFileType < mtFile::E_FILE_END)
	{
		return	m_pkFileList[pkDataUser->iFileType]->runWrite(pkDataUser);
	}
	
	return	0;
}

int mtFileMgr::completeLockFileEx( mtThreadWork::DataUser* pkDataUser)
{
	int		iResult		= mtProtocol::E_RESULT_SUCCESS;
	switch (pkDataUser->iFileRunType)
	{
	case	mtFile::E_FILE_RUN_READ_FILE:
		iResult	= m_pkFileList[pkDataUser->iFileType]->runReadFile(pkDataUser);
		break;
	case	mtFile::E_FILE_RUN_WRITE_FILE:
		iResult	= m_pkFileList[pkDataUser->iFileType]->runWriteFile(pkDataUser);
		break;
	case	mtFile::E_FILE_RUN_WRITE_FILE_CREATE:
		iResult	= m_pkFileList[pkDataUser->iFileType]->runWriteFileCreate(pkDataUser);
		break;
	}

	if (iResult <= 0)
	{
		mtFile::FileClose(&pkDataUser->hFile);
		return	pkDataUser->FileCompletionRoutine(iResult, pkDataUser);
	}

	return	iResult;
}

int mtFileMgr::completeWriteFile( mtThreadWork::DataUser* pkDataUser, unsigned long ulCompleteTransferBytes )
{
	UnlockFileEx(pkDataUser->hFile, 0, pkDataUser->ulTransferBytes, 0, &pkDataUser->kOverlapped);
	int		iResult		= 0;
	if (pkDataUser->ulTransferBytes == ulCompleteTransferBytes)
	{
		switch (pkDataUser->iFileRunType)
		{
		case	mtFile::E_FILE_RUN_WRITE_FILE_CREATE:
			return	m_pkFileList[pkDataUser->iFileType]->completeWriteCreate(pkDataUser);
		default:
			iResult		= mtProtocol::E_RESULT_SUCCESS;
		}
	}
	else 
	{
		iResult			= mtProtocol::E_RESULT_FALT_FILE_WRITE_COMPLETE;
	}

	mtFile::FileClose(&pkDataUser->hFile);
	return	pkDataUser->FileCompletionRoutine(iResult, pkDataUser);

}

int mtFileMgr::completeReadFile( mtThreadWork::DataUser* pkDataUser, unsigned long ulCompleteTransferBytes )
{
	UnlockFileEx(pkDataUser->hFile, 0, pkDataUser->ulTransferBytes, 0, &pkDataUser->kOverlapped);
	mtFile::FileClose(&pkDataUser->hFile);

	int		iResult		= (pkDataUser->ulTransferBytes == ulCompleteTransferBytes) ? 
		mtProtocol::E_RESULT_SUCCESS : mtProtocol::E_RESULT_FALT_FILE_READ_COMPLETE;

	return	pkDataUser->FileCompletionRoutine(iResult, pkDataUser);
}

