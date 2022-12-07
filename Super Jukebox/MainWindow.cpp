// MainWindow.cpp: implementation of the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainWindow.h"
#include "Resource.h"
#include "AfxRes.h"
#include "ConfigurationDialog.h"
#include "BetaBox.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainWindow::CMainWindow()
{
	m_hWndOldFocus=NULL;
	m_bAdjustingListViews=FALSE;
	m_bMaintainCursor=FALSE;
	m_bDontPaint=FALSE;
	m_nOldMouseX=m_nOldMouseY=-1;
	m_bPass=FALSE;
}

CMainWindow::~CMainWindow()
{

}

BOOL CMainWindow::RegisterClass(HINSTANCE hInstance)
{
	m_hInstance=hInstance;

	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,app.m_szAppName,&wcx))
	{
		wcx.style=0;
		wcx.lpfnWndProc=CWnd::WindowProc;
		wcx.cbClsExtra=0;
		wcx.cbWndExtra=0;
		wcx.hInstance=m_hInstance;
		wcx.hIcon=LoadIcon(m_hInstance,MAKEINTRESOURCE(IDI_SUPER_JUKEBOX));
		wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
		wcx.hbrBackground=(HBRUSH)(COLOR_3DFACE+1);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName=app.m_szAppName;
		wcx.hIconSm=NULL;

		if(!RegisterClassEx(&wcx))
			DisplayError(NULL,TRUE,"Failed to register the main window class.");
	}

	return TRUE;
}

BOOL CMainWindow::Create(int nCmdShow)
{
	m_bAdjustLayout=FALSE;
	RECT rc={0};
	if(!CWnd::Create(app.m_szAppName,app.m_szAppName,WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,WS_EX_APPWINDOW|WS_EX_ACCEPTFILES,rc,NULL,0,m_hInstance))
		DisplayError(NULL,TRUE,NULL);

	m_bAdjustLayout=TRUE;
	SetWindowPlacement(m_hWnd,&app.m_Reg.m_wp);
	AdjustLayout();
	RePaint(NULL);

	/*m_bBetaBox=TRUE;
	CBetaBox dlg;
	if(dlg.DoModal(IDD_BETABOX,this,m_hInstance)==-1)
		return FALSE;
	m_bBetaBox=FALSE;*/

	LPTSTR lpCmdLine=app.GetCommandLine();
	if(strlen(lpCmdLine))
	{
		TCHAR szFileBuf[MAX_PATH],szExt[_MAX_EXT];
		strncpy(szFileBuf,lpCmdLine,sizeof(szFileBuf));
		_splitpath(szFileBuf,NULL,NULL,NULL,szExt);
		if(!stricmp(szExt,".lnk"))
			GetLinkTarget(lpCmdLine,szFileBuf,sizeof(szFileBuf));
		_splitpath(szFileBuf,NULL,NULL,NULL,szExt);
		if(!strlen(szExt))
			strcat(szFileBuf,".pl");
		if(!stricmp(szExt,".pl"))
		{
			if(!m_PlayList.Load(szFileBuf))
			{
				TCHAR szBuf[1000];
				sprintf(szBuf,"Failed to load playlist: %s",szBuf);
				DisplayError(m_hWnd,FALSE,szBuf);
			}
		}
		LPCTSTR lpszSPCs[]={".spc",".sp0",".sp1",".sp2",".sp3",".sp4",".sp5",".sp6",".sp7",".sp8",".sp9"};
		for(UINT i=0;i<sizeof(lpszSPCs)/sizeof(lpszSPCs[0]);i++)
			if(!stricmp(szExt,lpszSPCs[i]))
			{
				m_PlayList.ClearFilesToAdd();
				m_PlayList.AddFileToAdd(szFileBuf);
				m_PlayList.AddFiles(FALSE,FALSE);
				m_ToolBar.SetSongList(&m_PlayList);
				m_ToolBar.OnClicked(IDC_BUTTON_PLAY);
				break;
			}
		
	}
	else if(app.m_Reg.m_bAutoLoadPlaylist&&strlen(app.m_Reg.m_szLastPlaylist))
	{
		if(!m_PlayList.Load(app.m_Reg.m_szLastPlaylist))
		{
			TCHAR szBuf[1000];
			sprintf(szBuf,"Failed to load the last used playlist: %s",app.m_Reg.m_szLastPlaylist);
			DisplayError(m_hWnd,FALSE,szBuf);
		}
		if(app.m_Reg.m_bAutoPlaySong&&(INT)app.m_Reg.m_dwLastPlayingSong>=0&&(INT)app.m_Reg.m_dwLastPlayingSong<m_PlayList.GetEntryCount())
		{
			SetFocus(m_PlayList.m_hWnd);
			m_PlayList.SelectEntry(m_PlayList.MapIndexToExternal(app.m_Reg.m_dwLastPlayingSong));
			m_PlayList.ProcessEntry(m_PlayList.MapIndexToExternal(app.m_Reg.m_dwLastPlayingSong));
		}
	}
	else
		m_PlayList.SetSortingColumn(app.m_Reg.m_szPlayListSortColumn,app.m_Reg.m_bPlayListSortOrder);

	return TRUE;
}

