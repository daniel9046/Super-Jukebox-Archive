// ToolBar.cpp: implementation of the CToolBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolBar.h"
#include "Resource.h"
#include "AfxRes.h"
#include "Main.h"
#include "Extractor.h"
#include "ToolbarCustomizeDialog.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolBar::CToolBar()
{
	m_SongList=NULL;
	m_bHasPictures=m_bHasTextLabels=TRUE;
	m_hImlColorIcons=m_hImlGrayIcons=NULL;
	m_uOldRowCount=0;

	LPCTSTR lpszButtonNames[]={"Separator","New","Open","Save","Save As","Import Tags","Export Tags","WAV Dump","Add","Remove","Add All","Remove All","Play","Pause","Stop","Previous","Next","Edit Playlist","Find","Edit Filter","Filter View","Shuffle","Resize Columns","AWP","File List","Play List","CPL","Visualizer","Layout","Vis Mode","Refresh","Configure","Info","Help"};
	INT nButtonIDs[]={0,ID_FILE_NEW,ID_FILE_OPEN,ID_FILE_SAVE,ID_FILE_SAVE_AS,ID_FILE_IMPORTID666TAGS,ID_FILE_EXPORTID666TAGS,IDC_BUTTON_WAVDUMP,IDC_BUTTON_ADD,IDC_BUTTON_REMOVE,IDC_BUTTON_ADDALL,IDC_BUTTON_REMOVEALL,IDC_BUTTON_PLAY,IDC_BUTTON_PAUSE,IDC_BUTTON_STOP,IDC_BUTTON_PREVIOUS,IDC_BUTTON_NEXT,ID_EDIT_EDITITEM,ID_EDIT_FIND,ID_VIEW_FILTER,IDC_BUTTON_FILTERVIEW,ID_VIEW_SHUFFLE,ID_VIEW_RESIZECOLUMNS,IDC_BUTTON_AW,ID_WINDOWS_FILELIST,ID_WINDOWS_PLAYLIST,ID_WINDOWS_CONTROLPANEL,ID_WINDOWS_VISUALIZER,IDC_BUTTON_LAYOUT,IDC_BUTTON_VISMODE,ID_VIEW_REFRESH,ID_OPTIONS_CONFIGURE,ID_HELP_ABOUTSUPERJUKEBOX,ID_HELP_CONTENTS};
	INT iBitmaps[]={-1,7,8,13,31,19,23,22,0,12,14,17,10,9,16,1,4,26,3,24,2,18,20,21,27,30,25,32,28,29,11,15,6,5};
	BYTE bStyles[]={TBSTYLE_SEP,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON};
	for(INT i=0;i<NUMBER_OF_BUTTONS;i++)
	{
		strncpy(m_Buttons[i].szName,lpszButtonNames[i],sizeof(m_Buttons[i].szName));
		m_Buttons[i].idCommand=nButtonIDs[i];
		m_Buttons[i].iBitmap=iBitmaps[i];
		m_Buttons[i].iString=i;
		m_Buttons[i].bStyle=bStyles[i];
	}
}

CToolBar::~CToolBar()
{
	Destroy();
}

BOOL CToolBar::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,TOOLBARCLASSNAME,&wcx))
		return FALSE;
	m_OriginalWndProc=wcx.lpfnWndProc;
	if(!GetClassInfoEx(hInstance,"MyToolBar",&wcx))
	{
		wcx.lpszClassName="MyToolBar";
		wcx.lpfnWndProc=CWnd::WindowProc;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

BOOL CToolBar::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(IsWindow(m_hWnd))	
		return FALSE;

	RECT rc={0,0,0,0};
	if(!m_Pager.Create(WC_PAGESCROLLER,NULL,WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|PGS_HORZ,0,rc,pParentWnd,IDC_TOOLBAR_PAGER,hInstance))
		return FALSE;

	if(!RegisterClass(hInstance))
		return FALSE;
	if(!CWnd::Create("MyToolBar",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|TBSTYLE_TRANSPARENT|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|TBSTYLE_WRAPABLE|CCS_NODIVIDER|CCS_TOP|CCS_NORESIZE|CCS_NOPARENTALIGN,WS_EX_TOOLWINDOW,rc,&m_Pager,uID,hInstance))
		return FALSE;

	return TRUE;
}

