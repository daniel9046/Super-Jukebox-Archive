#if !defined(_MAIN_H_)
#define _MAIN_H_

#define MAX_LOADSTRING 100
#define NUMBER_OF_COLUMNS 10
#define NUMBER_OF_BUTTONS 34
#define LIST_ID_DRV 0
#define LIST_ID_DIR 1
#define LIST_ID_SPC 2
#define LIST_ID_RAR 3
#define LIST_ID_ZIP 4
#define LIST_ID_ACE 5
#define LIST_ID_PL  6
#define PLAYLIST_VERSION 12
#define VIS_HEIGHT 130
#define WM_NOTIFYICON (WM_APP)
#define CSIDL_PROGRAM_FILES 0x26
#define SHGFP_TYPE_CURRENT 0

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow);
void DisplayError(HWND hWnd,BOOL bExit,LPCTSTR lpszMessage);
BOOL Line(HDC hdc,COLORREF cr,int nXStart,int nYStart,int nXEnd,int nYEnd);
HRESULT _SHGetFolderPath(HWND hwndOwner,int nFolder,HANDLE hToken,DWORD dwFlags,LPTSTR lpszPath);
BOOL _SHGetSpecialFolderPath(HWND hwndOwner,LPTSTR lpszPath,int nFolder,BOOL fCreate);
void GetFolderPath(HWND hwndOwner,LPTSTR lpszPath,int nFolder);
HRESULT CreateLink(LPCSTR lpszPathLink,LPCSTR lpszPathObj,LPCSTR lpszDesc);
HRESULT GetLinkTarget(LPCSTR lpszPathLink,LPCSTR lpszPathObj,INT nTextMax);

#endif