BOOL CMainWindow::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	m_hSysMenu=GetSystemMenu(m_hWnd,FALSE);
	if(m_hSysMenu!=NULL)
	{
		MENUITEMINFO mii;
		mii.cbSize=sizeof(mii);
		mii.fType=MFT_STRING;
		mii.fState=app.m_Reg.m_bAlwaysOnTop?MFS_CHECKED:MFS_UNCHECKED;
		mii.wID=IDM_ALWAYSONTOP;
		mii.dwTypeData="&Always On Top\tShift+Y";
		mii.cch=strlen("&Always On Top\tShift+Y");
		mii.fMask=MIIM_ID|MIIM_STATE|MIIM_TYPE;
		InsertMenuItem(m_hSysMenu,SC_CLOSE,FALSE,&mii);
		mii.fType=MFT_SEPARATOR;
		InsertMenuItem(m_hSysMenu,SC_CLOSE,FALSE,&mii);
	}
	SetWindowPos(m_hWnd,app.m_Reg.m_bAlwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	Uematsu_SetWindowHandle(m_hWnd);

	if(!app.m_Reg.m_bShowInTaskbar)
		RemoveTaskbarButton();
	if(!app.m_Reg.m_bShowTitleBar)
		SetWindowLong(m_hWnd,GWL_STYLE,GetWindowLong(m_hWnd,GWL_STYLE)&~WS_CAPTION);
	if(!m_Menu.Create(m_hInstance,this,IDR_MENU_SUPER_JUKEBOX))return FALSE;
	if(!app.m_Reg.m_bShowMenu)
		SetMenu(m_hWnd,NULL);
	if(!m_ReBar.Create(m_hInstance,this,IDC_REBAR))return FALSE;
	if(!m_FileList.Create(m_hInstance,this,IDC_LIST_FILELIST))return FALSE;
	if(!app.m_Reg.m_bShowFileList)
		m_FileList.IsVisible(FALSE);
	if(!m_PlayList.Create(m_hInstance,this,IDC_LIST_PLAYLIST))return FALSE;
	if(!app.m_Reg.m_bShowPlayList)
		m_PlayList.IsVisible(FALSE);
	if(!m_ControlPanel.Create(IDD_CPL,this,m_hInstance))return FALSE;
	if(!app.m_Reg.m_bShowControlPanel)
		m_ControlPanel.IsVisible(FALSE);
	if(app.m_Reg.m_bShowVisWindow&&!m_VisWindow.Create(m_hInstance,this,IDC_VIS_WINDOW))return FALSE;
	if(!m_StatusBar.Create(m_hInstance,this,IDC_STATUS_BAR))return FALSE;
	if(!app.m_Reg.m_bShowStatusBar)
		m_StatusBar.IsVisible(FALSE);

	if(app.m_Reg.m_bShowInSystemTray&&!AddSystemTrayIcon())
		DisplayError(m_hWnd,FALSE,"Failed to add the system tray icon.");

	m_Skin.IsActive(app.m_Reg.m_bUseSkin);
	m_Skin.SetLayout(app.m_Reg.m_dwSkinLayout);
	m_Skin.SetFileName(app.m_Reg.m_szSkinFileName);
	m_Skin.IsBlended(app.m_Reg.m_bBlendSkin);
	m_Skin.SetBlendingColor(app.m_Reg.m_crBlendingColor);
	m_Skin.SetTextColor(app.m_Reg.m_crTextColor);
	m_Skin.SetBlendingLevel(app.m_Reg.m_dwBlendingLevel);
	m_Skin.Load();

	if(stricmp(app.m_Reg.m_szActiveWallpaperPlugin,"(none)")&&!m_ActiveWallpaper.LoadPlugin(app.m_Reg.m_szActiveWallpaperPlugin))
	{
		TCHAR szBuf[MAX_PATH*2];
		sprintf(szBuf,"Failed to load the Active Wallpaper plugin (%s).",app.m_Reg.m_szActiveWallpaperPlugin);
		DisplayError(m_hWnd,FALSE,szBuf);
	}

	SetFocus(m_FileList.m_hWnd);

	if(!m_FileList.SetSystemImageList())return FALSE;
	m_FileList.LoadRegistrySettings();
	m_FileList.RefreshHeader();
	m_FileList.SetSource(app.m_Reg.m_szLastDirectory);
	m_FileList.LoadDriveList();
	m_FileList.LoadDirectoryList();
	m_FileList.LoadFileList();
	m_FileList.SortItems(m_FileList.m_szSortColumn,m_FileList.m_bSortOrder);
	m_FileList.ResizeColumns();
	m_FileList.SelectEntry(0);
	
	m_PlayList.Reset();
	m_PlayList.LoadRegistrySettings();
	m_PlayList.RefreshHeader();
	UpdateTitleBar();
	m_StatusBar.Update();
	if(!SetTimer(m_hWnd,1,100,NULL))return FALSE;

	return TRUE;
}

LRESULT CMainWindow::OnDestroy(HWND hwnd)
{
	m_ToolBar.OnClicked(IDC_BUTTON_STOP);
	m_ActiveWallpaper.UnloadPlugin();
	if((app.m_Reg.m_bMinimizeToSystemTray&&IsIconic(m_hWnd))||app.m_Reg.m_bShowInSystemTray)
		RemoveSystemTrayIcon();
	SyncRegistrySettings();
	WinHelp(m_hWnd,NULL,HELP_QUIT,0);
	PostQuitMessage(0);
	return 0;
}

void CMainWindow::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if(!m_bAdjustLayout)
		return;
	
	static BOOL bMinimized=FALSE;
	if(state==SIZE_MINIMIZED)
	{
		BOOL b=app.IsCallInternal(TRUE);
		if(m_VisWindow.IsRunning())
			m_VisWindow.Stop();
		if(m_ActiveWallpaper.IsRunning())
			m_ActiveWallpaper.Stop();
		app.IsCallInternal(b);
		if(app.m_Reg.m_bMinimizeToSystemTray)
		{
			if(!app.m_Reg.m_bShowInSystemTray)
				AddSystemTrayIcon();
			if(app.m_Reg.m_bShowInTaskbar)
				RemoveTaskbarButton();
		}
		bMinimized=TRUE;
		return;
	}

	if(bMinimized)
	{
		bMinimized=FALSE;
		if(!app.m_Player.IsStopped())
		{
			if(app.m_Reg.m_bShowVisWindow)
				m_VisWindow.Start();
			if(app.m_Reg.m_bUseActiveWallpaper)
				m_ActiveWallpaper.Start();
		}
		else
		{
			if(app.m_MainWindow.m_FileList.IsVisible()!=app.m_Reg.m_bShowFileList)
				app.m_MainWindow.m_FileList.IsVisible(app.m_Reg.m_bShowFileList);
			if(app.m_MainWindow.m_PlayList.IsVisible()!=app.m_Reg.m_bShowPlayList)
				app.m_MainWindow.m_PlayList.IsVisible(app.m_Reg.m_bShowPlayList);
			app.m_MainWindow.AdjustLayout();
			app.m_MainWindow.RePaint(NULL);
		}
		if(app.m_Reg.m_bMinimizeToSystemTray)
		{			
			if(!app.m_Reg.m_bShowInSystemTray)
				RemoveSystemTrayIcon();
			if(app.m_Reg.m_bShowInTaskbar)
				AddTaskbarButton();
		}		
	}

	AdjustLayout();
	if(GetActiveSkin().IsActive())
	{
		if(m_FileList.IsVisible()&&m_PlayList.IsVisible())
			InvalidateRect(m_FileList.m_hWnd,NULL,FALSE);
		else
			InvalidateRect(m_hWnd,&m_ClientRect,TRUE);
	}
	return;
}

