#include <windows.h>
#include <commctrl.h>
#include <memory.h>
#include <stdio.h>
#include "resource.h"
#include "mcispc.h"
#include "mixer.h"
#include "in2.h"

#define szAppName "MCISPCDLG"

extern "C" BOOL WINAPI thk0_ThunkConnect32(LPSTR,LPSTR,HINSTANCE,DWORD);

MCI_CONFIG_PARMS mciConfigParms;
char *szFreqs[]={
"4000 Hz",
"8000 Hz",
"16000 Hz",
"23900 Hz",
"24000 Hz",
"32000 Hz",
"44100 Hz",
"48000 Hz"
};
char *szVMs[]={
"x1",
"x2",
"x4",
"x8"
};
char *szPriorities[]={
"Idle",
"Normal",
"High",
"RealTime"
};
HINSTANCE hModuleInstance;
HMODULE hLibrary0,hLibrary1;
In_Module *in_mod;
Out_Module *out_mod;
typedef In_Module*(*pfnwinampGetInModule2)();
typedef Out_Module*(*pfnwinampGetOutModule)();
pfnwinampGetInModule2 winampGetInModule2;
pfnwinampGetOutModule winampGetOutModule;
BOOL bInitialized,bPlaying,bPaused;

// Empty functions for WinAMP functionality.
void SAVSAInit(int,int){}
void SAVSADeInit(){}
void SAAddPCMData(void*,int,int,int){}
int SAGetMode(){return 0;}
void SAAdd(void*,int,int){}
void VSAAddPCMData(void*,int,int,int){}
int VSAGetMode(int*,int*){return 0;}
void VSAAdd(void*,int){}
void VSASetInfo(int,int){}
int dsp_isactive(){return 0;}
int dsp_dosamples(short int*,int,int,int,int){return 0;}
void SetInfo(int,int,int,int){}

BOOL WINAPI DllMain(HINSTANCE hDLLInst,DWORD dwReason,LPVOID lpvReserved)
{
	if(!thk0_ThunkConnect32("MCISPC.DRV","MCISPCDLG.DLL",hDLLInst,dwReason))
		return FALSE;
	hModuleInstance=hDLLInst;
	return TRUE;
}

