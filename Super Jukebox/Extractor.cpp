// Extractor.cpp: implementation of the CExtractor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Extractor.h"
#include "Main.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HINSTANCE CExtractor::m_hInstUnaceDll=NULL;
pfnACEOpenArchive CExtractor::m_ACEOpenArchive;
pfnACECloseArchive CExtractor::m_ACECloseArchive;
pfnACEReadHeader CExtractor::m_ACEReadHeader;
pfnACEProcessFile CExtractor::m_ACEProcessFile;
pfnACESetPassword CExtractor::m_ACESetPassword;

CExtractor::CExtractor()
{
	if(!LoadUnaceDll())
	{
		MessageBox(NULL,"A required .DLL file, UNACE.DLL, was not found.","Error Starting Program",MB_ICONEXCLAMATION);
		ExitProcess(0);
	}
}

CExtractor::~CExtractor()
{
}

BOOL CExtractor::LoadUnaceDll()
{
	if(!m_hInstUnaceDll)
	{
		m_hInstUnaceDll=LoadLibrary("UNACE.DLL");
		if(!m_hInstUnaceDll)
			return FALSE;
		m_ACEOpenArchive=(pfnACEOpenArchive)GetProcAddress(m_hInstUnaceDll,"ACEOpenArchive");
		m_ACECloseArchive=(pfnACECloseArchive)GetProcAddress(m_hInstUnaceDll,"ACECloseArchive");
		m_ACEReadHeader=(pfnACEReadHeader)GetProcAddress(m_hInstUnaceDll,"ACEReadHeader");
		m_ACEProcessFile=(pfnACEProcessFile)GetProcAddress(m_hInstUnaceDll,"ACEProcessFile");
		m_ACESetPassword=(pfnACESetPassword)GetProcAddress(m_hInstUnaceDll,"ACESetPassword");

		if(!m_ACEOpenArchive||
		   !m_ACECloseArchive||
		   !m_ACEReadHeader||
		   !m_ACEProcessFile||
		   !m_ACESetPassword)
		{
			FreeLibrary(m_hInstUnaceDll);
			return FALSE;
		}		
	}
	return TRUE;
}

BOOL CExtractor::Open(LPCTSTR lpszFileName,BOOL bListOnly)
{
	m_bOpen=FALSE;
	strncpy(m_szArchiveFileName,lpszFileName,sizeof(m_szArchiveFileName));
	m_szArchiveFileName[sizeof(m_szArchiveFileName)-1]='\0';
	strcpy(m_szCurrentDirectory,"\\");

	TCHAR szExt[_MAX_EXT];
	_splitpath(m_szArchiveFileName,NULL,NULL,NULL,szExt);
	if(!stricmp(szExt,".ace"))
	{
		m_uArchiveType=LIST_ID_ACE;
		ACEOpenArchiveData oad;
		memset(&oad,0,sizeof(oad));
		oad.ArcName=(char*)m_szArchiveFileName;
		oad.CmtBuf=NULL;
		oad.OpenMode=(m_bListOnly=bListOnly)?ACEOPEN_LIST:ACEOPEN_EXTRACT;
		m_hArcData=m_ACEOpenArchive(&oad);
		if(oad.OpenResult)
		{
			if(m_hArcData)
				m_ACECloseArchive(m_hArcData);
			m_bOpen=FALSE;
		}
		else
		{
			m_bOpen=TRUE;
			if(m_bListOnly)
				m_bOpen=EnumArchiveDirectories();
		}
	}
	else if(!stricmp(szExt,".rar")||!stricmp(szExt,".rsn"))
	{
		m_uArchiveType=LIST_ID_RAR;
		RAROpenArchiveData oad;
		memset(&oad,0,sizeof(oad));
		oad.ArcName=(char*)m_szArchiveFileName;
		oad.CmtBuf=NULL;
		oad.OpenMode=(m_bListOnly=bListOnly)?RAR_OM_LIST:RAR_OM_EXTRACT;
		if(m_hArcData=RAROpenArchive(&oad))
		{
			m_bOpen=TRUE;
			if(m_bListOnly)
				m_bOpen=EnumArchiveDirectories();
		}
	}
	else if(!stricmp(szExt,".zip"))
	{
		m_uArchiveType=LIST_ID_ZIP;
		if(m_unzFile=unzOpen(m_szArchiveFileName))
		{
			m_bOpen=m_bUnzStatus=TRUE;
			if(m_bListOnly)
				m_bOpen=EnumArchiveDirectories();
		}
	}

	return m_bOpen;
}

