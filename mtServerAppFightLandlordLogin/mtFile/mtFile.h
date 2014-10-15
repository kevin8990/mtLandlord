#pragma once
#include "mtPlatform.h"
#include "mtThreadWork.h"

class   mtFileMgr;

class	mtFile 
{
public:
	enum	EFileType
	{
		E_FILE_BEG,

		E_FILE_ACCOUNT	= E_FILE_BEG,
		E_FILE_ID,
		E_FILE_QQ,
		E_FILE_PHONE,

		E_FILE_END
	};

	enum	EFileRunType
	{
		E_FILE_RUN_BEG	= 0,

		E_FILE_RUN_WRITE_FILE_CREATE,
		E_FILE_RUN_WRITE_FILE,
		E_FILE_RUN_READ_FILE,

//		E_FILE_RUN_READ,

		E_FILE_RUN_END
	};

	struct	DataInit
	{
		long				lStructBytes;
		char*				pcFolder;
		HANDLE				hIOCP;
	};

public:
	virtual	int runRead( mtThreadWork::DataUser* pkDataUser );
	virtual int runWrite( mtThreadWork::DataUser* pkDataUser );

	virtual int runWriteFileCreate( mtThreadWork::DataUser* pkDataUser );
	virtual int runWriteFile( mtThreadWork::DataUser* pkDataUser );
	virtual int	runReadFile( mtThreadWork::DataUser* pkDataUser );

	virtual int	completeWriteCreate(mtThreadWork::DataUser* pkDataUser);
public:
	mtFile();
	~mtFile();

	virtual	int				init(DataInit* pkDataInit);

	virtual	char*			getFileName(char* pcAccount)	= 0;
	virtual unsigned long	getFileOffset(char* pcAccount)	= 0;
	virtual unsigned long 	getCreateFileBytes()			= 0;
	virtual unsigned long	getDataFileBytes()				= 0;
	virtual unsigned long	getFolderBytes()				= 0;

public:
	/// 文件部分
	static int		FileCreate(HANDLE* phFile, char* pcFile, unsigned long ulOffsetBytes = 0);
	static int		FileOpen(HANDLE* phFile, char* pcFile);
	static int		FileRead(HANDLE hFile, unsigned long ulBytes, void* pData, OVERLAPPED* pkOverlapped);
	static int		FileWrite(HANDLE hFile, unsigned long ulBytes, void* pData, OVERLAPPED* pkOverlapped);
	static int		FileClose(HANDLE* phFile);
	static int		FileLock(HANDLE hFile, unsigned long ulBytes, OVERLAPPED* pkOverlapped, unsigned long ulFlags = 0);
	
	/// 目录部分
	static int		FolderCreate( char* pcFile, int iOffsetBytes = 0 );

private:
	static	char			ms_pcCreateFileData[11<<20];
	static	HANDLE			ms_hIOCP;
	static	volatile long	ms_lFileHandleTotal;	
};


//创建DATA目录
inline int mtFile::FolderCreate( char* pcFile, int iOffsetBytes )
{
	char*	pcFolder		= pcFile + iOffsetBytes;

	while (*pcFolder)
	{
		if ('\\' == *pcFolder)
		{
			*pcFolder		= 0;
			BOOL	bRet	= CreateDirectory(pcFile, NULL);
			*pcFolder	= '\\';
			if (FALSE == bRet && ERROR_ALREADY_EXISTS != GetLastError())
			{
				return	0;
			}
		}
		pcFolder++;
	}

	return	1;
}

inline int mtFile::FileCreate( HANDLE* phFile, char* pcFile, unsigned long ulOffsetBytes )
{
	if (0 == FolderCreate(pcFile, ulOffsetBytes))
	{
		return	0;
	}

	*phFile	= CreateFile(pcFile, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_NEW, 
		FILE_FLAG_OVERLAPPED, NULL);

	if (INVALID_HANDLE_VALUE == *phFile)
	{
		return	0;
	}

	if (NULL == CreateIoCompletionPort(*phFile, ms_hIOCP, 0, 0))
	{
		FileClose(phFile);
		return	0;
	}

#ifdef	_DEBUG
	InterlockedIncrement(&ms_lFileHandleTotal);
#endif	/// _DEBUG
	return	1;
}

inline int mtFile::FileOpen( HANDLE* phFile, char* pcFile )
{
	*phFile	= CreateFile(pcFile, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 
		FILE_FLAG_OVERLAPPED, NULL);

	if (INVALID_HANDLE_VALUE == *phFile)
	{
		return	0;
	}
	
	if (NULL == CreateIoCompletionPort(*phFile, ms_hIOCP, 0, 0))
	{
		FileClose(phFile);
		return	0;
	}

#ifdef	_DEBUG
	InterlockedIncrement(&ms_lFileHandleTotal);
#endif	/// _DEBUG

	return	1;
}

inline int mtFile::FileRead( HANDLE hFile, unsigned long ulBytes, void* pData, OVERLAPPED* pkOverlapped)
{
	DWORD		ulBytesRead	= 0;
	BOOL bRet	= ReadFile( hFile, pData, ulBytes, &ulBytesRead, pkOverlapped);

	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		return	0;
	}

	return	1;
}

inline int mtFile::FileWrite( HANDLE hFile, unsigned long ulBytes, void* pData, OVERLAPPED* pkOverlapped )
{
	DWORD			ulBytesWrite	= 0;
	BOOL bRet	= WriteFile( hFile, pData, ulBytes, &ulBytesWrite, pkOverlapped);

	int		iRet	= GetLastError();
	if (FALSE == bRet && ERROR_IO_PENDING != GetLastError())
	{
		return	0;
	}

	return	1;
}

inline int mtFile::FileClose( HANDLE* phFile )
{
	if (INVALID_HANDLE_VALUE != *phFile)
	{
#ifdef	_DEBUG
		InterlockedDecrement(&ms_lFileHandleTotal);
#endif	/// _DEBUG

		CloseHandle(*phFile);
	}
	*phFile	= INVALID_HANDLE_VALUE;

	return	1;
}


inline int mtFile::FileLock(HANDLE hFile, unsigned long ulBytes, OVERLAPPED* pkOverlapped, unsigned long ulFlags)
{
	BOOL	bLockFileEx				= LockFileEx(hFile, ulFlags, 0, 
		ulBytes, 0, pkOverlapped);
	if (FALSE == bLockFileEx && ERROR_IO_PENDING != GetLastError())
	{
		return	0;
	}

	return	1;
}