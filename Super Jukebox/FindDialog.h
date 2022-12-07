// FindDialog.h: interface for the CFindDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FINDDIALOG_H__C2F28002_8FD7_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_FINDDIALOG_H__C2F28002_8FD7_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CFindDialog : public CWnd
{
public:
	CFindDialog();
	virtual ~CFindDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	BOOL Validate();
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

private:
	TCHAR m_szFindWhat[1000];
	INT m_nSearchColumn;
	BOOL m_bMatchWholeWordOnly;
	BOOL m_bMatchCase;
	BOOL m_bDirection;
};

#endif // !defined(AFX_FINDDIALOG_H__C2F28002_8FD7_11D4_B2F9_00D0B7AE5907__INCLUDED_)
