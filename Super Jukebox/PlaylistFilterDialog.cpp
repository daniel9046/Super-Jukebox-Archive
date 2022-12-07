// PlaylistFilterDialog.cpp: implementation of the CPlaylistFilterDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlaylistFilterDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlaylistFilterDialog::CPlaylistFilterDialog()
{
}

CPlaylistFilterDialog::~CPlaylistFilterDialog()
{

}

BOOL CPlaylistFilterDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	UINT uIDs[NUMBER_OF_COLUMNS]={IDC_COMBO_FILTER_FILE,IDC_COMBO_FILTER_TITLE,IDC_COMBO_FILTER_GAME,IDC_COMBO_FILTER_ARTIST,IDC_COMBO_FILTER_PUBLISHER,IDC_COMBO_FILTER_DUMPER,IDC_COMBO_FILTER_DATE,IDC_COMBO_FILTER_COMMENT,IDC_COMBO_FILTER_LENGTH,IDC_COMBO_FILTER_FADE};

	UINT uMaxTextLen=0;
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
	{
		m_Fields[i].uID=uIDs[i];
		SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_LIMITTEXT,sizeof(app.m_Reg.m_szFilterFields[i])-1,0);
		SetDlgItemText(m_hWnd,m_Fields[i].uID,app.m_Reg.m_szFilterFields[i]);
		uMaxTextLen=max(uMaxTextLen,strlen(app.m_Reg.m_szFilterFields[i]));
	}
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_FILTER_APPLY),uMaxTextLen);
	
	RefreshComboBoxContents();
	RefreshComboBoxStates();

	CheckDlgButton(m_hWnd,IDC_CHECK_MATCH_WHOLE_WORD_ONLY,app.m_Reg.m_bFilterMatchWholeWordOnly?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_MATCH_CASE,app.m_Reg.m_bFilterMatchCase?BST_CHECKED:BST_UNCHECKED);
	CheckRadioButton(m_hWnd,IDC_RADIO_FILTER_AND,IDC_RADIO_FILTER_OR,app.m_Reg.m_bFilterLogicalRelation?IDC_RADIO_FILTER_OR:IDC_RADIO_FILTER_AND);

	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CPlaylistFilterDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	LRESULT lResult=1;
	TCHAR szBuf[100]={0};
	UINT uMaxTextLen=0,i;
	
	switch(codeNotify)
	{
	case CBN_EDITCHANGE:
		for(i=0;i<NUMBER_OF_COLUMNS;i++)
		{
			GetDlgItemText(m_hWnd,m_Fields[i].uID,szBuf,sizeof(szBuf));
			uMaxTextLen=max(uMaxTextLen,strlen(szBuf));
		}
		EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_FILTER_APPLY),uMaxTextLen);
		break;
	case CBN_SELENDOK:
	case CBN_SELCHANGE:
		INT nCurSel;
		for(i=0;i<NUMBER_OF_COLUMNS;i++)
		{
			nCurSel=SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_GETCURSEL,0,0);
			if(nCurSel!=CB_ERR)
			{
				SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_GETLBTEXT,nCurSel,(LPARAM)szBuf);
				uMaxTextLen=max(uMaxTextLen,strlen(szBuf));
			}
		}
		EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_FILTER_APPLY),uMaxTextLen);
		break;
	case BN_CLICKED:
		switch(id)
		{
		case IDC_BUTTON_FILTER_APPLY:
			OnApply();
			break;
		case IDC_BUTTON_FILTER_REMOVE:
			OnRemove();
			break;
		case IDC_CHECK_MATCH_WHOLE_WORD_ONLY:
			app.m_Reg.m_bFilterMatchWholeWordOnly=IsDlgButtonChecked(m_hWnd,id)==BST_CHECKED;
			break;
		case IDC_CHECK_MATCH_CASE:
			app.m_Reg.m_bFilterMatchCase=IsDlgButtonChecked(m_hWnd,id)==BST_CHECKED;
			break;
		case IDC_RADIO_FILTER_AND:
		case IDC_RADIO_FILTER_OR:
			app.m_Reg.m_bFilterLogicalRelation=IsDlgButtonChecked(m_hWnd,IDC_RADIO_FILTER_OR)==BST_CHECKED;
			break;
		}
		break;
	}
	if(lResult)
		return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
	return lResult;
}

