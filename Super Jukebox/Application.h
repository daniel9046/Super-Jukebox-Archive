// Application.h: interface for the CApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLICATION_H__CE9216E4_832B_11D4_8925_009027C5CF93__INCLUDED_)
#define AFX_APPLICATION_H__CE9216E4_832B_11D4_8925_009027C5CF93__INCLUDED_

#include "Player.h"
#include "MainWindow.h"
#include "Registry.h"	// Added by ClassView

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CApplication
{
public:
	BOOL AssociateFileExtension(LPCTSTR lpszExt,LPCTSTR lpszType);
	BOOL DisassociateFileExtension(LPCTSTR lpszExt);
	BOOL UnregisterFileType(LPCTSTR lpszType);
	BOOL AddFileTypeCommand(LPCTSTR lpszType,LPCTSTR lpszName,LPCTSTR lpszCommand);
	BOOL RegisterFileType(LPCTSTR lpszType,LPCTSTR lpszDescription,LPCTSTR lpszDefIcon,LPCTSTR lpszDefAction);
	LPTSTR GetCommandLine();
	void ReRun(BOOL bReRun);
	BOOL ReRun();
	BOOL CreateQuickLaunchIcon();
	BOOL CreateDesktopIcon();
	BOOL CreateStartMenuGroup();
	void EnableAccels(BOOL bEnable);
	CRegistry m_Reg;
	BOOL IsCallInternal(BOOL bInternalCall);
	BOOL IsCallInternal();
	WPARAM ProcessMessageQueue();
	BOOL Init(HINSTANCE hInstance,LPSTR lpCmdLine,int nCmdShow);
	CApplication();
	virtual ~CApplication();
	TCHAR m_szAppName[MAX_LOADSTRING];
	HINSTANCE m_hInstance;
	CPlayer m_Player;
	CMainWindow m_MainWindow;
	TCHAR m_szHelpFile[MAX_PATH];
	
private:
	LPTSTR m_lpCmdLine;
	BOOL m_bReRun;
	BOOL m_bNoAccels;
	BOOL m_bInternalCall;
};

#endif // !defined(AFX_APPLICATION_H__CE9216E4_832B_11D4_8925_009027C5CF93__INCLUDED_)
