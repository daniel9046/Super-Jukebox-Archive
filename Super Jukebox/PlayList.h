// PlayList.h: interface for the CPlayList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYLIST_H__F5D85A42_84B9_11D4_8925_009027C572AB__INCLUDED_)
#define AFX_PLAYLIST_H__F5D85A42_84B9_11D4_8925_009027C572AB__INCLUDED_

#include "List.h"
#include "Skin.h"
#include "LabelEdit.h"	// Added by ClassView
#include "AddFilesDialog.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPlayList : public CList  
{
public:
	void IsAutoAdvance(BOOL bAutoAdvance);
	BOOL IsAutoAdvance();
	LRESULT OnColumnClick(LPNMLISTVIEW lpNMListView);
	BOOL LocateFile(LPCTSTR lpszFile, LPTSTR lpszFoundFile);
	BOOL AddFileToAdd(LPCTSTR lpszFileName);
	void ClearFilesToAdd();
	BOOL AddFiles(BOOL bEnumFileList,BOOL bEnumOnlyCurrentDirectory);
	BOOL Exists();
	void EditItem(INT nIndex,INT nSubIndex);
	INT FilterItems(TCHAR szFields[NUMBER_OF_COLUMNS][100],BOOL bMatchWholeWord,BOOL bMatchCase,BOOL bLogicalRelation);
	virtual BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	BOOL EditListEntry(INT nIndex,LPCTSTR lpszField,LPCTSTR lpszNewValue);
	void IsDetectInactivity(BOOL bDetectInactivity);
	void IsRandomize(BOOL bRandomize);
	void IsAutoRewind(BOOL bAutoRewind);
	void SetIndSongFadeLength(UINT uSongFadeLength);
	void SetIndSongLength(UINT uSongLength);
	void SetUniSongFadeLength(UINT uSongFadeLength);
	void SetUniSongLength(UINT uSongLength);
	BOOL IsDetectInactivity();
	BOOL IsRandomize();
	BOOL IsAutoRewind();
	UINT GetIndSongFadeLength();
	UINT GetIndSongLength();
	UINT GetUniSongFadeLength();
	UINT GetUniSongLength();
	void SetPlayingMode(UINT uMode);
	UINT GetPlayingMode();
	BOOL IsSaved();
	BOOL Reset();
	BOOL Save(LPCTSTR lpszFileName);
	void Clear();
	BOOL RemoveEntry(INT nIndex);
	BOOL InsertEntry(CListEntry &ListEntry,INT nIndex=-1);
	BOOL ProcessEntry(INT nIndex);
	void GetName(LPTSTR lpszFileName,INT nTextMax);
	UINT GetSettingValue(LPCTSTR lpszSetting);
	void TrimString(LPTSTR lpszString);
	BOOL Load(LPCTSTR lpszFileName);
	CPlayList();
	virtual ~CPlayList();
	CSkin m_Skin;

protected:
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual LRESULT OnTimer(HWND hwnd, UINT id);
	virtual void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);

private:
	CAddFilesDialog m_AddFilesDlg;
	CListEntry* FindSpecificEntry(LPCTSTR lpszPath);
	CListEntry* GetNextEntryToAdd();
	vector<CListEntry>m_EnumeratedEntriesToAdd;
	vector<CListEntry>m_EntriesToAdd;
	BOOL EnumFilesToAdd(INT nLevel,CAddFilesDialog *pAddDlg,BOOL bEnumFileList);
	BOOL m_bEnumFileList,m_bEnumOnlyCurrentDirectory;
	static void __cdecl AddFilesThread(void *lpParameter);
	BOOL m_bExists;
	BOOL DoContextMenu(INT nXPos, INT nYPos);
	INT m_nSortColumn;
	BOOL m_bMovingLabelEdit;
	INT m_iItem,m_iSubItem;
	CLabelEdit m_LabelEdit;
	INT m_iOldItem,m_iOldSubItem;
	BOOL m_bSaved;
	TCHAR m_szFileName[MAX_PATH];
	UINT m_uMode;
	UINT m_uIndSongLength,m_uUniSongLength;
	UINT m_uIndSongFadeLength,m_uUniSongFadeLength;
	BOOL m_bAutoRewind,m_bRandomize,m_bDetectInactivity,m_bAutoAdvance;
};

#endif // !defined(AFX_PLAYLIST_H__F5D85A42_84B9_11D4_8925_009027C572AB__INCLUDED_)
