// Menu.cpp: implementation of the CMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Menu.h"
#include "Resource.h"
#include "AfxRes.h"
#include "PlayListDialog.h"
#include "ConfigurationDialog.h"
#include "WarningDialog.h"
#include "AboutDialog.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenu::CMenu()
{
	m_nSelectedID=0;
	m_bActive=FALSE;
	m_ClipBoard.clear();
}

CMenu::~CMenu()
{

}

BOOL CMenu::Create(HINSTANCE hInstance, CWnd *pParentWnd,UINT uID)
{
	m_hInstance=hInstance;
	m_pParentWnd=pParentWnd;
	m_uID=uID;

	m_hMenu=LoadMenu(hInstance,MAKEINTRESOURCE(m_uID));
	if(!m_hMenu)
		return FALSE;
	SetMenu(m_pParentWnd->m_hWnd,m_hMenu);
	return TRUE;
}

LRESULT CMenu::OnInitMenu(HWND hwnd, HMENU hMenu)
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	UINT uEntryCount=pl.GetEntryCount();
	UINT uSelectedCount=pl.GetSelectedCount();
	UINT uDisplayedCount=pl.GetDisplayedCount();

	HMENU hSubMenu=GetSubMenu(m_hMenu,0);
	EnableMenuItem(hSubMenu,ID_FILE_SAVE,pl.IsSaved()?MF_GRAYED:MF_ENABLED);
	EnableMenuItem(hSubMenu,ID_FILE_SAVE_AS,uEntryCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_FILE_SAVEASHTML,uEntryCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_FILE_IMPORTID666TAGS,uEntryCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_FILE_EXPORTID666TAGS,uEntryCount?MF_ENABLED:MF_GRAYED);
	hSubMenu=GetSubMenu(m_hMenu,1);
	EnableMenuItem(hSubMenu,ID_EDIT_EDITITEM,uSelectedCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_CUT,uSelectedCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_COPY,uSelectedCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_PASTE,m_ClipBoard.size()?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_DELETE,uSelectedCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_FIND,uDisplayedCount?MF_ENABLED:MF_GRAYED);
	EnableMenuItem(hSubMenu,ID_EDIT_GOTO,uDisplayedCount?MF_ENABLED:MF_GRAYED);
	hSubMenu=GetSubMenu(m_hMenu,2);
	CheckMenuItem(hSubMenu,ID_VIEW_TITLEBAR,app.m_Reg.m_bShowTitleBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_MENU,app.m_Reg.m_bShowMenu?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_STATUSBAR,app.m_Reg.m_bShowStatusBar?MF_CHECKED:MF_UNCHECKED);
	hSubMenu=GetSubMenu(hSubMenu,2);
	CheckMenuItem(hSubMenu,ID_VIEW_TOOLBARS_STANDARDBUTTONS,app.m_Reg.m_bShowToolBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_TOOLBARS_SOURCEBAR,app.m_Reg.m_bShowLocationBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_TOOLBARS_TEXTLABELS,app.m_Reg.m_bToolBarHasLabels?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_TOOLBARS_PICTURES,app.m_Reg.m_bToolBarHasPictures?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_VIEW_TOOLBARS_WRAP,app.m_Reg.m_bWrapToolBar?MF_CHECKED:MF_UNCHECKED);
	hSubMenu=GetSubMenu(m_hMenu,3);
	CheckMenuItem(hSubMenu,ID_OPTIONS_SAVESETTINGSONEXIT,app.m_Reg.m_bSaveSettingsOnExit?MF_CHECKED:MF_UNCHECKED);
	hSubMenu=GetSubMenu(m_hMenu,4);
	CheckMenuItem(hSubMenu,ID_WINDOWS_FILELIST,app.m_MainWindow.m_FileList.IsVisible()?MF_CHECKED:MF_UNCHECKED);
	EnableMenuItem(hSubMenu,ID_WINDOWS_FILELIST,!app.m_MainWindow.m_ActiveWallpaper.IsRunning()?MF_ENABLED:MF_GRAYED);
	CheckMenuItem(hSubMenu,ID_WINDOWS_PLAYLIST,app.m_MainWindow.m_PlayList.IsVisible()?MF_CHECKED:MF_UNCHECKED);
	EnableMenuItem(hSubMenu,ID_WINDOWS_PLAYLIST,!app.m_MainWindow.m_ActiveWallpaper.IsRunning()?MF_ENABLED:MF_GRAYED);
	CheckMenuItem(hSubMenu,ID_WINDOWS_CONTROLPANEL,app.m_Reg.m_bShowControlPanel?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,ID_WINDOWS_VISUALIZER,app.m_Reg.m_bShowVisWindow?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hSubMenu,app.m_Reg.m_bLayout?ID_WINDOWS_TILEHORIZONTALLY:ID_WINDOWS_TILEVERTICALLY,MF_CHECKED);
	CheckMenuItem(hSubMenu,!app.m_Reg.m_bLayout?ID_WINDOWS_TILEHORIZONTALLY:ID_WINDOWS_TILEVERTICALLY,MF_UNCHECKED);
	return 0;
}