LRESULT CMainWindow::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	LRESULT lResult=1;

	switch(id)
	{
	case IDC_BUTTON_ADD:
	case IDC_BUTTON_REMOVE:
	case IDC_BUTTON_ADDALL:
	case IDC_BUTTON_REMOVEALL:
	case IDC_BUTTON_PLAY:
	case IDC_BUTTON_PAUSE:
	case IDC_BUTTON_STOP:
	case IDC_BUTTON_PREVIOUS:
	case IDC_BUTTON_NEXT:
	case IDC_BUTTON_WAVDUMP:
	case IDC_BUTTON_AW:
	case IDC_BUTTON_FILTERVIEW:
	case IDC_BUTTON_LAYOUT:
	case IDC_BUTTON_VISMODE:
		lResult=m_ToolBar.OnClicked(id);break;
	case ID_MUTE0:
	case ID_MUTE1:
	case ID_MUTE2:
	case ID_MUTE3:
	case ID_MUTE4:
	case ID_MUTE5:
	case ID_MUTE6:
	case ID_MUTE7:
	case ID_MUTEALL:
		app.m_MainWindow.m_VisWindow.MuteChannels(id);lResult=0;break;
	case ID_FOCUSLOCATION:m_SourceBar.Focus();lResult=0;break;
	case ID_FILE_NEW:lResult=m_Menu.OnFileNew();break;
	case ID_FILE_OPEN:lResult=m_Menu.OnFileOpen();break;
	case ID_FILE_SAVE:lResult=m_Menu.OnFileSave();break;
	case ID_FILE_SAVE_AS:lResult=m_Menu.OnFileSaveAs();break;
	case ID_FILE_SAVEASHTML:lResult=m_Menu.OnFileSaveAsHTML();break;
	case ID_FILE_PROPERTIES:lResult=m_Menu.OnFileProperties();break;
	case ID_FILE_IMPORTID666TAGS:lResult=m_Menu.OnFileImportID666Tags();break;
	case ID_FILE_EXPORTID666TAGS:lResult=m_Menu.OnFileExportID666Tags();break;
	case ID_FILE_EXIT:lResult=m_Menu.OnFileExit();break;
	case ID_EDIT_EDITITEM:lResult=m_Menu.OnEditEditItem();break;
	case ID_EDIT_CUT:lResult=m_Menu.OnEditCut();break;
	case ID_EDIT_COPY:lResult=m_Menu.OnEditCopy();break;
	case ID_EDIT_PASTE:lResult=m_Menu.OnEditPaste();break;
	case ID_EDIT_DELETE:lResult=m_Menu.OnEditDelete();break;
	case ID_EDIT_SELECT_ALL:lResult=m_Menu.OnEditSelectAll();break;
	case ID_EDIT_INVERT_SELECTION:lResult=m_Menu.OnEditInvertSelection();break;
	case ID_EDIT_FIND:lResult=m_Menu.OnEditFind();break;
	case ID_EDIT_GOTO:lResult=m_Menu.OnEditGoTo();break;
	case ID_VIEW_TITLEBAR:lResult=m_Menu.OnViewTitleBar();break;
	case ID_VIEW_MENU:lResult=m_Menu.OnViewMenu();break;
	case ID_VIEW_TOOLBARS_STANDARDBUTTONS:lResult=m_Menu.OnViewToolbarsStandardButtons();break;
	case ID_VIEW_TOOLBARS_SOURCEBAR:lResult=m_Menu.OnViewToolbarsSourceBar();break;
	case ID_VIEW_TOOLBARS_TEXTLABELS:lResult=m_Menu.OnViewToolbarsTextLabels();break;
	case ID_VIEW_TOOLBARS_PICTURES:lResult=m_Menu.OnViewToolbarsPictures();break;
	case ID_VIEW_TOOLBARS_WRAP:lResult=m_Menu.OnViewToolbarsWrap();break;
	case ID_VIEW_TOOLBARS_CUSTOMIZE:lResult=m_Menu.OnViewToolbarsCustomize();break;
	case ID_VIEW_STATUSBAR:lResult=m_Menu.OnViewStatusBar();break;
	case ID_VIEW_FILTER:lResult=m_Menu.OnViewFilter();break;
	case ID_VIEW_SHUFFLE:lResult=m_Menu.OnViewShuffle();break;
	case ID_VIEW_RESIZECOLUMNS:lResult=m_Menu.OnViewResizeColumns();break;
	case ID_VIEW_REFRESH:lResult=m_Menu.OnViewRefresh();break;
	case ID_OPTIONS_CONFIGURE:lResult=m_Menu.OnOptionsConfigure();break;
	case ID_OPTIONS_SAVESETTINGSONEXIT:lResult=m_Menu.OnOptionsSaveSettingsOnExit();break;
	case ID_OPTIONS_SAVESETTINGSNOW:lResult=m_Menu.OnOptionsSaveSettingsNow();break;
	case ID_OPTIONS_RESTOREDEFAULTSETTINGS:lResult=m_Menu.OnOptionsRestoreDefaultSettings();break;
	case ID_WINDOWS_FILELIST:lResult=m_Menu.OnWindowFileList();break;
	case ID_WINDOWS_PLAYLIST:lResult=m_Menu.OnWindowPlayList();break;
	case ID_WINDOWS_CONTROLPANEL:lResult=m_Menu.OnWindowControlPanel();break;
	case ID_WINDOWS_VISUALIZER:lResult=m_Menu.OnWindowVisualizer();break;
	case ID_WINDOWS_TILEHORIZONTALLY:lResult=m_Menu.OnWindowTileHorizontally();break;
	case ID_WINDOWS_TILEVERTICALLY:lResult=m_Menu.OnWindowTileVertically();break;
	case ID_HELP_CONTENTS:lResult=m_Menu.OnHelpContents();break;
	case ID_HELP_ABOUTSUPERJUKEBOX:lResult=m_Menu.OnHelpAboutSuperJukebox();break;	
	}
	return lResult;
}