BOOL CToolBar::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	if(!CWnd::OnCreate(hwnd,lpCreateStruct))
		return FALSE;

	Pager_SetChild(m_Pager.m_hWnd,m_hWnd);
	SendMessage(m_hWnd,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);

	DisplayToolTips(app.m_Reg.m_bDisplayToolTips);

	m_hImlGrayIcons=ImageList_Create(24,24,ILC_COLOR8|ILC_MASK,NUMBER_OF_BUTTONS-1,0);
	if(!m_hImlGrayIcons)
		return FALSE;
	HBITMAP hBmpGrayIcons=LoadBitmap(m_hInstance,MAKEINTRESOURCE(IDB_GRAY_ICONS));
	if(!hBmpGrayIcons)
		return FALSE;
	ImageList_AddMasked(m_hImlGrayIcons,hBmpGrayIcons,CLR_DEFAULT);
	DeleteObject(hBmpGrayIcons);
	m_hImlColorIcons=ImageList_Create(24,24,ILC_COLOR8|ILC_MASK,NUMBER_OF_BUTTONS-1,0);
	if(!m_hImlColorIcons)
		return FALSE;
	HBITMAP hBmpColorIcons=LoadBitmap(m_hInstance,MAKEINTRESOURCE(IDB_COLOR_ICONS));
	if(!hBmpColorIcons)
		return FALSE;
	ImageList_AddMasked(m_hImlColorIcons,hBmpColorIcons,CLR_DEFAULT);
	DeleteObject(hBmpColorIcons);

	if(!GenerateButtons())
		return FALSE;

	if(!AddReBarBand())
		return FALSE;
	app.m_MainWindow.m_ReBar.ShowBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),app.m_Reg.m_bShowToolBar);

	return TRUE;
}

LRESULT CToolBar::OnDestroy(HWND hwnd)
{
	if(m_hImlGrayIcons)ImageList_Destroy(m_hImlGrayIcons);
	if(m_hImlColorIcons)ImageList_Destroy(m_hImlColorIcons);
	return CWnd::OnDestroy(hwnd);
}

