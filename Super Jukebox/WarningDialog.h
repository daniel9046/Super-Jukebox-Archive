// WarningDialog.h: interface for the CWarningDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WARNINGDIALOG_H__50190C20_A99F_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_WARNINGDIALOG_H__50190C20_A99F_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CWarningDialog : public CWnd  
{
public:
	BOOL IsNoMoreChecked();
	void SetWarningText(LPCTSTR lpszText);
	CWarningDialog();
	virtual ~CWarningDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	virtual BOOL Validate();
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	BOOL m_bDoNotShowAgain;
	TCHAR m_szWarningText[1000];
};

#endif // !defined(AFX_WARNINGDIALOG_H__50190C20_A99F_11D4_B2F9_00D0B7AE5907__INCLUDED_)