BOOL CALLBACK ConfigDlgProc(HWND hWndDlg,UINT uMessage,WPARAM wParam,LPARAM lParam)
{
	int i;
	char szBuffer[1024];
	
	switch(uMessage)
	{
	case WM_INITDIALOG:
		for(i=0;i<sizeof(szFreqs)/sizeof(szFreqs[0]);i++)
			SendDlgItemMessage(hWndDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)szFreqs[i]);
		CheckRadioButton(hWndDlg,IDC_RADIO5,IDC_RADIO6,mciConfigParms.dwEngine==0?IDC_RADIO5:IDC_RADIO6);
		switch(mciConfigParms.dwSampleRate)
		{
		case 4000:i=0;break;
		case 8000:i=1;break;
		case 16000:i=2;break;
		case 23900:i=3;break;
		case 24000:i=4;break;
		case 32000:i=5;break;
		case 44100:i=6;break;
		case 48000:i=7;break;
		default:i=0;break;
		}
		SendDlgItemMessage(hWndDlg,IDC_COMBO1,CB_SETCURSEL,i,0);
		CheckRadioButton(hWndDlg,IDC_RADIO1,IDC_RADIO2,mciConfigParms.dwSampleSize==8?IDC_RADIO1:IDC_RADIO2);
		CheckRadioButton(hWndDlg,IDC_RADIO3,IDC_RADIO4,mciConfigParms.dwChannels==1?IDC_RADIO3:IDC_RADIO4);
		for(i=0;i<sizeof(szVMs)/sizeof(szVMs[0]);i++)
			SendDlgItemMessage(hWndDlg,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)szVMs[i]);
		switch(mciConfigParms.dwVolumeMultiplier)
		{
		case 1:i=0;break;
		case 2:i=1;break;
		case 4:i=2;break;
		case 8:i=3;break;
		default:i=0;break;
		}
		SendDlgItemMessage(hWndDlg,IDC_COMBO2,CB_SETCURSEL,i,0);
		for(i=0;i<sizeof(szPriorities)/sizeof(szPriorities[0]);i++)
			SendDlgItemMessage(hWndDlg,IDC_COMBO3,CB_ADDSTRING,0,(LPARAM)szPriorities[i]);
		switch(mciConfigParms.dwPriorityClass)
		{
		case IDLE_PRIORITY_CLASS:i=0;break;
		case NORMAL_PRIORITY_CLASS:i=1;break;
		case HIGH_PRIORITY_CLASS:i=2;break;
		case REALTIME_PRIORITY_CLASS:i=3;break;
		default:i=1;break;
		}
		SendDlgItemMessage(hWndDlg,IDC_COMBO3,CB_SETCURSEL,i,0);
		itoa(mciConfigParms.dwSongLength,szBuffer,10);
		SetDlgItemText(hWndDlg,IDC_EDIT1,szBuffer);
		SendDlgItemMessage(hWndDlg,IDC_SPIN1,UDM_SETRANGE32,0,UD_MAXVAL);

		EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO1),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO1),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO2),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO3),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO4),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO2),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO3),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT1),mciConfigParms.dwEngine==0);
		EnableWindow(GetDlgItem(hWndDlg,IDC_SPIN1),mciConfigParms.dwEngine==0);
		return TRUE;
    case WM_COMMAND:
          switch(LOWORD(wParam))
          {
		  case IDC_RADIO5:
		  case IDC_RADIO6:
			  if(HIWORD(wParam)==BN_CLICKED)
			  {
					BOOL bEngine=IsDlgButtonChecked(hWndDlg,IDC_RADIO6)==BST_CHECKED;
					EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO1),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO1),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO2),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO3),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_RADIO4),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO2),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO3),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT1),!bEngine);
					EnableWindow(GetDlgItem(hWndDlg,IDC_SPIN1),!bEngine);
			  }
			  break;
          case IDOK:
			  mciConfigParms.dwEngine=IsDlgButtonChecked(hWndDlg,IDC_RADIO5)?0:1;
			  switch(SendDlgItemMessage(hWndDlg,IDC_COMBO1,CB_GETCURSEL,0,0))
			  {
			  case 0:mciConfigParms.dwSampleRate=4000;break;
			  case 1:mciConfigParms.dwSampleRate=8000;break;
			  case 2:mciConfigParms.dwSampleRate=16000;break;
			  case 3:mciConfigParms.dwSampleRate=23900;break;
			  case 4:mciConfigParms.dwSampleRate=24000;break;
			  case 5:mciConfigParms.dwSampleRate=32000;break;
			  case 6:mciConfigParms.dwSampleRate=44100;break;
			  case 7:mciConfigParms.dwSampleRate=48000;break;
			  }
			  mciConfigParms.dwSampleSize=IsDlgButtonChecked(hWndDlg,IDC_RADIO1)?8:16;
			  mciConfigParms.dwChannels=IsDlgButtonChecked(hWndDlg,IDC_RADIO3)?1:2;
			  switch(SendDlgItemMessage(hWndDlg,IDC_COMBO2,CB_GETCURSEL,0,0))
			  {
			  case 0:mciConfigParms.dwVolumeMultiplier=1;break;
			  case 1:mciConfigParms.dwVolumeMultiplier=2;break;
			  case 2:mciConfigParms.dwVolumeMultiplier=4;break;
			  case 3:mciConfigParms.dwVolumeMultiplier=8;break;
			  }
			  switch(SendDlgItemMessage(hWndDlg,IDC_COMBO3,CB_GETCURSEL,0,0))
			  {
			  case 0:mciConfigParms.dwPriorityClass=IDLE_PRIORITY_CLASS;break;
			  case 1:mciConfigParms.dwPriorityClass=NORMAL_PRIORITY_CLASS;break;
			  case 2:mciConfigParms.dwPriorityClass=HIGH_PRIORITY_CLASS;break;
			  case 3:mciConfigParms.dwPriorityClass=REALTIME_PRIORITY_CLASS;break;
			  }
			  GetDlgItemText(hWndDlg,IDC_EDIT1,szBuffer,sizeof(szBuffer));
			  mciConfigParms.dwSongLength=atoi(szBuffer);
              EndDialog(hWndDlg,IDOK);
              return TRUE;
          case IDCANCEL:
              EndDialog(hWndDlg,IDCANCEL);
              return TRUE;
          }
          break;
	}
	return FALSE;
}

extern "C" long WINAPI DisplayConfigDlg(HWND hWndParent, MCI_CONFIG_PARMS *mciConfigParms0)
{
	memcpy(&mciConfigParms,mciConfigParms0,sizeof(MCI_CONFIG_PARMS));
	switch(DialogBox(hModuleInstance,MAKEINTRESOURCE(IDD_DIALOG1),hWndParent,ConfigDlgProc))
	{
	case IDOK:
		memcpy(mciConfigParms0,&mciConfigParms,sizeof(MCI_CONFIG_PARMS));
		return 1;
	case -1:
		return -1;
	}
	return 0;
}

