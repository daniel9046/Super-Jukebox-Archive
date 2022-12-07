// ActiveWallpaper.cpp: implementation of the CActiveWallpaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActiveWallpaper.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActiveWallpaper::CActiveWallpaper()
{
	m_ulThread=-1;
	m_bRunning=m_bLibLoaded=m_bKillThread=FALSE;
	m_hPluginLib=NULL;
	m_nPriority=app.m_Reg.m_dwAWThreadPriority;
}

CActiveWallpaper::~CActiveWallpaper()
{
}

BOOL CActiveWallpaper::LoadPlugin(LPCTSTR lpszDllName)
{
	if(m_bLibLoaded)
		return FALSE;

	TCHAR szFullDllName[MAX_PATH],szProgramName[MAX_PATH];
	GetModuleFileName(NULL,szProgramName,sizeof(szProgramName));
	TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szProgramName,szDrive,szDir,NULL,NULL);
	sprintf(szFullDllName,"%s%sPlugins\\%s",szDrive,szDir,lpszDllName);

	m_hPluginLib=LoadLibrary(szFullDllName);
	if(!m_hPluginLib)
		return FALSE;

	m_pAW1_GetVersion=(AW1_GETVERSIONPROC)GetProcAddress(m_hPluginLib,"AW1_GetVersion");
	if(!m_pAW1_GetVersion||(*m_pAW1_GetVersion)()>1)
	{
		FreeLibrary(m_hPluginLib);
		m_hPluginLib=NULL;
		return FALSE;
	}

	m_pAW1_SetParameters=(AW1_SETPARAMETERSPROC)GetProcAddress(m_hPluginLib,"AW1_SetParameters");
	m_pAW1_Render=(AW1_RENDERPROC)GetProcAddress(m_hPluginLib,"AW1_Render");
	m_pAW1_Start=(AW1_STARTPROC)GetProcAddress(m_hPluginLib,"AW1_Start");
	m_pAW1_Stop=(AW1_STOPPROC)GetProcAddress(m_hPluginLib,"AW1_Stop");
	m_pAW1_GetName=(AW1_GETNAMEPROC)GetProcAddress(m_hPluginLib,"AW1_GetName");
	m_pAW1_Configure=(AW1_CONFIGUREPROC)GetProcAddress(m_hPluginLib,"AW1_Configure");
	m_pAW1_About=(AW1_ABOUTPROC)GetProcAddress(m_hPluginLib,"AW1_About");
	m_pAW1_SafeToRender=(AW1_SAFETORENDERPROC)GetProcAddress(m_hPluginLib,"AW1_SafeToRender");
	if(!m_pAW1_SetParameters||
	   !m_pAW1_Render||
	   !m_pAW1_Start||
	   !m_pAW1_Stop||
	   !m_pAW1_GetName||
	   !m_pAW1_Configure||
	   !m_pAW1_About||
	   !m_pAW1_SafeToRender)
	{
		FreeLibrary(m_hPluginLib);
		m_hPluginLib=NULL;
		return FALSE;
	}
	m_bLibLoaded=TRUE;
	return TRUE;
}

BOOL CActiveWallpaper::UnloadPlugin()
{
	if(!m_bLibLoaded)
		return FALSE;

	if(m_bRunning)
		Stop();

	FreeLibrary(m_hPluginLib);
	m_hPluginLib=NULL;
	m_bLibLoaded=FALSE;
	return TRUE;
}

BOOL CActiveWallpaper::SetParameters()
{
	if(!m_bLibLoaded)
		return FALSE;

	if(m_bRunning)
	{
		m_bKillThread=TRUE;
		if(WaitForSingleObject((HANDLE)m_ulThread,3000)==WAIT_TIMEOUT)
		{
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,"The Active Wallpaper thread won't cooperate. Press OK to terminate.");
			TerminateThread((HANDLE)m_ulThread,0);
		}
	}
	
	UEMATSUCONFIG cfg;
	app.m_Player.GetConfiguration(cfg);
	if(!(*m_pAW1_SetParameters)(app.m_MainWindow.m_hWnd,&app.m_MainWindow.m_ClientRect,&cfg,app.m_Reg.m_bDblBufferAW))
		return FALSE;

	if(m_bRunning)
	{
		m_bKillThread=FALSE;
		m_ulThread=_beginthread(Thread,0,this);
		if(m_ulThread==-1)
		{
			m_bRunning=FALSE;
			return FALSE;
		}
		::SetThreadPriority((HANDLE)m_ulThread,m_nPriority);
	}

	return TRUE;
}

