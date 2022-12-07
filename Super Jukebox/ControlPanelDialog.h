// ControlPanelDialog.h: interface for the CControlPanelDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLPANELDIALOG_H__FE2C05C0_B0C9_11D4_B2F7_00D0B79FA829__INCLUDED_)
#define AFX_CONTROLPANELDIALOG_H__FE2C05C0_B0C9_11D4_B2F7_00D0B79FA829__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CControlPanelDialog : public CWnd  
{
public:
	CControlPanelDialog();
	virtual ~CControlPanelDialog();

protected:
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
};

#endif // !defined(AFX_CONTROLPANELDIALOG_H__FE2C05C0_B0C9_11D4_B2F7_00D0B79FA829__INCLUDED_)
