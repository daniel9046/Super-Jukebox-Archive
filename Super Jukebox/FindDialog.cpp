// FindDialog.cpp: implementation of the CFindDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindDialog::CFindDialog()
{
	m_szFindWhat[0]='\0';
	m_nSearchColumn=0;
	m_bMatchWholeWordOnly=FALSE;
	m_bMatchCase=FALSE;
	m_bDirection=TRUE;
}

CFindDialog::~CFindDialog()
{

}

BOOL CFindDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	SendDlgItemMessage(m_hWnd,IDC_EDIT_FIND_WHAT,EM_LIMITTEXT,sizeof(m_szFindWhat)-1,0);
	SetDlgItemText(m_hWnd,IDC_EDIT_FIND_WHAT,m_szFindWhat);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SEARCH_COLUMN,CB_ADDSTRING,0,(LPARAM)"Any");
	INT nOrder[NUMBER_OF_COLUMNS];
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	ListView_GetColumnOrderArray(pl.m_hWnd,pl.GetColumnCount(),nOrder);
	for(UINT c=0;c<pl.GetColumnCount();c++)
	{
		TCHAR szColName[100];
		pl.GetColumnNameFromIndex(nOrder[c],szColName,sizeof(szColName));
		SendDlgItemMessage(m_hWnd,IDC_COMBO_SEARCH_COLUMN,CB_ADDSTRING,0,(LPARAM)szColName);
	}
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SEARCH_COLUMN,CB_SETCURSEL,m_nSearchColumn,0);
	CheckDlgButton(m_hWnd,IDC_CHECK_MATCH_WHOLE_WORD_ONLY,m_bMatchWholeWordOnly?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_MATCH_CASE,m_bMatchCase?BST_CHECKED:BST_UNCHECKED);
	CheckRadioButton(m_hWnd,IDC_RADIO_FIND_UP,IDC_RADIO_FIND_DOWN,m_bDirection?IDC_RADIO_FIND_DOWN:IDC_RADIO_FIND_UP);
	EnableWindow(GetDlgItem(m_hWnd,IDOK),SendDlgItemMessage(m_hWnd,IDC_EDIT_FIND_WHAT,EM_LINELENGTH,0,0));

	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CFindDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case EN_CHANGE:
		switch(id)
		{
		case IDC_EDIT_FIND_WHAT:
			EnableWindow(GetDlgItem(m_hWnd,IDOK),SendDlgItemMessage(m_hWnd,IDC_EDIT_FIND_WHAT,EM_LINELENGTH,0,0));
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}

BOOL CFindDialog::Validate()
{
	GetDlgItemText(m_hWnd,IDC_EDIT_FIND_WHAT,m_szFindWhat,sizeof(m_szFindWhat));
	m_nSearchColumn=SendDlgItemMessage(m_hWnd,IDC_COMBO_SEARCH_COLUMN,CB_GETCURSEL,0,0);
	m_bMatchWholeWordOnly=IsDlgButtonChecked(m_hWnd,IDC_CHECK_MATCH_WHOLE_WORD_ONLY)==BST_CHECKED;
	m_bMatchCase=IsDlgButtonChecked(m_hWnd,IDC_CHECK_MATCH_CASE)==BST_CHECKED;
	m_bDirection=IsDlgButtonChecked(m_hWnd,IDC_RADIO_FIND_DOWN)==BST_CHECKED;

	CPlayList &pl=app.m_MainWindow.m_PlayList;
	INT nMatch=-1;
	INT nOrder[NUMBER_OF_COLUMNS];
	ListView_GetColumnOrderArray(pl.m_hWnd,pl.GetColumnCount(),nOrder);
	if(m_nSearchColumn>0)
	{
		TCHAR szColName[1000];
		pl.GetColumnNameFromIndex(nOrder[m_nSearchColumn-1],szColName,sizeof(szColName));
		nMatch=pl.FindEntry(pl.GetSelectedEntry(),szColName,m_szFindWhat,m_bMatchWholeWordOnly,m_bMatchCase,m_bDirection);
	}
	else
	{
		for(UINT c=0;c<pl.GetColumnCount();c++)
		{
			TCHAR szColName[100];
			pl.GetColumnNameFromIndex(nOrder[c],szColName,sizeof(szColName));
			nMatch=pl.FindEntry(pl.GetSelectedEntry(),szColName,m_szFindWhat,m_bMatchWholeWordOnly,m_bMatchCase,m_bDirection);
			if(nMatch>=0)
				break;
		}
	}

	if(nMatch>=0)
		pl.SelectEntry(nMatch);
	else
		MessageBox(m_hWnd,"Search string not found.","Find",MB_ICONINFORMATION);
	
	return FALSE;
}

LRESULT CFindDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