BOOL CMainWindow::UpdateTitleBar()
{
	TCHAR szTitle[1000]={0};
	INT nPlayingIndex=app.m_Player.GetPlayingIndex();
	if(nPlayingIndex>=0)
	{
		CList *PlayingList=app.m_Player.GetPlayingList();
		nPlayingIndex=PlayingList->MapIndexToExternal(nPlayingIndex);
		PlayingList->GetListEntry(nPlayingIndex)->GetTitle(szTitle,sizeof(szTitle));
		if(!strlen(szTitle))
			PlayingList->GetListEntry(nPlayingIndex)->GetName(szTitle,sizeof(szTitle));
		strcat(szTitle," - ");
	}
	TCHAR szBuf[1000]={0};
	m_PlayList.GetName(szBuf,sizeof(szBuf));
	_splitpath(szBuf,NULL,NULL,szBuf,NULL);
	strcat(szTitle,szBuf);
	if(!m_PlayList.IsSaved())
		strcat(szTitle,"*");
	strcat(szTitle," - ");
	strcat(szTitle,app.m_szAppName);

	if((app.m_Reg.m_bMinimizeToSystemTray&&IsIconic(m_hWnd))||app.m_Reg.m_bShowInSystemTray)
	{
		NOTIFYICONDATA tnid;
		tnid.cbSize=sizeof(tnid);
		tnid.hWnd=m_hWnd;
		tnid.uID=IDI_SUPER_JUKEBOX;
		tnid.uFlags=NIF_TIP;
		strncpy(tnid.szTip,szTitle,sizeof(tnid.szTip));
		Shell_NotifyIcon(NIM_MODIFY,&tnid);
	}
	return SetWindowText(m_hWnd,szTitle);
}

LRESULT CMainWindow::OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr)
{
	LRESULT lResult=0;

	switch(lpNMHdr->idFrom)
	{
	case IDC_LIST_FILELIST:
		switch(lpNMHdr->code)
		{
		case NM_DBLCLK:lResult=m_FileList.OnDblClk((LPNMLISTVIEW)lpNMHdr);break;
		case NM_RETURN:lResult=m_FileList.OnReturn(lpNMHdr);break;
		case LVN_COLUMNCLICK:lResult=m_FileList.OnColumnClick((LPNMLISTVIEW)lpNMHdr);break;
		}
		break;
	case IDC_LIST_PLAYLIST:
		switch(lpNMHdr->code)
		{
		case NM_DBLCLK:lResult=m_PlayList.OnDblClk((LPNMLISTVIEW)lpNMHdr);break;
		case NM_RETURN:lResult=m_PlayList.OnReturn(lpNMHdr);break;
		case LVN_COLUMNCLICK:lResult=m_PlayList.OnColumnClick((LPNMLISTVIEW)lpNMHdr);break;
		}
		break;
	case IDC_TOOLBAR_PAGER:
		switch(lpNMHdr->code)
		{
		case PGN_CALCSIZE:
			LPNMPGCALCSIZE pCalcSize=(LPNMPGCALCSIZE)lpNMHdr;
			switch(pCalcSize->dwFlag)
			{
			case PGF_CALCWIDTH:
			case PGF_CALCHEIGHT:
				SIZE sz={0};
				if(!app.m_Reg.m_bWrapToolBar)
					SendMessage(m_ToolBar.m_hWnd,TB_GETMAXSIZE,0,(LPARAM)&sz);
				pCalcSize->iWidth=sz.cx;
				pCalcSize->iHeight=sz.cy;
				break;
			}
			break;
		}
		break;
	case IDC_REBAR:
		switch(lpNMHdr->code)
		{
		case RBN_HEIGHTCHANGE:
			AdjustLayout();
			break;
		case RBN_LAYOUTCHANGED:
			if(app.m_Reg.m_bShowToolBar)
				m_ToolBar.SaveReBarInfo();
			if(app.m_Reg.m_bShowLocationBar)
				m_SourceBar.SaveReBarInfo();
			break;
		}
		break;
	default:
		switch(lpNMHdr->code)
		{
		case TTN_GETDISPINFO:
			LPTOOLTIPTEXT lpttt=(LPTOOLTIPTEXT)lpNMHdr;
			lpttt->hinst=m_hInstance;
			lpttt->lpszText=MAKEINTRESOURCE(lpttt->hdr.idFrom);
			break;
		}
		break;
	}
	return lResult;
}

LRESULT CMainWindow::OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
	if((state&WA_ACTIVE)||(state&WA_CLICKACTIVE))
		SetFocus(m_hWndOldFocus);
	else
		m_hWndOldFocus=GetFocus();
	return 0;
}