BOOL CToolBar::GenerateButtons()
{
	if(!IsWindow(m_hWnd))
		return FALSE;

	if(m_bHasPictures)
	{
		SendMessage(m_hWnd,TB_SETIMAGELIST,0,(LPARAM)m_hImlGrayIcons);
		SendMessage(m_hWnd,TB_SETHOTIMAGELIST,0,(LPARAM)m_hImlColorIcons);
	}
	else
	{
		SendMessage(m_hWnd,TB_SETIMAGELIST,0,NULL);
		SendMessage(m_hWnd,TB_SETHOTIMAGELIST,0,NULL);
	}

	UINT uButtonCount=SendMessage(m_hWnd,TB_BUTTONCOUNT,0,0);
	for(INT i=uButtonCount-1;i>=0;i--)	
		SendMessage(m_hWnd,TB_DELETEBUTTON,i,0);
	for(i=0;i<NUMBER_OF_BUTTONS;i++)
		SendMessage(m_hWnd,TB_ADDSTRING,0,(LPARAM)m_Buttons[i].szName);

	UINT uNumButtons=app.m_Reg.m_ToolBarButtons.size();
	TBBUTTON *Button=new TBBUTTON[uNumButtons];
	memset(Button,0,uNumButtons*sizeof(Button[0]));
	for(i=0;i<uNumButtons;i++)
	{		
		INT nOrder=app.m_Reg.m_ToolBarButtons[i];
		Button[i].idCommand=m_Buttons[nOrder].idCommand;
		Button[i].fsState=TBSTATE_ENABLED;
		Button[i].fsStyle=m_Buttons[nOrder].bStyle;
		if(m_bHasPictures)
			Button[i].iBitmap=m_Buttons[nOrder].iBitmap;
		if(m_bHasTextLabels)
			Button[i].iString=m_Buttons[nOrder].iString;
		else
			Button[i].iString=-1;
	}
	SendMessage(m_hWnd,TB_ADDBUTTONS,uNumButtons,(LPARAM)Button);
	delete[]Button;
	SendMessage(m_hWnd,TB_AUTOSIZE,0,0);

	CheckButton(IDC_BUTTON_WAVDUMP,app.m_Reg.m_bOutputToWAV);
	CheckButton(IDC_BUTTON_FILTERVIEW,app.m_Reg.m_bFilterActive);
	CheckButton(IDC_BUTTON_AW,app.m_Reg.m_bUseActiveWallpaper);
	EnableButton(IDC_BUTTON_AW,stricmp(app.m_Reg.m_szActiveWallpaperPlugin,"(none)"));	
	CheckButton(ID_WINDOWS_FILELIST,app.m_Reg.m_bShowFileList);
	CheckButton(ID_WINDOWS_PLAYLIST,app.m_Reg.m_bShowPlayList);
	CheckButton(ID_WINDOWS_CONTROLPANEL,app.m_Reg.m_bShowControlPanel);
	CheckButton(ID_WINDOWS_VISUALIZER,app.m_Reg.m_bShowVisWindow);

	Pager_RecalcSize(m_Pager.m_hWnd);

	REBARBANDINFO rbBand;
	rbBand.cbSize=sizeof(rbBand);
	rbBand.fMask=RBBIM_CHILDSIZE;
	rbBand.cxMinChild=0;
	if(app.m_Reg.m_bWrapToolBar&&uNumButtons)
	{
		RECT rc;
		SendMessage(m_hWnd,TB_GETITEMRECT,uNumButtons-1,(LPARAM)&rc);
		rbBand.cyMinChild=rc.bottom;
	}
	else
	{
		DWORD dwBtnSize=SendMessage(m_hWnd,TB_GETBUTTONSIZE,0,0);
		rbBand.cyMinChild=HIWORD(dwBtnSize);
	}

	app.m_MainWindow.m_ReBar.SetBandInfo(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),&rbBand);

	m_uOldRowCount=SendMessage(m_hWnd,TB_GETROWS,0,0);
	
	return TRUE;
}

LRESULT CToolBar::OnClicked(UINT uID)
{
	LRESULT lResult=1;

	switch(uID)
	{
	case IDC_BUTTON_ADD:lResult=OnAdd();break;
	case IDC_BUTTON_REMOVE:lResult=OnRemove();break;
	case IDC_BUTTON_ADDALL:lResult=OnAddAll();break;
	case IDC_BUTTON_REMOVEALL:lResult=OnRemoveAll();break;
	case IDC_BUTTON_PLAY:lResult=OnPlay();break;
	case IDC_BUTTON_PAUSE:lResult=OnPause();break;
	case IDC_BUTTON_STOP:lResult=OnStop();break;
	case IDC_BUTTON_PREVIOUS:lResult=OnPrevious();break;
	case IDC_BUTTON_NEXT:lResult=OnNext();break;
	case IDC_BUTTON_WAVDUMP:lResult=OnWAVDump();break;
	case IDC_BUTTON_AW:lResult=OnAW();break;
	case IDC_BUTTON_FILTERVIEW:lResult=OnFilterView();break;
	case IDC_BUTTON_LAYOUT:lResult=OnLayout();break;
	case IDC_BUTTON_VISMODE:lResult=OnVisMode();break;
	}
	return lResult;
}

LRESULT CToolBar::OnAdd()
{
	app.m_MainWindow.m_PlayList.AddFiles(TRUE,FALSE);
	return 0;
}

