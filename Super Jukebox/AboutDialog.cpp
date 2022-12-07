// AboutDialog.cpp: implementation of the CAboutDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AboutDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAboutDialog::CAboutDialog()
{

}

CAboutDialog::~CAboutDialog()
{

}

BOOL CAboutDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    LOGFONT lf;
	lf.lfHeight=-11;
	lf.lfWidth=0;
	lf.lfEscapement=0;
	lf.lfOrientation=0;
	lf.lfWeight=FW_REGULAR;
	lf.lfItalic=FALSE;
	lf.lfUnderline=TRUE;
	lf.lfStrikeOut=FALSE;
	lf.lfCharSet=ANSI_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=VARIABLE_PITCH|FF_SWISS;
	strcpy(lf.lfFaceName,"MS Sans Serif");
 
    m_hFont=CreateFontIndirect(&lf);
	SendDlgItemMessage(m_hWnd,IDC_BUTTON_HOMEPAGE,WM_SETFONT,(WPARAM)m_hFont,0);
	SendDlgItemMessage(m_hWnd,IDC_BUTTON_MAILTO,WM_SETFONT,(WPARAM)m_hFont,0);
	SendDlgItemMessage(m_hWnd,IDC_BUTTON_ZDSPCARCHIVE,WM_SETFONT,(WPARAM)m_hFont,0);
	SendDlgItemMessage(m_hWnd,IDC_BUTTON_SNESAMPSPCARCHIVE,WM_SETFONT,(WPARAM)m_hFont,0);

	UINT uVer,uMinVer,uOpt;
	if(!Uematsu_GetCoreInfo(&uVer,&uMinVer,&uOpt))
		DisplayError(m_hWnd,FALSE,"Failed to get version information for SNESAPU.DLL.");

	TCHAR szText[5000],szBuf[5000],szBuf1[200];
	sprintf(szText,"SNESAPU.DLL Version: %X.%X",LOBYTE(HIWORD(uVer)),LOBYTE(HIWORD(uMinVer)));
	SetDlgItemText(m_hWnd,IDC_STATIC_COREINFO,szText);

	CPlayList &pl=app.m_MainWindow.m_PlayList;
	UINT uTotalLength=0,uDisplayedCount=pl.GetDisplayedCount();

	sprintf(szBuf,"Total songs in playlist\t: %u\r\n",pl.GetEntryCount());
	strcpy(szText,szBuf);
	sprintf(szBuf,"Total songs displayed\t: %u\r\n",uDisplayedCount);
	strcat(szText,szBuf);
	
	for(UINT i=0;i<uDisplayedCount;i++)
	{
		CListEntry *Entry=pl.GetListEntry(i);
		uTotalLength+=Entry->GetLength()+Entry->GetFadeLength();
	}
	pl.ConvertTimeDown(uTotalLength,szBuf1);
	sprintf(szBuf,"Total length of songs\t: %s\r\n",szBuf1);
	strcat(szText,szBuf);
	pl.ConvertTimeDown(uDisplayedCount?uTotalLength/uDisplayedCount:0,szBuf1);
	sprintf(szBuf,"Average length of songs\t: %s\r\n",szBuf1);
	strcat(szText,szBuf);

	vector<_tmp>TempList;
	TempList.clear();
	for(i=0;i<uDisplayedCount;i++)
	{
		CListEntry *Entry=pl.GetListEntry(i);
		_tmp tmp;
		Entry->GetProperty("Game",tmp.szBuf,sizeof(tmp.szBuf));
		BOOL bMatch=FALSE;
		for(UINT x=0;x<TempList.size();x++)
			if(!stricmp(TempList[x].szBuf,tmp.szBuf))
			{
				bMatch=TRUE;
				break;
			}
		if(!bMatch)
			TempList.push_back(tmp);
	}
	sprintf(szBuf,"Number of different games\t: %u\r\n",TempList.size());
	strcat(szText,szBuf);

	TempList.clear();
	for(i=0;i<uDisplayedCount;i++)
	{
		CListEntry *Entry=pl.GetListEntry(i);
		_tmp tmp;
		Entry->GetProperty("Artist",tmp.szBuf,sizeof(tmp.szBuf));
		BOOL bMatch=FALSE;
		for(UINT x=0;x<TempList.size();x++)
			if(!stricmp(TempList[x].szBuf,tmp.szBuf))
			{
				bMatch=TRUE;
				break;
			}
		if(!bMatch)
			TempList.push_back(tmp);
	}
	sprintf(szBuf,"Number of different artists\t: %u\r\n",TempList.size());
	strcat(szText,szBuf);

	TempList.clear();
	for(i=0;i<uDisplayedCount;i++)
	{
		CListEntry *Entry=pl.GetListEntry(i);
		_tmp tmp;
		Entry->GetProperty("Publisher",tmp.szBuf,sizeof(tmp.szBuf));
		BOOL bMatch=FALSE;
		for(UINT x=0;x<TempList.size();x++)
			if(!stricmp(TempList[x].szBuf,tmp.szBuf))
			{
				bMatch=TRUE;
				break;
			}
		if(!bMatch)
			TempList.push_back(tmp);
	}
	sprintf(szBuf,"Number of different publishers\t: %u\r\n",TempList.size());
	strcat(szText,szBuf);

	TempList.clear();
	for(i=0;i<uDisplayedCount;i++)
	{
		CListEntry *Entry=pl.GetListEntry(i);
		_tmp tmp;
		Entry->GetProperty("Dumper",tmp.szBuf,sizeof(tmp.szBuf));
		BOOL bMatch=FALSE;
		for(UINT x=0;x<TempList.size();x++)
			if(!strcmp(TempList[x].szBuf,tmp.szBuf))
			{
				bMatch=TRUE;
				break;
			}
		if(!bMatch)
			TempList.push_back(tmp);
	}
	sprintf(szBuf,"Number of different dumpers\t: %u",TempList.size());
	strcat(szText,szBuf);

	SetDlgItemText(m_hWnd,IDC_EDIT_STATISTICS,szText);
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