BOOL CExtractor::GetCurrentFile(BOOL bGlobal,LPWIN32_FIND_DATA lpFindFileData)
{
	if(!m_bOpen)
		return FALSE;

	memset(lpFindFileData,0,sizeof(WIN32_FIND_DATA));
	WIN32_FIND_DATA fd;
	while(GetCurrentArchiveEntry(&fd))
	{
		if(bGlobal)
		{
			sprintf(lpFindFileData->cFileName,"%s\\%s",m_szArchiveFileName,fd.cFileName);
			lpFindFileData->dwFileAttributes=fd.dwFileAttributes;
			lpFindFileData->ftLastWriteTime=fd.ftLastWriteTime;
			lpFindFileData->nFileSizeLow=fd.nFileSizeLow;
			lpFindFileData->nFileSizeHigh=fd.nFileSizeHigh;
			return TRUE;
		}
		else
		{
			TCHAR szBuf[MAX_PATH],szDir[_MAX_DIR],szName[_MAX_FNAME],szExt[_MAX_EXT];
			sprintf(szBuf,"\\%s",fd.cFileName);
			_splitpath(szBuf,NULL,szDir,szName,szExt);
			strcpy(szBuf,m_szCurrentDirectory);
			if(szBuf[strlen(szBuf)-1]!='\\')strcat(szBuf,"\\");
			if(!stricmp(szDir,szBuf))
			{
				sprintf(szBuf,"%s%s",szName,szExt);
				strcpy(lpFindFileData->cFileName,szBuf);
				lpFindFileData->dwFileAttributes=fd.dwFileAttributes;
				lpFindFileData->ftLastWriteTime=fd.ftLastWriteTime;
				lpFindFileData->nFileSizeLow=fd.nFileSizeLow;
				lpFindFileData->nFileSizeHigh=fd.nFileSizeHigh;
				return TRUE;
			}
		}
		switch(m_uArchiveType)
		{
		case LIST_ID_ACE:
			m_ACEProcessFile(m_hArcData,ACECMD_SKIP,NULL);
			break;
		case LIST_ID_RAR:
			RARProcessFile(m_hArcData,RAR_SKIP,NULL,NULL);
			break;
		case LIST_ID_ZIP:
			m_bUnzStatus=(unzGoToNextFile(m_unzFile)==UNZ_OK);
			break;
		}
	}
	return FALSE;
}

BOOL CExtractor::SkipCurrentFile()
{
	if(!m_bOpen)
		return FALSE;

	switch(m_uArchiveType)
	{
	case LIST_ID_ACE:
		return m_ACEProcessFile(m_hArcData,ACECMD_SKIP,NULL)?FALSE:TRUE;
		break;
	case LIST_ID_RAR:
		return RARProcessFile(m_hArcData,RAR_SKIP,NULL,NULL)?FALSE:TRUE;
		break;
	case LIST_ID_ZIP:
		return m_bUnzStatus=(unzGoToNextFile(m_unzFile)==UNZ_OK?TRUE:FALSE);
		break;
	}
	return FALSE;
}

BOOL CExtractor::ExtractCurrentFile(LPCTSTR lpszOutputFileName)
{
	if(!m_bOpen)
		return FALSE;

	BOOL bRes=FALSE;
	switch(m_uArchiveType)
	{
	case LIST_ID_ACE:
		bRes=m_ACEProcessFile(m_hArcData,ACECMD_EXTRACT,(char*)lpszOutputFileName)?FALSE:TRUE;
		SetFileAttributes(lpszOutputFileName,FILE_ATTRIBUTE_NORMAL);
		return bRes;
		break;
	case LIST_ID_RAR:
		bRes=RARProcessFile(m_hArcData,RAR_EXTRACT,NULL,(char*)lpszOutputFileName)?FALSE:TRUE;
		SetFileAttributes(lpszOutputFileName,FILE_ATTRIBUTE_NORMAL);
		return bRes;
		break;
	case LIST_ID_ZIP:
		unz_file_info ufiInfo;
		if(unzGetCurrentFileInfo(m_unzFile,&ufiInfo,NULL,0,NULL,0,NULL,0)!=UNZ_OK)
			return FALSE;
		void *buf=malloc(ufiInfo.uncompressed_size);
		if(!buf)
			return FALSE;
		if(unzOpenCurrentFile(m_unzFile)!=UNZ_OK)
		{
			free(buf);
			return FALSE;
		}
		if(unzReadCurrentFile(m_unzFile,buf,ufiInfo.uncompressed_size)!=ufiInfo.uncompressed_size)
		{
			free(buf);
			unzCloseCurrentFile(m_unzFile);
			return FALSE;
		}
		unzCloseCurrentFile(m_unzFile);
		FILE *file;
		if(!(file=fopen(lpszOutputFileName,"wb")))
		{
			free(buf);
			return FALSE;
		}
		if(fwrite(buf,ufiInfo.uncompressed_size,1,file)!=1)
		{
			free(buf);
			return FALSE;
		}
		free(buf);
		fclose(file);
		return TRUE;
		break;
	}
	return FALSE;
}