LRESULT CToolBar::OnRemove()
{
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
	INT nCount=app.m_MainWindow.m_PlayList.GetDisplayedCount();
	INT nIndex=app.m_MainWindow.m_PlayList.GetSelectedEntry();

	app.m_MainWindow.m_PlayList.SetRedraw(FALSE);
	for(INT i=nCount-1;i>=0;i--)
		if(app.m_MainWindow.m_PlayList.GetEntryState(i)&LVIS_SELECTED)
			app.m_MainWindow.m_PlayList.RemoveEntry(i);
	if(app.m_Reg.m_bAutoSizeColumns)
		app.m_MainWindow.m_PlayList.ResizeColumns();
	app.m_MainWindow.m_PlayList.SetRedraw(TRUE);
	InvalidateRect(app.m_MainWindow.m_PlayList.m_hWnd,NULL,TRUE);
	if(nIndex>=app.m_MainWindow.m_PlayList.GetDisplayedCount())
		nIndex=app.m_MainWindow.m_PlayList.GetDisplayedCount()-1;
	app.m_MainWindow.m_PlayList.SelectEntry(nIndex);
	app.m_MainWindow.UpdateTitleBar();
	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	return 0;
}

LRESULT CToolBar::OnAddAll()
{
	app.m_MainWindow.m_PlayList.AddFiles(TRUE,TRUE);
	return 0;
}

LRESULT CToolBar::OnRemoveAll()
{
	app.m_MainWindow.m_PlayList.Clear();
	if(app.m_Reg.m_bAutoSizeColumns)
		app.m_MainWindow.m_PlayList.ResizeColumns();
	app.m_MainWindow.UpdateTitleBar();
	return 0;
}