LRESULT CMenu::OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
{
	m_nSelectedID=item;

	if(hmenu==m_hMenu&&flags&MF_POPUP&&!(flags&MF_SYSMENU))
	{
		if(hmenuPopup==GetSubMenu(hmenu,0))m_nSelectedID=IDS_FILE_MENU;
		else if(hmenuPopup==GetSubMenu(hmenu,1))m_nSelectedID=IDS_EDIT_MENU;
		else if(hmenuPopup==GetSubMenu(hmenu,2))m_nSelectedID=IDS_VIEW_MENU;
		else if(hmenuPopup==GetSubMenu(hmenu,3))m_nSelectedID=IDS_OPTIONS_MENU;
		else if(hmenuPopup==GetSubMenu(hmenu,4))m_nSelectedID=IDS_WINDOWS_MENU;
		else if(hmenuPopup==GetSubMenu(hmenu,5))m_nSelectedID=IDS_HELP_MENU;
	}
	TCHAR szBuf[1000]={0};
	LoadString(m_hInstance,m_nSelectedID,szBuf,sizeof(szBuf));
	app.m_MainWindow.m_StatusBar.SetText(3,szBuf);
	if(LOWORD(flags)==0xFFFF&&hmenu==NULL)
		m_bActive=FALSE;
	else
		m_bActive=TRUE;
	return 0;
}

INT CMenu::GetSelectedID()
{
	return m_nSelectedID;
}

BOOL CMenu::IsActive()
{
	return m_bActive;
}

LRESULT CMenu::OnFileNew()
{
	if(!app.m_MainWindow.m_PlayList.IsSaved())
	{
		TCHAR szBuf[500],szPlayListName[MAX_PATH];
		app.m_MainWindow.m_PlayList.GetName(szPlayListName,MAX_PATH);
		sprintf(szBuf,"Save changes to \"%s\"?",szPlayListName);
		switch(MessageBox(app.m_MainWindow.m_hWnd,szBuf,app.m_szAppName,MB_ICONQUESTION|MB_YESNOCANCEL))
		{
		case IDYES:if(OnFileSave())return 0;break;
		case IDCANCEL:return 0;
		}
	}
	app.m_MainWindow.m_PlayList.Reset();
	app.m_MainWindow.UpdateTitleBar();
	app.m_MainWindow.m_StatusBar.Update();
	return 0;
}

LRESULT CMenu::OnFileOpen()
{
	if(!app.m_MainWindow.m_PlayList.IsSaved())
	{
		TCHAR szBuf[500],szPlayListName[MAX_PATH];
		app.m_MainWindow.m_PlayList.GetName(szPlayListName,MAX_PATH);
		sprintf(szBuf,"Save changes to \"%s\"?",szPlayListName);
		switch(MessageBox(app.m_MainWindow.m_hWnd,szBuf,app.m_szAppName,MB_ICONQUESTION|MB_YESNOCANCEL))
		{
		case IDYES:if(OnFileSave())return 0;break;
		case IDCANCEL:return 0;
		}
	}
	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=app.m_MainWindow.m_hWnd;
	ofn.lpstrFilter="Playlist Files (.PL)\0*.PL\0Archives (.ACE;.RAR;.RSN;.ZIP)\0*.ACE;*.RAR;*.RSN;*.ZIP\0SPC Songs(.SPC;.SP0;.SP1;.SP2;.SP3;.SP4;.SP5;.SP6;.SP7;.SP8;.SP9)\0*.SPC;*.SP0;*.SP1;*.SP2;*.SP3;*.SP4;*.SP5;*.SP6;*.SP7;*.SP8;*.SP9\0All Files (*.*)\0*.*\0";
	TCHAR szFileName[5000];
	app.m_MainWindow.m_PlayList.GetName(szFileName,MAX_PATH);
	ofn.lpstrFile=szFileName;
	ofn.nMaxFile=sizeof(szFileName);
	ofn.lpstrDefExt="PL";
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_EXPLORER;
	if(GetOpenFileName(&ofn))
	{
		TCHAR szDir[MAX_PATH];
		szFileName[ofn.nFileOffset-1]='\0';
		strncpy(szDir,szFileName,sizeof(szDir));
		LPTSTR lpszPos=szFileName;

		app.m_MainWindow.m_PlayList.ClearFilesToAdd();
		while(lpszPos=strchr(lpszPos,'\0'))
		{
			lpszPos++;
			TCHAR szFile[MAX_PATH];
			strncpy(szFile,lpszPos,sizeof(szFile));
			if(strlen(szFile))
			{
				TCHAR szPath[MAX_PATH];
				sprintf(szPath,"%s\\%s",szDir,szFile);

				LPCTSTR szExtensions[]={".pl",".ace",".rar",".rsn",".zip",".spc",".sp0",".sp1",".sp2",".sp3",".sp4",".sp5",".sp6",".sp7",".sp8",".sp9"};
				for(INT i=0;i<sizeof(szExtensions)/sizeof(szExtensions[0]);i++)
				{
					TCHAR szExt[_MAX_EXT];
					_splitpath(szPath,NULL,NULL,NULL,szExt);
					if(!stricmp(szExtensions[i],szExt))
					{
						app.m_MainWindow.m_PlayList.AddFileToAdd(szPath);
						break;
					}
				}
			}
			else
				break;
		}
		app.m_MainWindow.m_PlayList.AddFiles(FALSE,FALSE);
	}
	return 0;
}

