// Extractor.h: interface for the CExtractor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTRACTOR_H__A31E1AA0_908B_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_EXTRACTOR_H__A31E1AA0_908B_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef HANDLE (__stdcall *pfnACEOpenArchive)(struct ACEOpenArchiveData *ArchiveData);
typedef int (__stdcall *pfnACECloseArchive)(HANDLE hArcData);
typedef int (__stdcall *pfnACEReadHeader)(HANDLE hArcData,struct ACEHeaderData *HeaderData);
typedef int (__stdcall *pfnACEProcessFile)(HANDLE hArcData,int Operation,char *DestPath);
typedef int (__stdcall *pfnACESetPassword)(HANDLE hArcData,char *Password);

class CExtractor  
{
public:
	BOOL SetCurrentDirectory(LPCTSTR lpPath);
	BOOL Reset();
	BOOL Close();
	BOOL ExtractSpecificFile(LPCTSTR lpszSourceFileName, LPCTSTR lpszOutputFileName);
	BOOL ExtractCurrentFile(LPCTSTR lpszOutputFileName);
	BOOL SkipCurrentFile();
	BOOL GetCurrentFile(BOOL bGlobal,LPWIN32_FIND_DATA lpFindFileData);
	BOOL Open(LPCTSTR lpszFileName,BOOL bListOnly);
	CExtractor();
	virtual ~CExtractor();

private:
	BOOL GetCurrentArchiveEntry(LPWIN32_FIND_DATA lpfd);
	BOOL EnumArchiveDirectories();
	BOOL LoadUnaceDll();
	static pfnACEOpenArchive m_ACEOpenArchive;
	static pfnACECloseArchive m_ACECloseArchive;
	static pfnACEReadHeader m_ACEReadHeader;
	static pfnACEProcessFile m_ACEProcessFile;
	static pfnACESetPassword m_ACESetPassword;
	static HINSTANCE m_hInstUnaceDll;
	BOOL m_bListOnly;
	TCHAR m_szCurrentDirectory[MAX_PATH];
	TCHAR m_szArchiveFileName[MAX_PATH];
	HANDLE m_hArcData;
	unzFile m_unzFile;
	BOOL m_bUnzStatus,m_bOpen;
	UINT m_uArchiveType,m_uArchiveEntriesIndex;
	vector<WIN32_FIND_DATA>m_ArchiveDirectories;
};

#endif // !defined(AFX_EXTRACTOR_H__A31E1AA0_908B_11D4_B2FA_00D0B7AE5907__INCLUDED_)