BOOL CExtractor::Close()
{
	if(!m_bOpen)
		return FALSE;

	m_bOpen=FALSE;
	switch(m_uArchiveType)
	{
	case LIST_ID_ACE:
		return m_ACECloseArchive(m_hArcData)?FALSE:TRUE;
		break;
	case LIST_ID_RAR:
		return RARCloseArchive(m_hArcData)?FALSE:TRUE;
		break;
	case LIST_ID_ZIP:
		return unzClose(m_unzFile)==UNZ_OK?TRUE:FALSE;
		break;
	}
	return FALSE;
}

BOOL CExtractor::ExtractSpecificFile(LPCTSTR lpszSourceFileName, LPCTSTR lpszOutputFileName)
{
	if(!m_bOpen)
		return FALSE;

	if(!Reset())
		return FALSE;

	WIN32_FIND_DATA fd;
	while(GetCurrentFile(FALSE,&fd))
	{
		if(!stricmp(fd.cFileName,lpszSourceFileName))
		{
			if(ExtractCurrentFile(lpszOutputFileName))
				return TRUE;
		}
		SkipCurrentFile();
	}

	return FALSE;
}

BOOL CExtractor::Reset()
{
	if(!m_bOpen)
		return FALSE;

	if(!Close())
		return FALSE;
	TCHAR szBuf[MAX_PATH];
	strcpy(szBuf,m_szCurrentDirectory);
	if(!Open(m_szArchiveFileName,m_bListOnly))
		return FALSE;
	if(!SetCurrentDirectory(szBuf))
		return FALSE;
	return TRUE;
}

BOOL CExtractor::SetCurrentDirectory(LPCTSTR lpPath)
{
	strcpy(m_szCurrentDirectory,lpPath);
	return TRUE;
}

