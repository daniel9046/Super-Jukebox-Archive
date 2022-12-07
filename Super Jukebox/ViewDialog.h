// ViewDialog.h: interface for the CViewDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEWDIALOG_H__FD743242_AB49_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_VIEWDIALOG_H__FD743242_AB49_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "Main.h"

class CViewDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CViewDialog();
	virtual ~CViewDialog();

protected:
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	BOOL m_bLocationBar;
	struct {TCHAR szName[20];INT nWidth,nOrder;BOOL bActive;}m_ColumnInfos[NUMBER_OF_COLUMNS];
	UINT m_uCheckBoxIDs[NUMBER_OF_COLUMNS];
	BOOL m_bFullRowSelect,m_bShowGridLines;
	BOOL m_bTitleBar;
	BOOL m_bMenu;
	BOOL m_bToolBar;
	BOOL m_bFileList;
	BOOL m_bPlayList;
	BOOL m_bControlPanel;
	BOOL m_bStatusBar;
	BOOL m_bShowInTaskbar;
	BOOL m_bShowInSystemTray;
};

#endif // !defined(AFX_VIEWDIALOG_H__FD743242_AB49_11D4_B2F9_00D0B7AE5907__INCLUDED_)