void CPlaylistFilterDialog::RefreshComboBoxContents()
{
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
	{
		TCHAR szBuf[100];
		GetDlgItemText(m_hWnd,m_Fields[i].uID,szBuf,sizeof(szBuf)-1);
		if(strlen(szBuf))
		{
			BOOL bIn=FALSE;
			for(INT y=0;y<10;y++)
				if(!strcmp(app.m_Reg.m_szFilterHistoryList[i][y],szBuf))
				{
					bIn=TRUE;
					break;
				}
			if(!bIn)
			{
				for(INT y=0;y<10;y++)
					if(!strlen(app.m_Reg.m_szFilterHistoryList[i][y]))
					{
						strncpy(app.m_Reg.m_szFilterHistoryList[i][y],szBuf,sizeof(app.m_Reg.m_szFilterHistoryList[i][y]));
						break;
					}
				if(y==10)
				{
					for(y=1;y<10;y++)
						strcpy(app.m_Reg.m_szFilterHistoryList[i][y-1],app.m_Reg.m_szFilterHistoryList[i][y]);
					strncpy(app.m_Reg.m_szFilterHistoryList[i][9],szBuf,sizeof(app.m_Reg.m_szFilterHistoryList[i][9]));
				}
			}
		}
		SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_RESETCONTENT,0,0);
		for(INT y=9;y>=0;y--)
		{
			if(strlen(app.m_Reg.m_szFilterHistoryList[i][y]))
				SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_ADDSTRING,0,(LPARAM)app.m_Reg.m_szFilterHistoryList[i][y]);
		}
		SetDlgItemText(m_hWnd,m_Fields[i].uID,szBuf);
		SendDlgItemMessage(m_hWnd,m_Fields[i].uID,CB_SETEDITSEL,0,MAKELPARAM(0,-1));		
	}	
}

BOOL CPlaylistFilterDialog::OnApply()
{
	RefreshComboBoxContents();

	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
		GetDlgItemText(m_hWnd,m_Fields[i].uID,app.m_Reg.m_szFilterFields[i],sizeof(app.m_Reg.m_szFilterFields[i])-1);

	app.m_Reg.m_bFilterActive=TRUE;
	app.m_MainWindow.m_ToolBar.CheckButton(IDC_BUTTON_FILTERVIEW,app.m_Reg.m_bFilterActive);
	
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	pl.SetRedraw(FALSE);
	INT nSelectedIndex=pl.GetFocusedEntry();
	if(nSelectedIndex<0)
		nSelectedIndex=pl.GetSelectedEntry();
	nSelectedIndex=pl.MapIndexToInternal(nSelectedIndex);
	INT nTotalMatches=pl.FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
	pl.SortItems(pl.m_szSortColumn,pl.m_bSortOrder);
	if(app.m_Reg.m_bAutoSizeColumns)
		pl.ResizeColumns();
	pl.SetRedraw(TRUE);
	InvalidateRect(pl.m_hWnd,NULL,TRUE);
	pl.SelectEntry(pl.MapIndexToExternal(nSelectedIndex));

	TCHAR szTitle[1000];
	sprintf(szTitle,"Playlist Filter - Found %u match%s",nTotalMatches,nTotalMatches==1?"":"es");
	SetWindowText(m_hWnd,szTitle);

	app.m_MainWindow.m_StatusBar.Update();

	return TRUE;
}

BOOL CPlaylistFilterDialog::OnRemove()
{
	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Removing playlist filter...");
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));

	CPlayList &pl=app.m_MainWindow.m_PlayList;
	INT nSelectedIndex=pl.GetFocusedEntry();
	if(nSelectedIndex<0)
		nSelectedIndex=pl.GetSelectedEntry();
	nSelectedIndex=pl.MapIndexToInternal(nSelectedIndex);
	pl.SetRedraw(FALSE);
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
	app.m_Reg.m_bFilterActive=FALSE;
	app.m_MainWindow.m_ToolBar.CheckButton(IDC_BUTTON_FILTERVIEW,app.m_Reg.m_bFilterActive);
	TCHAR szTitle[1000];
	sprintf(szTitle,"Playlist Filter - Found %u match%s",pl.GetDisplayedCount(),pl.GetDisplayedCount()==1?"":"es");
	SetWindowText(m_hWnd,szTitle);
	app.m_MainWindow.m_StatusBar.Update();
	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);

	return TRUE;
}

LRESULT CPlaylistFilterDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}

void CPlaylistFilterDialog::RefreshComboBoxStates()
{
	for(INT i=0;i<NUMBER_OF_COLUMNS;i++)
		EnableWindow(GetDlgItem(m_hWnd,m_Fields[i].uID),app.m_Reg.m_PlayListColumnInfos[i].bActive);
}