LRESULT CToolBar::OnPlay()
{
	if(!m_SongList)
		return 0;

	INT nIndex=m_SongList->GetFocusedEntry();
	if(nIndex<0)
		nIndex=m_SongList->GetSelectedEntry();
	
	if(app.m_Player.IsPaused())
	{
		app.m_Player.UnPauseSong();
	}
	else if(nIndex>=0)
	{
		CListEntry* Entry=m_SongList->GetListEntry(nIndex);
		if(Entry->GetType()!=LIST_ID_SPC&&m_SongList==&app.m_MainWindow.m_FileList)
		{
			INT nIndex=app.m_MainWindow.m_PlayList.GetFocusedEntry();
			if(nIndex<0)
				nIndex=app.m_MainWindow.m_PlayList.GetSelectedEntry();
			if(nIndex<0)
				return FALSE;
			Entry=app.m_MainWindow.m_PlayList.GetListEntry(nIndex);
			if(Entry->GetType()==LIST_ID_SPC)
				m_SongList=&app.m_MainWindow.m_PlayList;
		}
		if(Entry->GetType()==LIST_ID_SPC)
		{
			if(app.m_Player.IsPlaying())
			{
				BOOL b=app.IsCallInternal(TRUE);
				OnStop();
				app.IsCallInternal(b);
			}

			if(app.m_Reg.m_bOutputToWAV)
			{
				TCHAR szWAVFileName[MAX_PATH]={0},szName[MAX_PATH];
				Entry->GetFullName(szName,sizeof(szName));
				_splitpath(szName,NULL,NULL,szName,NULL);
				strncpy(szWAVFileName,app.m_Reg.m_szWAVOutputDirectory,sizeof(szWAVFileName));
				if(strlen(szWAVFileName)&&szWAVFileName[strlen(szWAVFileName)-1]!='\\')
					strcat(szWAVFileName,"\\");
				strcat(szWAVFileName,szName);
				strcat(szWAVFileName,".wav");
				app.m_Player.SetWAVOutputFileName(szWAVFileName,app.m_Reg.m_bOutputWithSound);
			}
			else
				app.m_Player.SetWAVOutputFileName(NULL,app.m_Reg.m_bOutputWithSound);

			if(!app.m_Player.PlaySong(Entry))
			{
				if(app.m_MainWindow.m_FileList.IsVisible()!=app.m_Reg.m_bShowFileList)
					app.m_MainWindow.m_FileList.IsVisible(app.m_Reg.m_bShowFileList);
				if(app.m_MainWindow.m_PlayList.IsVisible()!=app.m_Reg.m_bShowPlayList)
					app.m_MainWindow.m_PlayList.IsVisible(app.m_Reg.m_bShowPlayList);
				app.m_MainWindow.AdjustLayout();
				app.m_MainWindow.RePaint(NULL);

				TCHAR szName[MAX_PATH];
				Entry->GetFullName(szName,sizeof(szName));
				TCHAR szBuf[MAX_PATH+100];
				sprintf(szBuf,"Failed to load and/or play \"%s\".",szName);
				DisplayError(app.m_MainWindow.m_hWnd,FALSE,szBuf);
				return 0;
			}
		}
		else
		{
			DisplayError(m_hWnd,FALSE,"Current active list selection is not an SPC.");
			return FALSE;
		}

		if(app.m_Player.IsPlaying())
		{
			DWORD dwOldTime=GetTickCount();
			while(!app.m_Player.IsEnginePlaying()&&GetTickCount()<dwOldTime+2000)
				Sleep(10);
			if(!IsIconic(app.m_MainWindow.m_hWnd)&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
			{
				if(app.m_Reg.m_bUseActiveWallpaper)
					app.m_MainWindow.m_ActiveWallpaper.Start();
				if(app.m_Reg.m_bShowVisWindow)
					app.m_MainWindow.m_VisWindow.Start();
			}
		}
		app.m_MainWindow.UpdateTitleBar();
		app.m_MainWindow.m_StatusBar.Update();
	}
	return 0;
}

LRESULT CToolBar::OnPause()
{
	if(!app.m_Player.IsStopped())
	{
		if(app.m_Player.IsPaused())
			app.m_Player.UnPauseSong();
		else
			app.m_Player.PauseSong();		
		app.m_MainWindow.m_StatusBar.Update();
		app.m_MainWindow.UpdateTitleBar();
	}
	return 0;
}

LRESULT CToolBar::OnStop()
{
	if(!app.m_Player.IsStopped())
	{
		if(!IsIconic(app.m_MainWindow.m_hWnd)&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
		{
			if(app.m_Reg.m_bUseActiveWallpaper)
				app.m_MainWindow.m_ActiveWallpaper.Stop();
			if(app.m_Reg.m_bShowVisWindow)
				app.m_MainWindow.m_VisWindow.Stop();
		}
		if(!app.m_Player.StopSong())
			DisplayError(m_hWnd,FALSE,"Failed to stop the song.");
		app.m_MainWindow.m_StatusBar.Update();
		app.m_MainWindow.UpdateTitleBar();
		if(app.m_Reg.m_bShowVisWindow)
			app.m_MainWindow.m_VisWindow.Redraw();
	}
	return 0;
}

LRESULT CToolBar::OnPrevious()
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	INT nIndex=(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)?pl.MapIndexToExternal(app.m_Player.GetPlayingIndex()):pl.GetSelectedEntry();

	if(pl.IsRandomize()&&pl.GetDisplayedCount()>1)
	{
		INT i;
		while((i=rand()%pl.GetDisplayedCount())==nIndex)Sleep(10);
		pl.SelectEntry(i);
		RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);
		if(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)
		{
			CList *OldList=m_SongList;
			m_SongList=&pl;
			BOOL b=app.IsCallInternal(TRUE);
			OnPlay();
			app.IsCallInternal(b);
			m_SongList=OldList;
		}
	}
	else if(pl.SelectEntry(nIndex-1))
	{
		RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);
		if(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)
		{
			CList *OldList=m_SongList;
			m_SongList=&pl;
			BOOL b=app.IsCallInternal(TRUE);
			OnPlay();
			app.IsCallInternal(b);
			m_SongList=OldList;
		}
	}
	return 0;
}

