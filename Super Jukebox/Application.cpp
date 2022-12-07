// Application.cpp: implementation of the CApplication class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Application.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApplication::CApplication()
{
	m_bInternalCall=FALSE;
	m_bNoAccels=FALSE;
	m_bReRun=FALSE;
}

CApplication::~CApplication()
{

}

BOOL CApplication::Init(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow)
{
	m_hInstance=hInstance;
	m_lpCmdLine=lpCmdLine;

	if(!m_bReRun)
	{
		m_Reg.Reset();
		m_Reg.Load();
	}
	SetPriorityClass(GetCurrentProcess(),m_Reg.m_dwPriorityClass);
	SetThreadPriority(GetCurrentThread(),m_Reg.m_dwMainThreadPriority);

	srand((unsigned)time(NULL));

	if(CoInitialize(NULL)!=S_OK)
		return FALSE;

	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize=sizeof(iccx);
	iccx.dwICC=ICC_WIN95_CLASSES|ICC_PAGESCROLLER_CLASS|ICC_COOL_CLASSES|ICC_USEREX_CLASSES;
	if(!InitCommonControlsEx(&iccx))
		return FALSE;

	LoadString(m_hInstance,IDS_APP_NAME,m_szAppName,MAX_LOADSTRING);

	TCHAR szProgramLocation[MAX_PATH],szBuf[MAX_PATH+20];
	GetModuleFileName(m_hInstance,szProgramLocation,sizeof(szProgramLocation));
	TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szProgramLocation,szDrv,szDir,NULL,NULL);
	sprintf(m_szHelpFile,"%s%sTips.hlp",szDrv,szDir);

	if(m_Reg.m_bRegFileTypes)
	{
		for(UINT i=0;i<strlen(m_Reg.m_szAssocExt);i++)
		{
			UINT c=0;
			TCHAR szExt[4];
			while(i<strlen(m_Reg.m_szAssocExt)&&m_Reg.m_szAssocExt[i]!=' ')
				szExt[c++]=m_Reg.m_szAssocExt[i++];
			szExt[c]='\0';
			if(!stricmp(szExt,"PL"))
			{
				sprintf(szBuf,"%s,0",szProgramLocation);
				if(!RegisterFileType("plfile","Super Jukebox Playlist",szBuf,"Open"))
					DisplayError(NULL,FALSE,NULL);
				sprintf(szBuf,"\"%s\" %%1",szProgramLocation);
				if(!AddFileTypeCommand("plfile","Open",szBuf))
					DisplayError(NULL,FALSE,NULL);
				TCHAR szWinDir[MAX_PATH];
				GetWindowsDirectory(szWinDir,sizeof(szWinDir));
				sprintf(szBuf,"\"%s\\Notepad.exe\" %%1",szWinDir);
				if(!AddFileTypeCommand("plfile","Edit With Notepad",szBuf))
					DisplayError(NULL,FALSE,NULL);

				if(!AssociateFileExtension(szExt,"plfile"))
					DisplayError(NULL,FALSE,NULL);
			}
			else
			{
				sprintf(szBuf,"%s,1",szProgramLocation);
				if(!RegisterFileType("spcfile","Super Nintendo Song",szBuf,"Play"))
					DisplayError(NULL,FALSE,NULL);
				sprintf(szBuf,"\"%s\" %%1",szProgramLocation);
				if(!AddFileTypeCommand("spcfile","Play",szBuf))
					DisplayError(NULL,FALSE,NULL);

				if(!AssociateFileExtension(szExt,"spcfile"))
					DisplayError(NULL,FALSE,NULL);
			}
		}
	}

	UEMATSUCONFIG cfg;
	m_Player.GetConfiguration(cfg);
	cfg.uSampleRate=m_Reg.m_dwSampleRate;
	cfg.uInterpolation=m_Reg.m_dwInterpolation;
	INT nMixers[]={MIX_NONE,MIX_80386,MIX_MMX,MIX_3DNOW,MIX_SSE};
	cfg.uMixingRoutine=nMixers[m_Reg.m_dwMixingMode];
	cfg.uBufferLength=m_Reg.m_dwBufferLength;
	cfg.uBitsPerSample=m_Reg.m_dwSampleSize;
	cfg.uChannels=m_Reg.m_dwChannels;
	cfg.uAPR=m_Reg.m_dwAPR;
	cfg.uThreshold=7872;
	cfg.fMiscOptions=(m_Reg.m_bUseLPF?DSP_LOW:0)|(m_Reg.m_bUseOldBRE?DSP_OLDSMP:0)|(m_Reg.m_bSurroundSound?DSP_SURND:0)|(m_Reg.m_bReverseStereo?DSP_REVERSE:0);
	m_Player.SetConfiguration(cfg);
	m_Player.SetThreadPriority(m_Reg.m_dwMixingThreadPriority);
	m_Player.MuteChannels(~m_Reg.m_dwActiveChannels);
	m_Player.SetPreAmp(m_Reg.m_dwPreamp);
	m_Player.SetSongSpeed(pow(2,(INT)m_Reg.m_dwTempo/50.0)*65536.0);
	m_Player.SetSongPitch((UINT)(pow(1.0145453349375236,(INT)m_Reg.m_dwPitch)*32000));
	m_Player.SetMainSS((m_Reg.m_dwMainSS<<16)/100);
	m_Player.SetEchoSS(((INT)m_Reg.m_dwEchoSS<<15)/100);

	if(!m_MainWindow.RegisterClass(m_hInstance))
		return FALSE;
	if(!m_MainWindow.Create(nCmdShow))
		return FALSE;

	m_bReRun=FALSE;
	return TRUE;
}