extern "C" BOOL WINAPI ReadRegistrySettings(MCI_CONFIG_PARMS *mciConfigParms)
{
	HKEY hKey;
	DWORD dwSpare,dwSize=sizeof(DWORD);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\MCISPC",0,KEY_ALL_ACCESS,&hKey);
	RegQueryValueEx(hKey,"Engine",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwEngine=dwSpare;
	RegQueryValueEx(hKey,"SampleRate",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwSampleRate=dwSpare;
	RegQueryValueEx(hKey,"SampleSize",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwSampleSize=dwSpare;
	RegQueryValueEx(hKey,"Channels",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwChannels=dwSpare;
	RegQueryValueEx(hKey,"VolumeMultiplier",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwVolumeMultiplier=dwSpare;
	RegQueryValueEx(hKey,"PriorityClass",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwPriorityClass=dwSpare;
	RegQueryValueEx(hKey,"SongLength",NULL,NULL,(LPBYTE)&dwSpare,&dwSize);
	mciConfigParms->dwSongLength=dwSpare;
	RegCloseKey(hKey);

	return TRUE;
}

extern "C" BOOL WINAPI WriteRegistrySettings(MCI_CONFIG_PARMS *mciConfigParms)
{
	HKEY hKey;
	DWORD dwSpare,dwSize=sizeof(DWORD);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\MCISPC",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL);
	dwSpare=mciConfigParms->dwEngine;
	RegSetValueEx(hKey,"Engine",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwSampleRate;
	RegSetValueEx(hKey,"SampleRate",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwSampleSize;
	RegSetValueEx(hKey,"SampleSize",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwChannels;
	RegSetValueEx(hKey,"Channels",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwVolumeMultiplier;
	RegSetValueEx(hKey,"VolumeMultiplier",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwPriorityClass;
	RegSetValueEx(hKey,"PriorityClass",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	dwSpare=mciConfigParms->dwSongLength;
	RegSetValueEx(hKey,"SongLength",0,REG_DWORD,(LPBYTE)&dwSpare,sizeof(DWORD));
	RegCloseKey(hKey);

	return TRUE;
}

extern "C" long WINAPI InitZSPC()
{
	if((hLibrary0=LoadLibrary("out_wave.dll"))==NULL)
		return 1;
	winampGetOutModule=(pfnwinampGetOutModule)GetProcAddress(hLibrary0,"winampGetOutModule");
	out_mod=winampGetOutModule();
	out_mod->hMainWindow=0;
	out_mod->hDllInstance=hLibrary0;
	out_mod->Init();
	if((hLibrary1=LoadLibrary("in_spc.dll"))==NULL)
	{
		out_mod->Quit();
		FreeLibrary(hLibrary0);
		return 2;
	}
	winampGetInModule2=(pfnwinampGetInModule2)GetProcAddress(hLibrary1,"winampGetInModule2");
	in_mod=winampGetInModule2();
	in_mod->hMainWindow=0;
	in_mod->hDllInstance=hLibrary1;
	in_mod->SAVSAInit=&SAVSAInit;
	in_mod->SAVSADeInit=&SAVSADeInit;
	in_mod->SAAddPCMData=&SAAddPCMData;
	in_mod->SAGetMode=&SAGetMode;
	in_mod->SAAdd=&SAAdd;
	in_mod->VSAAddPCMData=&VSAAddPCMData;
	in_mod->VSAGetMode=&VSAGetMode;
	in_mod->VSAAdd=&VSAAdd;
	in_mod->VSASetInfo=&VSASetInfo;
	in_mod->dsp_isactive=&dsp_isactive;
	in_mod->dsp_dosamples=&dsp_dosamples;
	in_mod->SetInfo=&SetInfo;
	in_mod->outMod=out_mod;
	in_mod->Init();
	UINT uMixerID=GetMixerIDForWaveMapper();
	DWORD dwControlID=GetMixerControlID(uMixerID,MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,MIXERCONTROL_CONTROLTYPE_VOLUME);
	in_mod->SetVolume(GetMixerControlValue(uMixerID,dwControlID)/256);
	bInitialized=TRUE;

	return 0;
}

extern "C" void WINAPI DeinitZSPC()
{
	if(bInitialized)
	{
		in_mod->Quit();
		FreeLibrary(hLibrary1);
		out_mod->Quit();
		FreeLibrary(hLibrary0);
		bInitialized=FALSE;
	}
}

extern "C" int WINAPI PlayZSPC(char *fn)
{
	int iRes=0;
	if(!bPaused&&!bPlaying)
		iRes=in_mod->Play(fn);
	else if(bPaused)
		in_mod->UnPause();
	if(!iRes){bPlaying=TRUE;bPaused=FALSE;}
	return iRes;
}

extern "C" void WINAPI StopZSPC()
{
	if(bPaused||bPlaying)
		in_mod->Stop();
	bPaused=bPlaying=FALSE;
}

extern "C" void WINAPI PauseZSPC()
{
	if(!bPaused&&bPlaying)
		in_mod->Pause();
	bPlaying=FALSE;bPaused=TRUE;
}