LRESULT CMenu::OnFileSave()
{
	if(app.m_MainWindow.m_PlayList.IsSaved())
		return 0;

	if(!app.m_MainWindow.m_PlayList.Exists())
		return OnFileSaveAs();
	TCHAR szPlayListFileName[MAX_PATH];
	app.m_MainWindow.m_PlayList.GetName(szPlayListFileName,sizeof(szPlayListFileName));
	app.m_MainWindow.m_PlayList.Save(szPlayListFileName);
	return 0;
}

LRESULT CMenu::OnFileSaveAs()
{
	if(!app.m_MainWindow.m_PlayList.GetEntryCount())
		return 1;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=app.m_MainWindow.m_hWnd;
	ofn.lpstrFilter="Playlist Files (.PL)\0*.PL\0All Files (*.*)\0*.*\0";
	TCHAR szFileName[MAX_PATH];
	app.m_MainWindow.m_PlayList.GetName(szFileName,MAX_PATH);
	ofn.lpstrFile=szFileName;
	ofn.nMaxFile=sizeof(szFileName);
	ofn.lpstrDefExt="PL";
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
	if(GetSaveFileName(&ofn))
	{
		app.m_MainWindow.m_PlayList.Save(szFileName);
		return 0;
	}
	return 1;
}

LRESULT CMenu::OnFileSaveAsHTML()
{
	if(!app.m_MainWindow.m_PlayList.GetEntryCount())
		return 1;

	OPENFILENAME ofn;
	memset(&ofn,0,sizeof(ofn));
	ofn.lStructSize=sizeof(ofn);
	ofn.hwndOwner=app.m_MainWindow.m_hWnd;
	ofn.lpstrFilter="HTML Files (.HTML;.HTM)\0*.HTML;*.HTM\0All Files (*.*)\0*.*\0";
	TCHAR szFileName[MAX_PATH];
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	pl.GetName(szFileName,sizeof(szFileName));
	TCHAR szBuf[1000];
	_splitpath(szFileName,NULL,NULL,szBuf,NULL);
	sprintf(szFileName,"%s.html",szBuf);
	ofn.lpstrFile=szFileName;
	ofn.nMaxFile=sizeof(szFileName);
	ofn.lpstrTitle="Save As HTML";
	ofn.lpstrDefExt="HTML";
	ofn.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
	if(GetSaveFileName(&ofn))
	{
		FILE *OutFile=fopen(szFileName,"wt");
		if(OutFile==NULL)
		{
			sprintf(szBuf,"Failed to create \"%s\".",szFileName);
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,szBuf);
			return 0;
		}
		sprintf(szBuf,"<html>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<head>\n");
		fputs(szBuf,OutFile);
		pl.GetName(szFileName,sizeof(szFileName));
		TCHAR szFile[_MAX_FNAME];
		_splitpath(szFileName,NULL,NULL,szFile,NULL);
		sprintf(szBuf,"<title>%s</title>\n",szFile);
		fputs(szBuf,OutFile);
		sprintf(szBuf,"</head>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<body>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<div align=\"center\"><h1>%s</h1></div>\n",szFile);
		fputs(szBuf,OutFile);
		UINT uTotalLength=0,uDisplayedCount=pl.GetDisplayedCount();
		sprintf(szBuf,"<big>Tracks in playlist: %u</big><br>\n",uDisplayedCount);
		fputs(szBuf,OutFile);
		for(UINT i=0;i<uDisplayedCount;i++)
		{
			CListEntry *Entry=pl.GetListEntry(i);
			uTotalLength+=Entry->GetLength()+Entry->GetFadeLength();
		}
		TCHAR szBuf1[20];
		pl.ConvertTimeDown(uTotalLength,szBuf1);
		sprintf(szBuf,"<big>Playlist length: %s</big><br>\n",szBuf1);
		fputs(szBuf,OutFile);
		pl.ConvertTimeDown(uDisplayedCount?uTotalLength/uDisplayedCount:0,szBuf1);
		sprintf(szBuf,"<big>Average track length: %s</big><br>\n",szBuf1);
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<br>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<table width=\"100%%\" cellspacing=\"0\" border=\"1\" cols=\"%u\">\n",pl.GetColumnCount());
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<tr valign=\"top\">\n");
		fputs(szBuf,OutFile);
		UINT uColumnCount=pl.GetColumnCount();
		INT nOrder[NUMBER_OF_COLUMNS];
		ListView_GetColumnOrderArray(pl.m_hWnd,uColumnCount,nOrder);
		for(i=0;i<uColumnCount;i++)
		{
			TCHAR szColName[50];
			pl.GetColumnNameFromIndex(nOrder[i],szColName,sizeof(szColName));
			sprintf(szBuf,"<th align=\"left\">%s</th>\n",szColName);
			fputs(szBuf,OutFile);
		}
		sprintf(szBuf,"</tr>\n");
		fputs(szBuf,OutFile);
		for(i=0;i<uDisplayedCount;i++)
		{
			sprintf(szBuf,"<tr valign=\"top\">\n");
			fputs(szBuf,OutFile);
			TCHAR szProp[500];
			CListEntry *Entry=pl.GetListEntry(i);
			for(UINT c=0;c<uColumnCount;c++)
			{
				TCHAR szColName[50];
				pl.GetColumnNameFromIndex(nOrder[c],szColName,sizeof(szColName));
				Entry->GetProperty(szColName,szProp,sizeof(szProp));
				sprintf(szBuf,"<td>%s</td>\n",szProp);
				fputs(szBuf,OutFile);
			}
			sprintf(szBuf,"</tr>\n");
			fputs(szBuf,OutFile);
		}
		sprintf(szBuf,"</table>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<br>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<div align=\"center\">\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<small>Written by Super Jukebox 3.0</small>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<br>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"<small><a href=\"http://superjukebox.zophar.net/\">http://superjukebox.zophar.net/</a></small>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"</div>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"</body>\n");
		fputs(szBuf,OutFile);
		sprintf(szBuf,"</html>\n");
		fputs(szBuf,OutFile);
		fclose(OutFile);

		return 0;
	}
	return 1;
}

