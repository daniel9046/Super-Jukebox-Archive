// GotoDialog.h: interface for the CGotoDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GOTODIALOG_H__C2338180_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_GOTODIALOG_H__C2338180_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CGotoDialog : public CWnd
{
public:
	CGotoDialog();
	virtual ~CGotoDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	BOOL Validate();
	LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	INT m_nSongNumber;
};

#endif // !defined(AFX_GOTODIALOG_H__C2338180_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_)
