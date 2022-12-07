#include "stdafx.h"
#include "Main.h"
#include "Resource.h"
#include "Application.h"

CApplication app;

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	//Uematsu_Init();
	WPARAM wRes=0;
	do
	{
		if(app.Init(hInstance,lpCmdLine,nCmdShow))
			wRes=app.ProcessMessageQueue();
		else
		{
			DisplayError(NULL,FALSE,NULL);
			return 0;
		}
	}while(app.ReRun());
	//Uematsu_DeInit();
	
	return wRes;
}

void DisplayError(HWND hWnd,BOOL bExit,LPCTSTR lpszMessage)
{
	LPVOID lpMsgBuf;
	TCHAR szAppName[MAX_LOADSTRING];

	DWORD dwMessageId=GetLastError();
	if(lpszMessage)
	{
		lpMsgBuf=LocalAlloc(0,strlen(lpszMessage)+1);
		strcpy((char*)lpMsgBuf,lpszMessage);
	}
	else if(dwMessageId==ERROR_SUCCESS)
	{
		lpMsgBuf=LocalAlloc(0,50);
		strcpy((char*)lpMsgBuf,"An unspecified error has occurred.\n");
	}
	else
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,NULL,dwMessageId,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)&lpMsgBuf,0,NULL);

	if(!lpszMessage)
	{
		LPVOID lpMsgBuf1=LocalAlloc(0,strlen((char*)lpMsgBuf)+100);
		sprintf((char*)lpMsgBuf1,"Error: %u\n\n%s",dwMessageId,(char*)lpMsgBuf);
		LocalFree(lpMsgBuf);
		lpMsgBuf=lpMsgBuf1;
	}

	if(bExit)
	{
		LPVOID lpMsgBuf1=LocalAlloc(0,strlen((char*)lpMsgBuf)+100);
		strcpy((char*)lpMsgBuf1,(char*)lpMsgBuf);
		strcat((char*)lpMsgBuf1,"\nThis is a severe error and the program must terminate.");
		LocalFree(lpMsgBuf);
		lpMsgBuf=lpMsgBuf1;
	}
	LoadString(GetModuleHandle(NULL),IDS_APP_NAME,szAppName,MAX_LOADSTRING);
	MessageBox(hWnd,(LPCTSTR)lpMsgBuf,szAppName,MB_OK|MB_ICONERROR);
	LocalFree(lpMsgBuf);

	if(bExit)
		ExitProcess(0);
}

BOOL Line(HDC hdc,COLORREF cr,int nXStart,int nYStart,int nXEnd,int nYEnd)
{
	HPEN hPen=CreatePen(PS_SOLID,1,cr);
	if(!hPen)return FALSE;
	HPEN hOldPen=(HPEN)SelectObject(hdc,hPen);
	if(!hOldPen)return FALSE;
	if(!MoveToEx(hdc,nXStart,nYStart,NULL))return FALSE;
	if(!LineTo(hdc,nXEnd,nYEnd))return FALSE;
	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
	return TRUE;
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

HRESULT GetLinkTarget(LPCSTR lpszPathLink,LPCSTR lpszPathObj,INT nTextMax)
{
	HRESULT hres;
	IShellLink* psl;

	hres=CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&psl);
	if(SUCCEEDED(hres))
	{
		IPersistFile* ppf;

		hres=psl->QueryInterface(IID_IPersistFile,(void**)&ppf);
		if(SUCCEEDED(hres))
		{
			WCHAR wsz[MAX_PATH];

			MultiByteToWideChar(CP_ACP,0,lpszPathLink,-1,wsz,MAX_PATH);
			hres=ppf->Load(wsz,STGM_READ);
			if(SUCCEEDED(hres))
			{
				hres=psl->Resolve(NULL,SLR_ANY_MATCH);
				if(SUCCEEDED(hres))
				{
					WIN32_FIND_DATA wfd;
					hres=psl->GetPath((LPSTR)lpszPathObj,nTextMax,&wfd,0);
				}
			}
			ppf->Release();
		}
		psl->Release();
	}
	return hres;
}
