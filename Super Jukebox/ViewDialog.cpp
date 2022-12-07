// ViewDialog.cpp: implementation of the CViewDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ViewDialog.h"
#include "Resource.h"
#include "VisualizationDialog.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CViewDialog::CViewDialog()
{
	m_uID=IDD_VIEW;
	UINT uIDs[NUMBER_OF_COLUMNS]={IDC_CHECK_PC_FILE,IDC_CHECK_PC_TITLE,IDC_CHECK_PC_GAME,IDC_CHECK_PC_ARTIST,IDC_CHECK_PC_PUBLISHER,IDC_CHECK_PC_DUMPER,IDC_CHECK_PC_DATE,IDC_CHECK_PC_COMMENT,IDC_CHECK_PC_LENGTH,IDC_CHECK_PC_FADE};
	memcpy(m_uCheckBoxIDs,uIDs,sizeof(m_uCheckBoxIDs));
}

CViewDialog::~CViewDialog()
{

}

void CViewDialog::InitMembers()
{
	memcpy(m_ColumnInfos,app.m_MainWindow.m_PlayList.m_ColumnInfos,sizeof(m_ColumnInfos));
	m_bFullRowSelect=app.m_Reg.m_bFullRowSelect;
	m_bShowGridLines=app.m_Reg.m_bShowGridLines;
	m_bTitleBar=app.m_Reg.m_bShowTitleBar;
	m_bMenu=app.m_Reg.m_bShowMenu;
	m_bToolBar=app.m_Reg.m_bShowToolBar;
	m_bLocationBar=app.m_Reg.m_bShowLocationBar;
	m_bFileList=app.m_MainWindow.m_FileList.IsVisible();
	m_bPlayList=app.m_MainWindow.m_PlayList.IsVisible();
	m_bControlPanel=app.m_Reg.m_bShowControlPanel;
	m_bStatusBar=app.m_Reg.m_bShowStatusBar;
	m_bShowInTaskbar=app.m_Reg.m_bShowInTaskbar;
	m_bShowInSystemTray=app.m_Reg.m_bShowInSystemTray;
}

BOOL CViewDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{	
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
		CheckDlgButton(m_hWnd,m_uCheckBoxIDs[i],m_ColumnInfos[i].bActive?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_FULLROWSELECT,m_bFullRowSelect?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_SHOWGRIDLINES,m_bShowGridLines?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_TITLEBAR,m_bTitleBar?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_MENU,m_bMenu?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_TOOLBAR,m_bToolBar?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_LOCATIONBAR,m_bLocationBar?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_FILELIST,m_bFileList?BST_CHECKED:BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_FILELIST),!app.m_MainWindow.m_ActiveWallpaper.IsRunning());
	CheckDlgButton(m_hWnd,IDC_CHECK_PLAYLIST,m_bPlayList?BST_CHECKED:BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_PLAYLIST),!app.m_MainWindow.m_ActiveWallpaper.IsRunning());
	CheckDlgButton(m_hWnd,IDC_CHECK_VISUALIZATION,CVisualizationDialog::m_bActive?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_CPL,m_bControlPanel?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_STATUSBAR,m_bStatusBar?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_TASKBAR,m_bShowInTaskbar?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_SYSTRAY,m_bShowInSystemTray?BST_CHECKED:BST_UNCHECKED);

	return FALSE;
}

BOOL CViewDialog::Validate()
{
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
		m_ColumnInfos[i].bActive=IsDlgButtonChecked(m_hWnd,m_uCheckBoxIDs[i])==BST_CHECKED;
	m_bFullRowSelect=IsDlgButtonChecked(m_hWnd,IDC_CHECK_FULLROWSELECT)==BST_CHECKED;
	m_bShowGridLines=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SHOWGRIDLINES)==BST_CHECKED;
	m_bTitleBar=IsDlgButtonChecked(m_hWnd,IDC_CHECK_TITLEBAR)==BST_CHECKED;
	m_bMenu=IsDlgButtonChecked(m_hWnd,IDC_CHECK_MENU)==BST_CHECKED;
	m_bToolBar=IsDlgButtonChecked(m_hWnd,IDC_CHECK_TOOLBAR)==BST_CHECKED;
	m_bLocationBar=IsDlgButtonChecked(m_hWnd,IDC_CHECK_LOCATIONBAR)==BST_CHECKED;
	m_bFileList=IsDlgButtonChecked(m_hWnd,IDC_CHECK_FILELIST)==BST_CHECKED;
	m_bPlayList=IsDlgButtonChecked(m_hWnd,IDC_CHECK_PLAYLIST)==BST_CHECKED;
	CVisualizationDialog::m_bActive=IsDlgButtonChecked(m_hWnd,IDC_CHECK_VISUALIZATION)==BST_CHECKED;
	m_bControlPanel=IsDlgButtonChecked(m_hWnd,IDC_CHECK_CPL)==BST_CHECKED;
	m_bStatusBar=IsDlgButtonChecked(m_hWnd,IDC_CHECK_STATUSBAR)==BST_CHECKED;
	m_bShowInTaskbar=IsDlgButtonChecked(m_hWnd,IDC_CHECK_TASKBAR)==BST_CHECKED;
	m_bShowInSystemTray=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SYSTRAY)==BST_CHECKED;

	return TRUE;
}

