// GotoDialog.cpp: implementation of the CGotoDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GotoDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGotoDialog::CGotoDialog()
{
}

CGotoDialog::~CGotoDialog()
{

}

BOOL CGotoDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_nSongNumber=app.m_MainWindow.m_PlayList.GetSelectedEntry()+1;
	if(m_nSongNumber<1)m_nSongNumber=1;
	else if(m_nSongNumber>app.m_MainWindow.m_PlayList.GetDisplayedCount())m_nSongNumber=app.m_MainWindow.m_PlayList.GetDisplayedCount();
	SetDlgItemInt(m_hWnd,IDC_EDIT_GOTO_SONG_NUMBER,m_nSongNumber,TRUE);
	SendDlgItemMessage(m_hWnd,IDC_SPIN_GOTO_SONG_NUMBER,UDM_SETRANGE32,1,app.m_MainWindow.m_PlayList.GetDisplayedCount());

	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CGotoDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case EN_CHANGE:
		switch(id)
		{
		case IDC_EDIT_GOTO_SONG_NUMBER:
			m_nSongNumber=GetDlgItemInt(m_hWnd,IDC_EDIT_GOTO_SONG_NUMBER,NULL,TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDOK),m_nSongNumber>0&&m_nSongNumber<=app.m_MainWindow.m_PlayList.GetDisplayedCount());
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}

BOOL CGotoDialog::Validate()
{
	m_nSongNumber=GetDlgItemInt(m_hWnd,IDC_EDIT_GOTO_SONG_NUMBER,NULL,TRUE);
	if(m_nSongNumber>0&&m_nSongNumber<=app.m_MainWindow.m_PlayList.GetDisplayedCount())
		app.m_MainWindow.m_PlayList.SelectEntry(m_nSongNumber-1);
	else
		MessageBox(m_hWnd,"Song number is out of range.","Go To",MB_ICONINFORMATION);
	return FALSE;
}

LRESULT CGotoDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