LRESULT CMenu::OnFileProperties()
{
	CPlayListDialog dlg;
	if(dlg.DoModal(IDD_PLAYLIST,&app.m_MainWindow,m_hInstance)==-1)
		DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	return 0;
}

LRESULT CMenu::OnFileImportID666Tags()
{
	if(!app.m_MainWindow.m_PlayList.GetEntryCount())
		return 0;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Importing ID666 tags...");

	CPlayList &pl=app.m_MainWindow.m_PlayList;

	if(pl.GetEntryCount())
	{
		vector<CListEntry>TempList;
		TempList.clear();
		for(INT i=0;i<pl.GetEntryCount();i++)
			TempList.push_back(*pl.GetListEntryUnmapped(i));
		pl.ClearFilesToAdd();
		for(i=0;i<TempList.size();i++)
		{
			TCHAR szFileName[MAX_PATH];
			TempList[i].GetPath(szFileName,sizeof(szFileName));
			pl.AddFileToAdd(szFileName);
		}
		INT nIndex=pl.GetSelectedEntry();
		pl.Clear();
		pl.AddFiles(FALSE,FALSE);
		pl.SelectEntry(nIndex);
	}

	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return 0;
}

LRESULT CMenu::OnFileExportID666Tags()
{
	if(!app.m_MainWindow.m_PlayList.GetEntryCount())
		return 0;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Exporting ID666 tags...");

	CPlayList &pl=app.m_MainWindow.m_PlayList;

	if(pl.GetEntryCount())
	{
		INT nRes=IDOK;
		if(app.m_Reg.m_bWarnAgainstEditingSPCInArchive)
		{
			CWarningDialog dlg;
			dlg.SetWarningText("SPC files that are inside archives will not be updated.");
			nRes=dlg.DoModal(IDD_WARNING,&app.m_MainWindow,m_hInstance);
			app.m_Reg.m_bWarnAgainstEditingSPCInArchive=dlg.IsNoMoreChecked()^TRUE;
		}

		if(nRes==IDOK)
		{
			vector<CListEntry>TempList;
			TempList.clear();
			for(INT i=0;i<pl.GetEntryCount();i++)
			{
				CListEntry *Entry=pl.GetListEntryUnmapped(i);
				if(!Entry->IsArchived()&&!Entry->SaveID666Tag())
				{
					TCHAR szFileName[MAX_PATH],szBuf[5000];
					Entry->GetPath(szFileName,sizeof(szFileName));
					sprintf(szBuf,"Failed to update ID666 tag for \"%s\".",szFileName);
					DisplayError(app.m_MainWindow.m_hWnd,FALSE,szBuf);
				}
			}
		}
	}

	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return 0;
}

