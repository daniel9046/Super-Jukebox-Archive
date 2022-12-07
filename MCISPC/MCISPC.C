#include <windows.h>
#include <mmsystem.h>
#include <mmddk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcispc.h"
#include "spc700~1.h"

#define szAppName "MCISPC.DRV"
#define LOADDS _loadds
#define NORMAL_PRIORITY_CLASS       0x00000020
#define IDLE_PRIORITY_CLASS         0x00000040
#define HIGH_PRIORITY_CLASS         0x00000080
#define REALTIME_PRIORITY_CLASS     0x00000100

BOOL WINAPI thk0_ThunkConnect16(LPSTR,LPSTR,WORD,DWORD);
DWORD WINAPI DisplayConfigDlg(DWORD,MCI_CONFIG_PARMS*);
DWORD WINAPI ReadRegistrySettings(MCI_CONFIG_PARMS*);
DWORD WINAPI WriteRegistrySettings(MCI_CONFIG_PARMS*);
DWORD WINAPI InitZSPC();
void WINAPI DeinitZSPC();
DWORD WINAPI PlayZSPC(LPCSTR);
void WINAPI PauseZSPC();
void WINAPI StopZSPC();
DWORD WINAPI SetPriorityClass(DWORD,DWORD);
DWORD WINAPI GetPriorityClass(DWORD);
DWORD WINAPI GetCurrentProcess();

char *szProduct="SPC700 Emulator";
char szBuffer[1024];
char szElementName[_MAX_PATH];
BOOL bFileLoaded,bPlaying,bPaused;
BOOL bDisplayMessage;
DWORD dwOldPriorityClass;
MCI_GENERIC_PARMS Generic={0};
MCI_CONFIG_PARMS mciConfigParms;

int WINAPI LOADDS LibMain(HINSTANCE hModInst, WORD wDataSeg, WORD cbHeapSize, LPSTR lpszCmdLine)
{
    return 1;
}

BOOL WINAPI LOADDS DllEntryPoint(DWORD dwReason,WORD hInst,WORD wDS,WORD wHeapSize,DWORD dwReserved1,WORD wReserved2)
{                         
	if(!thk0_ThunkConnect16("MCISPC.DRV","MCISPCDLG.DLL",hInst,dwReason))
		return FALSE;
	return TRUE;
}

DWORD WINAPI LOADDS MciNotifyReturn(HWND hWndNotify,UINT uDriverID,DWORD dwFlags,UINT uNotifyMsg,DWORD dwReturnMsg)
{
	if(dwFlags&MCI_NOTIFY)
	{
		if(uNotifyMsg==MCI_NOTIFY_FAILURE)
			return dwReturnMsg;
			
		mciDriverNotify(hWndNotify,uDriverID,uNotifyMsg);
	}
	return dwReturnMsg;
}

