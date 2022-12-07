#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdio.h>
#include "OutWave.h"
#include "Uematsu.h"

typedef struct _WAVEHEADER
{
	TCHAR szRIFF[4];
	DWORD dwRIFFSize;
	TCHAR szWAVE[4];
	TCHAR szFMT[4];
	DWORD dwFMTSize;
	WORD wFormatTag;
	WORD wChannels;
	DWORD dwSampleRate;
	DWORD dwAvgBytesPerSec;
	WORD wBlockAlign;
	WORD wBitsPerSample;
	TCHAR szDATA[4];
	DWORD dwDATASize;
}WAVEHEADER,*PWAVEHEADER,FAR *LPWAVEHEADER;

TCHAR szDllName[]="UEMATSU.DLL";

UEMATSUCONFIG GlblConfig=
{
	44100,
	16,
	2,
	MIX_80386,
	INT_NONE,
	1000,
	30,
	0,
	7872,
	0,
};

UEMATSUVISPARAMS SynchedVisParams;

void *lpSPCRAM;
u8 *lpExtraRAM;
u8 *lpSPCOut;
u32 *lpT64Cnt;
DSPRAM *lpDSP;
Channel *lpMix;
u32 *lpVMMaxL;
u32 *lpVMMaxR;

volatile LPBYTE lpPCMBuffer;
volatile BOOL bKillThread,bThreadFailed,bDone,bLoaded,bPlaying,bPaused,bWithSound,bFastSeek;
volatile BOOL bSeeking;
volatile UINT uNumSmp,uFrameSize,uSmpRate;
volatile UINT uOutputTime,uLastKeyOnTime;
volatile unsigned long ulThreadHandle=-1;
volatile LPBYTE lpSPCBackup;
TCHAR szWAVFileName[MAX_PATH];
volatile HANDLE hWAVFile=INVALID_HANDLE_VALUE;
WAVEHEADER WAVHDR;
DWORD dwBytesWritten;

volatile BOOL bSetSongLength,bSetPlayTime,bSetSongSpeed,bSetSongPitch,bSetMainSS,bSetEchoSS,bSetPreAmp,bSetMixingThreadPriority;
volatile UINT uSongLen,uFadeLen,uPlayTime,uAPUSmpClk=65536,uDSPPitch=32000,uMainSS=32768;
volatile INT nEchoSS=32768,uAmplification=50,nMixingThreadPriority=THREAD_PRIORITY_NORMAL;
volatile UINT uAPR,uThreshold,uPreamp,uIncTime;
volatile HWND hWndParent;

void SetDate(LPTSTR lpszDate,u16 year,u8 month,u8 day);
BOOL IsString(LPCTSTR lpszStr,INT nLen);
void __cdecl EmuSPCThread(void *p);
BOOL StartThread();
BOOL StopThread();

