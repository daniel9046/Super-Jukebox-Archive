// WarningDialog.cpp: implementation of the CWarningDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WarningDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWarningDialog::CWarningDialog()
{
	m_szWarningText[0]='\0';
	m_bDoNotShowAgain=FALSE;
}

CWarningDialog::~CWarningDialog()
{

}

BOOL CWarningDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SendDlgItemMessage(m_hWnd,IDC_STATIC_ICON,STM_SETICON,(WPARAM)LoadIcon(NULL,IDI_WARNING),0);
	SetDlgItemText(m_hWnd,IDC_EDIT_WARNING,m_szWarningText);
	CheckDlgButton(m_hWnd,IDC_CHECK_NOMORE,m_bDoNotShowAgain?BST_CHECKED:BST_UNCHECKED);

	MessageBeep(MB_ICONEXCLAMATION);
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

void CWarningDialog::SetWarningText(LPCTSTR lpszText)
{
	strncpy(m_szWarningText,lpszText,sizeof(m_szWarningText));
}

BOOL CWarningDialog::Validate()
{
	m_bDoNotShowAgain=IsDlgButtonChecked(m_hWnd,IDC_CHECK_NOMORE)==BST_CHECKED;
	return TRUE;
}

BOOL CWarningDialog::IsNoMoreChecked()
{
	return m_bDoNotShowAgain;
}

LRESULT CWarningDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
