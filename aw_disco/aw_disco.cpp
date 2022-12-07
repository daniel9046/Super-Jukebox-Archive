// aw_disco.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "aw_disco.h"
#include "resource.h"

#define AW_VERSION (1)
#define COLOR_DEPTH (255)
#define SMALL_VOL_LIMIT (48)
#define BIG_VOL_LIMIT (130)
#define ENVELOPE_LIMIT (8191)
#define PITCH_LIMIT (130078)

char szPluginName[]="Disco Spheres";

int nRate,nXPos,nYPos,nWidth,nHeight;
BOOL bStarted;
HINSTANCE hInstance;
int nReaction;
BOOL bGradient,bConnectTheDots;
BOOL bSafeToRender=TRUE;
MGLDC *winDC,*memDC;
pixel_format_t pf;
color_t colors[10];
HWND hWndDest;

/*************************************************************************************************
Loads the the plugin settings from the registry. This function is specific to this plugin.
*************************************************************************************************/
BOOL LoadRegistrySettings()
{
	HKEY hKey;
	DWORD dwSize;

	nReaction=2;
	bGradient=TRUE;
	bConnectTheDots=TRUE;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox\\Disco Spheres",0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
		return FALSE;
	dwSize=sizeof(nReaction);
	RegQueryValueEx(hKey,"Reaction",NULL,NULL,(LPBYTE)&nReaction,&dwSize);
	dwSize=sizeof(bGradient);
	RegQueryValueEx(hKey,"Use gradient spheres",NULL,NULL,(LPBYTE)&bGradient,&dwSize);
	dwSize=sizeof(bConnectTheDots);
	RegQueryValueEx(hKey,"Connect the dots",NULL,NULL,(LPBYTE)&bConnectTheDots,&dwSize);
	RegCloseKey(hKey);

	return TRUE;
}

/*************************************************************************************************
Saves registry settings. Specific to this plugin.
*************************************************************************************************/
BOOL SaveRegistrySettings()
{
	HKEY hKey;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox\\Disco Spheres",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;

	RegSetValueEx(hKey,"Reaction",0,REG_DWORD,(LPBYTE)&nReaction,sizeof(nReaction));
	RegSetValueEx(hKey,"Use gradient spheres",0,REG_DWORD,(LPBYTE)&bGradient,sizeof(bGradient));
	RegSetValueEx(hKey,"Connect the dots",0,REG_DWORD,(LPBYTE)&bConnectTheDots,sizeof(bConnectTheDots));
	RegCloseKey(hKey);
	
	return TRUE;
}

/*************************************************************************************************
Entry point for plugin. Loads registry settings upon startup, saves them upon termination.
This function is optional.
*************************************************************************************************/
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	hInstance=(HINSTANCE)hModule;
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MGL_registerDriver(MGL_PACKED8NAME,PACKED8_driver);
			MGL_registerDriver(MGL_PACKED16NAME,PACKED16_driver);
			MGL_registerDriver(MGL_PACKED24NAME,PACKED24_driver);
			MGL_registerDriver(MGL_PACKED32NAME,PACKED32_driver);

			if(!MGL_initWindowed(""))
				return FALSE;
			LoadRegistrySettings();
			break;
		case DLL_PROCESS_DETACH:
			MGL_exit();
			SaveRegistrySettings();
			break;
    }
    return TRUE;
}