LRESULT CMenu::OnFileExit()
{
	SendMessage(app.m_MainWindow.m_hWnd,WM_CLOSE,0,0);
	return 0;
}

LRESULT CMenu::OnEditEditItem()
{
	INT nIndex=app.m_MainWindow.m_PlayList.GetFocusedEntry();
	if(nIndex<0)
		nIndex=app.m_MainWindow.m_PlayList.GetSelectedEntry();
	if(nIndex<0)
		return 0;

	INT nOrder[NUMBER_OF_COLUMNS];
	ListView_GetColumnOrderArray(app.m_MainWindow.m_PlayList.m_hWnd,app.m_MainWindow.m_PlayList.GetColumnCount(),nOrder);
	app.m_MainWindow.m_PlayList.EditItem(nIndex,nOrder[0]);
	return 0;
}

LRESULT CMenu::OnEditCut()
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	if(!pl.GetSelectedCount())return 0;
	m_ClipBoard.clear();
	INT nCount=pl.GetDisplayedCount();

	for(INT i=0;i<nCount;i++)
	{
		if(pl.GetEntryState(i)&LVIS_SELECTED)
			m_ClipBoard.push_back(*pl.GetListEntry(i));
	}
	app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_REMOVE);
	return 0;
}

LRESULT CMenu::OnEditCopy()
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	if(!pl.GetSelectedCount())return 0;
	m_ClipBoard.clear();
	INT nCount=pl.GetDisplayedCount();

	for(INT i=0;i<nCount;i++)
	{
		if(pl.GetEntryState(i)&LVIS_SELECTED)
			m_ClipBoard.push_back(*pl.GetListEntry(i));
	}
	return 0;
}

LRESULT CMenu::OnEditPaste()
{
	INT nCount=m_ClipBoard.size();
	if(!nCount)return 0;
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	pl.SetRedraw(FALSE);
	INT nIndex=pl.GetSelectedEntry();
	for(INT i=0;i<nCount;i++)
	{
		pl.InsertEntry(m_ClipBoard[i],nIndex);
		if(nIndex>=0)
			nIndex++;
	}
	if(app.m_Reg.m_bAutoSizeColumns)
		pl.ResizeColumns();
	pl.SetRedraw(TRUE);
	InvalidateRect(pl.m_hWnd,NULL,TRUE);
	if(i)
		app.m_MainWindow.UpdateTitleBar();
	return 0;
}

LRESULT CMenu::OnEditDelete()
{
	app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_REMOVE);
	return 0;
}

LRESULT CMenu::OnEditSelectAll()
{
	CList *List=NULL;
	if(GetFocus()==app.m_MainWindow.m_FileList.m_hWnd)
		List=&app.m_MainWindow.m_FileList;
	else if(GetFocus()==app.m_MainWindow.m_PlayList.m_hWnd)
		List=&app.m_MainWindow.m_PlayList;

	if(List)
	{
		UINT uCount=List->GetDisplayedCount();
		for(UINT i=0;i<uCount;i++)
			List->SetEntryState(i,List->GetEntryState(i)|LVIS_SELECTED);
	}
	return 0;
}

