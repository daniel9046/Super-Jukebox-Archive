// List.h: interface for the CList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIST_H__726DDCA0_8411_11D4_8925_009027C5DD32__INCLUDED_)
#define AFX_LIST_H__726DDCA0_8411_11D4_8925_009027C5DD32__INCLUDED_

#include "ListEntry.h"
#include "Wnd.h"
#include "Main.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CList : public CWnd
{
public:
	virtual void SaveHeaderInfo();
	virtual BOOL RefreshHeader();
	virtual BOOL SaveRegistrySettings();
	virtual BOOL LoadRegistrySettings();
	virtual void SetSortingColumn(LPCTSTR lpszColumn, BOOL bOrder);
	virtual void SortItems(LPCTSTR lpszColumn,BOOL bOrder);
	virtual LRESULT OnColumnClick(LPNMLISTVIEW lpNMListView);
	BOOL RefreshListView();
	BOOL SortEntries(INT nFirst,INT nLast);
	virtual BOOL IsVisible();
	UINT GetDisplayedCount();
	BOOL IsMapped();
	void IsMapped(BOOL bMapped);
	BOOL SearchEntry(INT nEntry, LPCTSTR lpszColumn, LPCTSTR lpszSearchString, BOOL bMatchWholeWord, BOOL bMatchCase);
	INT MapIndexToInternal(INT nListViewItem);
	INT MapIndexToExternal(INT nListViewItem);
	virtual void IsVisible(BOOL bVisible);
	INT FindEntry(INT nStartEntry,LPCTSTR lpszColumn,LPCTSTR lpszSearchString,BOOL bMatchWholeWord,BOOL bMatchCase,BOOL bDirection);
	virtual BOOL SetSystemImageList();
	virtual void UpdateListView(BOOL bUpdate);
	BOOL GetColumnNameFromIndex(INT nIndex,LPTSTR lpszName,INT nTextMax);
	INT GetColumnIndexFromName(LPCTSTR lpszName);
	BOOL EnsureVisible(INT iItem,INT iSubItem);
	UINT GetColumnCount();
	UINT GetTypeCount(INT nType,BOOL bOnlySelected);
	UINT GetSelectedCount();
	virtual BOOL ProcessEntry(INT nIndex);
	virtual LRESULT OnReturn(LPNMHDR lpNMHdr);
	virtual LRESULT OnDblClk(LPNMLISTVIEW lpNMListView);
	virtual void SetEntryState(INT nIndex,UINT uState);
	virtual UINT GetEntryState(INT nIndex);
	virtual BOOL ResizeColumns(INT nColumn=-1);
	virtual BOOL RegisterClass(HINSTANCE hInstance);
	virtual BOOL SelectRelativeEntry(INT nOffset);
	virtual void Clear();
	virtual BOOL RemoveEntry(INT nIndex);
	virtual BOOL InsertEntry(CListEntry &ListEntry,INT nIndex=-1);
	virtual BOOL SelectEntry(INT iIndex);
	virtual INT GetFocusedEntry();
	virtual INT GetSelectedEntry();
	virtual CListEntry* GetListEntry(INT nIndex);
	virtual CListEntry* GetListEntryUnmapped(INT nIndex);
	virtual UINT ConvertTimeUp(LPCTSTR lpszBuffer);
	virtual void ConvertTimeDown(INT nSeconds, LPTSTR lpszBuffer);
	virtual BOOL InsertColumn(LPCTSTR lpszLabel,INT nWidth);
	virtual UINT GetEntryCount();
	virtual BOOL DisplayEntries(INT nFirst=-1,INT nLast=-1);
	virtual BOOL Create(HINSTANCE hInstance,CWnd *pParentWnd,UINT uID);
	CList();
	virtual ~CList();
	HWND m_hWndHeader;
	TCHAR m_szSortColumn[50];
	BOOL m_bSortOrder;
	struct{TCHAR szName[20];INT nWidth,nOrder;BOOL bActive;}m_ColumnInfos[NUMBER_OF_COLUMNS];

protected:
	virtual void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
	virtual LRESULT OnTimer(HWND hwnd, UINT id);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnScroll(INT dx, INT dy);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual LRESULT OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr);
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
	virtual void OnPaint(HWND hwnd);
	BOOL m_bHasImageList;
	HIMAGELIST m_hImageList;
	virtual BOOL OnEnsureVisible(INT nIndex, BOOL bPartialOK);
	virtual void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
	UINT m_uColumnCount;
	vector<CListEntry>m_Entries;
	BOOL m_bMapped;
	UINT m_uMaxColumnCount;
	
private:
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	struct _SEARCHITEM{TCHAR szBuf[100];BOOL bRelation;};
	BOOL m_bDontPaint;
	RECT m_ClientRect,m_ParentRect;
	SIZE m_ClientSize,m_OldClientSize;
	HDC m_hDCBack;
	HBITMAP m_hBmpBack;
	HPEN m_hGridPen;
	BOOL m_bUpdateListView;
	HIMAGELIST m_hImlDownUpArrows;
	TCHAR m_szOldSortColumn[50];
};

#endif // !defined(AFX_LIST_H__726DDCA0_8411_11D4_8925_009027C5DD32__INCLUDED_)
