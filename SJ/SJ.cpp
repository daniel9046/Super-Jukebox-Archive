#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <stdio.h>
#include "resource.h"
#include "unzip.h"

#define CSIDL_PROGRAM_FILES 0x26
#define SHGFP_TYPE_CURRENT 0
#define PACKVERSION(major,minor) MAKELONG(minor,major)
#define MCI_SUPPORT 1
#define PL_SUPPORT 2
#define SPC_SUPPORT 4
#define DESKTOP_SUPPORT 8
#define STARTMENU_SUPPORT 16
#define QUICKLAUNCH_SUPPORT 32

char szAppName[500];
HINSTANCE hInst;
DWORD dwInstOptions;

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

DWORD GetDllVersion(LPCTSTR lpszDllName)
{    
	HINSTANCE hinstDll;
    DWORD dwVersion = 0;    
	hinstDll = LoadLibrary(lpszDllName);
	if(hinstDll)
    {        
		DLLGETVERSIONPROC pDllGetVersion;
		pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");
		if(pDllGetVersion)        
		{
            DLLVERSIONINFO dvi;            
			HRESULT hr;
			ZeroMemory(&dvi, sizeof(dvi));            
			dvi.cbSize = sizeof(dvi);
			hr = (*pDllGetVersion)(&dvi);            
			if(SUCCEEDED(hr))
            {
                dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
            }        
		}                
		FreeLibrary(hinstDll);    
	}
    return dwVersion;
}

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

HRESULT CreateLink(LPCSTR lpszPathLink,LPCSTR lpszPathObj,LPCSTR lpszDesc)
{
	HRESULT hres;
	IShellLink* psl;

	hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl);
	if(SUCCEEDED(hres))
	{
		char szDrv[MAX_PATH],szDir[_MAX_DIR];

		_splitpath(lpszPathObj,szDrv,szDir,NULL,NULL);
		strcat(szDrv,szDir);
		if(strlen(szDrv)>3)
			szDrv[strlen(szDrv)-1]='\0';

		psl->SetPath(lpszPathObj);
		psl->SetDescription(lpszDesc);
		psl->SetWorkingDirectory(szDrv);

		IPersistFile* ppf;
		hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if(SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];

			MultiByteToWideChar(CP_ACP,0,lpszPathLink,-1,wsz,MAX_PATH);
			hres=ppf->Save(wsz,TRUE);
			ppf->Release();
		}
		psl->Release();
	}
	return hres;
}