LRESULT CMenu::OnEditInvertSelection()
{
	CList *List=NULL;
	if(GetFocus()==app.m_MainWindow.m_FileList.m_hWnd)
		List=&app.m_MainWindow.m_FileList;
	else if(GetFocus()==app.m_MainWindow.m_PlayList.m_hWnd)
		List=&app.m_MainWindow.m_PlayList;

	if(List)
	{
		UINT uCount=List->GetDisplayedCount();
		for(UINT i=0;i<uCount;i++)
			List->SetEntryState(i,List->GetEntryState(i)^LVIS_SELECTED);
	}
	return 0;
}

LRESULT CMenu::OnEditFind()
{
	if(!app.m_MainWindow.m_PlayList.GetDisplayedCount())
		return 0;

	if(!IsWindow(app.m_MainWindow.m_FindDlg.m_hWnd))
	{
		if(!app.m_MainWindow.m_FindDlg.Create(IDD_FIND,&app.m_MainWindow,m_hInstance))
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	}
	else
	{
		SetForegroundWindow(app.m_MainWindow.m_FindDlg.m_hWnd);
	}
	return 0;
}

LRESULT CMenu::OnEditGoTo()
{
	if(!app.m_MainWindow.m_PlayList.GetDisplayedCount())
		return 0;

	if(!IsWindow(app.m_MainWindow.m_GotoDlg.m_hWnd))
	{
		if(!app.m_MainWindow.m_GotoDlg.Create(IDD_GOTO,&app.m_MainWindow,m_hInstance))
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	}
	else
	{
		SetForegroundWindow(app.m_MainWindow.m_GotoDlg.m_hWnd);
	}
	return 0;
}

LRESULT CMenu::OnViewTitleBar()
{
	app.m_Reg.m_bShowTitleBar^=1;
	LONG lStyle=GetWindowLong(app.m_MainWindow.m_hWnd,GWL_STYLE);
	SetWindowLong(app.m_MainWindow.m_hWnd,GWL_STYLE,app.m_Reg.m_bShowTitleBar?(lStyle|WS_CAPTION):(lStyle&~WS_CAPTION));
	SetWindowPos(app.m_MainWindow.m_hWnd,NULL,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME|SWP_FRAMECHANGED);
	return 0;
}

LRESULT CMenu::OnViewMenu()
{
	app.m_Reg.m_bShowMenu^=1;
	SetMenu(m_pParentWnd->m_hWnd,app.m_Reg.m_bShowMenu?m_hMenu:NULL);
	return 0;
}

LRESULT CMenu::OnViewToolbarsStandardButtons()
{
	app.m_Reg.m_bShowToolBar^=1;
	app.m_MainWindow.m_ToolBar.IsVisible(app.m_Reg.m_bShowToolBar);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return 0;
}

LRESULT CMenu::OnViewToolbarsSourceBar()
{
	app.m_Reg.m_bShowLocationBar^=1;
	app.m_MainWindow.m_SourceBar.IsVisible(app.m_Reg.m_bShowLocationBar);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return 0;
}

LRESULT CMenu::OnViewToolbarsTextLabels()
{
	app.m_Reg.m_bToolBarHasLabels^=1;
	app.m_MainWindow.m_ToolBar.HasTextLabels(app.m_Reg.m_bToolBarHasLabels);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return 0;
}

LRESULT CMenu::OnViewToolbarsPictures()
{
	app.m_Reg.m_bToolBarHasPictures^=1;
	app.m_MainWindow.m_ToolBar.HasPictures(app.m_Reg.m_bToolBarHasPictures);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return 0;
}

LRESULT CMenu::OnViewToolbarsWrap()
{
	app.m_Reg.m_bWrapToolBar^=1;
	Pager_RecalcSize(app.m_MainWindow.m_ToolBar.m_Pager.m_hWnd);
	return 0;
}

LRESULT CMenu::OnViewToolbarsCustomize()
{
	if(!IsWindow(app.m_MainWindow.m_ToolbarCustomizeDlg.m_hWnd))
	{
		if(!app.m_MainWindow.m_ToolbarCustomizeDlg.Create(IDD_TOOLBAR_CUSTOMIZE,&app.m_MainWindow,m_hInstance))
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	}
	else
	{
		SetForegroundWindow(app.m_MainWindow.m_ToolbarCustomizeDlg.m_hWnd);
	}
	return 0;
}

LRESULT CMenu::OnViewStatusBar()
{
	app.m_Reg.m_bShowStatusBar^=1;
	app.m_MainWindow.m_StatusBar.IsVisible(app.m_Reg.m_bShowStatusBar);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return 0;
}