/*************************************************************************************************
Called by SJ whenever the relevant parameters have changed. Use the 'hWndDest' parameter as the
handle to the window to render into, use the 'lprc' parameter which is a pointer to a 
rectangle to draw into, don't draw outside this rectangle. Use the 'cfg' parameter to determine
the current player settings, such as sample rate, read the "Uematsu.h" header file to find the 
specific layout of this structure. The 'bDoubleBuffer' parameter specifies if the user has
requested the plugin to draw using a double buffer method or not. Returns non-zero if successful.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_SetParameters(HWND hWndDest,LPRECT lprc,UEMATSUCONFIG *cfg,BOOL bDoubleBuffer)
{
	::hWndDest=hWndDest;
	nRate=cfg->uVisualizationRate;
	nXPos=lprc->left;
	nYPos=lprc->top;
	nWidth=lprc->right-lprc->left;
	nHeight=lprc->bottom-lprc->top;

	if(bStarted)
	{
		MGL_makeCurrentDC(NULL);
		MGL_destroyDC(memDC);
		MGL_destroyDC(winDC);

		winDC=MGL_createWindowedDC(hWndDest);
		if(!winDC)
			return 0;
		int nBits=MGL_getBitsPerPixel(winDC);
		if(nBits<15)
		{
			MessageBox(hWndDest,"This plug-in requires a color depth of at least 15-bits per pixel.",szPluginName,MB_ICONERROR);
			return 0;
		}
		MGL_getPixelFormat(winDC,&pf);
		memDC=MGL_createMemoryDC(nWidth,nHeight,nBits,&pf);
		if(!memDC)
			return 0;
		MGL_makeCurrentDC(memDC);
	}
	return 1;
}

/*************************************************************************************************
Retrieves the name for the plugin, copy only upto the number of characters specified by the
'nTextMax' parameter. Returns non-zero if successful.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_GetName(char *lpszName,int nTextMax)
{
	strncpy(lpszName,szPluginName,nTextMax);
	lpszName[nTextMax-1]='\0';
	return 1;
}

/*************************************************************************************************
Retrieves the Active Wallpaper version that the plugin was made for.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_GetVersion()
{
	return AW_VERSION;
}

/*************************************************************************************************
Window callback procedure for the configuration dialog box. Specific to this plugin.
*************************************************************************************************/
BOOL CALLBACK ConfigureDialogProc(HWND hWndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	int i;
	LPCTSTR lpszReactors[]={"Envelope","Pitch","Volume"};

	switch(uMsg)
	{
	case WM_INITDIALOG:		
		for(i=0;i<sizeof(lpszReactors)/sizeof(lpszReactors[0]);i++)
			SendDlgItemMessage(hWndDlg,IDC_COMBO_REACTTO,CB_ADDSTRING,0,(LPARAM)lpszReactors[i]);
		SendDlgItemMessage(hWndDlg,IDC_COMBO_REACTTO,CB_SETCURSEL,nReaction,0);
		CheckDlgButton(hWndDlg,IDC_CHECK_GRADIENT,bGradient);
		CheckDlgButton(hWndDlg,IDC_CHECK_CONNECTTHEDOTS,bConnectTheDots);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			bSafeToRender=FALSE;
			Sleep(500);
			nReaction=SendDlgItemMessage(hWndDlg,IDC_COMBO_REACTTO,CB_GETCURSEL,0,0);
			bGradient=IsDlgButtonChecked(hWndDlg,IDC_CHECK_GRADIENT)==BST_CHECKED;
			bConnectTheDots=IsDlgButtonChecked(hWndDlg,IDC_CHECK_CONNECTTHEDOTS)==BST_CHECKED;
			bSafeToRender=TRUE;
			EndDialog(hWndDlg,IDOK);
			return 1;
		case IDCANCEL:
			EndDialog(hWndDlg,IDCANCEL);
			return 1;
		}
		break;
	}
	return 0;
}

/*************************************************************************************************
Function for displaying the configuration dialog box. Use the 'hWndParent' window handle to
attach to the dialog box.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_Configure(HWND hWndParent)
{
	if(DialogBox(hInstance,MAKEINTRESOURCE(IDD_CONFIGURE),hWndParent,ConfigureDialogProc)==-1)
		return 0;
	return 1;
}

/*************************************************************************************************
Window callback procedure for the About dialog box. Specific to this plugin.
*************************************************************************************************/
BOOL CALLBACK AboutDialogProc(HWND hWndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWndDlg,LOWORD(wParam));
			return 1;
		}
		break;
	}
	return 0;
}

