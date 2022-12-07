// SkinDialog.h: interface for the CSkinDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINDIALOG_H__46A6C6E1_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_SKINDIALOG_H__46A6C6E1_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "Skin.h"

class CSkinDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CSkinDialog();
	virtual ~CSkinDialog();

protected:
	virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	CSkin m_Skin;
	BOOL m_bAlwaysUseProgramSkin;
	COLORREF m_CustomColors[16];
};

#endif // !defined(AFX_SKINDIALOG_H__46A6C6E1_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