DWORD WINAPI LOADDS MciDriverProc(WORD dwDriverID,WORD uMessage,DWORD lParam1,DWORD lParam2)
{
	MCI_OPEN_PARMS *lpMciOpenParms;
	MCI_GETDEVCAPS_PARMS *lpMciGetDevCapsParms;
	MCI_INFO_PARMS *lpMciInfoParms;
	MCI_STATUS_PARMS *lpMciStatusParms;
	MCI_LOAD_PARMS *lpMciLoadParms;
	MCI_SET_PARMS *lpMciSetParms;
	static DWORD dwStartTime,dwTickCount,dwPausedTime;
	DWORD dwSpare,dwResult;
	
	if(!lParam2)lParam2=(DWORD)&Generic;

	switch(uMessage)
	{
	//
	// Required command
	//
	case MCI_OPEN_DRIVER:
		lpMciOpenParms=(MCI_OPEN_PARMS*)lParam2;
		bFileLoaded=FALSE;
		if(lParam1&MCI_OPEN_ELEMENT)
		{
			strcpy(szElementName,lpMciOpenParms->lpstrElementName);
			if(!mciConfigParms.dwEngine)
			{
				if((dwResult=InitSPC(SPC_APU,mciConfigParms.dwChannels,mciConfigParms.dwSampleSize,mciConfigParms.dwSampleRate,mciConfigParms.dwVolumeMultiplier))!=MMSYSERR_NOERROR)
				{
					waveOutGetErrorText((UINT)dwResult,szBuffer,MAXERRORLENGTH);
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_WAVE_OUTPUTSINUSE);
				}
				if(LoadSPC(szElementName))
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_FILE_NOT_FOUND);
			}
			else
			{
				switch(InitZSPC())
				{
				case 1:
					sprintf(szBuffer,"OUT_WAVE.DLL was not found. This file is required for the ZSNES engine.");
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_FILE_NOT_FOUND);
					break;
				case 2:
					sprintf(szBuffer,"IN_SPC.DLL was not found. This file is required for the ZSNES engine.");
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_FILE_NOT_FOUND);
					break;
				}
			}
			bFileLoaded=TRUE;
		}
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_CLOSE_DRIVER:
		if(bPlaying||bPaused)
		{
			if(!mciConfigParms.dwEngine)
			{
				StopSPC();
				DeinitSPC(SPC_SND);
			}
			else
				StopZSPC();
			SetPriorityClass(GetCurrentProcess(),dwOldPriorityClass);
		}
		if(!mciConfigParms.dwEngine)
			DeinitSPC(SPC_APU);
		else
			DeinitZSPC();
		bPlaying=FALSE;bPaused=FALSE;
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_GETDEVCAPS:
		lpMciGetDevCapsParms=(MCI_GETDEVCAPS_PARMS*)lParam2;
		if(lParam1&MCI_GETDEVCAPS_ITEM)
		{
			switch(lpMciGetDevCapsParms->dwItem)
			{
			case MCI_GETDEVCAPS_CAN_PLAY:
			case MCI_GETDEVCAPS_COMPOUND_DEVICE:
			case MCI_GETDEVCAPS_HAS_AUDIO:
			case MCI_GETDEVCAPS_USES_FILES:
				lpMciGetDevCapsParms->dwReturn=TRUE;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_GETDEVCAPS_DEVICE_TYPE:
				lpMciGetDevCapsParms->dwReturn=MCI_DEVTYPE_OTHER;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			default:
				lpMciGetDevCapsParms->dwReturn=FALSE;
			}
		}
		break;	
	case MCI_INFO:
		lpMciInfoParms=(MCI_INFO_PARMS*)lParam2;
		if(lParam1&MCI_INFO_PRODUCT&&lpMciInfoParms->dwRetSize>strlen(szProduct))
		{
			strcpy(lpMciInfoParms->lpstrReturn,szProduct);
			return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		}
		break;
	case MCI_STATUS:
		lpMciStatusParms=(MCI_STATUS_PARMS*)lParam2;
		if(lParam1&MCI_STATUS_ITEM)
		{
			switch(lpMciStatusParms->dwItem)
			{
			case MCI_STATUS_MODE:
				lpMciStatusParms->dwReturn=bPlaying?MCI_MODE_PLAY:bPaused?MCI_MODE_PAUSE:MCI_MODE_STOP;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_STATUS_READY:
				lpMciStatusParms->dwReturn=TRUE;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_STATUS_TIME_FORMAT:
				lpMciStatusParms->dwReturn=MCI_FORMAT_MILLISECONDS;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_STATUS_LENGTH:
				lpMciStatusParms->dwReturn=mciConfigParms.dwSongLength*1000;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_STATUS_POSITION:
				if(bPaused)lpMciStatusParms->dwReturn=dwPausedTime-dwStartTime;
				else if(bPlaying)lpMciStatusParms->dwReturn=GetTickCount()-dwStartTime;
				else lpMciStatusParms->dwReturn=0;
				if(lpMciStatusParms->dwReturn>=(DWORD)(mciConfigParms.dwSongLength*1000))
				{
					if(!mciConfigParms.dwEngine)
						StopSPC();
					else
						StopZSPC();					
					bPlaying=FALSE;bPaused=FALSE;
				}
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			case MCI_STATUS_NUMBER_OF_TRACKS:
				lpMciStatusParms->dwReturn=1;
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
				break;
			default:
				lpMciStatusParms->dwReturn=FALSE;
			}
		}
		if(lParam1&MCI_STATUS_START)
		{
			lpMciStatusParms->dwReturn=0;
			return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		}
		break;
	//
	// Basic commands
	//
	case MCI_SET:
		lpMciSetParms=(MCI_SET_PARMS*)lParam2;
		if(lParam1&MCI_SET_TIME_FORMAT)
		{
			if(lpMciSetParms->dwTimeFormat==MCI_FORMAT_MILLISECONDS)
				return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		}
		if(lParam1&MCI_SET_AUDIO)
			return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_SEEK:
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_LOAD:
		lpMciLoadParms=(MCI_LOAD_PARMS*)lParam2;
		bFileLoaded=FALSE;
		if(lParam1&MCI_LOAD_FILE)
		{
			if(bPlaying)
				if(!mciConfigParms.dwEngine)
					StopSPC();
				else
					StopZSPC();
			strcpy(szElementName,lpMciLoadParms->lpfilename);
			if(!mciConfigParms.dwEngine)
				if(LoadSPC(szElementName))
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_FILE_NOT_FOUND);
			bFileLoaded=TRUE;
			return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		}
		break;
	case MCI_PLAY:
	case MCI_RESUME:
		if(bFileLoaded)
		{
			dwOldPriorityClass=GetPriorityClass(GetCurrentProcess());
			SetPriorityClass(GetCurrentProcess(),mciConfigParms.dwPriorityClass);
			if(!mciConfigParms.dwEngine)
				if((dwResult=InitSPC(SPC_SND,mciConfigParms.dwChannels,mciConfigParms.dwSampleSize,mciConfigParms.dwSampleRate,mciConfigParms.dwVolumeMultiplier))!=MMSYSERR_NOERROR)
				{
					waveOutGetErrorText((UINT)dwResult,szBuffer,MAXERRORLENGTH);
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_WAVE_OUTPUTSINUSE);
				}
			if(!bPaused)
			{
				if(!mciConfigParms.dwEngine)
				{
					StopSPC();
					PlaySPC();
				}
				else
				{
					StopZSPC();
					PlayZSPC(szElementName);
				}
				dwStartTime=GetTickCount();
				bPlaying=TRUE;bPaused=FALSE;
			}
			else
			{   
				if(!mciConfigParms.dwEngine)
					PlaySPC();
				else
					PlayZSPC(szElementName);
				dwStartTime=GetTickCount()-(dwPausedTime-dwStartTime);
				bPlaying=TRUE;bPaused=FALSE;
			}
		}
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_PAUSE:
		if(bPlaying)
		{
			if(!mciConfigParms.dwEngine)
			{
				PauseSPC();
				DeinitSPC(SPC_SND);
			}
			else
				PauseZSPC();
			dwPausedTime=GetTickCount();
			bPlaying=FALSE;bPaused=TRUE;
			SetPriorityClass(GetCurrentProcess(),dwOldPriorityClass);
		}
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	case MCI_STOP:
		if(bPlaying||bPaused)
		{
			if(!mciConfigParms.dwEngine)
			{
				StopSPC();
				DeinitSPC(SPC_SND);
			}
			else
				StopZSPC();
			dwPausedTime=GetTickCount();
			bPlaying=FALSE;bPaused=FALSE;
			SetPriorityClass(GetCurrentProcess(),dwOldPriorityClass);
		}
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	//
	// Extra commands
	//
	case MCI_CONFIG:
		memcpy(&mciConfigParms,(void*)lParam2,sizeof(MCI_CONFIG_PARMS));
		if(dwSpare!=mciConfigParms.dwEngine)
		{
			if(!dwSpare)
			{   
				StopSPC();
				DeinitSPC(SPC_APU|SPC_SND);
				switch(InitZSPC())
				{
				case 1:
					sprintf(szBuffer,"OUT_WAVE.DLL was not found. This file is required for the ZSNES engine.");
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					break;
				case 2:
					sprintf(szBuffer,"IN_SPC.DLL was not found. This file is required for the ZSNES engine.");
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					break;
				}
			}
			else
			{
				StopZSPC();
				DeinitZSPC();
				if((dwResult=InitSPC(SPC_APU,mciConfigParms.dwChannels,mciConfigParms.dwSampleSize,mciConfigParms.dwSampleRate,mciConfigParms.dwVolumeMultiplier))!=MMSYSERR_NOERROR)
				{
					waveOutGetErrorText((UINT)dwResult,szBuffer,MAXERRORLENGTH);
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
				}
				if(LoadSPC(szElementName))
				{
					sprintf(szBuffer,"%s was not found.",szElementName);
					MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
				}
			}
		}					
		if(bPlaying)
			MciDriverProc(LOWORD(dwDriverID),(WORD)MCI_PLAY,lParam1,lParam2);
		else if(bPaused)
			MciDriverProc(LOWORD(dwDriverID),(WORD)MCI_STOP,lParam1,lParam2);
		return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_SUCCESSFUL,MMSYSERR_NOERROR);
		break;
	}
	return MciNotifyReturn((HWND)((MCI_GENERIC_PARMS*)lParam2)->dwCallback,dwDriverID,lParam1,MCI_NOTIFY_FAILURE,MCIERR_UNSUPPORTED_FUNCTION);
}