LRESULT CMainWindow::OnTimer(HWND hwnd, UINT id)
{
	if(id==1)
	{
		if(app.m_Player.HasFailed())
		{
			m_ToolBar.OnClicked(IDC_BUTTON_STOP);
			return 0;
		}
		if(app.m_Player.IsPlaying())
		{
			m_StatusBar.Update();
			INT nPlayTime=app.m_Player.GetPlayTime();
			INT nTotalLength=app.m_Player.GetLength()*1000;
			if(nTotalLength>0)
				nTotalLength+=app.m_Player.GetFadeLength()*1000;
			if((nTotalLength>0&&nPlayTime>=nTotalLength)||(m_PlayList.IsDetectInactivity()&&!app.m_Player.IsEngineActive()))
			{
				BOOL b=app.IsCallInternal(m_PlayList.IsAutoAdvance()?TRUE:FALSE);
				m_ToolBar.OnClicked(m_PlayList.IsAutoAdvance()?IDC_BUTTON_NEXT:IDC_BUTTON_STOP);
				app.IsCallInternal(b);
			}
		}
	}
	else if(id==2)
	{
		if(IsIconic(m_hWnd))
			ShowWindow(m_hWnd,SW_RESTORE);
		SetForegroundWindow(m_hWnd);
		KillTimer(m_hWnd,2);
	}
	return 0;
}

LRESULT CMainWindow::OnInitMenu(HWND hwnd, HMENU hMenu)
{
	return m_Menu.OnInitMenu(hwnd,hMenu);
}

LRESULT CMainWindow::OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
{
	return m_Menu.OnMenuSelect(hwnd,hmenu,item,hmenuPopup,flags);
}

HWND CMainWindow::OnNextDlgCtl(HWND hwnd, HWND hwndSetFocus, BOOL fNext)
{
	if(fNext)
	{
		SetFocus(hwndSetFocus);
		SendMessage(m_hWnd,DM_SETDEFID,GetDlgCtrlID(hwndSetFocus),0);
	}
	return 0;
}

void CMainWindow::OnSysColorChange(HWND hwnd)
{
	SendMessage(m_ToolBar.m_hWnd,WM_SYSCOLORCHANGE,0,0);
}

void CMainWindow::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT pt={x,y};
	if(m_FileList.IsVisible()&&m_PlayList.IsVisible()&&PtInRect(&m_SplitterRect,pt))
		SetCursor(LoadCursor(NULL,app.m_Reg.m_bLayout?IDC_SIZENS:IDC_SIZEWE));
	if(m_bAdjustingListViews&&(app.m_Reg.m_bLayout?y!=m_nOldMouseY:x!=m_nOldMouseX))
	{
		m_nOldMouseX=x;m_nOldMouseY=y;
		INT nListPaneSeparator=app.m_Reg.m_bLayout?y-m_StartPoint.y:x-m_StartPoint.x;
		app.m_Reg.m_dListPaneRatio=app.m_Reg.m_bLayout?(double)nListPaneSeparator/m_ClientRect.bottom:(double)nListPaneSeparator/m_ClientRect.right;
		RECT FileListRect,PlayListRect;
		if(app.m_Reg.m_bLayout)
		{
			SetRect(&m_SplitterRect,m_ClientRect.left,nListPaneSeparator,m_ClientRect.right,nListPaneSeparator+4);
			SetRect(&FileListRect,m_ClientRect.left,m_ClientRect.top,m_ClientRect.right,m_SplitterRect.top);
			SetRect(&PlayListRect,m_ClientRect.left,m_SplitterRect.bottom,m_ClientRect.right,m_ClientRect.bottom);
		}
		else
		{
			SetRect(&m_SplitterRect,nListPaneSeparator,m_ClientRect.top,nListPaneSeparator+4,m_ClientRect.bottom);
			SetRect(&FileListRect,m_ClientRect.left,m_ClientRect.top,m_SplitterRect.left,m_ClientRect.bottom);
			SetRect(&PlayListRect,m_SplitterRect.right,m_ClientRect.top,m_ClientRect.right,m_ClientRect.bottom);
		}
		HDWP hDWP=BeginDeferWindowPos(2);
		DeferWindowPos(hDWP,m_FileList.m_hWnd,NULL,FileListRect.left,FileListRect.top,FileListRect.right-FileListRect.left,FileListRect.bottom-FileListRect.top,SWP_NOZORDER|(GetActiveSkin().IsActive()?SWP_NOREDRAW:0));
		DeferWindowPos(hDWP,m_PlayList.m_hWnd,NULL,PlayListRect.left,PlayListRect.top,PlayListRect.right-PlayListRect.left,PlayListRect.bottom-PlayListRect.top,SWP_NOZORDER|(GetActiveSkin().IsActive()?SWP_NOREDRAW:0));
		EndDeferWindowPos(hDWP);
		if(GetActiveSkin().IsActive())
			RePaint(&m_ClientRect);
	}
}

BOOL CMainWindow::OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	POINT pt;
	GetCursorPos(&pt);
	::ScreenToClient(m_hWnd,&pt);
	if((m_FileList.IsVisible()&&m_PlayList.IsVisible()&&PtInRect(&m_SplitterRect,pt))||m_bMaintainCursor)
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}
	return CWnd::OnSetCursor(hwnd,hwndCursor,codeHitTest,msg);
}

void CMainWindow::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	POINT pt={x,y};
	if(m_FileList.IsVisible()&&m_PlayList.IsVisible()&&PtInRect(&m_SplitterRect,pt))
	{
		SetCapture(m_hWnd);
		RECT rc=m_ClientRect;
		if(app.m_Reg.m_bLayout)
		{
			rc.top+=50;rc.bottom-=50;
		}
		else
		{
			rc.left+=50;rc.right-=50;
		}
		ClientToScreen(m_hWnd,&rc);
		ClipCursor(&rc);
		m_StartPoint.x=x-m_SplitterRect.left;
		m_StartPoint.y=y-m_SplitterRect.top;
		m_bAdjustingListViews=TRUE;
	}
}

void CMainWindow::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(m_bAdjustingListViews)
	{
		ReleaseCapture();
		ClipCursor(NULL);
		m_bAdjustingListViews=FALSE;
	}
}

void CMainWindow::OnClose(HWND hwnd)
{
	if(!m_PlayList.IsSaved())
	{
		TCHAR szBuf[500],szPlayListName[MAX_PATH];
		m_PlayList.GetName(szPlayListName,MAX_PATH);
		sprintf(szBuf,"Save changes to \"%s\"?",szPlayListName);
		switch(MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONQUESTION|MB_YESNOCANCEL))
		{
		case IDYES:if(m_Menu.OnFileSave())return;break;
		case IDCANCEL:return;
		}
	}
	
	DestroyWindow(m_hWnd);
}