WPARAM CApplication::ProcessMessageQueue()
{
	MSG msg;
	HACCEL hAccelTable;

	hAccelTable=LoadAccelerators(m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_SUPER_JUKEBOX));
	if(!hAccelTable)
		DisplayError(m_MainWindow.m_hWnd,TRUE,NULL);

	BOOL bPass;
	HWND hWndFore,hWndFocus;
	TCHAR szParentWndClass[100];
	while(GetMessage(&msg,NULL,0,0)>0)
	{
		hWndFore=GetForegroundWindow();
		hWndFocus=GetFocus();
		szParentWndClass[0]='\0';
		while(hWndFocus)
		{
			GetClassName(hWndFocus,szParentWndClass,sizeof(szParentWndClass));
			if(!stricmp(szParentWndClass,"#32770"))
				break;
			hWndFocus=GetParent(hWndFocus);
		}

		bPass=TRUE;
		if(!m_bNoAccels&&hWndFore==m_MainWindow.m_hWnd)
			bPass=TranslateAccelerator(m_MainWindow.m_hWnd,hAccelTable,&msg)?FALSE:TRUE;
		if(bPass&&!stricmp(szParentWndClass,"#32770"))
			bPass=IsDialogMessage(hWndFore,&msg)?FALSE:TRUE;

		if(bPass)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if(!m_bReRun&&m_Reg.m_bSaveSettingsOnExit)
		m_Reg.Save();

	CoUninitialize();

	return msg.wParam;
}

BOOL CApplication::IsCallInternal()
{
	return m_bInternalCall;
}

BOOL CApplication::IsCallInternal(BOOL bInternalCall)
{
	BOOL b=m_bInternalCall;
	m_bInternalCall=bInternalCall;
	return b;
}

void CApplication::EnableAccels(BOOL bEnable)
{
	m_bNoAccels=bEnable^1;
}

BOOL CApplication::CreateStartMenuGroup()
{
	TCHAR szStartMenuDir[MAX_PATH];

	GetFolderPath(NULL,szStartMenuDir,CSIDL_PROGRAMS);
	char szBuf[MAX_PATH];
	sprintf(szBuf,"%s\\Super Jukebox",szStartMenuDir);
	WIN32_FIND_DATA w32fd;
	HANDLE hFindFile;
	if((hFindFile=FindFirstFile(szBuf,&w32fd))==INVALID_HANDLE_VALUE)
	{
		if(!CreateDirectory(szBuf,NULL))
			return FALSE;
	}
	else
		FindClose(hFindFile);

	TCHAR szProgramDir[MAX_PATH];
	GetModuleFileName(NULL,szProgramDir,sizeof(szProgramDir));
	TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
	_splitpath(szProgramDir,szDrv,szDir,NULL,NULL);
	sprintf(szProgramDir,"%s%s",szDrv,szDir);
	LPCTSTR lpszFileNames[]={"Super Jukebox.exe","My Thoughts.doc","UnInstall.exe"};
	for(INT i=0;i<sizeof(lpszFileNames)/sizeof(lpszFileNames[0]);i++)
	{
		char szBuf0[MAX_PATH],szBuf1[MAX_PATH],szBuf2[MAX_PATH];
		sprintf(szBuf0,"%s%s",szProgramDir,lpszFileNames[i]);
		_splitpath(lpszFileNames[i],NULL,NULL,szBuf2,NULL);
		strcat(szBuf2,".lnk");
		sprintf(szBuf1,"%s\\%s",szBuf,szBuf2);
		HRESULT hRes;
		hRes=CreateLink(szBuf1,szBuf0,NULL);
		if(!SUCCEEDED(hRes))
			return FALSE;
	}
	return TRUE;
}

BOOL CApplication::CreateDesktopIcon()
{
	TCHAR szDesktopDir[MAX_PATH];

	GetFolderPath(NULL,szDesktopDir,CSIDL_DESKTOP);
	TCHAR szProgram[MAX_PATH];
	GetModuleFileName(NULL,szProgram,sizeof(szProgram));
	char szBuf[MAX_PATH];
	sprintf(szBuf,"%s\\Super Jukebox.lnk",szDesktopDir);
	HRESULT hRes;
	hRes=CreateLink(szBuf,szProgram,NULL);
	return SUCCEEDED(hRes);
}