LRESULT WINAPI LOADDS DriverProc(DWORD dwDriverID,HDRVR hDriver,UINT uMessage,LPARAM lParam1,LPARAM lParam2)
{   
	MCI_OPEN_DRIVER_PARMS *lpMciOpenDriverParms;
	LRESULT lResult;
	DWORD dwSpare,dwResult;

	switch(uMessage)
	{
	case DRV_LOAD:
		lResult=1;
		break;
	case DRV_FREE:
		lResult=0;
		break;
	case DRV_ENABLE:
		lResult=0;
		break;
	case DRV_DISABLE:
		lResult=0;
		break;
	case DRV_OPEN:
		if(lParam2)
		{
			lpMciOpenDriverParms=(MCI_OPEN_DRIVER_PARMS*)lParam2;
			lpMciOpenDriverParms->wCustomCommandTable=MCI_NO_COMMAND_TABLE;
			lpMciOpenDriverParms->wType=MCI_DEVTYPE_OTHER;
			lResult=lpMciOpenDriverParms->wDeviceID;
			ReadRegistrySettings(&mciConfigParms);
		}
		else lResult=0;
		break;
	case DRV_CLOSE:
		lResult=1;
		break;
	case DRV_QUERYCONFIGURE:
		lResult=TRUE;
		break;
	case DRV_CONFIGURE:
		dwSpare=mciConfigParms.dwEngine;
		switch(DisplayConfigDlg((HWND)lParam1,&mciConfigParms))
		{
		case -1:
			MessageBeep((UINT)-1);
			MessageBox(NULL,"Failed to display configuration dialog.",szAppName,MB_ICONSTOP);
			lResult=DRVCNF_CANCEL;
			break;
		case 1:
			if(dwSpare!=mciConfigParms.dwEngine)
			{
				if(!dwSpare)
				{   
					StopSPC();
					DeinitSPC(SPC_APU|SPC_SND);
					switch(InitZSPC())
					{
					case 1:
						sprintf(szBuffer,"OUT_WAVE.DLL was not found. This file is required for the ZSNES engine.");
						MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
						break;
					case 2:
						sprintf(szBuffer,"IN_SPC.DLL was not found. This file is required for the ZSNES engine.");
						MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
						break;
					}
				}
				else
				{
					StopZSPC();
					DeinitZSPC();
					if((dwResult=InitSPC(SPC_APU,mciConfigParms.dwChannels,mciConfigParms.dwSampleSize,mciConfigParms.dwSampleRate,mciConfigParms.dwVolumeMultiplier))!=MMSYSERR_NOERROR)
					{
						waveOutGetErrorText((UINT)dwResult,szBuffer,MAXERRORLENGTH);
						MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					}
					if(LoadSPC(szElementName))
					{
						sprintf(szBuffer,"%s was not found.",szElementName);
						MessageBox(NULL,szBuffer,szAppName,MB_ICONSTOP);
					}
				}
			}					
			if(bPlaying)
				MciDriverProc(LOWORD(dwDriverID),(WORD)MCI_PLAY,lParam1,lParam2);
			else if(bPaused)
				MciDriverProc(LOWORD(dwDriverID),(WORD)MCI_STOP,lParam1,lParam2);
			WriteRegistrySettings(&mciConfigParms);
			lResult=DRVCNF_OK;
			break;
		default:
			lResult=DRVCNF_CANCEL;
			break;
		}
		break;	
	default:
		if((!HIWORD(dwDriverID))&&(uMessage>=DRV_MCI_FIRST)&&(uMessage<=DRV_MCI_LAST))
		{
			lResult=MciDriverProc(LOWORD(dwDriverID),(WORD)uMessage,lParam1,lParam2);
		}
		else
		{
			lResult=DefDriverProc(dwDriverID,hDriver,uMessage,lParam1,lParam2);
		}
	}
	if(bDisplayMessage)
	{
		MessageBeep((UINT)-1);
		MessageBox(NULL,szBuffer,szAppName,0);
		bDisplayMessage=FALSE;
	}
	return lResult;
}