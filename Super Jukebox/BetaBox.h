// BetaBox.h: interface for the BetaBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BETABOX_H__61D63322_C088_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_BETABOX_H__61D63322_C088_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CBetaBox : public CWnd  
{
public:
	CBetaBox();
	virtual ~CBetaBox();

protected:
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};

#endif // !defined(AFX_BETABOX_H__61D63322_C088_11D4_B2F9_00D0B7AE5907__INCLUDED_)