LRESULT CToolBar::OnNext()
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	INT nIndex=(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)?pl.MapIndexToExternal(app.m_Player.GetPlayingIndex()):pl.GetSelectedEntry();

	if(app.m_Player.GetPlayingList()!=&pl&&app.IsCallInternal())
	{
		BOOL b=app.IsCallInternal(FALSE);
		OnStop();
		app.IsCallInternal(b);
	}		
	else if(pl.IsRandomize()&&pl.GetDisplayedCount()>1)
	{
		INT i;
		while((i=rand()%pl.GetDisplayedCount())==nIndex)Sleep(10);
		pl.SelectEntry(i);
		RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);
		if(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)
		{
			CList *OldList=m_SongList;
			m_SongList=&pl;
			BOOL b=app.IsCallInternal(TRUE);
			OnPlay();
			app.IsCallInternal(b);
			m_SongList=OldList;
		}
	}
	else if(pl.SelectEntry(nIndex+1))
	{
		RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);
		if(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)
		{
			CList *OldList=m_SongList;
			m_SongList=&pl;
			BOOL b=app.IsCallInternal(TRUE);
			OnPlay();
			app.IsCallInternal(b);
			m_SongList=OldList;
		}
	}
	else if(app.IsCallInternal()&&pl.IsAutoRewind())
	{
		pl.SelectEntry(0);
		RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);
		if(app.m_Player.IsPlaying()&&app.m_Player.GetPlayingList()==&pl)
		{
			CList *OldList=m_SongList;
			m_SongList=&pl;
			OnPlay();
			m_SongList=OldList;
		}
	}
	else if(app.IsCallInternal())
	{
		BOOL b=app.IsCallInternal(FALSE);
		OnStop();
		app.IsCallInternal(b);
	}
	return 0;
}

LRESULT CToolBar::OnWAVDump()
{
	INT nState=SendMessage(m_hWnd,TB_ISBUTTONCHECKED,IDC_BUTTON_WAVDUMP,0);
	if(nState!=-1)
		app.m_Reg.m_bOutputToWAV=nState;
	if(!app.m_Player.IsStopped())
	{
		if(app.m_Reg.m_bOutputToWAV)
		{
			if(!app.IsCallInternal()&&!app.m_Reg.m_bOutputWithSound)
			{
				app.m_MainWindow.m_VisWindow.Stop();
				app.m_MainWindow.m_ActiveWallpaper.Stop();
			}
			INT nIndex=app.m_Player.GetPlayingIndex();
			CList *List=app.m_Player.GetPlayingList();

			CListEntry* Entry=List->GetListEntryUnmapped(nIndex);

			TCHAR szWAVFileName[MAX_PATH]={0},szName[MAX_PATH];
			Entry->GetFullName(szName,sizeof(szName));
			_splitpath(szName,NULL,NULL,szName,NULL);
			strncpy(szWAVFileName,app.m_Reg.m_szWAVOutputDirectory,sizeof(szWAVFileName));
			if(strlen(szWAVFileName)&&szWAVFileName[strlen(szWAVFileName)-1]!='\\')
				strcat(szWAVFileName,"\\");
			strcat(szWAVFileName,szName);
			strcat(szWAVFileName,".wav");
			app.m_Player.SetWAVOutputFileName(szWAVFileName,app.m_Reg.m_bOutputWithSound);
			if(app.m_Reg.m_bShowVisWindow)
				app.m_MainWindow.m_VisWindow.Redraw();
		}
		else
		{
			app.m_Player.SetWAVOutputFileName(NULL,app.m_Reg.m_bOutputWithSound);
			if(!app.IsCallInternal())
			{
				if(app.m_Reg.m_bShowVisWindow&&!app.m_MainWindow.m_VisWindow.IsRunning())
					app.m_MainWindow.m_VisWindow.Start();
				if(app.m_Reg.m_bUseActiveWallpaper&&!app.m_MainWindow.m_ActiveWallpaper.IsRunning())
					app.m_MainWindow.m_ActiveWallpaper.Start();
			}			
		}
	}
	return 0;
}