LRESULT CMenu::OnViewFilter()
{
	if(!IsWindow(app.m_MainWindow.m_PlaylistFilterDlg.m_hWnd))
	{
		if(!app.m_MainWindow.m_PlaylistFilterDlg.Create(IDD_FILTER,&app.m_MainWindow,m_hInstance))
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	}
	else
	{
		SetForegroundWindow(app.m_MainWindow.m_PlaylistFilterDlg.m_hWnd);
	}
	return 0;
}

LRESULT CMenu::OnViewShuffle()
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;
	UINT uDisplayedCount=pl.GetDisplayedCount();

	if(uDisplayedCount>0)
	{
		TCHAR szOldStatus[500];
		app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
		app.m_MainWindow.m_StatusBar.SetText(3,"Shuffling playlist entries...");
		app.m_MainWindow.MaintainCursor(TRUE);
		app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
		pl.SetSortingColumn(NULL,FALSE);

		vector<CListEntry>TempList;
		TempList.clear();
		vector<INT>nAddedEntries;
		nAddedEntries.clear();

		for(INT i=0;i<uDisplayedCount;i++)
		{
			TempList.push_back(*pl.GetListEntry(i));
			INT nEntry;
			while(TRUE)
			{
				nEntry=rand()%uDisplayedCount;
				BOOL bMatch=FALSE;
				for(INT x=0;x<nAddedEntries.size();x++)
					if(nAddedEntries[x]==nEntry)
					{
						bMatch=TRUE;
						break;
					}
				if(!bMatch)
				{
					nAddedEntries.push_back(nEntry);
					break;
				}
			}
		}
		INT nOldSelection=pl.MapIndexToInternal(pl.GetSelectedEntry());
		pl.SetRedraw(FALSE);
		ListView_DeleteAllItems(pl.m_hWnd);
		for(i=0;i<nAddedEntries.size();i++)
		{
			INT x=TempList[nAddedEntries[i]].GetListIndex();
			pl.DisplayEntries(x,x);
		}
		pl.SetRedraw(TRUE);
		InvalidateRect(pl.m_hWnd,NULL,TRUE);
		pl.SelectEntry(pl.MapIndexToExternal(nOldSelection));

		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	}

	return 0;
}

LRESULT CMenu::OnViewResizeColumns()
{
	app.m_MainWindow.m_FileList.ResizeColumns();
	app.m_MainWindow.m_PlayList.ResizeColumns();
	return 0;
}

LRESULT CMenu::OnViewRefresh()
{
	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Refreshing program...");

	CFileList &fl=app.m_MainWindow.m_FileList;

	INT nIndex=fl.GetSelectedEntry();
	fl.SetRedraw(FALSE);
	fl.Clear();
	fl.LoadDriveList();
	fl.LoadDirectoryList();
	fl.LoadFileList();
	fl.SortItems(fl.m_szSortColumn,fl.m_bSortOrder);
	fl.ResizeColumns();
	fl.SetRedraw(TRUE);
	InvalidateRect(fl.m_hWnd,NULL,TRUE);
	fl.SelectEntry(nIndex);

	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);

	app.m_MainWindow.UpdateTitleBar();
	app.m_MainWindow.m_StatusBar.Update();
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return 0;
}

LRESULT CMenu::OnOptionsConfigure()
{
	CConfigurationDialog dlg;
	if(dlg.DoModal(IDD_CONFIGURATION,&app.m_MainWindow,m_hInstance)==-1)
		DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	return 0;
}

LRESULT CMenu::OnOptionsSaveSettingsOnExit()
{
	app.m_Reg.m_bSaveSettingsOnExit^=1;
	HKEY hKey;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	RegSetValueEx(hKey,"Save settings on exit",0,REG_DWORD,(LPBYTE)&app.m_Reg.m_bSaveSettingsOnExit,sizeof(app.m_Reg.m_bSaveSettingsOnExit));
	RegCloseKey(hKey);
	return 0;
}

LRESULT CMenu::OnOptionsSaveSettingsNow()
{
	app.m_MainWindow.SyncRegistrySettings();
	app.m_Reg.Save();
	return 0;
}

LRESULT CMenu::OnOptionsRestoreDefaultSettings()
{
	if(MessageBox(app.m_MainWindow.m_hWnd,"This will reset *all* the settings, are you sure you want to continue?",app.m_szAppName,MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2)==IDYES)
	{
		app.m_MainWindow.Destroy();
		BOOL b=app.m_Reg.m_bSaveSettingsOnExit;
		app.m_Reg.Reset();
		app.m_Reg.m_bSaveSettingsOnExit=b;
		app.ReRun(TRUE);
	}
	return 0;
}