BOOL CExtractor::EnumArchiveDirectories()
{
	if(!m_bOpen)
		return FALSE;

	m_ArchiveDirectories.clear();
	switch(m_uArchiveType)
	{
	case LIST_ID_ACE:
		ACEHeaderData acehd;
		acehd.CmtBuf=NULL;
		while(!m_ACEReadHeader(m_hArcData,&acehd))
		{
			for(INT i=0;i<strlen(acehd.FileName);i++)
				if(acehd.FileName[i]=='/')
					acehd.FileName[i]='\\';
			TCHAR szDir[_MAX_DIR];
			_splitpath(acehd.FileName,NULL,szDir,NULL,NULL);
			if(strlen(szDir))
			{
				if(szDir[strlen(szDir)-1]=='\\')
					szDir[strlen(szDir)-1]='\0';
				BOOL bIn=FALSE;
				for(i=0;i<m_ArchiveDirectories.size();i++)
				{
					if(!stricmp(m_ArchiveDirectories[i].cFileName,szDir))
					{
						bIn=TRUE;
						break;
					}
				}
				if(!bIn)
				{
					WIN32_FIND_DATA fd;
					memset(&fd,0,sizeof(fd));
					strcpy(fd.cFileName,szDir);
					fd.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
					DosDateTimeToFileTime(HIWORD(acehd.FileTime),LOWORD(acehd.FileTime),&fd.ftLastWriteTime);
					fd.nFileSizeLow=acehd.UnpSize;
					fd.nFileSizeHigh=0;
					m_ArchiveDirectories.push_back(fd);
				}
			}
			m_ACEProcessFile(m_hArcData,ACECMD_SKIP,NULL);
		}
		m_ACECloseArchive(m_hArcData);
		ACEOpenArchiveData ACEoad;
		memset(&ACEoad,0,sizeof(ACEoad));
		ACEoad.ArcName=(char*)m_szArchiveFileName;
		ACEoad.CmtBuf=NULL;
		ACEoad.OpenMode=m_bListOnly?ACEOPEN_LIST:ACEOPEN_EXTRACT;
		m_hArcData=m_ACEOpenArchive(&ACEoad);
		m_bOpen=!ACEoad.OpenResult;
		m_uArchiveEntriesIndex=0;
		break;
	case LIST_ID_RAR:
		RARHeaderData rarhd;
		rarhd.CmtBuf=NULL;
		while(!RARReadHeader(m_hArcData,&rarhd))
		{
			for(INT i=0;i<strlen(rarhd.FileName);i++)
				if(rarhd.FileName[i]=='/')
					rarhd.FileName[i]='\\';
			TCHAR szDir[_MAX_DIR];
			_splitpath(rarhd.FileName,NULL,szDir,NULL,NULL);
			if(strlen(szDir))
			{
				if(szDir[strlen(szDir)-1]=='\\')
					szDir[strlen(szDir)-1]='\0';
				BOOL bIn=FALSE;
				for(i=0;i<m_ArchiveDirectories.size();i++)
				{
					if(!stricmp(m_ArchiveDirectories[i].cFileName,szDir))
					{
						bIn=TRUE;
						break;
					}
				}
				if(!bIn)
				{
					WIN32_FIND_DATA fd;
					memset(&fd,0,sizeof(fd));
					strcpy(fd.cFileName,szDir);
					fd.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
					DosDateTimeToFileTime(HIWORD(rarhd.FileTime),LOWORD(rarhd.FileTime),&fd.ftLastWriteTime);
					fd.nFileSizeLow=rarhd.UnpSize;
					fd.nFileSizeHigh=0;
					m_ArchiveDirectories.push_back(fd);
				}
			}
			RARProcessFile(m_hArcData,RAR_SKIP,NULL,NULL);
		}
		RARCloseArchive(m_hArcData);
		RAROpenArchiveData RARoad;
		memset(&RARoad,0,sizeof(RARoad));
		RARoad.ArcName=(char*)m_szArchiveFileName;
		RARoad.CmtBuf=NULL;
		RARoad.OpenMode=m_bListOnly?RAR_OM_LIST:RAR_OM_EXTRACT;
		m_bOpen=(m_hArcData=RAROpenArchive(&RARoad))?TRUE:FALSE;
		m_uArchiveEntriesIndex=0;
		break;
	case LIST_ID_ZIP:		
		do
		{
			unz_file_info ufiInfo;
			TCHAR szFileName[MAX_PATH];
			if(unzGetCurrentFileInfo(m_unzFile,&ufiInfo,szFileName,sizeof(szFileName),NULL,0,NULL,0)==UNZ_OK)
			{
				for(INT i=0;i<strlen(szFileName);i++)
					if(szFileName[i]=='/')
						szFileName[i]='\\';
				TCHAR szDir[_MAX_DIR];
				_splitpath(szFileName,NULL,szDir,NULL,NULL);
				if(strlen(szDir))
				{
					if(szDir[strlen(szDir)-1]=='\\')
						szDir[strlen(szDir)-1]='\0';
					BOOL bIn=FALSE;
					for(i=0;i<m_ArchiveDirectories.size();i++)
					{
						if(!stricmp(m_ArchiveDirectories[i].cFileName,szDir))
						{
							bIn=TRUE;
							break;
						}
					}
					if(!bIn)
					{
						WIN32_FIND_DATA fd;
						memset(&fd,0,sizeof(fd));
						strcpy(fd.cFileName,szDir);
						fd.dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
						DosDateTimeToFileTime(HIWORD(ufiInfo.dosDate),LOWORD(ufiInfo.dosDate),&fd.ftLastWriteTime);
						fd.nFileSizeLow=ufiInfo.uncompressed_size;
						fd.nFileSizeHigh=0;
						m_ArchiveDirectories.push_back(fd);
					}
				}
			}
		}while(unzGoToNextFile(m_unzFile)==UNZ_OK);
		unzGoToFirstFile(m_unzFile);
		m_uArchiveEntriesIndex=0;
		m_bOpen=TRUE;
		break;
	}
	return m_bOpen;
}