LRESULT CToolBar::OnFilterView()
{
	INT nState=SendMessage(m_hWnd,TB_ISBUTTONCHECKED,IDC_BUTTON_FILTERVIEW,0);
	if(nState!=-1)
		app.m_Reg.m_bFilterActive=nState;

	CPlayList &pl=app.m_MainWindow.m_PlayList;

	if(app.m_Reg.m_bFilterActive)
	{
		pl.SetRedraw(FALSE);
		INT nSelectedIndex=pl.GetFocusedEntry();
		if(nSelectedIndex<0)
			nSelectedIndex=pl.GetSelectedEntry();
		nSelectedIndex=pl.MapIndexToInternal(nSelectedIndex);
		pl.FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
		pl.SortItems(pl.m_szSortColumn,pl.m_bSortOrder);
		if(app.m_Reg.m_bAutoSizeColumns)
			pl.ResizeColumns();
		pl.SetRedraw(TRUE);
		InvalidateRect(pl.m_hWnd,NULL,TRUE);
		pl.SelectEntry(pl.MapIndexToExternal(nSelectedIndex));
	}
	else
	{
		TCHAR szOldStatus[500];
		app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
		app.m_MainWindow.m_StatusBar.SetText(3,"Removing playlist filter...");
		app.m_MainWindow.MaintainCursor(TRUE);
		app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
		pl.SetRedraw(FALSE);
		INT nSelectedIndex=pl.GetFocusedEntry();
		if(nSelectedIndex<0)
			nSelectedIndex=pl.GetSelectedEntry();
		nSelectedIndex=pl.MapIndexToInternal(nSelectedIndex);
		pl.IsMapped(FALSE);
		ListView_DeleteAllItems(pl.m_hWnd);
		pl.DisplayEntries();
		pl.SortItems(pl.m_szSortColumn,pl.m_bSortOrder);
		if(app.m_Reg.m_bAutoSizeColumns)
			pl.ResizeColumns();
		pl.IsMapped(TRUE);
		pl.SetRedraw(TRUE);
		InvalidateRect(pl.m_hWnd,NULL,TRUE);
		pl.SelectEntry(pl.MapIndexToExternal(nSelectedIndex));		
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	}
	app.m_MainWindow.m_StatusBar.Update();

	return 0;
}

LRESULT CToolBar::OnAW()
{
	INT nState=SendMessage(m_hWnd,TB_ISBUTTONCHECKED,IDC_BUTTON_AW,0);
	if(nState!=-1)
		app.m_Reg.m_bUseActiveWallpaper=nState;
	if(!app.m_Player.IsStopped())
	{
		if(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound)
		{
			if(app.m_Reg.m_bUseActiveWallpaper)
			{
				app.m_MainWindow.m_ActiveWallpaper.SetParameters();
				app.m_MainWindow.m_ActiveWallpaper.Start();
			}
			else
				app.m_MainWindow.m_ActiveWallpaper.Stop();
		}
	}
	return 0;
}

LRESULT CToolBar::OnLayout()
{
	if(app.m_Reg.m_bLayout)
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_TILEVERTICALLY,0),(LPARAM)m_hWnd);
	else
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_TILEHORIZONTALLY,0),(LPARAM)m_hWnd);
	return 0;
}

LRESULT CToolBar::OnVisMode()
{
	app.m_Reg.m_dwVisMode^=1;
	return 0;
}

BOOL CToolBar::HasTextLabels()
{
	return m_bHasTextLabels;
}

BOOL CToolBar::HasTextLabels(BOOL bHasTextLabels)
{
	if(m_bHasTextLabels!=bHasTextLabels)
 	{
 		m_bHasTextLabels=bHasTextLabels;
		GenerateButtons();
 		return TRUE;
	}
	return FALSE;
}

BOOL CToolBar::HasPictures()
{
	return m_bHasPictures;
}

BOOL CToolBar::HasPictures(BOOL bHasPictures)
{
	if(m_bHasPictures!=bHasPictures)
	{
		m_bHasPictures=bHasPictures;
		GenerateButtons();
		return TRUE;
	}
	return FALSE;
}

void CToolBar::IsVisible(BOOL bVisible)
{
	app.m_MainWindow.m_ReBar.ShowBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),bVisible);
}

void CToolBar::Destroy()
{
	RemoveReBarBand();
	CWnd::Destroy();
	m_Pager.Destroy();
	ImageList_Destroy(m_hImlColorIcons);
	ImageList_Destroy(m_hImlGrayIcons);
	m_hImlColorIcons=m_hImlGrayIcons=NULL;
}

BOOL CToolBar::IsVisible()
{
	return CWnd::IsVisible();
}

void CToolBar::IsChanged(BOOL bChanged)
{
	m_bChanged=bChanged;
}

