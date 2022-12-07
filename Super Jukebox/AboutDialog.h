// AboutDialog.h: interface for the CAboutDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ABOUTDIALOG_H__3FA77780_BA40_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_ABOUTDIALOG_H__3FA77780_BA40_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CAboutDialog : public CWnd  
{
public:
	void DrawLinkButton(const DRAWITEMSTRUCT *lpDrawItem);
	CAboutDialog();
	virtual ~CAboutDialog();

protected:
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	HFONT m_hFont;
	struct _tmp{TCHAR szBuf[500];};
};

#endif // !defined(AFX_ABOUTDIALOG_H__3FA77780_BA40_11D4_B2F9_00D0B7AE5907__INCLUDED_)