LRESULT CMenu::OnWindowFileList()
{
	app.m_Reg.m_bShowFileList^=1;
	app.m_MainWindow.m_FileList.IsVisible(app.m_Reg.m_bShowFileList);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_FILELIST,app.m_Reg.m_bShowFileList);
	return 0;
}

LRESULT CMenu::OnWindowPlayList()
{
	app.m_Reg.m_bShowPlayList^=1;
	app.m_MainWindow.m_PlayList.IsVisible(app.m_Reg.m_bShowPlayList);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_PLAYLIST,app.m_Reg.m_bShowPlayList);
	return 0;
}

LRESULT CMenu::OnWindowControlPanel()
{
	app.m_Reg.m_bShowControlPanel^=1;
	app.m_MainWindow.m_ControlPanel.IsVisible(app.m_Reg.m_bShowControlPanel);
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_CONTROLPANEL,app.m_Reg.m_bShowControlPanel);
	return 0;
}

LRESULT CMenu::OnWindowVisualizer()
{
	app.m_Reg.m_bShowVisWindow^=1;
	if(app.m_Reg.m_bShowVisWindow)
	{
		app.m_MainWindow.m_VisWindow.Create(m_hInstance,&app.m_MainWindow,IDC_VIS_WINDOW);
		if(!app.m_Player.IsStopped()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
			app.m_MainWindow.m_VisWindow.Start();
	}
	else
		app.m_MainWindow.m_VisWindow.Destroy();
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_VISUALIZER,app.m_Reg.m_bShowVisWindow);
	return 0;
}

LRESULT CMenu::OnWindowTileHorizontally()
{
	if(!app.m_Reg.m_bLayout)
	{
		INT nListPaneSeparator=app.m_MainWindow.m_ClientRect.bottom*app.m_Reg.m_dListPaneRatio;
		if(nListPaneSeparator<app.m_MainWindow.m_ClientRect.top+54)
		{
			nListPaneSeparator=app.m_MainWindow.m_ClientRect.top+54;
			app.m_Reg.m_dListPaneRatio=(double)nListPaneSeparator/app.m_MainWindow.m_ClientRect.bottom;
		}
		if(nListPaneSeparator>app.m_MainWindow.m_ClientRect.bottom-54)
		{
			nListPaneSeparator=app.m_MainWindow.m_ClientRect.bottom-54;
			app.m_Reg.m_dListPaneRatio=(double)nListPaneSeparator/app.m_MainWindow.m_ClientRect.bottom;
		}		
		app.m_Reg.m_bLayout=TRUE;
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
	}
	return 0;
}

LRESULT CMenu::OnWindowTileVertically()
{
	if(app.m_Reg.m_bLayout)
	{
		INT nListPaneSeparator=app.m_MainWindow.m_ClientRect.right*app.m_Reg.m_dListPaneRatio;
		if(nListPaneSeparator<app.m_MainWindow.m_ClientRect.left+54)
		{
			nListPaneSeparator=app.m_MainWindow.m_ClientRect.left+54;
			app.m_Reg.m_dListPaneRatio=(double)nListPaneSeparator/app.m_MainWindow.m_ClientRect.right;
		}
		if(nListPaneSeparator>app.m_MainWindow.m_ClientRect.right-54)
		{
			nListPaneSeparator=app.m_MainWindow.m_ClientRect.right-54;
			app.m_Reg.m_dListPaneRatio=(double)nListPaneSeparator/app.m_MainWindow.m_ClientRect.right;
		}		
		app.m_Reg.m_bLayout=FALSE;
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
	}
	return 0;
}

LRESULT CMenu::OnHelpContents()
{
	TCHAR szProgramLocation[MAX_PATH];
	GetModuleFileName(app.m_hInstance,szProgramLocation,sizeof(szProgramLocation));
	TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szProgramLocation,szDrv,szDir,NULL,NULL);
	sprintf(szProgramLocation,"%s%sSuper Jukebox.chm",szDrv,szDir);

	if((INT)ShellExecute(app.m_MainWindow.m_hWnd,NULL,szProgramLocation,NULL,NULL,SW_SHOWDEFAULT)<=32)
	{
		TCHAR szBuf[MAX_PATH*2];
		sprintf(szBuf,"Failed to open help file \"%s\".",szProgramLocation);
		DisplayError(app.m_MainWindow.m_hWnd,FALSE,szBuf);
	}

	return 0;
}

LRESULT CMenu::OnHelpAboutSuperJukebox()
{
	CAboutDialog dlg;
	if(dlg.DoModal(IDD_ABOUTBOX,&app.m_MainWindow,m_hInstance)==-1)
		DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
	return 0;
}