BOOL CMainWindow::AdjustLayout()
{
	if(!IsVisible())
		return FALSE;

	GetClientRect(m_hWnd,&m_ClientRect);
	if(m_ClientRect.right-m_ClientRect.left<=0||m_ClientRect.bottom-m_ClientRect.top<=0)
		return FALSE;

	HDWP hDWP=BeginDeferWindowPos(6);
	UINT uFlags=SWP_NOZORDER;
	RECT rc;
	if(m_ReBar.IsVisible())
	{
		GetWindowRect(m_ReBar.m_hWnd,&rc);
		SetRect(&rc,0,0,m_ClientRect.right,rc.bottom-rc.top);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_ReBar.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
		m_ClientRect.top=rc.bottom;
	}
	if(m_StatusBar.IsVisible())
	{
		GetWindowRect(m_StatusBar.m_hWnd,&rc);
		SetRect(&rc,0,m_ClientRect.bottom-(rc.bottom-rc.top),m_ClientRect.right,m_ClientRect.bottom);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_StatusBar.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
		m_ClientRect.bottom=rc.top;
	}
	if(m_VisWindow.IsActive())
	{
		SetRect(&rc,0,m_ClientRect.bottom-VIS_HEIGHT+(app.m_Reg.m_bShowVisLabels?0:14),m_ClientRect.right,m_ClientRect.bottom);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_VisWindow.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
		m_ClientRect.bottom=rc.top;
	}
	if(m_ControlPanel.IsVisible())
	{
		GetWindowRect(m_ControlPanel.m_hWnd,&rc);
		SetRect(&rc,0,m_ClientRect.bottom-(rc.bottom-rc.top),m_ClientRect.right,m_ClientRect.bottom);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_ControlPanel.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
		m_ClientRect.bottom=rc.top;
	}

	if(m_ClientRect.right-m_ClientRect.left<0)m_ClientRect.right=m_ClientRect.left;
	if(m_ClientRect.bottom-m_ClientRect.top<0)m_ClientRect.bottom=m_ClientRect.top;

	if(!m_ActiveWallpaper.IsRunning()&&!GetActiveSkin().Refresh())
		return FALSE;
	if(app.m_Reg.m_bUseActiveWallpaper)
		m_ActiveWallpaper.SetParameters();

	INT nListPaneSeparator=app.m_Reg.m_bLayout?m_ClientRect.bottom:m_ClientRect.right;
	nListPaneSeparator*=app.m_Reg.m_dListPaneRatio;
	
	if(app.m_Reg.m_bLayout)
		SetRect(&m_SplitterRect,m_ClientRect.left,nListPaneSeparator,m_ClientRect.right,nListPaneSeparator+4);
	else
		SetRect(&m_SplitterRect,nListPaneSeparator,m_ClientRect.top,nListPaneSeparator+4,m_ClientRect.bottom);

	if(m_FileList.IsVisible())
	{
		if(app.m_Reg.m_bLayout)
			SetRect(&rc,m_ClientRect.left,m_ClientRect.top,m_ClientRect.right,m_PlayList.IsVisible()?m_SplitterRect.top:m_ClientRect.bottom);
		else
			SetRect(&rc,m_ClientRect.left,m_ClientRect.top,m_PlayList.IsVisible()?m_SplitterRect.left:m_ClientRect.right,m_ClientRect.bottom);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_FileList.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
	}
	if(m_PlayList.IsVisible())
	{
		if(app.m_Reg.m_bLayout)
			SetRect(&rc,m_ClientRect.left,m_FileList.IsVisible()?m_SplitterRect.bottom:m_ClientRect.top,m_ClientRect.right,m_ClientRect.bottom);
		else
			SetRect(&rc,m_FileList.IsVisible()?m_SplitterRect.right:m_ClientRect.left,m_ClientRect.top,m_ClientRect.right,m_ClientRect.bottom);
		if(rc.right-rc.left<0)rc.right=rc.left;
		if(rc.bottom-rc.top<0)rc.bottom=rc.top;
		hDWP=DeferWindowPos(hDWP,m_PlayList.m_hWnd,NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,uFlags);
	}

	EndDeferWindowPos(hDWP);
	RedrawWindow(m_hWnd,NULL,NULL,RDW_ALLCHILDREN|RDW_UPDATENOW);
	return TRUE;
}

BOOL CMainWindow::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
	if(m_ActiveWallpaper.IsRunning())
		return TRUE;
	else if(GetActiveSkin().IsActive()&&!m_FileList.IsVisible()&&!m_PlayList.IsVisible())
	{
		BitBlt(hdc,m_ClientRect.left,m_ClientRect.top,m_ClientRect.right-m_ClientRect.left,m_ClientRect.bottom-m_ClientRect.top,GetActiveSkin().GetDC(),0,0,SRCCOPY);
		return TRUE;
	}
	return CWnd::OnEraseBkgnd(hwnd,hdc);
}

CSkin& CMainWindow::GetActiveSkin()
{
	if(!app.m_Reg.m_bAlwaysUseProgramSkin)
	{
		if(m_PlayList.m_Skin.IsActive())
			return m_PlayList.m_Skin;
	}
	return m_Skin;
}

void CMainWindow::MaintainCursor(BOOL bFlag)
{
	m_bMaintainCursor=bFlag;
}

void CMainWindow::SetCursor(HCURSOR hCursor)
{
	m_hCursor=hCursor;
	if(!m_hCursor)
		m_hCursor=(HCURSOR)GetClassLong(app.m_MainWindow.m_hWnd,GCL_HCURSOR);
	::SetCursor(m_hCursor);
}

