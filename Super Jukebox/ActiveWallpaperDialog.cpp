// ActiveWallpaperDialog.cpp: implementation of the CActiveWallpaperDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActiveWallpaperDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActiveWallpaperDialog::CActiveWallpaperDialog()
{
	m_uID=IDD_ACTIVEWALLPAPER;
	m_szCurrentPlugin[0]='\0';
	m_Names.clear();
	m_DllNames.clear();
}

CActiveWallpaperDialog::~CActiveWallpaperDialog()
{
	for(INT i=0;i<m_Names.size();i++)
		delete[]m_Names[i];
	m_Names.clear();
	for(i=0;i<m_DllNames.size();i++)
		delete[]m_DllNames[i];
	m_DllNames.clear();
}

void CActiveWallpaperDialog::InitMembers()
{
	m_Names.push_back(new TCHAR[MAX_PATH]);
	strncpy(m_Names[m_Names.size()-1],"(none)",MAX_PATH);
	m_DllNames.push_back(new TCHAR[MAX_PATH]);
	strncpy(m_DllNames[m_DllNames.size()-1],"(none)",MAX_PATH);

	TCHAR szPluginDir[MAX_PATH];
	GetModuleFileName(NULL,szPluginDir,sizeof(szPluginDir));
	TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szPluginDir,szDrv,szDir,NULL,NULL);
	sprintf(szPluginDir,"%s%sPlugins\\*.dll",szDrv,szDir);

	HANDLE hFindFile;
	BOOL bFirst=TRUE;
	WIN32_FIND_DATA wfd;
	while(bFirst?(hFindFile=FindFirstFile(szPluginDir,&wfd))!=INVALID_HANDLE_VALUE:FindNextFile(hFindFile,&wfd))
	{
		if(m_aw.LoadPlugin(wfd.cFileName))
		{
			TCHAR szName[MAX_PATH];
			m_aw.GetName(szName,sizeof(szName));
			m_Names.push_back(new TCHAR[MAX_PATH]);
			strncpy(m_Names[m_Names.size()-1],szName,MAX_PATH);
			m_DllNames.push_back(new TCHAR[MAX_PATH]);
			strncpy(m_DllNames[m_DllNames.size()-1],wfd.cFileName,MAX_PATH);
			m_aw.UnloadPlugin();
		}
		bFirst=FALSE;
	}
	FindClose(hFindFile);
	strncpy(m_szCurrentPlugin,app.m_Reg.m_szActiveWallpaperPlugin,sizeof(m_szCurrentPlugin));

	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	for(INT i=0;i<sizeof(nThreadPriorities)/sizeof(nThreadPriorities[0]);i++)
		if(app.m_Reg.m_dwAWThreadPriority==nThreadPriorities[i])
		{
			m_nPriority=i;
			break;
		}
	m_bDblBuffer=app.m_Reg.m_bDblBufferAW;
}