BOOL CViewDialog::CarryOutActions()
{
	if(app.m_Reg.m_bFullRowSelect!=m_bFullRowSelect||app.m_Reg.m_bShowGridLines!=m_bShowGridLines)
	{
		app.m_Reg.m_bFullRowSelect=m_bFullRowSelect;
		app.m_Reg.m_bShowGridLines=m_bShowGridLines;
		ListView_SetExtendedListViewStyleEx(app.m_MainWindow.m_FileList.m_hWnd,LVS_EX_FULLROWSELECT,app.m_Reg.m_bFullRowSelect?LVS_EX_FULLROWSELECT:0);
		ListView_SetExtendedListViewStyleEx(app.m_MainWindow.m_FileList.m_hWnd,LVS_EX_GRIDLINES,app.m_Reg.m_bShowGridLines?LVS_EX_GRIDLINES:0);
		app.m_MainWindow.m_FileList.RePaint(NULL);
		ListView_SetExtendedListViewStyleEx(app.m_MainWindow.m_PlayList.m_hWnd,LVS_EX_FULLROWSELECT,app.m_Reg.m_bFullRowSelect?LVS_EX_FULLROWSELECT:0);
		ListView_SetExtendedListViewStyleEx(app.m_MainWindow.m_PlayList.m_hWnd,LVS_EX_GRIDLINES,app.m_Reg.m_bShowGridLines?LVS_EX_GRIDLINES:0);
		app.m_MainWindow.m_PlayList.RePaint(NULL);
	}
	BOOL bChanged=FALSE;
	app.m_MainWindow.m_PlayList.SaveHeaderInfo();
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
	{
		if(m_ColumnInfos[i].bActive!=app.m_MainWindow.m_PlayList.m_ColumnInfos[i].bActive)
		{
			if(m_ColumnInfos[i].bActive)
			{
				for(INT c=0;c<NUMBER_OF_COLUMNS;c++)
					if(c!=i&&app.m_MainWindow.m_PlayList.m_ColumnInfos[c].nOrder>=app.m_MainWindow.m_PlayList.m_ColumnInfos[i].nOrder)
						app.m_MainWindow.m_PlayList.m_ColumnInfos[c].nOrder++;
			}
			else
			{
				for(INT c=0;c<NUMBER_OF_COLUMNS;c++)
					if(c!=i&&app.m_MainWindow.m_PlayList.m_ColumnInfos[c].nOrder>app.m_MainWindow.m_PlayList.m_ColumnInfos[i].nOrder)
						app.m_MainWindow.m_PlayList.m_ColumnInfos[c].nOrder--;
			}
			app.m_MainWindow.m_PlayList.m_ColumnInfos[i].bActive=m_ColumnInfos[i].bActive;
			bChanged=TRUE;
		}
	}
	if(bChanged)
	{
		app.m_MainWindow.m_PlayList.SaveRegistrySettings();
		app.m_MainWindow.m_PlaylistFilterDlg.RefreshComboBoxStates();
		app.m_MainWindow.m_PlayList.SetRedraw(FALSE);
		INT nIndex=app.m_MainWindow.m_PlayList.GetSelectedEntry();
		ListView_DeleteAllItems(app.m_MainWindow.m_PlayList.m_hWnd);
		app.m_MainWindow.m_PlayList.RefreshHeader();
		app.m_MainWindow.m_PlayList.DisplayEntries();
		app.m_MainWindow.m_PlayList.FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
		app.m_MainWindow.m_PlayList.SortItems(app.m_Reg.m_szPlayListSortColumn,app.m_Reg.m_bPlayListSortOrder);
		if(app.m_Reg.m_bAutoSizeColumns)
			app.m_MainWindow.m_PlayList.ResizeColumns();
		app.m_MainWindow.m_PlayList.SetRedraw(TRUE);
		InvalidateRect(app.m_MainWindow.m_PlayList.m_hWnd,NULL,TRUE);
		app.m_MainWindow.m_PlayList.SelectEntry(nIndex);
	}

	bChanged=FALSE;
	if(app.m_Reg.m_bShowTitleBar!=m_bTitleBar)
	{
		app.m_Reg.m_bShowTitleBar=m_bTitleBar;
		LONG lStyle=GetWindowLong(app.m_MainWindow.m_hWnd,GWL_STYLE);
		SetWindowLong(app.m_MainWindow.m_hWnd,GWL_STYLE,app.m_Reg.m_bShowTitleBar?(lStyle|WS_CAPTION):(lStyle&~WS_CAPTION));
		SetWindowPos(app.m_MainWindow.m_hWnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME|SWP_FRAMECHANGED);
	}
	if(app.m_Reg.m_bShowMenu!=m_bMenu)
	{
		app.m_Reg.m_bShowMenu=m_bMenu;
		SetMenu(app.m_MainWindow.m_hWnd,app.m_Reg.m_bShowMenu?app.m_MainWindow.m_Menu.m_hMenu:NULL);
	}
	if(app.m_Reg.m_bShowToolBar!=m_bToolBar)
	{
		app.m_Reg.m_bShowToolBar=m_bToolBar;
		app.m_MainWindow.m_ToolBar.IsVisible(app.m_Reg.m_bShowToolBar);
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowLocationBar!=m_bLocationBar)
	{
		app.m_Reg.m_bShowLocationBar=m_bLocationBar;
		app.m_MainWindow.m_SourceBar.IsVisible(app.m_Reg.m_bShowLocationBar);
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowFileList!=m_bFileList&&(!app.m_Reg.m_bUseActiveWallpaper||app.m_Player.IsStopped()))
	{
		app.m_Reg.m_bShowFileList=m_bFileList;
		app.m_MainWindow.m_FileList.IsVisible(app.m_Reg.m_bShowFileList);
		app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_FILELIST,app.m_Reg.m_bShowFileList);
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowPlayList!=m_bPlayList&&(!app.m_Reg.m_bUseActiveWallpaper||app.m_Player.IsStopped()))
	{
		app.m_Reg.m_bShowPlayList=m_bPlayList;
		app.m_MainWindow.m_PlayList.IsVisible(app.m_Reg.m_bShowPlayList);
		app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_PLAYLIST,app.m_Reg.m_bShowPlayList);
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowVisWindow!=CVisualizationDialog::m_bActive)
	{
		app.m_Reg.m_bShowVisWindow=CVisualizationDialog::m_bActive;
		if(app.m_Reg.m_bShowVisWindow)
		{
			app.m_MainWindow.m_VisWindow.Create(app.m_hInstance,&app.m_MainWindow,IDC_VIS_WINDOW);
			if(app.m_Player.IsPlaying()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
				app.m_MainWindow.m_VisWindow.Start();
		}
		else
			app.m_MainWindow.m_VisWindow.Destroy();
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowControlPanel!=m_bControlPanel)
	{
		app.m_Reg.m_bShowControlPanel=m_bControlPanel;
		app.m_MainWindow.m_ControlPanel.IsVisible(app.m_Reg.m_bShowControlPanel);
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bShowStatusBar!=m_bStatusBar)
	{
		app.m_Reg.m_bShowStatusBar=m_bStatusBar;
		app.m_MainWindow.m_StatusBar.IsVisible(app.m_Reg.m_bShowStatusBar);
		bChanged=TRUE;
	}
	if(bChanged)
	{
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
	}
	if(app.m_Reg.m_bShowInTaskbar!=m_bShowInTaskbar)
	{
		app.m_Reg.m_bShowInTaskbar=m_bShowInTaskbar;
		if(app.m_Reg.m_bShowInTaskbar)
			app.m_MainWindow.AddTaskbarButton();
		else
			app.m_MainWindow.RemoveTaskbarButton();
	}
	if(app.m_Reg.m_bShowInSystemTray!=m_bShowInSystemTray)
	{
		app.m_Reg.m_bShowInSystemTray=m_bShowInSystemTray;
		if(app.m_Reg.m_bShowInSystemTray)
			app.m_MainWindow.AddSystemTrayIcon();
		else
			app.m_MainWindow.RemoveSystemTrayIcon();
	}

	return TRUE;
}
