#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>

#define szAppName "Super Jukebox UnInstall"
#define CSIDL_PROGRAM_FILES 0x26
#define SHGFP_TYPE_CURRENT 0
#define MCI_SUPPORT 1
#define PL_SUPPORT 2
#define SPC_SUPPORT 4
#define DESKTOP_SUPPORT 8
#define STARTMENU_SUPPORT 16
#define QUICKLAUNCH_SUPPORT 32

typedef BOOL (CALLBACK* SHGETSPECIALFOLDERPATHPROC)(HWND,LPTSTR,int,BOOL);

struct
{
	char szName[MAX_PATH];
	int iInstDir;
}InstallationFiles[]=
{
	"Super Jukebox.exe",0,
	"UnInstall.exe",0,
	"My Thoughts.doc",0,
	"Uematsu.dll",0,
	"SNESAPU.DLL",0,
	"ZLIB.DLL",0,
	"UNRAR.DLL",0,
	"UNACE.DLL",0,
	"FFT.DLL",0,
	"MGLFX.DLL",0,
	"Super Jukebox.chm",0,
	"Tips.hlp",0,
	"Uematsu.html",0,
	"Uematsu.h",0,
	"SNESAPU.h",0,
	"Uematsu.lib",0,
	"MCISPC.txt",0,
	"Super Jukebox Homepage.url",0,
	"EMail Marius Fodor.url",0,
	"Zophar's Domain SPC Archive.url",0,
	"SNESamp Homepage.url",0,
	"AW_DISCO.DLL",3,
	"SPC700EMU.DLL",2,
	"IN_SPC.DLL",2,
	"OUT_WAVE.DLL",2,
	"MCISPC.DRV",2,
	"MCISPCDLG.DLL",2,
};

HRESULT _SHGetFolderPath(HWND hwndOwner,int nFolder,HANDLE hToken,DWORD dwFlags,LPTSTR lpszPath)
{
	GetWindowsDirectory(lpszPath,MAX_PATH);
	switch(nFolder)
	{
	case CSIDL_APPDATA:
		strcat(lpszPath,"\\Application Data");
		break;
	case CSIDL_DESKTOP:
		strcat(lpszPath,"\\Desktop");
		break;
	case CSIDL_PROGRAM_FILES:
		strcpy(lpszPath,"C:\\Program Files");
		break;
	case CSIDL_PROGRAMS:
		strcat(lpszPath,"\\Start Menu\\Programs");
		break;
	}
	HRESULT hres=S_FALSE;
	HINSTANCE hInstDll=LoadLibrary("SHFOLDER.DLL");
	if(hInstDll)
	{
		typedef HRESULT (CALLBACK* SHGETFOLDERPATHPROC)(HWND,int,HANDLE,DWORD,LPTSTR);
		SHGETFOLDERPATHPROC pSHGetFolderPath;
		pSHGetFolderPath=(SHGETFOLDERPATHPROC)GetProcAddress(hInstDll,"SHGetFolderPathA");
		if(pSHGetFolderPath)
		{
			char szBuf[MAX_PATH];
			hres=(*pSHGetFolderPath)(hwndOwner,nFolder,hToken,dwFlags,szBuf);
			if(SUCCEEDED(hres))
				strcpy(lpszPath,szBuf);
		}
		FreeLibrary(hInstDll);
	}
	return hres;
}

BOOL _SHGetSpecialFolderPath(HWND hwndOwner,LPTSTR lpszPath,int nFolder,BOOL fCreate)
{
	GetWindowsDirectory(lpszPath,MAX_PATH);
	switch(nFolder)
	{
	case CSIDL_APPDATA:
		strcat(lpszPath,"\\Application Data");
		break;
	case CSIDL_DESKTOP:
		strcat(lpszPath,"\\Desktop");
		break;
	case CSIDL_PROGRAM_FILES:
		strcpy(lpszPath,"C:\\Program Files");
		break;
	case CSIDL_PROGRAMS:
		strcat(lpszPath,"\\Start Menu\\Programs");
		break;
	}
	BOOL bRes=FALSE;
	HINSTANCE hInstDll=LoadLibrary("SHELL32.DLL");
	if(hInstDll)
	{
		typedef BOOL (CALLBACK* SHGETSPECIALFOLDERPATHPROC)(HWND,LPTSTR,int,BOOL);
		SHGETSPECIALFOLDERPATHPROC pSHGetSpecialFolderPath;
		pSHGetSpecialFolderPath=(SHGETSPECIALFOLDERPATHPROC)GetProcAddress(hInstDll,"SHGetSpecialFolderPathA");
		if(pSHGetSpecialFolderPath)
		{
			char szBuf[MAX_PATH];
			bRes=(*pSHGetSpecialFolderPath)(hwndOwner,szBuf,nFolder,fCreate);
			if(bRes==TRUE)
				strcpy(lpszPath,szBuf);
		}
		FreeLibrary(hInstDll);
	}
	return bRes;
}

