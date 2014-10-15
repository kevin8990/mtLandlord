#include "mtFile.h"
#include "mtProtocol.h"


char			mtFile::ms_pcCreateFileData[11<<20]	= {0};
HANDLE			mtFile::ms_hIOCP					= NULL;
volatile long	mtFile::ms_lFileHandleTotal			= 0;

mtFile::mtFile()
{

}

mtFile::~mtFile()
{

}

int mtFile::runRead( mtThreadWork::DataUser* pkDataUser )
{
	char*	pcFile	= getFileName(pkDataUser->pcAccount);

	if (NULL == pcFile)
	{
		return	mtProtocol::E_RESULT_FALT_FILE_INVALIDE_PATH;
	}

	if (0 == FileOpen(&pkDataUser->hFile, pcFile))
	{
		return	mtProtocol::E_RESULT_FALT_FILE_NOT_EXIST;
	}

	pkDataUser->iFileOffset			= getFileOffset(pkDataUser->pcAccount);
	pkDataUser->ulTransferBytes		= getDataFileBytes();

	pkDataUser->kOverlapped.Offset	= pkDataUser->iFileOffset;
	pkDataUser->eOverlappedType		= mtThread::E_OLT_LOCKFILEEx;
	pkDataUser->iFileRunType		= E_FILE_RUN_READ_FILE;
	
	if (0 == FileLock(pkDataUser->hFile, pkDataUser->ulTransferBytes, &pkDataUser->kOverlapped))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_LOCK;
	}

	return	mtProtocol::E_RESULT_SUCCESS;
}

int mtFile::runReadFile( mtThreadWork::DataUser* pkDataUser )
{
	pkDataUser->eOverlappedType		= mtThread::E_OLT_READFILE;
 	if (0 == FileRead(pkDataUser->hFile, pkDataUser->ulTransferBytes, pkDataUser->pcDataTransfer, 
 		&pkDataUser->kOverlapped))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_READ_FUNCTION;
	}

	return	mtProtocol::E_RESULT_SUCCESS;
}

int mtFile::runWrite( mtThreadWork::DataUser* pkDataUser )
{
	char*	pcFile	= getFileName(pkDataUser->pcAccount);

	if (NULL == pcFile)
	{
		return	mtProtocol::E_RESULT_FALT_FILE_INVALIDE_PATH;
	}

	if (FileOpen(&pkDataUser->hFile, pcFile))
	{
 		pkDataUser->iFileOffset			= getFileOffset(pkDataUser->pcAccount);
 		pkDataUser->ulTransferBytes		= getDataFileBytes();
		pkDataUser->iFileRunType		= E_FILE_RUN_WRITE_FILE;
	}
	else 
	{
		/// 创建文件
		if (FileCreate(&pkDataUser->hFile, pcFile, getFolderBytes()))
		{
			pkDataUser->iFileOffset			= 0;
			pkDataUser->ulTransferBytes		= getCreateFileBytes();
			pkDataUser->iFileRunType		= E_FILE_RUN_WRITE_FILE_CREATE;
		}
		else
		{
			return	mtProtocol::E_RESULT_FALT_FILE_CREATE_FUNCTION;
		}
	}

	pkDataUser->kOverlapped.Offset	= pkDataUser->iFileOffset;
	pkDataUser->eOverlappedType		= mtThread::E_OLT_LOCKFILEEx;
	if (0 == FileLock(pkDataUser->hFile, pkDataUser->ulTransferBytes, &pkDataUser->kOverlapped, LOCKFILE_EXCLUSIVE_LOCK))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_LOCK;
	}

	return	mtProtocol::E_RESULT_SUCCESS;
}

int mtFile::runWriteFile( mtThreadWork::DataUser* pkDataUser )
{
	pkDataUser->eOverlappedType		= mtThread::E_OLT_WRITEFILE;
	if (0 == FileWrite(pkDataUser->hFile, pkDataUser->ulTransferBytes, pkDataUser->pcDataTransfer, 
		&pkDataUser->kOverlapped))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_READ_FUNCTION;
	}

	return	mtProtocol::E_RESULT_SUCCESS;

}

int mtFile::runWriteFileCreate( mtThreadWork::DataUser* pkDataUser )
{
	pkDataUser->eOverlappedType		= mtThread::E_OLT_WRITEFILE;
	if (0 == FileWrite(pkDataUser->hFile, pkDataUser->ulTransferBytes, 
		ms_pcCreateFileData, &pkDataUser->kOverlapped))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_READ_FUNCTION;
	}

	return	mtProtocol::E_RESULT_SUCCESS;
}

int mtFile::completeWriteCreate( mtThreadWork::DataUser* pkDataUser )
{
	pkDataUser->iFileOffset			= getFileOffset(pkDataUser->pcAccount);
	pkDataUser->ulTransferBytes		= getDataFileBytes();
	pkDataUser->iFileRunType		= E_FILE_RUN_WRITE_FILE;

	pkDataUser->kOverlapped.Offset	= pkDataUser->iFileOffset;
	pkDataUser->eOverlappedType		= mtThread::E_OLT_LOCKFILEEx;
	if (0 == FileLock(pkDataUser->hFile, pkDataUser->ulTransferBytes, &pkDataUser->kOverlapped, LOCKFILE_EXCLUSIVE_LOCK))
	{
		FileClose(&pkDataUser->hFile);
		return	mtProtocol::E_RESULT_FALT_FILE_LOCK;
	}

	return	mtProtocol::E_RESULT_SUCCESS;

}

int mtFile::init( DataInit* pkDataInit )
{
	ms_hIOCP	= pkDataInit->hIOCP;

	return	1;
}