BOOL CActiveWallpaperDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	for(INT i=0;i<m_Names.size();i++)
	{
		SendDlgItemMessage(m_hWnd,IDC_LIST_AW,LB_ADDSTRING,0,(LPARAM)m_Names[i]);
		if(!stricmp(m_szCurrentPlugin,m_DllNames[i]))
		{
			SendDlgItemMessage(m_hWnd,IDC_LIST_AW,LB_SETCURSEL,i,0);
			if(i)
				m_aw.LoadPlugin(m_DllNames[i]);
		}
	}
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_AW_CONFIGURE),stricmp(m_szCurrentPlugin,"(none)"));
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_AW_ABOUT),stricmp(m_szCurrentPlugin,"(none)"));

	LPCTSTR lpszPriorities[]={"Idle","Lowest","Below Normal","Normal","Above Normal","Highest","Time Critical"};
	for(i=0;i<sizeof(lpszPriorities)/sizeof(lpszPriorities[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_AW_PRIORITY,CB_ADDSTRING,0,(LPARAM)lpszPriorities[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_AW_PRIORITY,CB_SETCURSEL,m_nPriority,0);
	CheckDlgButton(m_hWnd,IDC_CHECK_AW_DBLBUF,m_bDblBuffer?BST_CHECKED:BST_UNCHECKED);
	return FALSE;
}

BOOL CActiveWallpaperDialog::Validate()
{
	m_aw.UnloadPlugin();
	m_nPriority=SendDlgItemMessage(m_hWnd,IDC_COMBO_AW_PRIORITY,CB_GETCURSEL,0,0);
	m_bDblBuffer=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AW_DBLBUF)==BST_CHECKED;
	return TRUE;
}

BOOL CActiveWallpaperDialog::CarryOutActions()
{
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	if(stricmp(m_szCurrentPlugin,app.m_Reg.m_szActiveWallpaperPlugin))
	{
		if(app.m_MainWindow.m_ActiveWallpaper.IsLoaded())
			app.m_MainWindow.m_ActiveWallpaper.UnloadPlugin();

		strncpy(app.m_Reg.m_szActiveWallpaperPlugin,m_szCurrentPlugin,sizeof(app.m_Reg.m_szActiveWallpaperPlugin));
		if(app.m_Reg.m_bUseActiveWallpaper=stricmp(app.m_Reg.m_szActiveWallpaperPlugin,"(none)"))
		{
			if(!app.m_MainWindow.m_ActiveWallpaper.LoadPlugin(app.m_Reg.m_szActiveWallpaperPlugin))
			{
				TCHAR szBuf[MAX_PATH*2];
				sprintf(szBuf,"Failed to load the Active Wallpaper plugin (%s).",app.m_Reg.m_szActiveWallpaperPlugin);
				DisplayError(m_hWnd,FALSE,szBuf);
			}
			else
			{
				app.m_Reg.m_bDblBufferAW=m_bDblBuffer;
				app.m_MainWindow.m_ActiveWallpaper.SetParameters();
				app.m_Reg.m_dwAWThreadPriority=nThreadPriorities[m_nPriority];
				app.m_MainWindow.m_ActiveWallpaper.SetThreadPriority(app.m_Reg.m_dwAWThreadPriority);
				if(!app.m_Player.IsStopped()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
					app.m_MainWindow.m_ActiveWallpaper.Start();
			}
		}
	}
	if(app.m_Reg.m_bDblBufferAW!=m_bDblBuffer)
	{
		app.m_Reg.m_bDblBufferAW=m_bDblBuffer;
		if(app.m_Reg.m_bUseActiveWallpaper)
			app.m_MainWindow.m_ActiveWallpaper.SetParameters();
	}
	if(app.m_Reg.m_dwAWThreadPriority!=nThreadPriorities[m_nPriority])
	{	
		app.m_Reg.m_dwAWThreadPriority=nThreadPriorities[m_nPriority];
		if(app.m_Reg.m_bUseActiveWallpaper)
			app.m_MainWindow.m_ActiveWallpaper.SetThreadPriority(app.m_Reg.m_dwAWThreadPriority);
	}

	if(!app.m_Reg.m_bUseActiveWallpaper&&stricmp(app.m_Reg.m_szActiveWallpaperPlugin,"(none)"))
	{
		app.m_Reg.m_bUseActiveWallpaper=TRUE;
		app.m_MainWindow.m_ToolBar.CheckButton(IDC_BUTTON_AW,app.m_Reg.m_bUseActiveWallpaper);
		app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_AW);
	}
	else
		app.m_MainWindow.m_ToolBar.CheckButton(IDC_BUTTON_AW,app.m_Reg.m_bUseActiveWallpaper);
	app.m_MainWindow.m_ToolBar.EnableButton(IDC_BUTTON_AW,stricmp(app.m_Reg.m_szActiveWallpaperPlugin,"(none)"));

	return TRUE;
}

LRESULT CActiveWallpaperDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	INT nIndex;

	switch(codeNotify)
	{
	case LBN_SELCHANGE:
		nIndex=SendDlgItemMessage(m_hWnd,IDC_LIST_AW,LB_GETCURSEL,0,0);
		if(nIndex!=LB_ERR)
			strncpy(m_szCurrentPlugin,m_DllNames[nIndex],sizeof(m_szCurrentPlugin));
		EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_AW_CONFIGURE),stricmp(m_szCurrentPlugin,"(none)")&&nIndex!=LB_ERR);
		EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_AW_ABOUT),stricmp(m_szCurrentPlugin,"(none)")&&nIndex!=LB_ERR);
		m_aw.UnloadPlugin();
		if(nIndex>0&&nIndex!=LB_ERR)
			m_aw.LoadPlugin(m_DllNames[nIndex]);
		break;
	case LBN_DBLCLK:
		nIndex=SendDlgItemMessage(m_hWnd,IDC_LIST_AW,LB_GETCURSEL,0,0);
		if(nIndex>0&&nIndex!=LB_ERR&&!m_aw.Configure(m_hWnd))
			DisplayError(m_hWnd,FALSE,"Configuration failed.");
		break;
	case BN_CLICKED:
		switch(id)
		{
		case IDC_BUTTON_AW_CONFIGURE:
			if(!m_aw.Configure(m_hWnd))
				DisplayError(m_hWnd,FALSE,"Configuration failed.");
			break;
		case IDC_BUTTON_AW_ABOUT:
			if(!m_aw.About(m_hWnd))
				DisplayError(m_hWnd,FALSE,"About box failed.");
			break;
		}
		break;
	}
	return 0;
}