void GetFolderPath(HWND hwndOwner,LPTSTR lpszPath,int nFolder)
{
	HRESULT hres=_SHGetFolderPath(hwndOwner,nFolder,NULL,SHGFP_TYPE_CURRENT,lpszPath);
	if(!SUCCEEDED(hres))
		_SHGetSpecialFolderPath(hwndOwner,lpszPath,nFolder,FALSE);
}

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR lpCmdLine,int nShowCmd)
{
	int iTotalFiles=sizeof(InstallationFiles)/sizeof(InstallationFiles[0]);
	char szBuffer[MAX_PATH],szBuffer1[MAX_PATH],szBuffer2[MAX_PATH];
	char szInstDir[MAX_PATH],szPluginsDir[MAX_PATH],szWinDir[MAX_PATH],szSysDir[MAX_PATH];
	char szDesktopDir[MAX_PATH],szStartMenuDir[MAX_PATH],szQuickLaunchDir[MAX_PATH];
	DWORD dwInstOptions=0,dwSize=sizeof(DWORD);
	HKEY hKey;

	if(MessageBox(NULL,"Are you sure you want to remove Super Jukebox?",szAppName,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDNO)
		return 0;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
	{
		MessageBox(NULL,"Failed to open the Super Jukebox registry key.",szAppName,MB_ICONERROR);
		return 0;
	}
	RegQueryValueEx(hKey,"Installation options",NULL,NULL,(LPBYTE)&dwInstOptions,&dwSize);
	memset(szInstDir,0,sizeof(szInstDir));
	dwSize=sizeof(szInstDir);
	RegQueryValueEx(hKey,"Installation directory",NULL,NULL,(LPBYTE)szInstDir,&dwSize);
	RegCloseKey(hKey);
	if(szInstDir[strlen(szInstDir)-1]=='\\')
		szInstDir[strlen(szInstDir)-1]='\0';
	sprintf(szPluginsDir,"%s\\Plugins",szInstDir);
	GetWindowsDirectory(szWinDir,MAX_PATH);
	GetSystemDirectory(szSysDir,MAX_PATH);

	if(dwInstOptions&MCI_SUPPORT)
	{
		WritePrivateProfileString("mci","spc700emu",NULL,"system.ini");
		WritePrivateProfileString("mci extensions","spc",NULL,"win.ini");
		for(int i=0;i<=9;i++)
		{
			char szBuffer[500];
			sprintf(szBuffer,"sp%d",i);
			WritePrivateProfileString("mci extensions",szBuffer,NULL,"win.ini");
		}
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\MCISPC");
	}
	else iTotalFiles-=5;

	if(dwInstOptions&PL_SUPPORT)
	{
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\.pl");
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\plfile");
	}

	if(dwInstOptions&SPC_SUPPORT)
	{
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\.spc");
		for(int i=0;i<=9;i++)
		{
			char szBuffer[500];
			sprintf(szBuffer,"SOFTWARE\\Classes\\.sp%d",i);
			RegDeleteKey(HKEY_LOCAL_MACHINE,szBuffer);
		}
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\spcfile");
		RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\MIME\\Database\\Content Type\\audio/spc");
	}
	RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox");
	RegDeleteKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Super Jukebox");

	if(dwInstOptions&DESKTOP_SUPPORT)
	{
		GetFolderPath(NULL,szDesktopDir,CSIDL_DESKTOP);
		sprintf(szBuffer,"%s\\Super Jukebox.lnk",szDesktopDir);
		DeleteFile(szBuffer);
	}
	if(dwInstOptions&STARTMENU_SUPPORT)
	{
		GetFolderPath(NULL,szStartMenuDir,CSIDL_PROGRAMS);
		sprintf(szBuffer1,"%s\\Super Jukebox",szStartMenuDir);
		for(int i=0;i<3;i++)
		{
			_splitpath(InstallationFiles[i].szName,NULL,NULL,szBuffer2,NULL);
			strcat(szBuffer2,".lnk");
			sprintf(szBuffer,"%s\\%s",szBuffer1,szBuffer2);
			DeleteFile(szBuffer);
		}
		RemoveDirectory(szBuffer1);
	}
	if(dwInstOptions&QUICKLAUNCH_SUPPORT)
	{
		GetFolderPath(NULL,szQuickLaunchDir,CSIDL_APPDATA);
		strcat(szQuickLaunchDir,"\\Microsoft\\Internet Explorer\\Quick Launch");
		sprintf(szBuffer,"%s\\Super Jukebox.lnk",szQuickLaunchDir);
		DeleteFile(szBuffer);
	}

	for(int i=0;i<iTotalFiles;i++)
	{
		switch(InstallationFiles[i].iInstDir)
		{
		case 0:sprintf(szBuffer,"%s\\%s",szInstDir,InstallationFiles[i].szName);break;
		case 1:sprintf(szBuffer,"%s\\%s",szWinDir,InstallationFiles[i].szName);break;
		case 2:sprintf(szBuffer,"%s\\%s",szSysDir,InstallationFiles[i].szName);break;
		case 3:sprintf(szBuffer,"%s\\%s",szPluginsDir,InstallationFiles[i].szName);break;
		}
		DeleteFile(szBuffer);
	}
	RemoveDirectory(szPluginsDir);
	RemoveDirectory(szInstDir);

	MessageBox(NULL,"Uninstallation complete. Note, however, that Windows does not allow programs to delete themselves (commit suicide). So you will have to remove the \"UnInstall.exe\" file. Sorry it had to come to this.",szAppName,MB_ICONINFORMATION);
	return 0;
}