BOOL CMainWindow::AddSystemTrayIcon()
{
	NOTIFYICONDATA tnid;

	tnid.cbSize=sizeof(tnid);
	tnid.hWnd=m_hWnd;
	tnid.uID=IDI_SUPER_JUKEBOX;
	tnid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnid.uCallbackMessage=WM_NOTIFYICON;
	tnid.hIcon=(HICON)GetClassLong(m_hWnd,GCL_HICON);
	GetWindowText(m_hWnd,tnid.szTip,sizeof(tnid.szTip));

	return Shell_NotifyIcon(NIM_ADD,&tnid);
}

BOOL CMainWindow::RemoveSystemTrayIcon()
{
	NOTIFYICONDATA tnid;

	tnid.cbSize=sizeof(tnid);
	tnid.hWnd=m_hWnd;
	tnid.uID=IDI_SUPER_JUKEBOX;

	return Shell_NotifyIcon(NIM_DELETE,&tnid);
}

void CMainWindow::OnNotifyIcon(UINT uID, UINT uMouseMsg)
{
	if(m_bBetaBox)
		return;

	switch(uMouseMsg)
	{
	case WM_LBUTTONDOWN:
		switch(uID)
		{
		case IDI_SUPER_JUKEBOX:
			m_bPass=TRUE;
			break;
		}
		break;
	case WM_LBUTTONUP:
		switch(uID)
		{
		case IDI_SUPER_JUKEBOX:
			if(m_bPass)
			{
				SetTimer(m_hWnd,2,GetDoubleClickTime(),NULL);
				m_bPass=FALSE;
			}
			break;
		}
		break;
	case WM_LBUTTONDBLCLK:
		switch(uID)
		{
		case IDI_SUPER_JUKEBOX:
			KillTimer(m_hWnd,2);
			m_bPass=FALSE;
			switch(app.m_Reg.m_dwSysTrayCmd)
			{
			case 0:
				m_ToolBar.OnClicked(IDC_BUTTON_PLAY);
				break;
			case 1:
				m_ToolBar.OnClicked(IDC_BUTTON_PAUSE);
				break;
			case 2:
				m_ToolBar.OnClicked(IDC_BUTTON_STOP);
				break;
			case 3:
				m_ToolBar.OnClicked(IDC_BUTTON_PREVIOUS);
				break;
			case 4:
				m_ToolBar.OnClicked(IDC_BUTTON_NEXT);
				break;
			case 5:
				SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_OPTIONS_CONFIGURE,0),(LPARAM)m_hWnd);
				break;
			case 6:
				SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_FILE_EXIT,0),(LPARAM)m_hWnd);
				break;
			}
			break;
		}
		break;
	case WM_RBUTTONUP:
		switch(uID)
		{
		case IDI_SUPER_JUKEBOX:
			POINT pt;
			GetCursorPos(&pt);
			DoSystemTrayMenu(pt.x,pt.y);
			break;
		}
		break;
	}
}

BOOL CMainWindow::DoSystemTrayMenu(INT nXPos, INT nYPos)
{
	HMENU hMenu=CreatePopupMenu();
	if(!hMenu)
		return FALSE;
	AppendMenu(hMenu,MF_STRING,1,"&Play");
	AppendMenu(hMenu,MF_STRING,2,"P&ause");
	AppendMenu(hMenu,MF_STRING,3,"&Stop");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,4,"P&revious");
	AppendMenu(hMenu,MF_STRING,5,"&Next");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,6,"&Configure...");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,7,"Show &Window");
	AppendMenu(hMenu,MF_STRING,8,"Remove &Icon");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,9,"E&xit");

	INT nSelection=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|TPM_RIGHTBUTTON,nXPos,nYPos,0,m_hWnd,NULL);
	DestroyMenu(hMenu);

	switch(nSelection)
	{
	case 1:
		m_ToolBar.OnClicked(IDC_BUTTON_PLAY);
		break;
	case 2:
		m_ToolBar.OnClicked(IDC_BUTTON_PAUSE);
		break;
	case 3:
		m_ToolBar.OnClicked(IDC_BUTTON_STOP);
		break;
	case 4:
		m_ToolBar.OnClicked(IDC_BUTTON_PREVIOUS);
		break;
	case 5:
		m_ToolBar.OnClicked(IDC_BUTTON_NEXT);
		break;
	case 6:
		if(IsIconic(m_hWnd))
			ShowWindow(m_hWnd,SW_RESTORE);
		SetForegroundWindow(m_hWnd);
		SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_OPTIONS_CONFIGURE,0),(LPARAM)m_hWnd);
		break;
	case 7:
		if(IsIconic(m_hWnd))
			ShowWindow(m_hWnd,SW_RESTORE);
		SetForegroundWindow(m_hWnd);
		break;
	case 8:
		app.m_Reg.m_bShowInSystemTray=FALSE;
		RemoveSystemTrayIcon();
		break;
	case 9:
		if(!m_PlayList.IsSaved()&&IsIconic(m_hWnd))
		{
			ShowWindow(m_hWnd,SW_RESTORE);
			SetForegroundWindow(m_hWnd);
		}
		SendMessage(m_hWnd,WM_CLOSE,0,0);
		break;
	}
	return TRUE;
}

void CMainWindow::AddTaskbarButton()
{
	BOOL bShow=FALSE;
	if(IsVisible())
	{
		ShowWindow(m_hWnd,SW_HIDE);
		bShow=TRUE;
	}
	SetWindowLong(m_hWnd,GWL_EXSTYLE,(GetWindowLong(m_hWnd,GWL_EXSTYLE)|WS_EX_APPWINDOW)&~WS_EX_TOOLWINDOW);
	SetWindowPos(m_hWnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME|SWP_FRAMECHANGED);
	if(bShow)
		ShowWindow(m_hWnd,SW_SHOW);
	AdjustLayout();
}

void CMainWindow::RemoveTaskbarButton()
{
	BOOL bShow=FALSE;
	if(IsVisible())
	{
		ShowWindow(m_hWnd,SW_HIDE);
		bShow=TRUE;
	}
	SetWindowLong(m_hWnd,GWL_EXSTYLE,(GetWindowLong(m_hWnd,GWL_EXSTYLE)|WS_EX_TOOLWINDOW)&~WS_EX_APPWINDOW);
	SetWindowPos(m_hWnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME|SWP_FRAMECHANGED);
	if(bShow)
		ShowWindow(m_hWnd,SW_SHOW);
	AdjustLayout();
}