BOOL CExtractor::GetCurrentArchiveEntry(LPWIN32_FIND_DATA lpfd)
{
	if(!m_bOpen)
		return FALSE;

	switch(m_uArchiveType)
	{
	case LIST_ID_ACE:
		if(m_uArchiveEntriesIndex<m_ArchiveDirectories.size())
		{
			memcpy(lpfd,&m_ArchiveDirectories[m_uArchiveEntriesIndex++],sizeof(WIN32_FIND_DATA));
			return TRUE;
		}
		else
		{
			ACEHeaderData acehd;
			acehd.CmtBuf=NULL;
			INT iRes;
			do
			{
				iRes=m_ACEReadHeader(m_hArcData,&acehd);
			}while(!iRes&&acehd.FileAttr&FILE_ATTRIBUTE_DIRECTORY&&!m_ACEProcessFile(m_hArcData,ACECMD_SKIP,NULL));
			if(!iRes)
			{
				for(INT i=0;i<strlen(acehd.FileName);i++)
					if(acehd.FileName[i]=='/')
						acehd.FileName[i]='\\';
				strncpy(lpfd->cFileName,acehd.FileName,sizeof(lpfd->cFileName));
				lpfd->dwFileAttributes=acehd.FileAttr;
				DosDateTimeToFileTime(HIWORD(acehd.FileTime),LOWORD(acehd.FileTime),&lpfd->ftLastWriteTime);
				lpfd->nFileSizeLow=acehd.UnpSize;
				lpfd->nFileSizeHigh=0;
			}
			return !iRes;
		}
		break;
	case LIST_ID_RAR:
		if(m_uArchiveEntriesIndex<m_ArchiveDirectories.size())
		{
			memcpy(lpfd,&m_ArchiveDirectories[m_uArchiveEntriesIndex++],sizeof(WIN32_FIND_DATA));
			return TRUE;
		}
		else
		{
			RARHeaderData rarhd;
			rarhd.CmtBuf=NULL;
			INT iRes;
			do
			{
				iRes=RARReadHeader(m_hArcData,&rarhd);
			}while(!iRes&&rarhd.FileAttr&FILE_ATTRIBUTE_DIRECTORY&&!RARProcessFile(m_hArcData,RAR_SKIP,NULL,NULL));
			if(!iRes)
			{
				for(INT i=0;i<strlen(rarhd.FileName);i++)
					if(rarhd.FileName[i]=='/')
						rarhd.FileName[i]='\\';
				strncpy(lpfd->cFileName,rarhd.FileName,sizeof(lpfd->cFileName));
				lpfd->dwFileAttributes=rarhd.FileAttr;
				DosDateTimeToFileTime(HIWORD(rarhd.FileTime),LOWORD(rarhd.FileTime),&lpfd->ftLastWriteTime);
				lpfd->nFileSizeLow=rarhd.UnpSize;
				lpfd->nFileSizeHigh=0;
			}
			return !iRes;
		}
		break;
	case LIST_ID_ZIP:
		if(m_bUnzStatus)
		{
			if(m_uArchiveEntriesIndex<m_ArchiveDirectories.size())
			{
				memcpy(lpfd,&m_ArchiveDirectories[m_uArchiveEntriesIndex++],sizeof(WIN32_FIND_DATA));
				return TRUE;
			}
			else
			{
				unz_file_info ufiInfo;
				INT iRes;
				do
				{
					iRes=unzGetCurrentFileInfo(m_unzFile,&ufiInfo,lpfd->cFileName,MAX_PATH,NULL,0,NULL,0);
				}while(ufiInfo.external_fa&FILE_ATTRIBUTE_DIRECTORY&&(m_bUnzStatus=(unzGoToNextFile(m_unzFile)==UNZ_OK)));
				if(iRes==UNZ_OK)
				{
					for(INT i=0;i<strlen(lpfd->cFileName);i++)
						if(lpfd->cFileName[i]=='/')
							lpfd->cFileName[i]='\\';
					lpfd->dwFileAttributes=ufiInfo.external_fa;
					DosDateTimeToFileTime(HIWORD(ufiInfo.dosDate),LOWORD(ufiInfo.dosDate),&lpfd->ftLastWriteTime);
					lpfd->nFileSizeLow=ufiInfo.uncompressed_size;
					lpfd->nFileSizeHigh=0;
				}
				return (iRes==UNZ_OK);
			}
		}
	}
	return FALSE;
}