extern "C" BOOL WINAPI Uematsu_Init()
{
	if(bLoaded)
		return FALSE;

	SetAPUOpt(GlblConfig.uMixingRoutine,GlblConfig.uChannels,GlblConfig.uBitsPerSample,GlblConfig.uSampleRate,GlblConfig.uInterpolation,GlblConfig.fMiscOptions);
	GetAPUData(&lpSPCRAM,&lpExtraRAM,&lpSPCOut,&lpT64Cnt,&lpDSP,&lpMix,&lpVMMaxL,&lpVMMaxR);
	lpPCMBuffer=(LPBYTE)malloc(65536);
	if(!lpPCMBuffer)
		return FALSE;
	bLoaded=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_DeInit()
{
	if(!bLoaded)
		return FALSE;

	if(bPlaying)
		Uematsu_Stop();

	free(lpPCMBuffer);
	bLoaded=FALSE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetWindowHandle(HWND hWnd)
{
	hWndParent=hWnd;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_GetConfiguration(LPUEMATSUCONFIG lpuc)
{
	if(!bLoaded)
		return FALSE;

	if(lpuc)
	{
		memcpy(lpuc,&GlblConfig,sizeof(GlblConfig));
		return TRUE;
	}
	return FALSE;
}

extern "C" BOOL WINAPI Uematsu_SetConfiguration(LPUEMATSUCONFIG lpuc)
{
	if(!bLoaded)
		return FALSE;

	if(lpuc)
	{
		BOOL bRestart=FALSE;
		if(ulThreadHandle!=-1)
		{
			if(!StopThread())
				return FALSE;
			bRestart=TRUE;
		}

		memcpy(&GlblConfig,lpuc,sizeof(GlblConfig));
		uNumSmp=GlblConfig.uSampleRate/GlblConfig.uVisualizationRate;
		uFrameSize=(GlblConfig.uChannels-1)+(GlblConfig.uBitsPerSample>>4);
		uSmpRate=GlblConfig.uSampleRate;
		uAPR=GlblConfig.uAPR&3;
		uPreamp=uAmplification;
		uIncTime=0;
		uThreshold=GlblConfig.uThreshold+32768;
		SetAPUOpt(GlblConfig.uMixingRoutine,GlblConfig.uChannels,GlblConfig.uBitsPerSample,GlblConfig.uSampleRate,GlblConfig.uInterpolation,GlblConfig.fMiscOptions);
		SetDSPAmp(uAmplification);
		if(OW_IsOpen())
		{
			OW_Close();
			memset(lpPCMBuffer,0,65536);
			if(OW_Open(GlblConfig.uSampleRate,GlblConfig.uChannels,GlblConfig.uBitsPerSample,GlblConfig.uBufferLength,GlblConfig.uVisualizationRate)<0)
				return FALSE;
		}

		if(bRestart&&!StartThread())
			return FALSE;
		return TRUE;
	}

	return FALSE;
}

extern "C" BOOL WINAPI Uematsu_Play(LPCTSTR lpszFileName)
{
	if(!bLoaded)
		return FALSE;

	if(bPlaying)
		if(!Uematsu_Stop())
			return FALSE;

	HANDLE hFile;
	hFile=CreateFile(lpszFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	DWORD dwFileSize=GetFileSize(hFile,NULL);
	lpSPCBackup=(LPBYTE)malloc(dwFileSize);
	if(!lpSPCBackup)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	DWORD dwNumBytesRead;
	if(!ReadFile(hFile,lpSPCBackup,dwFileSize,&dwNumBytesRead,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	if(!CloseHandle(hFile))
		return FALSE;

	ResetAPU(uAmplification);

	SPCHEADER SPC;
	memcpy(&SPC,lpSPCBackup,256);
	memcpy(lpSPCRAM,lpSPCBackup+256,65472);
	memcpy(lpDSP,lpSPCBackup+65792,128);
	memcpy(lpExtraRAM,lpSPCBackup+65984,64);

	FixAPU(*(u16*)(&SPC.Regs.PC),SPC.Regs.A,SPC.Regs.Y,SPC.Regs.X,SPC.Regs.PSW,SPC.Regs.SP);

	uNumSmp=GlblConfig.uSampleRate/GlblConfig.uVisualizationRate;
	uFrameSize=(GlblConfig.uChannels-1)+(GlblConfig.uBitsPerSample>>4);
	uSmpRate=GlblConfig.uSampleRate;
	uAPR=GlblConfig.uAPR&3;
	uPreamp=uAmplification;
	uIncTime=0;
	uThreshold=GlblConfig.uThreshold+32768;

	if(strlen(szWAVFileName))
	{
		strncpy(WAVHDR.szRIFF,"RIFF",4);
		WAVHDR.dwRIFFSize=0;
		strncpy(WAVHDR.szWAVE,"WAVE",4);
		strncpy(WAVHDR.szFMT,"fmt ",4);
		WAVHDR.dwFMTSize=16;
		WAVHDR.wFormatTag=1;
		WAVHDR.wChannels=GlblConfig.uChannels;
		WAVHDR.dwSampleRate=GlblConfig.uSampleRate;
		WAVHDR.dwAvgBytesPerSec=GlblConfig.uSampleRate*GlblConfig.uChannels*GlblConfig.uBitsPerSample/8;
		WAVHDR.wBlockAlign=GlblConfig.uChannels*GlblConfig.uBitsPerSample/8;
		WAVHDR.wBitsPerSample=GlblConfig.uBitsPerSample;
		strncpy(WAVHDR.szDATA,"data",4);
		WAVHDR.dwDATASize=0;

		hWAVFile=CreateFile(szWAVFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hWAVFile==INVALID_HANDLE_VALUE)
			return FALSE;
		if(!WriteFile(hWAVFile,&WAVHDR,sizeof(WAVHDR),&dwBytesWritten,NULL))
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
	}
	if(hWAVFile==INVALID_HANDLE_VALUE||bWithSound)
	{
		memset(lpPCMBuffer,0,65536);
		if(OW_Open(GlblConfig.uSampleRate,GlblConfig.uChannels,GlblConfig.uBitsPerSample,GlblConfig.uBufferLength,GlblConfig.uVisualizationRate)<0)
			return FALSE;
	}

	bSetSongSpeed=bSetSongPitch=bSetMainSS=bSetEchoSS=bSetPreAmp=bSetMixingThreadPriority=TRUE;

	if(!StartThread())
	{
		OW_Close();
		CloseHandle(hWAVFile);
		DeleteFile(szWAVFileName);
		return FALSE;
	}

	bPlaying=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_Stop()
{
	if(!bLoaded||!bPlaying)
		return FALSE;

	if(ulThreadHandle!=-1)
	{
		if(!StopThread())
		{
			MessageBox(hWndParent,"The emulation thread won't cooperate. Press OK to terminate!",szDllName,MB_ICONERROR);
			TerminateThread((HANDLE)ulThreadHandle,0);
			ulThreadHandle=-1;
			OW_Close();
			return FALSE;
		}
	}
	OW_Close();
	if(hWAVFile!=INVALID_HANDLE_VALUE)
	{
		WAVHDR.dwRIFFSize=dwBytesWritten-8;
		WAVHDR.dwDATASize=dwBytesWritten-sizeof(WAVHDR);
		if(SetFilePointer(hWAVFile,0,NULL,FILE_BEGIN)==0xFFFFFFFF)
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
		if(!WriteFile(hWAVFile,&WAVHDR,sizeof(WAVHDR),&dwBytesWritten,NULL))
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
		if(!CloseHandle(hWAVFile))
			return FALSE;
		hWAVFile=INVALID_HANDLE_VALUE;
	}
	if(lpSPCBackup)
		free(lpSPCBackup);

	bPlaying=FALSE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_Pause()
{
	if(!bLoaded||!bPlaying)
		return FALSE;

	OW_Pause(TRUE);
	bPaused=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_UnPause()
{
	if(!bLoaded||!bPlaying)
		return FALSE;

	OW_Pause(FALSE);
	bPaused=FALSE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetWAVOutputFileName(LPCTSTR lpszFileName,BOOL bWithSound)
{
	if(lpszFileName)
		strncpy(szWAVFileName,lpszFileName,sizeof(szWAVFileName));
	else
		szWAVFileName[0]='\0';
	::bWithSound=bWithSound;

	if(!bPlaying)
		return TRUE;

	BOOL bRestart=FALSE;
	if(ulThreadHandle!=-1)
	{
		if(!StopThread())
			return FALSE;
		bRestart=TRUE;
	}
	if(hWAVFile!=INVALID_HANDLE_VALUE)
	{
		WAVHDR.dwRIFFSize=dwBytesWritten-8;
		WAVHDR.dwDATASize=dwBytesWritten-sizeof(WAVHDR);
		if(SetFilePointer(hWAVFile,0,NULL,FILE_BEGIN)==0xFFFFFFFF)
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
		if(!WriteFile(hWAVFile,&WAVHDR,sizeof(WAVHDR),&dwBytesWritten,NULL))
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
		if(!CloseHandle(hWAVFile))
			return FALSE;
		hWAVFile=INVALID_HANDLE_VALUE;
		if(!OW_IsOpen())
		{
			memset(lpPCMBuffer,0,65536);
			if(OW_Open(GlblConfig.uSampleRate,GlblConfig.uChannels,GlblConfig.uBitsPerSample,GlblConfig.uBufferLength,GlblConfig.uVisualizationRate)<0)
				return FALSE;
			uOutputTime=*lpT64Cnt/64;
		}
	}
	if(strlen(szWAVFileName))
	{
		strncpy(WAVHDR.szRIFF,"RIFF",4);
		WAVHDR.dwRIFFSize=0;
		strncpy(WAVHDR.szWAVE,"WAVE",4);
		strncpy(WAVHDR.szFMT,"fmt ",4);
		WAVHDR.dwFMTSize=16;
		WAVHDR.wFormatTag=1;
		WAVHDR.wChannels=GlblConfig.uChannels;
		WAVHDR.dwSampleRate=GlblConfig.uSampleRate;
		WAVHDR.dwAvgBytesPerSec=GlblConfig.uSampleRate*GlblConfig.uChannels*GlblConfig.uBitsPerSample/8;
		WAVHDR.wBlockAlign=GlblConfig.uChannels*GlblConfig.uBitsPerSample/8;
		WAVHDR.wBitsPerSample=GlblConfig.uBitsPerSample;
		strncpy(WAVHDR.szDATA,"data",4);
		WAVHDR.dwDATASize=0;

		hWAVFile=CreateFile(szWAVFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(hWAVFile==INVALID_HANDLE_VALUE)
			return FALSE;
		if(!WriteFile(hWAVFile,&WAVHDR,sizeof(WAVHDR),&dwBytesWritten,NULL))
		{
			CloseHandle(hWAVFile);
			return FALSE;
		}
		if(!bWithSound)
			OW_Close();
	}
	if(bRestart)
		StartThread();
	return TRUE;
}

extern "C" UINT WINAPI Uematsu_GetPlayTime()
{
	if(!bLoaded)
		return FALSE;

	return (bSeeking||!OW_IsOpen())?*lpT64Cnt/64:uOutputTime;
}

extern "C" BOOL WINAPI Uematsu_SetPlayTime(UINT uTime, BOOL bFastSeek)
{
	if(!bLoaded)
		return FALSE;

	uPlayTime=uTime;
	::bFastSeek=bFastSeek;
	bSetPlayTime=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetSongLength(UINT uSong,UINT uFade)
{
	if(!bLoaded)
		return FALSE;

	uSongLen=uSong;
	uFadeLen=uFade;
	bSetSongLength=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetSongSpeed(UINT uSpeed)
{
	if(!bLoaded)
		return FALSE;

	uAPUSmpClk=uSpeed;
	bSetSongSpeed=TRUE;	
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetSongPitch(UINT uPitch)
{
	if(!bLoaded)
		return FALSE;

	uDSPPitch=uPitch;
	bSetSongPitch=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetMainSS(UINT uSeparation)
{
	if(!bLoaded)
		return FALSE;

	uMainSS=uSeparation;
	bSetMainSS=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetEchoSS(INT nSeparation)
{
	if(!bLoaded)
		return FALSE;

	nEchoSS=nSeparation;
	bSetEchoSS=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetPreAmp(UINT uPreAmp)
{
	if(!bLoaded)
		return FALSE;

	uAmplification=uPreAmp;
	bSetPreAmp=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SetMixingThreadPriority(INT nPriority)
{
	if(!bLoaded)
		return FALSE;

	nMixingThreadPriority=nPriority;
	bSetMixingThreadPriority=TRUE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_IsActive()
{
	if(!bLoaded)
		return FALSE;

	if((hWAVFile==INVALID_HANDLE_VALUE||bWithSound)&&!bSetPlayTime)
		return ((INT)uLastKeyOnTime>(INT)uOutputTime-3000);
	else
		return !bDone;
	return FALSE;
}

extern "C" BOOL WINAPI Uematsu_IsPlaying()
{
	if(!bLoaded)
		return FALSE;

	if((hWAVFile==INVALID_HANDLE_VALUE||bWithSound)&&!bSetPlayTime)
		return OW_IsPlaying();
	else
		return !bDone;
	return FALSE;
}

extern "C" BOOL WINAPI Uematsu_HasFailed()
{
	return bThreadFailed;
}

extern "C" BOOL WINAPI Uematsu_MuteChannels(INT nChannels)
{
	if(!bLoaded)
		return FALSE;

	for(INT i=0;i<8;i++)
	{
		if(nChannels&(1<<i))
			lpMix[i].MFlg|=1;
		else
			lpMix[i].MFlg&=~1;
	}
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_GetVisParams(LPUEMATSUVISPARAMS lpuvp)
{
	if(!bLoaded)
		return FALSE;

	memset(lpuvp,0,sizeof(UEMATSUVISPARAMS));
	if(hWAVFile==INVALID_HANDLE_VALUE||bWithSound)
	{
		memcpy(&lpuvp->DSP,&SynchedVisParams.DSP,sizeof(SynchedVisParams.DSP));
		memcpy(lpuvp->Mix,SynchedVisParams.Mix,sizeof(SynchedVisParams.Mix));
		lpuvp->nVMMaxL=SynchedVisParams.nVMMaxL;
		lpuvp->nVMMaxR=SynchedVisParams.nVMMaxR;
		memcpy(lpuvp->nDecibelList,SynchedVisParams.nDecibelList,sizeof(SynchedVisParams.nDecibelList));
		lpuvp->lpPCMBuffer=SynchedVisParams.lpPCMBuffer;
		lpuvp->uPCMBufferLength=SynchedVisParams.uPCMBufferLength;
		lpuvp->uCount=SynchedVisParams.uCount;
		lpuvp->uT64Cnt=SynchedVisParams.uT64Cnt;
		lpuvp->uPreAmp=SynchedVisParams.uPreAmp;
	}
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_LoadID666Tag(LPCTSTR lpszFileName,LPID666TAG lpTag)
{
	if(!bLoaded)
		return FALSE;

	HANDLE hFile;
	hFile=CreateFile(lpszFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	SPCHEADER SPC;
	DWORD dwNumBytesRead;
	if(!ReadFile(hFile,&SPC,sizeof(SPC),&dwNumBytesRead,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}

	TCHAR SongStr[4]={0},FadeStr[6]={0};

	memset(lpTag,0,sizeof(ID666TAG));
	memcpy(lpTag->szTitle,SPC.Title,32);
	memcpy(lpTag->szGame,SPC.Game,32);
	memcpy(lpTag->szDumper,SPC.Dumper,16);
	memcpy(lpTag->szDate,SPC.Date,11);
	memcpy(lpTag->szComment,SPC.Comment,32);
	memcpy(SongStr,SPC.SongLen,3);
	memcpy(FadeStr,SPC.FadeLen,5);

	BOOL Bin=FALSE;

	if(IsString(SongStr,3)&&IsString(FadeStr,5)&&IsString(lpTag->szDate,11))
	{
		if(SongStr[0]!=0||FadeStr[0]!=0||lpTag->szDate[0]!=0)
			Bin=FALSE;
	}
	else Bin=TRUE;

	s32 i,j;
	s8 str[12],*t;
	u32 d,m,y;
	if(Bin)
	{
		j=*(u32*)(FadeStr);
		if(j>59999)j=59999;

		i=*(u16*)(SongStr);
		if(i>959)i=959;

		lpTag->uSong_ms=i;
		lpTag->uFade_ms=j;

		memcpy(lpTag->szArtist,SPC.Artist-1,32);
		lpTag->bEmulator=SPC.ChnDis;

		y=*(u16*)(&lpTag->szDate[2]);
		m=*(u8*)(&lpTag->szDate[1]);
		d=*(u8*)(&lpTag->szDate[0]);
	}
	else
	{
		lpTag->uSong_ms=atoi(SongStr);
		lpTag->uFade_ms=atoi(FadeStr);
		if(lpTag->uSong_ms>959)lpTag->uSong_ms=959;
		if(lpTag->uFade_ms>59999)lpTag->uFade_ms=59999;

		memcpy(lpTag->szArtist,SPC.Artist,32);
		lpTag->bEmulator=SPC.Emulator;
		if(lpTag->bEmulator>=0x30&&lpTag->bEmulator<=0x39)lpTag->bEmulator-=0x30;
		else lpTag->bEmulator=0;

		y=0;
		strcpy(str,lpTag->szDate);
		if(str[0]!=0)
		{
			t=strchr(str,'/');
			if(t&&*(++t)!=0)
			{
				d=atoi(t);
				t=strchr(t,'/');
				if(t&&*(++t)!=0)
				{
					y=atoi(t);
					m=atoi(str);
				}
			}
		}
	}
	
	SetDate(lpTag->szDate,y,m,d);
	lpTag->uSong_ms*=1000;

	if(SetFilePointer(hFile,66048,NULL,FILE_BEGIN)!=66048)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	DWORD dwBytesRead;
	SUBCHK sub;
	TCHAR szBuf[5]={0};
	ReadFile(hFile,szBuf,4,&dwBytesRead,NULL);

	if(!stricmp(szBuf,"xid6"))
	{
		SetFilePointer(hFile,4,NULL,FILE_CURRENT);
		ReadFile(hFile,&sub,sizeof(sub),&dwBytesRead,NULL);

		while(dwBytesRead)
		{
			switch(sub.id)
			{
			case XID_SONG:
				ReadFile(hFile,lpTag->szTitle,sub.data,&dwBytesRead,NULL);
				break;
			case XID_GAME:
				ReadFile(hFile,lpTag->szGame,sub.data,&dwBytesRead,NULL);
				break;
			case XID_ARTIST:
				ReadFile(hFile,lpTag->szArtist,sub.data,&dwBytesRead,NULL);
				break;
			case XID_DUMPER:
				ReadFile(hFile,lpTag->szDumper,sub.data,&dwBytesRead,NULL);
				break;
			case XID_DATE:
				UINT uDate;
				ReadFile(hFile,&uDate,sub.data,&dwBytesRead,NULL);
				SetDate(lpTag->szDate,uDate>>16,(uDate>>8)&0xFF,uDate&0xFF);
				break;
			case XID_EMU:
				lpTag->bEmulator=(BYTE)sub.data;
				break;
			case XID_CMNTS:
				ReadFile(hFile,lpTag->szComment,sub.data,&dwBytesRead,NULL);
				break;
			case XID_OST:
				ReadFile(hFile,lpTag->szOST,sub.data,&dwBytesRead,NULL);
				break;
			case XID_DISC:
				lpTag->bDisc=(BYTE)sub.data;
				break;
			case XID_TRACK:
				lpTag->wTrack=(WORD)sub.data;
				break;
			case XID_PUB:
				ReadFile(hFile,lpTag->szPublisher,sub.data,&dwBytesRead,NULL);
				break;
			case XID_COPY:
				lpTag->wCopyright=(WORD)sub.data;
				break;
			case XID_INTRO:
				ReadFile(hFile,&lpTag->uSong_ms,sub.data,&dwBytesRead,NULL);
				if(lpTag->uSong_ms>61376000)lpTag->uSong_ms=61376000;
				lpTag->uSong_ms/=64;
				break;
			case XID_LOOP:
				ReadFile(hFile,&lpTag->uLoop_ms,sub.data,&dwBytesRead,NULL);
				if(lpTag->uLoop_ms>383936000)lpTag->uLoop_ms=383936000;
				lpTag->uLoop_ms/=64;
				break;
			case XID_END:
				ReadFile(hFile,&lpTag->uEnd_ms,sub.data,&dwBytesRead,NULL);
				if(lpTag->uEnd_ms>61376000)lpTag->uEnd_ms=61376000;
				lpTag->uEnd_ms/=64;
				break;
			case XID_FADE:
				ReadFile(hFile,&lpTag->uFade_ms,sub.data,&dwBytesRead,NULL);
				if(lpTag->uFade_ms>3839360)lpTag->uFade_ms=3839360;
				lpTag->uFade_ms/=64;
				break;
			case XID_MUTE:
				lpTag->bMute=(BYTE)sub.data;
				break;
			default:
				if(sub.type)
					SetFilePointer(hFile,sub.data,NULL,FILE_CURRENT);
				break;
			}
			ReadFile(hFile,&sub,sizeof(sub),&dwBytesRead,NULL);
		}
	}

	if(!CloseHandle(hFile))
		return FALSE;
	return TRUE;
}

extern "C" BOOL WINAPI Uematsu_SaveID666Tag(LPCTSTR lpszFileName,LPID666TAG lpTag)
{
	if(!bLoaded)
		return FALSE;

	TCHAR SongStr[3]={0},FadeStr[5]={0};

	sprintf(SongStr,"%i",lpTag->uSong_ms/1000);
	sprintf(FadeStr,"%i",lpTag->uFade_ms);

	HANDLE hFile;
	hFile=CreateFile(lpszFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwNumBytesWritten;
	SPCHEADER SPC;
	if(!ReadFile(hFile,&SPC,256,&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	TCHAR SPCID[]={"SNES-SPC700 Sound File Data v0.30"};
	TCHAR SPCTerm[]={26,26,26,30};
	memcpy(SPC.FTag,SPCID,sizeof(SPCID));
	memcpy(SPC.Term,SPCTerm,sizeof(SPCTerm));
	memcpy(SPC.Artist,lpTag->szArtist,32);
	memcpy(SPC.Date,lpTag->szDate,11);
	memcpy(SPC.SongLen,SongStr,3);
	memcpy(SPC.FadeLen,FadeStr,5);
	SPC.Emulator=lpTag->bEmulator+48;
	memcpy(SPC.Title,lpTag->szTitle,32);
	memcpy(SPC.Game,lpTag->szGame,32);
	memcpy(SPC.Dumper,lpTag->szDumper,16);
	memcpy(SPC.Comment,lpTag->szComment,32);

	void *lpBuf=malloc(65792);
	if(!lpBuf)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	if(!ReadFile(hFile,lpBuf,65792,&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	CloseHandle(hFile);
	hFile=CreateFile(lpszFileName,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
		return FALSE;

	if(!WriteFile(hFile,&SPC,256,&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	if(!WriteFile(hFile,lpBuf,65792,&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	free(lpBuf);
	TCHAR szID[]="xid6";
	if(!WriteFile(hFile,szID,strlen(szID),&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	DWORD dwChkSize=0;
	if(!WriteFile(hFile,&dwChkSize,sizeof(dwChkSize),&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	SUBCHK sub;
	sub.id=XID_SONG;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szTitle)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szTitle,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_GAME;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szGame)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szGame,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_ARTIST;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szArtist)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szArtist,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_DUMPER;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szDumper)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szDumper,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	/*sub.id=XID_DATE;
	sub.type=XTYPE_INT;
	sub.data=sizeof(UINT);
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;*/
	sub.id=XID_EMU;
	sub.type=XTYPE_DATA;
	sub.data=lpTag->bEmulator;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_CMNTS;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szComment)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szComment,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_OST;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szOST)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szOST,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_DISC;
	sub.type=XTYPE_DATA;
	sub.data=lpTag->bDisc;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_TRACK;
	sub.type=XTYPE_DATA;
	sub.data=lpTag->wTrack;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_PUB;
	sub.type=XTYPE_STR;
	sub.data=strlen(lpTag->szPublisher)+1;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,lpTag->szPublisher,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_COPY;
	sub.type=XTYPE_DATA;
	sub.data=lpTag->wCopyright;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_INTRO;
	sub.type=XTYPE_INT;
	UINT uTime=lpTag->uSong_ms*64;
	sub.data=sizeof(uTime);
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,&uTime,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_LOOP;
	sub.type=XTYPE_INT;
	uTime=lpTag->uLoop_ms*64;
	sub.data=sizeof(uTime);
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,&uTime,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_END;
	sub.type=XTYPE_INT;
	uTime=lpTag->uEnd_ms*64;
	sub.data=sizeof(uTime);
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,&uTime,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_FADE;
	sub.type=XTYPE_INT;
	uTime=lpTag->uFade_ms*64;
	sub.data=sizeof(uTime);
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	WriteFile(hFile,&uTime,sub.data,&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;
	sub.id=XID_MUTE;
	sub.type=XTYPE_DATA;
	sub.data=lpTag->bMute;
	WriteFile(hFile,&sub,sizeof(sub),&dwNumBytesWritten,NULL);dwChkSize+=dwNumBytesWritten;

	SetFilePointer(hFile,66052,NULL,FILE_BEGIN);
	if(!WriteFile(hFile,&dwChkSize,sizeof(dwChkSize),&dwNumBytesWritten,NULL))
	{
		CloseHandle(hFile);
		return FALSE;
	}
	
	if(!CloseHandle(hFile))
		return FALSE;

	return TRUE;
}

BOOL WINAPI Uematsu_GetCoreInfo(LPUINT lpuVersion,LPUINT lpuMinVersion,LPUINT lpuOptions)
{
	SNESAPUInfo(lpuVersion,lpuMinVersion,lpuOptions);
	return TRUE;
}

UINT WINAPI Uematsu_GetCurrentPreamp()
{	
	return SynchedVisParams.uPreAmp;
}

int I2BCD(int i)
{
	int bcd[3];

	_asm
	{
		fild	dword ptr [i]
		fbstp	tbyte ptr [bcd]
	}
	return(bcd[0]);
}

int BCD2I(int bcd)
{
	int btemp[3],itemp;

	btemp[0]=bcd;
	btemp[1]=0;
	btemp[2]=0;

	_asm
	{
		fbld	tbyte ptr [btemp]
		fistp	dword ptr [itemp]
	}
	return(itemp);
}

void SetDate(LPTSTR lpszDate,u16 year,u8 month,u8 day)
{
	if(year<100)
	{
		year+=1900;
		if(year<1997)year+=100;
	}
	if(year<1997)year=0;
	if(month<1||month>12)year=0;
	if(day<1||day>31)year=0;

	if(year)
	{
		if(month>2)
		{
			if(!(year&3)&&year%100)day++;
			else if(!(year%400))day++;
		}
		sprintf(lpszDate,"%u/%u/%u",month,day,year);
	}
	else lpszDate[0]='\0';
}

BOOL IsString(LPCTSTR lpszStr,INT nLen)
{
	while(--nLen>=0&&lpszStr[nLen]==0);
	while(nLen>=0&&((lpszStr[nLen]>=0x30&&lpszStr[nLen]<=0x39)||lpszStr[nLen]=='/'))nLen--;
	return ((nLen==-1)?TRUE:FALSE);
}

BOOL StartThread()
{
	bKillThread=bThreadFailed=bDone=bPaused=FALSE;
	ulThreadHandle=_beginthread(EmuSPCThread,0,NULL);
	if(ulThreadHandle==-1)
		return FALSE;
	return TRUE;
}

BOOL StopThread()
{
	bKillThread=TRUE;
	if(WaitForSingleObject((HANDLE)ulThreadHandle,3000)!=WAIT_OBJECT_0)
		return FALSE;
	ulThreadHandle=-1;
	return TRUE;
}

void __cdecl EmuSPCThread(void *p)
{
	LPBYTE lpPCMDblBuffer=lpPCMBuffer;
	BOOL bPass=TRUE;

	while(!bKillThread)
	{
		if(bSetSongLength)
		{
			if(uSongLen)
				SetAPULength(uSongLen*64,uFadeLen*64);
			else
				SetAPULength(-1,0);
			bSetSongLength=FALSE;
		}
		if(bSetPlayTime)
		{
			bSeeking=TRUE;
			if(OW_IsOpen())
				OW_Flush();			
			if(*lpT64Cnt>uPlayTime*64)
			{
				ResetAPU(uAmplification);

				SPCHEADER SPC;
				memcpy(&SPC,lpSPCBackup,256);
				memcpy(lpSPCRAM,lpSPCBackup+256,65472);
				memcpy(lpDSP,lpSPCBackup+65792,128);
				memcpy(lpExtraRAM,lpSPCBackup+65984,64);

				FixAPU(*(u16*)(&SPC.Regs.PC),SPC.Regs.A,SPC.Regs.Y,SPC.Regs.X,SPC.Regs.PSW,SPC.Regs.SP);

				if(uSongLen)
					SetAPULength(uSongLen*64,uFadeLen*64);
				else
					SetAPULength(-1,0);
			}
			while(*lpT64Cnt<uPlayTime*64&&!bKillThread)
				SeekAPU(6400,bFastSeek!=0);			
			uOutputTime=*lpT64Cnt/64;
			bSetPlayTime=bSeeking=FALSE;
		}
		if(bSetSongSpeed)
		{
			SetAPUSmpClk(uAPUSmpClk);
			bSetSongSpeed=FALSE;
		}
		if(bSetSongPitch)
		{
			SetDSPPitch(uDSPPitch);
			bSetSongPitch=FALSE;
		}
		if(bSetMainSS)
		{
			SetDSPStereo(uMainSS);
			bSetMainSS=FALSE;
		}
		if(bSetEchoSS)
		{
			SetDSPEFBCT(nEchoSS);
			bSetEchoSS=FALSE;
		}
		if(bSetPreAmp)
		{
			SetDSPAmp(uAmplification);
			bSetPreAmp=FALSE;
		}
		if(bSetMixingThreadPriority)
		{
			SetThreadPriority(GetCurrentThread(),nMixingThreadPriority);
			bSetMixingThreadPriority=FALSE;
		}

		if(bDone)
		{
			if(hWAVFile==INVALID_HANDLE_VALUE?!OW_IsPlaying():bWithSound?!OW_IsPlaying():TRUE)
			{
				_endthread();
				return;
			}
		}
		else if((OW_IsOpen()?OW_CanWrite()>=(uNumSmp<<uFrameSize):!bPaused)&&!bKillThread)
		{
			for(UINT i=0;i<8;i++)
				*lpVMMaxL=*lpVMMaxR=lpMix[i].VMaxL=lpMix[i].VMaxR=0;

			while((UINT)lpPCMDblBuffer<(UINT)lpPCMBuffer+(2*uNumSmp<<uFrameSize)&&!bKillThread)
				lpPCMDblBuffer=(LPBYTE)EmuAPU(lpPCMDblBuffer,uNumSmp*(APU_CLK/uSmpRate),0);

			if(!OW_IsOpen()&&!bKillThread)
			{
				DWORD dw;
				if(!WriteFile(hWAVFile,lpPCMBuffer,uNumSmp<<uFrameSize,&dw,NULL))
				{
					MessageBox(hWndParent,"Failed to write to the WAV output file.",szDllName,MB_ICONERROR);
					bThreadFailed=bDone=TRUE;
					_endthread();
					return;
				}
				dwBytesWritten+=dw;
			}
			else if(!bKillThread)
				OW_Write((char*)lpPCMBuffer,uNumSmp<<uFrameSize);

			UINT uMoveSize=(lpPCMDblBuffer-lpPCMBuffer)-(uNumSmp<<uFrameSize);
			memmove(lpPCMBuffer,lpPCMBuffer+(uNumSmp<<uFrameSize),uMoveSize);
			lpPCMDblBuffer=lpPCMBuffer+uMoveSize;

			if(uAPR)
			{
				if(*lpVMMaxL>(int)uThreshold||*lpVMMaxR>(int)uThreshold)
				{
					u32 nGain=max(*lpVMMaxL,*lpVMMaxR);

					_asm
					{
						mov eax,dword ptr [uThreshold]
						mul dword ptr [uPreamp]
						div dword ptr [nGain]
						cmp eax,[uPreamp]
						adc al,-1
						mov [uPreamp],eax
					}

					SetDSPAmp(uPreamp);
				}
				else if(uAPR==2)
				{
					if(*lpT64Cnt-uIncTime>16000)
					{
						uIncTime=*lpT64Cnt;
						if(++uPreamp>MaxAmp)
							uPreamp=MaxAmp;
						SetDSPAmp(uPreamp);
					}
				}
			}

			//Taken out because it's faulty on some computers...
			//if(uSongLen&&(*lpT64Cnt/64)>=(uSongLen+uFadeLen))
			//	bDone=TRUE;
		}
		Sleep(10);
	}
	bDone=TRUE;
	_endthread();
}

extern "C" BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		if(!Uematsu_Init())
			return FALSE;
		break;
	case DLL_PROCESS_DETACH:
		if(!Uematsu_DeInit())
			return FALSE;
		break;
	}
	return TRUE;
}