/*************************************************************************************************
Function for displaying the About dialog box. Use the 'hWndParent' window handle to attach to the
dialog box.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_About(HWND hWndParent)
{
	if(DialogBox(hInstance,MAKEINTRESOURCE(IDD_ABOUT),hWndParent,AboutDialogProc)==-1)
		return 0;
	return 1;
}

/*************************************************************************************************
Called by SJ to determine if it's safe to call the AW1_Render() function. Since the rendering
function can be called at *any* time, you might want to specify that it is not safe to render
at specific times, such as when you change the settings for the rendering code. Returns non-zero
if it's safe to call AW1_Render(), otherwise returns zero.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_SafeToRender()
{
	return bSafeToRender;
}

/*************************************************************************************************
Function called by SJ immediately after a song has started to play and before SJ goes into the
rendering thread which repeatedly calls AW1_Render(). Use this function to allocate or initialize
any data needed by the plugin to render. AW1_Render() will never be called without this function
being called first. Returns non-zero if successful.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_Start()
{
	if(bStarted)
		return 0;
	winDC=MGL_createWindowedDC(hWndDest);
	if(!winDC)
		return 0;
	int nBits=MGL_getBitsPerPixel(winDC);
	if(nBits<15)
	{
		MessageBox(hWndDest,"This plug-in requires a color depth of at least 15-bits per pixel.",szPluginName,MB_ICONERROR);
		return 0;
	}
	MGL_getPixelFormat(winDC,&pf);
	memDC=MGL_createMemoryDC(nWidth,nHeight,nBits,&pf);
	if(!memDC)
		return 0;
	MGL_makeCurrentDC(memDC);

	colors[0]=MGL_packColor(&pf,0,255,0);
	colors[1]=MGL_packColor(&pf,0,0,255);
	colors[2]=MGL_packColor(&pf,255,0,0);
	colors[3]=MGL_packColor(&pf,0,192,0);
	colors[4]=MGL_packColor(&pf,0,0,192);
	colors[5]=MGL_packColor(&pf,192,0,0);

	bStarted=TRUE;
	return 1;
}

/*************************************************************************************************
Called by SJ immediately before a song stops playing and after SJ has finished the rendering thread.
Use this function to undo any allocations done by AW1_Start(). AW1_Render() will never be called
after this function has been called. Returns non-zero if successful.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_Stop()
{
	if(!bStarted)
		return 0;
	MGL_makeCurrentDC(NULL);
	MGL_destroyDC(memDC);
	MGL_destroyDC(winDC);
	bStarted=FALSE;
	return 1;
}

/*************************************************************************************************
Called by SJ while a song is playing or even while it is paused, from within a thread. This function
can be called absolutely at *any* time, but not before AW1_Start() and not after AW1_Stop() are called.
Use the 'vp' parameter for the various visualization related parameters, read the "Uematsu.h" header
file to find the specific layout of this structure. Returns non-zero if successful.

NOTE: This function is mandatory, and the plugin will not load if it is not exported.
*************************************************************************************************/
AW_DISCO_API int AW1_Render(UEMATSUVISPARAMS *vp)
{
	if(!bStarted)
		return 0;

	double rad=0,rad_inc=3.1415926535*2/8;
	int w=int(nWidth/7.25),h=int(nHeight/7.25);

	int xPos[9],yPos[9],nVols[9],nRads[9],nCols[9];

	for(int i=0;i<8;i++)
	{
		switch(nReaction)
		{
		case 0:
			nVols[i]=vp->Mix[i].EVal;
			if(nVols[i]>ENVELOPE_LIMIT)nVols[i]=ENVELOPE_LIMIT;
			nRads[i]=nVols[i]*COLOR_DEPTH/ENVELOPE_LIMIT;
			break;
		case 1:
			nVols[i]=vp->Mix[i].PRate;
			if(nVols[i]>PITCH_LIMIT)nVols[i]=PITCH_LIMIT;
			nRads[i]=nVols[i]*COLOR_DEPTH/PITCH_LIMIT;
			break;
		case 2:
			nVols[i]=max(((vp->nDecibelList[i*2]>>16)-48)*SMALL_VOL_LIMIT/48,((vp->nDecibelList[i*2+1]>>16)-48)*SMALL_VOL_LIMIT/48);
			if(nVols[i]>SMALL_VOL_LIMIT)nVols[i]=SMALL_VOL_LIMIT;
			nRads[i]=nVols[i]*COLOR_DEPTH/SMALL_VOL_LIMIT;
			break;
		}
		
		xPos[i]=(int)(cos(rad)*(nWidth/2-w)+(nWidth/2));
		yPos[i]=(int)(sin(rad)*(nHeight/2-h)+(nHeight/2));
		nCols[i]=i%3;
		rad+=rad_inc;
	}
	nVols[8]=max(((vp->nDecibelList[16]>>16)-48)*BIG_VOL_LIMIT/48,((vp->nDecibelList[17]>>16)-48)*BIG_VOL_LIMIT/48);
	if(nVols[8]>BIG_VOL_LIMIT)nVols[8]=BIG_VOL_LIMIT;
	nRads[8]=nVols[8]*COLOR_DEPTH/BIG_VOL_LIMIT;
	xPos[8]=nWidth/2;
	yPos[8]=nHeight/2;
	nCols[8]=2;

	BOOL bErase=TRUE;

	if(bConnectTheDots)
	{
		BOOL bDone[9]={FALSE};
		INT nColorCount[3]={0};
		for(i=0;i<9;i++)
		{
			if(i>=8||!(vp->Mix[i].MFlg&8))
			{
				for(int x=0;x<9;x++)
				{
					if(x>=8||!(vp->Mix[x].MFlg&8))
					{
						if(i!=x&&!bDone[x]&&nCols[i]==nCols[x]&&nRads[i]>=4&&nRads[i]==nRads[x])
						{
							nColorCount[i%3]++;
							bDone[i]=TRUE;
						}
					}
				}
			}
		}
		int nCount=0;
		for(i=0;i<3;i++)
			if(nColorCount[i]==3)
				nCount++;
		while(nCount)
		{
			if(nColorCount[i=rand()%3]==3)
			{
				MGL_setBackColor(colors[i+3]);
				MGL_clearDevice();
				bErase=FALSE;
				break;
			}
		}
	}
	if(bErase)
	{
		MGL_setBackColor(MGL_packColor(&pf,0,0,0));
		MGL_clearDevice();
	}

	for(i=0;i<8;i++)
	{
		if(!(vp->Mix[i].MFlg&8)&&nRads[i]>=4)
		{
			int xr=nRads[i]*w/COLOR_DEPTH;
			int yr=nRads[i]*h/COLOR_DEPTH;

			int _xr0=-1,_yr0=-1;
			for(int c=COLOR_DEPTH;c>=0;c--)
			{
				int xr0=c*xr/COLOR_DEPTH;
				int yr0=c*yr/COLOR_DEPTH;
				if(xr0!=_xr0||yr0!=_yr0)
				{
					if(bGradient)
					{
						if(i%3==0)
							MGL_setColor(MGL_packColor(&pf,0,255-c,0));
						else if(i%3==1)
							MGL_setColor(MGL_packColor(&pf,0,0,255-c));
						else if(i%3==2)
							MGL_setColor(MGL_packColor(&pf,255-c,0,0));
					}
					else
						MGL_setColor(colors[i%3]);
					MGL_fillEllipseCoord(xPos[i],yPos[i],xr0,yr0);
					_xr0=xr0;_yr0=yr0;
				}
			}
		}
	}

	if(nRads[8]>=4)
	{
		w=int(nWidth/4.5);
		h=int(nHeight/4.5);
		int xr=nRads[8]*w/COLOR_DEPTH;
		int yr=nRads[8]*h/COLOR_DEPTH;

		int _xr0=-1,_yr0=-1;
		for(int c=COLOR_DEPTH;c>=0;c--)
		{
			int xr0=c*xr/COLOR_DEPTH;
			int yr0=c*yr/COLOR_DEPTH;
			if(xr0!=_xr0||yr0!=_yr0)
			{
				if(bGradient)
					MGL_setColor(MGL_packColor(&pf,255-c,0,0));
				else
					MGL_setColor(colors[2]);
				MGL_fillEllipseCoord(xPos[8],yPos[8],xr0,yr0);
				_xr0=xr0;_yr0=yr0;
			}
		}
	}

	if(bConnectTheDots)
	{
		BOOL bDone[9]={FALSE};
		for(i=0;i<9;i++)
		{
			if(i>=8||!(vp->Mix[i].MFlg&8))
			{
				MGL_setColor(colors[i%3]);
				for(int x=0;x<9;x++)
				{
					if(x>=8||!(vp->Mix[x].MFlg&8))
					{
						if(i!=x&&!bDone[x]&&nCols[i]==nCols[x]&&nRads[i]>=4&&nRads[i]==nRads[x])
						{
							MGL_lineCoord(xPos[i],yPos[i],xPos[x],yPos[x]);
							bDone[i]=TRUE;
						}
					}
				}
			}
		}
	}

	MGL_bitBltCoord(winDC,memDC,0,0,nWidth,nHeight,nXPos,nYPos,MGL_REPLACE_MODE);
	return 1;
}