void CMainWindow::SyncRegistrySettings()
{
	GetWindowPlacement(m_hWnd,&app.m_Reg.m_wp);
	app.m_Reg.m_bUseSkin=m_Skin.IsActive();
	app.m_Reg.m_dwSkinLayout=m_Skin.GetLayout();
	strncpy(app.m_Reg.m_szSkinFileName,m_Skin.GetFileName(),sizeof(app.m_Reg.m_szSkinFileName));
	app.m_Reg.m_bBlendSkin=m_Skin.IsBlended();
	app.m_Reg.m_crBlendingColor=m_Skin.GetBlendingColor();
	app.m_Reg.m_crTextColor=m_Skin.GetTextColor();
	app.m_Reg.m_dwBlendingLevel=m_Skin.GetBlendingLevel();	
	m_FileList.GetSource(app.m_Reg.m_szLastDirectory,sizeof(app.m_Reg.m_szLastDirectory));
	if(m_PlayList.Exists())
		m_PlayList.GetName(app.m_Reg.m_szLastPlaylist,sizeof(app.m_Reg.m_szLastPlaylist));
	else
		memset(&app.m_Reg.m_szLastPlaylist,0,sizeof(app.m_Reg.m_szLastPlaylist));
	m_FileList.SaveHeaderInfo();
	m_FileList.SaveRegistrySettings();
	m_PlayList.SaveHeaderInfo();
	m_PlayList.SaveRegistrySettings();
}

void CMainWindow::OnSysCommand(HWND hwnd, UINT cmd, int x, int y)
{
	switch(cmd)
	{
	case IDM_ALWAYSONTOP:
		app.m_Reg.m_bAlwaysOnTop^=1;
		CheckMenuItem(m_hSysMenu,IDM_ALWAYSONTOP,app.m_Reg.m_bAlwaysOnTop?MF_CHECKED:MF_UNCHECKED);
		SetWindowPos(m_hWnd,app.m_Reg.m_bAlwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		break;
	}
	CWnd::OnSysCommand(hwnd,cmd,x,y);
}

void CMainWindow::OnDropFiles(HWND hwnd, HDROP hdrop)
{
	UINT uFileCount=DragQueryFile(hdrop,0xFFFFFFFF,NULL,0);

	if(uFileCount>0)
	{
		m_PlayList.ClearFilesToAdd();
		for(UINT i=0;i<uFileCount;i++)
		{
			TCHAR szFileName[MAX_PATH];
			DragQueryFile(hdrop,i,szFileName,sizeof(szFileName));
			m_PlayList.AddFileToAdd(szFileName);
		}
		DragFinish(hdrop);
		m_PlayList.AddFiles(FALSE,FALSE);
	}
	else
		DragFinish(hdrop);
}

void CMainWindow::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT pt={x,y};
	if(PtInRect(&m_ClientRect,pt))
	{
		HMENU hMenu=CreatePopupMenu();
		if(!hMenu)
			return;
		if(m_ActiveWallpaper.IsRunning())
		{
			AppendMenu(hMenu,MF_STRING,1,"&Disable");
			AppendMenu(hMenu,MF_STRING,2,"&Configure...");
		}
		else
		{
			AppendMenu(hMenu,MF_STRING,3,"&File List");
			CheckMenuItem(hMenu,3,app.m_Reg.m_bShowFileList?MF_CHECKED:MF_UNCHECKED);
			AppendMenu(hMenu,MF_STRING,4,"&Play List");
			CheckMenuItem(hMenu,4,app.m_Reg.m_bShowPlayList?MF_CHECKED:MF_UNCHECKED);
			AppendMenu(hMenu,MF_STRING,5,"&Control Panel");
			CheckMenuItem(hMenu,5,app.m_Reg.m_bShowControlPanel?MF_CHECKED:MF_UNCHECKED);
			AppendMenu(hMenu,MF_STRING,6,"&Visualizer");
			CheckMenuItem(hMenu,6,app.m_Reg.m_bShowVisWindow?MF_CHECKED:MF_UNCHECKED);
		}

		::ClientToScreen(m_hWnd,&pt);
		INT nSelection=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,0,m_hWnd,NULL);
		DestroyMenu(hMenu);

		CConfigurationDialog dlg;
		switch(nSelection)
		{
		case 1:
			app.m_Reg.m_bUseActiveWallpaper=FALSE;
			m_ToolBar.CheckButton(IDC_BUTTON_AW,app.m_Reg.m_bUseActiveWallpaper);
			m_ToolBar.OnClicked(IDC_BUTTON_AW);
			break;
		case 2:
			dlg.SetInitialSelection(5);
			if(dlg.DoModal(IDD_CONFIGURATION,this,m_hInstance)==-1)
				DisplayError(m_hWnd,FALSE,NULL);
			break;
		case 3:
			SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_FILELIST,0),(LPARAM)m_hWnd);
			break;
		case 4:
			SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_PLAYLIST,0),(LPARAM)m_hWnd);
			break;
		case 5:
			SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_CONTROLPANEL,0),(LPARAM)m_hWnd);
			break;
		case 6:
			SendMessage(m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_VISUALIZER,0),(LPARAM)m_hWnd);
			break;
		}
	}
}

BOOL CMainWindow::OnQueryEndSession(HWND hwnd)
{
	if(!m_PlayList.IsSaved())
	{
		TCHAR szBuf[500],szPlayListName[MAX_PATH];
		m_PlayList.GetName(szPlayListName,MAX_PATH);
		sprintf(szBuf,"Save changes to \"%s\"?",szPlayListName);
		switch(MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONQUESTION|MB_YESNOCANCEL))
		{
		case IDYES:if(m_Menu.OnFileSave())return FALSE;break;
		case IDCANCEL:return FALSE;break;
		}
	}
	return CWnd::OnQueryEndSession(hwnd);
}
