
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AW_DISCO_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AW_DISCO_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef AW_DISCO_EXPORTS
#define AW_DISCO_API extern "C" __declspec(dllexport)
#else
#define AW_DISCO_API extern "C" __declspec(dllimport)
#endif

AW_DISCO_API int AW1_SetParameters(HWND hWndDest,LPRECT lprc,UEMATSUCONFIG *cfg,BOOL bDoubleBuffer);
AW_DISCO_API int AW1_GetName(char *lpszName,int nTextMax);
AW_DISCO_API int AW1_GetVersion();
AW_DISCO_API int AW1_Configure(HWND hWndParent);
AW_DISCO_API int AW1_About(HWND hWndParent);
AW_DISCO_API int AW1_SafeToRender();
AW_DISCO_API int AW1_Start();
AW_DISCO_API int AW1_Stop();
AW_DISCO_API int AW1_Render(UEMATSUVISPARAMS *vp);