void CToolBar::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	CWnd::OnRButtonUp(hwnd,x,y,keyFlags);

	POINT pt={x,y};
	::ClientToScreen(m_hWnd,&pt);
	HMENU hMenu=GetSubMenu(GetSubMenu(app.m_MainWindow.m_Menu.m_hMenu,2),2);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_STANDARDBUTTONS,app.m_Reg.m_bShowToolBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_SOURCEBAR,app.m_Reg.m_bShowLocationBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_TEXTLABELS,app.m_Reg.m_bToolBarHasLabels?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_PICTURES,app.m_Reg.m_bToolBarHasPictures?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_WRAP,app.m_Reg.m_bWrapToolBar?MF_CHECKED:MF_UNCHECKED);
	TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,app.m_MainWindow.m_hWnd,NULL);
}

void CToolBar::SetSongList(CList *SongList)
{
	m_SongList=SongList;
}

void CToolBar::DisplayToolTips(BOOL bDisplay)
{
	HWND hWndTT=(HWND)SendMessage(m_hWnd,TB_GETTOOLTIPS,0,0);
	if(hWndTT)
		SendMessage(hWndTT,TTM_ACTIVATE,(WPARAM)bDisplay,0);
}

BOOL CToolBar::AddReBarBand()
{
	REBARBANDINFO rbBand;
	rbBand.cbSize=sizeof(rbBand);
	rbBand.fMask=RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_ID|RBBIM_SIZE;
	rbBand.fStyle=app.m_Reg.m_dwToolBarStyle;

	DWORD dwBtnSize=SendMessage(m_hWnd,TB_GETBUTTONSIZE,0,0);
	rbBand.hwndChild=m_Pager.m_hWnd;
	rbBand.cxMinChild=0;
	rbBand.cyMinChild=HIWORD(dwBtnSize);
	rbBand.wID=m_uID;
	rbBand.cx=app.m_Reg.m_dwToolBarBandWidth;

	return app.m_MainWindow.m_ReBar.AddBand(app.m_Reg.m_dwToolBarBandIndex,&rbBand);
}

BOOL CToolBar::RemoveReBarBand()
{
	return app.m_MainWindow.m_ReBar.DeleteBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID));
}

BOOL CToolBar::SaveReBarInfo()
{
	app.m_Reg.m_dwToolBarBandIndex=app.m_MainWindow.m_ReBar.GetBandIndex(m_uID);
	if(app.m_Reg.m_dwToolBarBandIndex!=-1)
	{
		REBARBANDINFO rbBand;
		rbBand.cbSize=sizeof(rbBand);
		rbBand.fMask=RBBIM_SIZE|RBBIM_STYLE;
		SendMessage(app.m_MainWindow.m_ReBar.m_hWnd,RB_GETBANDINFO,app.m_Reg.m_dwToolBarBandIndex,(LPARAM)&rbBand);
		app.m_Reg.m_dwToolBarBandWidth=rbBand.cx;
		app.m_Reg.m_dwToolBarStyle=rbBand.fStyle;
	}
	return TRUE;
}

void CToolBar::CheckButton(UINT uCmdId, BOOL bCheck)
{
	SendMessage(m_hWnd,TB_CHECKBUTTON,uCmdId,MAKELONG(bCheck,0));
}

void CToolBar::EnableButton(UINT uCmdId, BOOL bEnable)
{
	SendMessage(m_hWnd,TB_ENABLEBUTTON,uCmdId,MAKELONG(bEnable,0));
}

void CToolBar::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	CWnd::OnSize(hwnd,state,cx,cy);

	if(m_uOldRowCount!=SendMessage(m_hWnd,TB_GETROWS,0,0))
	{
		REBARBANDINFO rbBand;
		rbBand.cbSize=sizeof(rbBand);
		rbBand.fMask=RBBIM_CHILDSIZE;
		RECT rc;
		SendMessage(m_hWnd,TB_GETITEMRECT,SendMessage(m_hWnd,TB_BUTTONCOUNT,0,0)-1,(LPARAM)&rc);
		rbBand.cxMinChild=0;
		rbBand.cyMinChild=rc.bottom;
		app.m_MainWindow.m_ReBar.SetBandInfo(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),&rbBand);
		m_uOldRowCount=SendMessage(m_hWnd,TB_GETROWS,0,0);
	}
}
