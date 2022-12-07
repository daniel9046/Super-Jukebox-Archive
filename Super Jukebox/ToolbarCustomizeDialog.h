// ToolbarCustomizeDialog.h: interface for the CToolbarCustomizeDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARCUSTOMIZEDIALOG_H__FB98E820_A69F_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_TOOLBARCUSTOMIZEDIALOG_H__FB98E820_A69F_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "Main.h"

class CToolbarCustomizeDialog : public CWnd  
{
public:
	CToolbarCustomizeDialog();
	virtual ~CToolbarCustomizeDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	virtual LRESULT OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	BOOL m_bChanged;
	void OnClose();
	INT GetSelectedItem(HWND hWnd);
	BOOL SetSelectedItem(HWND hWnd,INT nIndex);
	void OnMoveDown();
	void OnMoveUp();
	void OnReset();
	void OnRemove();
	void OnAdd();
	HWND m_hWndLeftList,m_hWndRightList;
	HIMAGELIST m_hImlColorIcons;
	struct{TCHAR szName[50];INT idCommand,iBitmap,iString;BYTE bStyle;BOOL bPosition;}m_Buttons[NUMBER_OF_BUTTONS];
};

#endif // !defined(AFX_TOOLBARCUSTOMIZEDIALOG_H__FB98E820_A69F_11D4_B2F9_00D0B7AE5907__INCLUDED_)
