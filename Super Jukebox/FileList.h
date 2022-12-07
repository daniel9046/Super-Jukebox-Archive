// FileList.h: interface for the CFileList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELIST_H__726DDCA1_8411_11D4_8925_009027C5DD32__INCLUDED_)
#define AFX_FILELIST_H__726DDCA1_8411_11D4_8925_009027C5DD32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"

class CFileList : public CList  
{
public:
	LONGLONG GetDriveSize(LPCTSTR lpszRoot);
	INT GetFileImage(LPCTSTR lpszPath,BOOL bOpened);
	void EnumOnly(BOOL bFlag);
	void GetSubSource(LPTSTR lpszSubSource,DWORD nBufferLength);
	INT GetNextPathPart(LPCTSTR lpPath,INT nStartPos,LPTSTR lpPart);
	BOOL IsPathInArchive(LPCTSTR lpPath,LPTSTR lpArchive,LPTSTR lpSubPath);
	void GetSource(LPTSTR lpszSource,DWORD nBufferLength);
	BOOL SetSource(LPCTSTR lpszSource,BOOL bUpdateLocationBar=TRUE);
	DWORD SHGetFileInfo(LPCTSTR pszPath,DWORD dwFileAttributes,SHFILEINFO FAR *psfi,UINT cbFileInfo,UINT uFlags);
	BOOL FindClose(HANDLE hFindFile);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	HANDLE FindFirstFile(LPWIN32_FIND_DATA lpFindFileData);
	LPCTSTR GetArchiveFileName();
	BOOL IsArchived();
	BOOL ProcessEntry(INT iIndex);
	BOOL LoadFileList();
	BOOL LoadDirectoryList();
	BOOL LoadDriveList();
	CFileList();
	virtual ~CFileList();

private:
	BOOL m_bEnumOnly;
	BOOL DoContextMenu(INT nXPos,INT nYPos);
	TCHAR m_szSubSource[MAX_PATH];
	TCHAR m_szSource[MAX_PATH];
	TCHAR m_szArchiveFileName[MAX_PATH];
	BOOL m_bArchived;

protected:
	virtual void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
};

#endif // !defined(AFX_FILELIST_H__726DDCA1_8411_11D4_8925_009027C5DD32__INCLUDED_)