void CAboutDialog::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem)
{
	switch(lpDrawItem->CtlType)
	{
	case ODT_BUTTON:
		switch(lpDrawItem->CtlID)
		{
		case IDC_BUTTON_HOMEPAGE:
		case IDC_BUTTON_MAILTO:
		case IDC_BUTTON_ZDSPCARCHIVE:
		case IDC_BUTTON_SNESAMPSPCARCHIVE:
			DrawLinkButton(lpDrawItem);
			break;
		}
		break;
	}
	CWnd::OnDrawItem(hwnd,lpDrawItem);
}

void CAboutDialog::DrawLinkButton(const DRAWITEMSTRUCT *lpDrawItem)
{
	TCHAR szBuf[100];
	GetDlgItemText(m_hWnd,lpDrawItem->CtlID,szBuf,sizeof(szBuf));
	SetTextColor(lpDrawItem->hDC,lpDrawItem->itemState&ODS_SELECTED?RGB(255,0,0):RGB(0,0,255));
	DrawText(lpDrawItem->hDC,szBuf,strlen(szBuf),(LPRECT)&lpDrawItem->rcItem,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX);
}

LRESULT CAboutDialog::OnDestroy(HWND hwnd)
{
	DeleteObject(m_hFont);
	return CWnd::OnDestroy(hwnd);
}

LRESULT CAboutDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	TCHAR szProgramLocation[MAX_PATH];
	GetModuleFileName(app.m_hInstance,szProgramLocation,sizeof(szProgramLocation));
	TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szProgramLocation,szDrv,szDir,NULL,NULL);
	sprintf(szProgramLocation,"%s%s",szDrv,szDir);

	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_BUTTON_HOMEPAGE:
			strcat(szProgramLocation,"Super Jukebox Homepage.url");
			if((INT)ShellExecute(m_hWnd,NULL,szProgramLocation,NULL,NULL,SW_SHOWDEFAULT)<=32)
				DisplayError(m_hWnd,FALSE,"Failed to open URL file.");
			break;
		case IDC_BUTTON_MAILTO:
			strcat(szProgramLocation,"EMail Marius Fodor.url");
			if((INT)ShellExecute(m_hWnd,NULL,szProgramLocation,NULL,NULL,SW_SHOWDEFAULT)<=32)
				DisplayError(m_hWnd,FALSE,"Failed to open URL file.");
			break;
		case IDC_BUTTON_ZDSPCARCHIVE:
			strcat(szProgramLocation,"Zophar's Domain SPC Archive.url");
			if((INT)ShellExecute(m_hWnd,NULL,szProgramLocation,NULL,NULL,SW_SHOWDEFAULT)<=32)
				DisplayError(m_hWnd,FALSE,"Failed to open URL file.");
			break;
		case IDC_BUTTON_SNESAMPSPCARCHIVE:
			strcat(szProgramLocation,"SNESamp Homepage.url");
			if((INT)ShellExecute(m_hWnd,NULL,szProgramLocation,NULL,NULL,SW_SHOWDEFAULT)<=32)
				DisplayError(m_hWnd,FALSE,"Failed to open URL file.");
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}