int UpdateRegistry(const char *szWinDir,const char *szInstDir)
{
	char szCommand[MAX_PATH],szDefaultIcon[MAX_PATH];
	HKEY hKey;
	DWORD dwSpare;

	if(dwInstOptions&MCI_SUPPORT)
	{
		WritePrivateProfileString("mci","spc700emu","mcispc.drv","system.ini");
		WritePrivateProfileString("mci extensions","spc","spc700emu","win.ini");
		for(int i=0;i<=9;i++)
		{
			char szBuffer[500];
			sprintf(szBuffer,"sp%d",i);
			WritePrivateProfileString("mci extensions",szBuffer,"spc700emu","win.ini");
		}
		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\MCISPC",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		dwSpare=0;
		RegSetValueEx(hKey,"Engine",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=32000;
		RegSetValueEx(hKey,"SampleRate",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=16;
		RegSetValueEx(hKey,"SampleSize",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=2;
		RegSetValueEx(hKey,"Channels",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=4;
		RegSetValueEx(hKey,"VolumeMultiplier",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=NORMAL_PRIORITY_CLASS;
		RegSetValueEx(hKey,"PriorityClass",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		dwSpare=120;
		RegSetValueEx(hKey,"SongLength",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
		RegCloseKey(hKey);
	}

	if(dwInstOptions&PL_SUPPORT)
	{
		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\.pl",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"plfile",strlen("plfile")+1);
		RegCloseKey(hKey);

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\plfile",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Super Jukebox Playlist",strlen("Super Jukebox Playlist")+1);
		RegCreateKeyEx(hKey,"shell",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Play",strlen("Play")+1);
		RegCreateKeyEx(hKey,"play",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Play",strlen("Play")+1);
		RegCreateKeyEx(hKey,"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		sprintf(szCommand,"\"%s\\Super Jukebox.EXE\" %%1",szInstDir);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szCommand,strlen(szCommand)+1);
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\plfile\\shell",0,KEY_ALL_ACCESS,&hKey);
		RegCreateKeyEx(hKey,"edit",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Edit With Notepad",strlen("Edit With Notepad")+1);
		RegCreateKeyEx(hKey,"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		sprintf(szCommand,"\"%s\\Notepad.EXE\" %%1",szWinDir);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szCommand,strlen(szCommand)+1);
		RegCloseKey(hKey);

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\plfile\\DefaultIcon",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		sprintf(szDefaultIcon,"%s\\Super Jukebox.exe,0",szInstDir);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szDefaultIcon,strlen(szDefaultIcon)+1);
		RegCloseKey(hKey);
	}

	if(dwInstOptions&SPC_SUPPORT)
	{
		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\.spc",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"spcfile",strlen("spcfile")+1);
		RegSetValueEx(hKey,"Content Type",0,REG_SZ,(LPBYTE)"audio/spc",strlen("audio/spc")+1);
		RegCloseKey(hKey);

		for(int i=0;i<=9;i++)
		{
			char szBuffer[500];
			sprintf(szBuffer,"SOFTWARE\\Classes\\.sp%d",i);
			RegCreateKeyEx(HKEY_LOCAL_MACHINE,szBuffer,0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
			RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"spcfile",strlen("spcfile")+1);
			RegSetValueEx(hKey,"Content Type",0,REG_SZ,(LPBYTE)"audio/spc",strlen("audio/spc")+1);
			RegCloseKey(hKey);
		}

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\spcfile",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Super Nintendo Song",strlen("Super Nintendo Song")+1);
		RegCreateKeyEx(hKey,"shell",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Play",strlen("Play")+1);
		RegCreateKeyEx(hKey,"play",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)"Play",strlen("Play")+1);
		RegCreateKeyEx(hKey,"command",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		sprintf(szCommand,"\"%s\\MPLAYER.EXE\" /PLAY /CLOSE %%1",szWinDir);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szCommand,strlen(szCommand)+1);
		RegCloseKey(hKey);

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\spcfile\\DefaultIcon",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		sprintf(szDefaultIcon,"%s\\Super Jukebox.exe,1",szInstDir);
		RegSetValueEx(hKey,NULL,0,REG_SZ,(LPBYTE)szDefaultIcon,strlen(szDefaultIcon)+1);
		RegCloseKey(hKey);

		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Classes\\MIME\\Database\\Content Type\\audio/spc",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
		RegSetValueEx(hKey,"Extension",0,REG_SZ,(LPBYTE)".spc",strlen(".spc")+1);
		RegCloseKey(hKey);
	}

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	RegSetValueEx(hKey,"Installation options",0,REG_DWORD,(LPBYTE)&dwInstOptions,sizeof(DWORD));
	RegSetValueEx(hKey,"Installation directory",0,REG_SZ,(LPBYTE)szInstDir,strlen(szInstDir)+1);
	RegCloseKey(hKey);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Super Jukebox",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	RegSetValueEx(hKey,"DisplayName",0,REG_SZ,(LPBYTE)"Super Jukebox (Remove Only)",strlen("Super Jukebox (Remove Only)")+1);
	sprintf(szCommand,"%s\\Uninstall.exe",szInstDir);
	RegSetValueEx(hKey,"UninstallString",0,REG_SZ,(LPBYTE)szCommand,strlen(szCommand)+1);
	RegCloseKey(hKey);
	return 0;
}

BOOL ExtractFileFromZIP(const char *szInputFilename,const char *szOutputFilename)
{
	voidp vpBuffer;
	uLong l;
	unz_file_info ufiInfo;
	unzFile ufFile;
	FILE *fileOut;
	char szBuffer[MAX_PATH],szName[MAX_PATH],szExt[MAX_PATH];

	_splitpath(szOutputFilename,NULL,NULL,szName,szExt);
	sprintf(szBuffer,"%s%s",szName,szExt);
	if((ufFile=unzOpen(szInputFilename))==NULL) return FALSE;
	if(unzLocateFile(ufFile,szBuffer,2)==UNZ_OK)
	{
		unzGetCurrentFileInfo(ufFile,&ufiInfo,NULL,0,NULL,0,NULL,0);
		if((vpBuffer=malloc(ufiInfo.uncompressed_size))==NULL) return FALSE;
		if(unzOpenCurrentFile(ufFile)!=UNZ_OK) {free(vpBuffer); return FALSE;}
		l=unzReadCurrentFile(ufFile,vpBuffer,ufiInfo.uncompressed_size);
		if(l<=0||l!=ufiInfo.uncompressed_size) {free(vpBuffer); return FALSE;}
		if(unzCloseCurrentFile(ufFile)==UNZ_CRCERROR) {free(vpBuffer); return FALSE;}
		if(unzClose(ufFile)!=UNZ_OK) {free(vpBuffer); return FALSE;}

		if((fileOut=fopen(szOutputFilename,"wb"))==NULL) {free(vpBuffer); return FALSE;}
		if(fwrite(vpBuffer,ufiInfo.uncompressed_size,1,fileOut)!=1) {free(vpBuffer); return FALSE;}
		if(fclose(fileOut)!=0) {free(vpBuffer); return FALSE;}
		free(vpBuffer);
		return TRUE;
	}
	return FALSE;
}

BOOL InstallFile(const char *szZIPFilename, const char *szFilename, const char *szDstDir)
{
	char szBuffer[MAX_PATH],szBuffer1[MAX_PATH];

	if(!GetTempPath(MAX_PATH,szBuffer1))return FALSE;
	strcat(szBuffer1,szFilename);
	if(!ExtractFileFromZIP(szZIPFilename,szBuffer1))return FALSE;
	sprintf(szBuffer,"%s\\%s",szDstDir,szFilename);
	if(!CopyFile(szBuffer1,szBuffer,FALSE))return FALSE;
	if(!DeleteFile(szBuffer1))return FALSE;
	return TRUE;
}

BOOL InstallDllIfNeccessary(LPCTSTR lpszDllFileName,LPCTSTR lpszZipFileName,LPCTSTR lpszInstDir)
{
	HMODULE hDllInst=LoadLibrary(lpszDllFileName);
	if(!hDllInst)
		InstallFile(lpszZipFileName,lpszDllFileName,lpszInstDir);
	else
		FreeLibrary(hDllInst);
	return TRUE;
}

BOOL CALLBACK MainDlgProc(HWND hWndDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	BROWSEINFO bi;
	LPITEMIDLIST lpIDL;
	HRSRC hRsrc;
	HGLOBAL hGlobal;
	FILE *fileOut;
	WIN32_FIND_DATA win32FindData;
	BOOL bFailure;
	int iTotalFiles;
	char szBuffer[1024];
	char szInstDir[MAX_PATH],szPluginsDir[MAX_PATH];
	char szWinDir[MAX_PATH],szSysDir[MAX_PATH],szTmpDir[MAX_PATH];
	char szDesktopDir[MAX_PATH],szStartMenuDir[MAX_PATH],szQuickLaunchDir[MAX_PATH];
	static char *szTempName, szHelpFile[MAX_PATH];

	switch(uMessage)
	{
	case WM_INITDIALOG:
		SetWindowText(hWndDlg,szAppName);
		SetClassLong(hWndDlg,GCL_HICON,(LONG)LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON1)));
		GetWindowRect(hWndDlg,&rect);
		SetWindowPos(hWndDlg,NULL,(GetSystemMetrics(SM_CXSCREEN)-rect.right-rect.left)/2,(GetSystemMetrics(SM_CYSCREEN)-rect.bottom-rect.top)/2,0,0,SWP_NOSIZE|SWP_NOZORDER);

		memset(szInstDir,0,sizeof(szInstDir));
		HKEY hKey;
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,KEY_ALL_ACCESS,&hKey)==ERROR_SUCCESS)
		{
			DWORD dwSize=sizeof(szInstDir),dwType=REG_SZ;
			RegQueryValueEx(hKey,"Installation directory",NULL,&dwType,(LPBYTE)szInstDir,&dwSize);
			RegCloseKey(hKey);
		}
		if(!strlen(szInstDir))
		{			
			GetFolderPath(NULL,szInstDir,CSIDL_PROGRAM_FILES);
			strcat(szInstDir,"\\Super Jukebox");
		}
		SendDlgItemMessage(hWndDlg,IDC_EDIT1,EM_LIMITTEXT,sizeof(szInstDir),0);
		SetDlgItemText(hWndDlg,IDC_EDIT1,szInstDir);
		CheckDlgButton(hWndDlg,IDC_CHECK1,BST_CHECKED);
		CheckDlgButton(hWndDlg,IDC_CHECK2,BST_CHECKED);
		CheckDlgButton(hWndDlg,IDC_CHECK3,BST_CHECKED);
		CheckDlgButton(hWndDlg,IDC_CHECK4,BST_CHECKED);
		CheckDlgButton(hWndDlg,IDC_CHECK5,BST_CHECKED);
		CheckDlgButton(hWndDlg,IDC_CHECK6,BST_CHECKED);
		if((hRsrc=FindResource(hInst,MAKEINTRESOURCE(IDR_ZIP_DATA1),"ZIP_DATA"))==NULL)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,szBuffer,1024,NULL);
			MessageBox(hWndDlg,szBuffer,szAppName,MB_ICONERROR);
			return TRUE;
		}
		if((hGlobal=LoadResource(hInst,hRsrc))==NULL)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,GetLastError(),0,szBuffer,1024,NULL);
			MessageBox(hWndDlg,szBuffer,szAppName,MB_ICONERROR);
			return TRUE;
		}
		szTempName=_tempnam(NULL,NULL);
		if((fileOut=fopen(szTempName,"wb"))==NULL)
		{
			MessageBox(hWndDlg,"Unable to create temporary file.",szAppName,MB_ICONERROR);
			return TRUE;
		}
		fwrite(hGlobal,SizeofResource(hInst,hRsrc),1,fileOut);
		fclose(fileOut);
		GetTempPath(MAX_PATH,szHelpFile);
		strcat(szHelpFile,"SJ.HLP");
		if(!ExtractFileFromZIP(szTempName,szHelpFile))
		{
			MessageBox(hWndDlg,"Unable to extract temporary help file.",szAppName,MB_ICONERROR);
			return TRUE;
		}
		return TRUE;
	case WM_DESTROY:
		DeleteFile(szHelpFile);
		DeleteFile(szTempName);
		free(szTempName);
		return FALSE;
	case WM_HELP:
		if(((HELPINFO*)lParam)->iContextType==HELPINFO_WINDOW)
		{
			WinHelp(hWndDlg,szHelpFile,HELP_CONTEXTPOPUP,((HELPINFO*)lParam)->dwContextId);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EnableWindow(GetDlgItem(hWndDlg,IDOK),FALSE);
			EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),FALSE);
			bFailure=FALSE;
			dwInstOptions=0;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK1)==BST_CHECKED)dwInstOptions|=MCI_SUPPORT;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK2)==BST_CHECKED)dwInstOptions|=PL_SUPPORT;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK3)==BST_CHECKED)dwInstOptions|=SPC_SUPPORT;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK4)==BST_CHECKED)dwInstOptions|=DESKTOP_SUPPORT;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK5)==BST_CHECKED)dwInstOptions|=STARTMENU_SUPPORT;
			if(IsDlgButtonChecked(hWndDlg,IDC_CHECK6)==BST_CHECKED)dwInstOptions|=QUICKLAUNCH_SUPPORT;
			GetDlgItemText(hWndDlg,IDC_EDIT1,szInstDir,sizeof(szInstDir));
			if(szInstDir[strlen(szInstDir)-1]=='\\')
				szInstDir[strlen(szInstDir)-1]='\0';
			sprintf(szPluginsDir,"%s\\Plugins",szInstDir);
			HANDLE hFindFile;
			if((hFindFile=FindFirstFile(szInstDir,&win32FindData))==INVALID_HANDLE_VALUE)
			{
				if(!CreateDirectory(szInstDir,NULL))
				{
					EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
					EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
					MessageBox(hWndDlg,"Unable to create the installation directory. Make sure that only the last directory needs to be created.",szAppName,MB_ICONERROR);
					bFailure=TRUE;
					return TRUE;
				}
			}
			else
				FindClose(hFindFile);
			if((hFindFile=FindFirstFile(szPluginsDir,&win32FindData))==INVALID_HANDLE_VALUE)
			{
				if(!CreateDirectory(szPluginsDir,NULL))
				{
					EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
					EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
					MessageBox(hWndDlg,"Unable to create the installation directory for the plugins. Make sure that only the last directory needs to be created.",szAppName,MB_ICONERROR);
					bFailure=TRUE;
					return TRUE;
				}
			}
			else
				FindClose(hFindFile);
			iTotalFiles=sizeof(InstallationFiles)/sizeof(InstallationFiles[0]);
			if(!(dwInstOptions&MCI_SUPPORT))iTotalFiles-=5;
			SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETRANGE,0,MAKELPARAM(0,iTotalFiles));
			GetWindowsDirectory(szWinDir,MAX_PATH);
			GetSystemDirectory(szSysDir,MAX_PATH);
			GetTempPath(MAX_PATH,szTmpDir);
			int i;
			for(i=0;i<iTotalFiles;i++)
			{
				BOOL bRes;
				switch(InstallationFiles[i].iInstDir)
				{
				case 0:bRes=InstallFile(szTempName,InstallationFiles[i].szName,szInstDir);break;
				case 1:bRes=InstallFile(szTempName,InstallationFiles[i].szName,szWinDir);break;
				case 2:bRes=InstallFile(szTempName,InstallationFiles[i].szName,szSysDir);break;
				case 3:bRes=InstallFile(szTempName,InstallationFiles[i].szName,szPluginsDir);break;
				}
				SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,i+1,0);
				if(!bRes)
				{
					EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
					EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
					SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,0,0);
					sprintf(szBuffer,"Failed to extract and/or install: %s\n",InstallationFiles[i].szName);
					MessageBox(hWndDlg,szBuffer,szAppName,MB_ICONERROR);
					bFailure=TRUE;
				}
				Sleep(100); //Slow down a bit for really fast computers, so users can see the progress indicator. :-)
			}
			InstallDllIfNeccessary("MSIMG32.DLL",szTempName,szInstDir);
			UpdateRegistry(szWinDir,szInstDir);
			if(dwInstOptions&DESKTOP_SUPPORT)
			{
				GetFolderPath(NULL,szDesktopDir,CSIDL_DESKTOP);
				char szBuf1[MAX_PATH];
				sprintf(szBuffer,"%s\\%s",szInstDir,InstallationFiles[0].szName);
				sprintf(szBuf1,"%s\\Super Jukebox.lnk",szDesktopDir);
				HRESULT hRes;
				hRes=CreateLink(szBuf1,szBuffer,NULL);
				if(!SUCCEEDED(hRes))
				{
					EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
					EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
					SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,0,0);
					MessageBox(hWndDlg,"Failed to create a shortcut to Super Jukebox on the Desktop.",szAppName,MB_ICONERROR);
					bFailure=TRUE;
				}
			}
			if(dwInstOptions&STARTMENU_SUPPORT)
			{
				GetFolderPath(NULL,szStartMenuDir,CSIDL_PROGRAMS);
				char szBuf1[MAX_PATH];
				sprintf(szBuf1,"%s\\Super Jukebox",szStartMenuDir);
				HANDLE hFindFile;
				if((hFindFile=FindFirstFile(szBuf1,&win32FindData))==INVALID_HANDLE_VALUE)
				{
					if(!CreateDirectory(szBuf1,NULL))
					{
						EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
						EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
						SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,0,0);
						MessageBox(hWndDlg,"Failed to create the program group in the Start Menu.",szAppName,MB_ICONERROR);
						bFailure=TRUE;
					}
				}
				else
					FindClose(hFindFile);
				for(i=0;i<3;i++)
				{
					char szBuf2[MAX_PATH],szBuf3[MAX_PATH];
					sprintf(szBuffer,"%s\\%s",szInstDir,InstallationFiles[i].szName);
					_splitpath(InstallationFiles[i].szName,NULL,NULL,szBuf2,NULL);
					strcat(szBuf2,".lnk");
					sprintf(szBuf3,"%s\\%s",szBuf1,szBuf2);
					HRESULT hRes;
					hRes=CreateLink(szBuf3,szBuffer,NULL);
					if(!SUCCEEDED(hRes))
					{
						EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
						EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
						SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,0,0);
						MessageBox(hWndDlg,"Failed to create a shortcut in the program group.",szAppName,MB_ICONERROR);
						bFailure=TRUE;
					}
				}
			}
			if(dwInstOptions&QUICKLAUNCH_SUPPORT)
			{
				GetFolderPath(NULL,szQuickLaunchDir,CSIDL_APPDATA);
				strcat(szQuickLaunchDir,"\\Microsoft\\Internet Explorer\\Quick Launch");
				char szBuf1[MAX_PATH];
				sprintf(szBuffer,"%s\\%s",szInstDir,InstallationFiles[0].szName);
				sprintf(szBuf1,"%s\\Super Jukebox.lnk",szQuickLaunchDir);
				HRESULT hRes;
				hRes=CreateLink(szBuf1,szBuffer,NULL);
				if(!SUCCEEDED(hRes))
				{
					EnableWindow(GetDlgItem(hWndDlg,IDOK),TRUE);
					EnableWindow(GetDlgItem(hWndDlg,IDCANCEL),TRUE);
					SendDlgItemMessage(hWndDlg,IDC_PROGRESS1,PBM_SETPOS,0,0);
					MessageBox(hWndDlg,"Failed to create the QuickLaunch shortcut to Super Jukebox .",szAppName,MB_ICONERROR);
					bFailure=TRUE;
				}
			}
			if(!bFailure)
				MessageBox(hWndDlg,"Super Jukebox has been installed successfuly. Thank you for trying it out.",szAppName,MB_ICONINFORMATION);
			else
				MessageBox(hWndDlg,"The installation is complete. Note, however, that there were some problems during the installation, but heck, give it a try anyway!",szAppName,MB_ICONINFORMATION);
			EndDialog(hWndDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hWndDlg,IDCANCEL);
			return TRUE;
		case IDC_BUTTON1:
			bi.hwndOwner=hWndDlg;
			bi.pidlRoot=NULL;
			bi.pszDisplayName=szBuffer;
			bi.lpszTitle="Please select your folder...";
			bi.ulFlags=BIF_EDITBOX|BIF_RETURNONLYFSDIRS;
			bi.lpfn=NULL;
			bi.lParam=0;
			bi.iImage=0;
			if((lpIDL=SHBrowseForFolder(&bi))!=NULL)
			{
				SHGetPathFromIDList(lpIDL,szBuffer);
				SendDlgItemMessage(hWndDlg,IDC_EDIT1,WM_SETTEXT,0,(LPARAM)szBuffer);
				LPMALLOC pMalloc=NULL;
				SHGetMalloc(&pMalloc);
				pMalloc->Free(lpIDL);
				pMalloc->Release();
			}
			return TRUE;
		}
	}
	return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	hInst=hInstance;
	if(!SUCCEEDED(CoInitialize(NULL)))
	{
		MessageBox(NULL,"Failed to initialize the COM library.",szAppName,MB_ICONERROR);
		return 1;
	}
	LoadString(hInst,IDS_STRING1,szAppName,sizeof(szAppName));
	DialogBox(hInst,MAKEINTRESOURCE(IDD_DIALOG1),NULL,MainDlgProc);
	CoUninitialize();
	return 0;
}
