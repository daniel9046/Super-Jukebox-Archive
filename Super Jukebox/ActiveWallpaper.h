// ActiveWallpaper.h: interface for the CActiveWallpaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVEWALLPAPER_H__CD738DA0_B235_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_ACTIVEWALLPAPER_H__CD738DA0_B235_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int (*AW1_SETPARAMETERSPROC)(HWND,LPRECT,UEMATSUCONFIG*,BOOL);
typedef int (*AW1_RENDERPROC)(UEMATSUVISPARAMS*);
typedef int (*AW1_STARTPROC)();
typedef int (*AW1_STOPPROC)();
typedef int (*AW1_GETNAMEPROC)(char*,int);
typedef int (*AW1_GETVERSIONPROC)();
typedef int (*AW1_CONFIGUREPROC)(HWND);
typedef int (*AW1_ABOUTPROC)(HWND);
typedef int (*AW1_SAFETORENDERPROC)();

class CActiveWallpaper  
{
public:
	BOOL ResumeThread();
	BOOL SuspendThread();
	BOOL IsLoaded();
	BOOL SetThreadPriority(INT nPriority);
	BOOL SafeToRender();
	INT GetVersion();
	BOOL Configure(HWND hWndParent);
	BOOL About(HWND hWndParent);
	BOOL IsRunning();
	BOOL GetName(LPTSTR lpszName,INT nTextMax);
	BOOL Render(UEMATSUVISPARAMS &uvp);
	BOOL Stop();
	BOOL Start();
	BOOL SetParameters();
	BOOL UnloadPlugin();
	BOOL LoadPlugin(LPCTSTR lpszDllName);
	CActiveWallpaper();
	virtual ~CActiveWallpaper();

private:
	INT m_nPriority;
	HWND m_hWndOldFocus;
	static void __cdecl Thread(void *p);
	unsigned long m_ulThread;
	HINSTANCE m_hPluginLib;
	volatile BOOL m_bRunning,m_bLibLoaded,m_bKillThread;
	AW1_SETPARAMETERSPROC m_pAW1_SetParameters;
	AW1_RENDERPROC m_pAW1_Render;
	AW1_STARTPROC m_pAW1_Start;
	AW1_STOPPROC m_pAW1_Stop;
	AW1_GETNAMEPROC m_pAW1_GetName;
	AW1_GETVERSIONPROC m_pAW1_GetVersion;
	AW1_CONFIGUREPROC m_pAW1_Configure;
	AW1_ABOUTPROC m_pAW1_About;
	AW1_SAFETORENDERPROC m_pAW1_SafeToRender;
};

#endif // !defined(AFX_ACTIVEWALLPAPER_H__CD738DA0_B235_11D4_B2F9_00D0B7AE5907__INCLUDED_)
