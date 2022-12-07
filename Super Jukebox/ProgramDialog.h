// ProgramDialog.h: interface for the CProgramDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMDIALOG_H__9ED54BC3_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_PROGRAMDIALOG_H__9ED54BC3_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CProgramDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CProgramDialog();
	virtual ~CProgramDialog();

protected:
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	INT m_nMainThreadPriority;
	INT m_nProcessPriority;
	struct EXT{TCHAR szName[4];BOOL bSelected,bOrgSelected;}m_Extensions[12];
	TCHAR m_szAssocExt[100];
	BOOL m_bRegFileTypes;
};

#endif // !defined(AFX_PROGRAMDIALOG_H__9ED54BC3_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_)
