// MiscDialog.cpp: implementation of the CMiscDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MiscDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMiscDialog::CMiscDialog()
{
	m_uID=IDD_MISC;
}

CMiscDialog::~CMiscDialog()
{

}

void CMiscDialog::InitMembers()
{
	m_bAutoLoadLastPlayList=app.m_Reg.m_bAutoLoadPlaylist;
	m_bAutoPlayLastSong=app.m_Reg.m_bAutoPlaySong;
	m_bAutoSizePlayListColumns=app.m_Reg.m_bAutoSizeColumns;
	m_bMinimizeToSysTray=app.m_Reg.m_bMinimizeToSystemTray;
	m_bReadID666Tags=app.m_Reg.m_bReadID666;
	m_bSaveID666Tags=app.m_Reg.m_bSaveID666;
	m_bDisplayToolTips=app.m_Reg.m_bDisplayToolTips;
	m_bUseFastSeek=app.m_Reg.m_bUseFastSeek;
	m_dwSysTrayCmd=app.m_Reg.m_dwSysTrayCmd;
}

BOOL CMiscDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	CheckDlgButton(m_hWnd,IDC_CHECK_AUTOLOAD_PLAYLIST,m_bAutoLoadLastPlayList?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_AUTOPLAY_SONG,m_bAutoPlayLastSong?BST_CHECKED:BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_AUTOPLAY_SONG),m_bAutoLoadLastPlayList);
	CheckDlgButton(m_hWnd,IDC_CHECK_AUTOSIZE_COLUMNS,m_bAutoSizePlayListColumns?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_MINIMIZETOSYSTRAY,m_bMinimizeToSysTray?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_READ_ID666_TAGS,m_bReadID666Tags?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_SAVE_ID666_TAGS,m_bSaveID666Tags?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_DISPLAYTOOLTIPS,m_bDisplayToolTips?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_USEFASTSEEK,m_bUseFastSeek?BST_CHECKED:BST_UNCHECKED);

	LPCTSTR lpszCmds[]={"Play","Pause","Stop","Previous","Next","Configure","Exit"};
	for(UINT i=0;i<sizeof(lpszCmds)/sizeof(lpszCmds[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_SYSTRAYCMD,CB_ADDSTRING,0,(LPARAM)lpszCmds[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SYSTRAYCMD,CB_SETCURSEL,m_dwSysTrayCmd,0);
	return FALSE;
}

BOOL CMiscDialog::Validate()
{
	m_bAutoLoadLastPlayList=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTOLOAD_PLAYLIST)==BST_CHECKED;
	m_bAutoPlayLastSong=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTOPLAY_SONG)==BST_CHECKED;
	m_bAutoSizePlayListColumns=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTOSIZE_COLUMNS)==BST_CHECKED;
	m_bMinimizeToSysTray=IsDlgButtonChecked(m_hWnd,IDC_CHECK_MINIMIZETOSYSTRAY)==BST_CHECKED;
	m_bReadID666Tags=IsDlgButtonChecked(m_hWnd,IDC_CHECK_READ_ID666_TAGS)==BST_CHECKED;
	m_bSaveID666Tags=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SAVE_ID666_TAGS)==BST_CHECKED;
	m_bDisplayToolTips=IsDlgButtonChecked(m_hWnd,IDC_CHECK_DISPLAYTOOLTIPS)==BST_CHECKED;
	m_bUseFastSeek=IsDlgButtonChecked(m_hWnd,IDC_CHECK_USEFASTSEEK)==BST_CHECKED;
	m_dwSysTrayCmd=SendDlgItemMessage(m_hWnd,IDC_COMBO_SYSTRAYCMD,CB_GETCURSEL,0,0);
	return TRUE;
}

BOOL CMiscDialog::CarryOutActions()
{
	app.m_Reg.m_bAutoLoadPlaylist=m_bAutoLoadLastPlayList;
	app.m_Reg.m_bAutoPlaySong=m_bAutoPlayLastSong;
	if(app.m_Reg.m_bAutoSizeColumns!=m_bAutoSizePlayListColumns)
	{
		app.m_Reg.m_bAutoSizeColumns=m_bAutoSizePlayListColumns;
		if(app.m_Reg.m_bAutoSizeColumns)
			app.m_MainWindow.m_PlayList.ResizeColumns();
	}
	app.m_Reg.m_bMinimizeToSystemTray=m_bMinimizeToSysTray;
	app.m_Reg.m_bReadID666=m_bReadID666Tags;
	app.m_Reg.m_bSaveID666=m_bSaveID666Tags;
	if(app.m_Reg.m_bDisplayToolTips!=m_bDisplayToolTips)
	{
		app.m_Reg.m_bDisplayToolTips=m_bDisplayToolTips;
		app.m_MainWindow.m_ToolBar.DisplayToolTips(app.m_Reg.m_bDisplayToolTips);
		app.m_MainWindow.m_SourceBar.DisplayToolTips(app.m_Reg.m_bDisplayToolTips);
	}
	app.m_Reg.m_bUseFastSeek=m_bUseFastSeek;
	app.m_Reg.m_dwSysTrayCmd=m_dwSysTrayCmd;
	return TRUE;
}

LRESULT CMiscDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_CHECK_AUTOLOAD_PLAYLIST:
			EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_AUTOPLAY_SONG),IsDlgButtonChecked(m_hWnd,id)==BST_CHECKED);
			break;
		}
		break;
	}
	return 0;
}