BOOL CApplication::CreateQuickLaunchIcon()
{
	TCHAR szQuickLaunchDir[MAX_PATH];

	GetFolderPath(NULL,szQuickLaunchDir,CSIDL_APPDATA);
	strcat(szQuickLaunchDir,"\\Microsoft\\Internet Explorer\\Quick Launch");
	TCHAR szProgram[MAX_PATH];
	GetModuleFileName(NULL,szProgram,sizeof(szProgram));
	char szBuf[MAX_PATH];
	sprintf(szBuf,"%s\\Super Jukebox.lnk",szQuickLaunchDir);
	HRESULT hRes;
	hRes=CreateLink(szBuf,szProgram,NULL);
	return SUCCEEDED(hRes);
}

BOOL CApplication::ReRun()
{
	return m_bReRun;
}

void CApplication::ReRun(BOOL bReRun)
{
	m_bReRun=bReRun;
}

LPTSTR CApplication::GetCommandLine()
{
	return m_lpCmdLine;
}

BOOL CApplication::RegisterFileType(LPCTSTR lpszType, LPCTSTR lpszDescription, LPCTSTR lpszDefIcon, LPCTSTR lpszDefAction)
{
	HKEY hKey;
	TCHAR szBuf[500],szBuf0[500];
	strncpy(szBuf0,lpszType,sizeof(szBuf0));
	strlwr(szBuf0);

	sprintf(szBuf,"SOFTWARE\\Classes\\%s",szBuf0);
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)lpszDescription,strlen(lpszDescription)+1)!=ERROR_SUCCESS)
		return FALSE;
	RegCloseKey(hKey);

	sprintf(szBuf,"SOFTWARE\\Classes\\%s\\DefaultIcon",szBuf0);
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)lpszDefIcon,strlen(lpszDefIcon)+1)!=ERROR_SUCCESS)
		return FALSE;
	RegCloseKey(hKey);

	sprintf(szBuf,"SOFTWARE\\Classes\\%s\\shell",szBuf0);
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)lpszDefAction,strlen(lpszDefAction)+1)!=ERROR_SUCCESS)
		return FALSE;
	RegCloseKey(hKey);

	return TRUE;
}

BOOL CApplication::AddFileTypeCommand(LPCTSTR lpszType,LPCTSTR lpszName, LPCTSTR lpszCommand)
{
	HKEY hKey;
	TCHAR szBuf[500],szBuf0[500];
	strncpy(szBuf0,lpszType,sizeof(szBuf0));
	strlwr(szBuf0);

	sprintf(szBuf,"SOFTWARE\\Classes\\%s\\shell\\%s",szBuf0,lpszName);
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)lpszName,strlen(lpszName)+1)!=ERROR_SUCCESS)
		return FALSE;
	if(RegCreateKeyEx(hKey,"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)lpszCommand,strlen(lpszCommand)+1)!=ERROR_SUCCESS)
		return FALSE;
	RegCloseKey(hKey);

	return TRUE;
}

BOOL CApplication::UnregisterFileType(LPCTSTR lpszType)
{
	TCHAR szBuf[500],szBuf0[500];
	strncpy(szBuf0,lpszType,sizeof(szBuf0));
	strlwr(szBuf0);

	sprintf(szBuf,"SOFTWARE\\Classes\\%s",szBuf0);
	RegDeleteKey(HKEY_LOCAL_MACHINE,szBuf);

	return TRUE;
}

BOOL CApplication::AssociateFileExtension(LPCTSTR lpszExt,LPCTSTR lpszType)
{
	HKEY hKey;
	TCHAR szBuf[500],szBuf0[500],szExt[4];
	strncpy(szExt,lpszExt,sizeof(szExt));
	strlwr(szExt);
	strncpy(szBuf0,lpszType,sizeof(szBuf0));
	strlwr(szBuf0);

	sprintf(szBuf,"SOFTWARE\\Classes\\.%s",szExt);
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuf,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;
	if(RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szBuf0,strlen(szBuf0)+1)!=ERROR_SUCCESS)
		return FALSE;
	RegCloseKey(hKey);
	return TRUE;
}

BOOL CApplication::DisassociateFileExtension(LPCTSTR lpszExt)
{
	TCHAR szBuf[500],szExt[4];
	strncpy(szExt,lpszExt,sizeof(szExt));
	strlwr(szExt);

	sprintf(szBuf,"SOFTWARE\\Classes\\.%s",szExt);
	RegDeleteKey(HKEY_LOCAL_MACHINE,szBuf);
	return TRUE;
}