BOOL CActiveWallpaper::Start()
{
	if(!m_bLibLoaded)
		return FALSE;

	if(m_ulThread==-1&&!app.m_Player.IsStopped()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
	{
		if(!(*m_pAW1_Start)())
		{
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,"Failed to start the Active Wallpaper plugin.");
			return FALSE;
		}

		m_bKillThread=FALSE;
		m_ulThread=_beginthread(Thread,0,this);
		if(m_ulThread!=-1)
		{
			m_bRunning=TRUE;
			::SetThreadPriority((HANDLE)m_ulThread,m_nPriority);

			if(!app.IsCallInternal())
			{
				if(app.m_MainWindow.m_FileList.IsVisible()||app.m_MainWindow.m_PlayList.IsVisible())
					m_hWndOldFocus=GetFocus();
				else
					m_hWndOldFocus=NULL;
				if(app.m_Reg.m_bShowFileList)
					app.m_MainWindow.m_FileList.IsVisible(FALSE);
				if(app.m_Reg.m_bShowPlayList)
					app.m_MainWindow.m_PlayList.IsVisible(FALSE);

				app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_FILELIST,app.m_MainWindow.m_FileList.IsVisible());
				app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_FILELIST,!IsRunning());
				app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_PLAYLIST,app.m_MainWindow.m_PlayList.IsVisible());
				app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_PLAYLIST,!IsRunning());
			}
			
			return TRUE;
		}
	}
	else
	{
		if(app.m_Reg.m_bShowFileList)
			app.m_MainWindow.m_FileList.IsVisible(TRUE);
		if(app.m_Reg.m_bShowPlayList)
			app.m_MainWindow.m_PlayList.IsVisible(TRUE);
		if(app.m_MainWindow.m_FileList.m_hWnd==m_hWndOldFocus||app.m_MainWindow.m_PlayList.m_hWnd==m_hWndOldFocus)
		{
			SetFocus(m_hWndOldFocus);
			m_hWndOldFocus=NULL;
		}
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);

		app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_FILELIST,app.m_MainWindow.m_FileList.IsVisible());
		app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_FILELIST,!IsRunning());
		app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_PLAYLIST,app.m_MainWindow.m_PlayList.IsVisible());
		app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_PLAYLIST,!IsRunning());
	}

	return FALSE;
}

BOOL CActiveWallpaper::Stop()
{
	if(!m_bLibLoaded)
		return FALSE;

	if(m_ulThread!=-1)
	{
		m_bKillThread=TRUE;
		if(WaitForSingleObject((HANDLE)m_ulThread,3000)==WAIT_TIMEOUT)
		{
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,"The Active Wallpaper thread won't cooperate. Press OK to terminate.");
			TerminateThread((HANDLE)m_ulThread,0);
		}
		if(!(*m_pAW1_Stop)())
		{
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,"Failed to stop the Active Wallpaper plugin.");
			return FALSE;
		}
		m_ulThread=-1;
		m_bRunning=FALSE;

		if(!app.IsCallInternal())
		{
			if(app.m_Reg.m_bShowFileList)
				app.m_MainWindow.m_FileList.IsVisible(TRUE);
			if(app.m_Reg.m_bShowPlayList)
				app.m_MainWindow.m_PlayList.IsVisible(TRUE);
			if(app.m_MainWindow.m_FileList.m_hWnd==m_hWndOldFocus||app.m_MainWindow.m_PlayList.m_hWnd==m_hWndOldFocus)
			{
				SetFocus(m_hWndOldFocus);
				m_hWndOldFocus=NULL;
			}
			app.m_MainWindow.AdjustLayout();
			app.m_MainWindow.RePaint(NULL);

			app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_FILELIST,app.m_MainWindow.m_FileList.IsVisible());
			app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_FILELIST,!IsRunning());
			app.m_MainWindow.m_ToolBar.CheckButton(ID_WINDOWS_PLAYLIST,app.m_MainWindow.m_PlayList.IsVisible());
			app.m_MainWindow.m_ToolBar.EnableButton(ID_WINDOWS_PLAYLIST,!IsRunning());
		}
		return TRUE;
	}
	return FALSE;
}

void __cdecl CActiveWallpaper::Thread(void *p)
{
	CActiveWallpaper *aw=(CActiveWallpaper*)p;

	UEMATSUVISPARAMS uvp,old_uvp;
	app.m_Player.GetVisParams(old_uvp);
	
	while(!aw->m_bKillThread)
	{
		app.m_Player.GetVisParams(uvp);
		if((uvp.uCount!=old_uvp.uCount||app.m_Player.IsPaused())&&aw->SafeToRender())
		{
			if(!aw->Render(uvp))
				DisplayError(app.m_MainWindow.m_hWnd,FALSE,"The Active Wallpaper plugin has failed to render.");
			old_uvp=uvp;
		}
		Sleep(10);
	}
}

BOOL CActiveWallpaper::Render(UEMATSUVISPARAMS &uvp)
{
	if(!m_bLibLoaded)
		return FALSE;

	return (*m_pAW1_Render)(&uvp);
}

BOOL CActiveWallpaper::GetName(LPTSTR lpszName, INT nTextMax)
{
	return (*m_pAW1_GetName)(lpszName,nTextMax);
}

BOOL CActiveWallpaper::IsRunning()
{
	return m_bRunning;
}

INT CActiveWallpaper::GetVersion()
{
	return (*m_pAW1_GetVersion)();
}

BOOL CActiveWallpaper::Configure(HWND hWndParent)
{
	return (*m_pAW1_Configure)(hWndParent);
}

BOOL CActiveWallpaper::About(HWND hWndParent)
{
	return (*m_pAW1_About)(hWndParent);
}

BOOL CActiveWallpaper::SafeToRender()
{
	return (*m_pAW1_SafeToRender)();
}

BOOL CActiveWallpaper::SetThreadPriority(INT nPriority)
{
	m_nPriority=nPriority;
	if(m_ulThread!=-1)
		return ::SetThreadPriority((HANDLE)m_ulThread,m_nPriority);
	return TRUE;
}

BOOL CActiveWallpaper::IsLoaded()
{
	return m_bLibLoaded;
}

BOOL CActiveWallpaper::SuspendThread()
{
	if(m_ulThread!=-1)
		::SuspendThread((HANDLE)m_ulThread);
	return TRUE;
}

BOOL CActiveWallpaper::ResumeThread()
{
	if(m_ulThread!=-1)
		::ResumeThread((HANDLE)m_ulThread);
	return TRUE;
}
