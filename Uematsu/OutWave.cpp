#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "OutWave.h"
#include "Uematsu.h"

struct _SNDBUFFER
{
	BOOL bFull,bPlaying;
	DWORD dwSize;
	WAVEHDR wh;
	UEMATSUVISPARAMS uvp;
}
*SndBuffers;
HWAVEOUT hwo;
INT bps;
BOOL bFlushing=FALSE,bOpen=FALSE;
extern volatile HANDLE hWAVFile;
extern DWORD dwBytesWritten;
extern volatile BOOL bThreadFailed,bWithSound;
extern volatile HWND hWndParent;
extern TCHAR szDllName[];

extern void *lpSPCRAM;
extern u8 *lpExtraRAM;
extern u8 *lpSPCOut;
extern u32 *lpT64Cnt;
extern DSPRAM *lpDSP;
extern Channel *lpMix;
extern u32 *lpVMMaxL;
extern u32 *lpVMMaxR;
extern UEMATSUVISPARAMS SynchedVisParams;
extern volatile UINT uNumSmp,uFrameSize,uSmpRate,uPreamp;
extern volatile UINT uOutputTime,uLastKeyOnTime;

BOOL StartThread();
BOOL StopThread();

void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	WAVEHDR *whdr=(WAVEHDR*)dwParam1;
	
	switch(uMsg)
	{
	case WOM_DONE:
		waveOutUnprepareHeader(hwo,whdr,sizeof(WAVEHDR));
		int b=whdr->dwUser;

		if(!bFlushing)
		{
			memcpy(&SynchedVisParams.DSP,&SndBuffers[b].uvp.DSP,sizeof(SndBuffers[b].uvp.DSP));
			memcpy(SynchedVisParams.Mix,SndBuffers[b].uvp.Mix,sizeof(SndBuffers[b].uvp.Mix));
			memcpy(SynchedVisParams.lpPCMBuffer,SndBuffers[b].uvp.lpPCMBuffer,SndBuffers[b].uvp.uPCMBufferLength);
			memcpy(SynchedVisParams.nDecibelList,SndBuffers[b].uvp.nDecibelList,sizeof(SndBuffers[b].uvp.nDecibelList));
			SynchedVisParams.nVMMaxL=SndBuffers[b].uvp.nVMMaxL;
			SynchedVisParams.nVMMaxR=SndBuffers[b].uvp.nVMMaxR;
			SynchedVisParams.uPCMBufferLength=SndBuffers[b].uvp.uPCMBufferLength;
			SynchedVisParams.uT64Cnt=SndBuffers[b].uvp.uT64Cnt;
			SynchedVisParams.uPreAmp=SndBuffers[b].uvp.uPreAmp;
			SynchedVisParams.uCount++;

			uOutputTime=SynchedVisParams.uT64Cnt/64;
			for(int i=0;i<8;i++)
			{
				if(!(SndBuffers[b].uvp.Mix[i].MFlg&8))
					uLastKeyOnTime=uOutputTime;
			}

			if(hWAVFile!=INVALID_HANDLE_VALUE)
			{
				DWORD dw;
				if(!WriteFile(hWAVFile,SndBuffers[b].uvp.lpPCMBuffer,SndBuffers[b].uvp.uPCMBufferLength,&dw,NULL))
				{
					MessageBox(hWndParent,"Failed to write to the WAV output file.",szDllName,MB_ICONERROR);
					StopThread();
					bThreadFailed=TRUE;
				}
				else
					dwBytesWritten+=dw;
			}
		}
		SndBuffers[b].bPlaying=FALSE;
		SndBuffers[b].bFull=FALSE;
		break;
	}
}
int OW_Open(int SampleRate, int NumChannels, int BitsPerSmp, int BufferLenms, int VisRate)
{
	if(bOpen)
		return -1;

	int nFrameSize=(NumChannels-1)+(BitsPerSmp>>4);
	int nNumSmp=SampleRate/VisRate;

	WAVEFORMATEX wfx;
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nChannels=(unsigned short)NumChannels;
	wfx.nSamplesPerSec=SampleRate;
	wfx.wBitsPerSample=(unsigned short)BitsPerSmp;
	wfx.nBlockAlign=(unsigned short)(NumChannels*BitsPerSmp/8);
	wfx.nAvgBytesPerSec=SampleRate*wfx.nBlockAlign;
	wfx.cbSize=0;

	if(waveOutOpen(&hwo,WAVE_MAPPER,&wfx,(DWORD)waveOutProc,0,CALLBACK_FUNCTION)!=MMSYSERR_NOERROR)
		return -1;

	bps=VisRate*BufferLenms/1000;
	int bpb=(nNumSmp<<nFrameSize);
	SndBuffers=(_SNDBUFFER*)malloc(sizeof(_SNDBUFFER)*bps);
	for(int b=0;b<bps;b++)
	{		
		SndBuffers[b].bFull=FALSE;
		SndBuffers[b].bPlaying=FALSE;
		SndBuffers[b].wh.dwBufferLength=SndBuffers[b].dwSize=SndBuffers[b].uvp.uPCMBufferLength=bpb;
		SndBuffers[b].wh.dwUser=b;
		if((SndBuffers[b].uvp.lpPCMBuffer=SndBuffers[b].wh.lpData=(LPSTR)malloc(bpb))==NULL)
			return -1;
	}
	if((SynchedVisParams.lpPCMBuffer=malloc(bpb))==NULL)
		return -1;
	SynchedVisParams.uCount=uOutputTime=uLastKeyOnTime=0;
	bOpen=TRUE;

	return bpb*bps*1000/wfx.nAvgBytesPerSec;
}
void OW_Close()
{
	if(!bOpen)
		return;

	OW_Flush();
	waveOutClose(hwo);
	for(int b=0;b<bps;b++)
		free(SndBuffers[b].wh.lpData);
	free(SndBuffers);
	free(SynchedVisParams.lpPCMBuffer);
	memset(&SynchedVisParams,0,sizeof(SynchedVisParams));
	bOpen=FALSE;
}
int OW_Write(char *Buf, int Len)
{
	if(!bOpen)
		return 0;

	BOOL bWritten=FALSE;

	for(int b=0;b<bps;b++)
	{
		if(!SndBuffers[b].bFull&&!bWritten)
		{
			memcpy(&SndBuffers[b].uvp.DSP,lpDSP,sizeof(DSPRAM));
			memcpy(SndBuffers[b].uvp.Mix,lpMix,sizeof(Channel)*8);
			SndBuffers[b].uvp.nVMMaxL=*lpVMMaxL;
			SndBuffers[b].uvp.nVMMaxR=*lpVMMaxR;
			VMax2dB(SndBuffers[b].uvp.nDecibelList,false);
			memcpy(SndBuffers[b].uvp.lpPCMBuffer,Buf,Len);			
			SndBuffers[b].uvp.uT64Cnt=*lpT64Cnt;
			SndBuffers[b].uvp.uPreAmp=uPreamp;
			SndBuffers[b].bFull=TRUE;
			bWritten=TRUE;
		}
		if(SndBuffers[b].bFull&&!SndBuffers[b].bPlaying)
		{
			SndBuffers[b].bPlaying=TRUE;
			SndBuffers[b].wh.dwFlags=0;
			waveOutPrepareHeader(hwo,&SndBuffers[b].wh,sizeof(WAVEHDR));
			waveOutWrite(hwo,&SndBuffers[b].wh,sizeof(WAVEHDR));
		}
	}

	return bWritten?0:1;
}
DWORD OW_CanWrite()
{
	if(!bOpen)
		return 0;

	for(int b=0;b<bps;b++)
	{
		if(!SndBuffers[b].bFull)
			return SndBuffers[b].dwSize;
	}
	return 0;
}
int OW_IsPlaying()
{
	if(!bOpen)
		return 0;

	int iPlayingCount=0;
	for(int b=0;b<bps;b++)
		if(SndBuffers[b].bPlaying)iPlayingCount++;
	return iPlayingCount;
}
int OW_Pause(int Pause)
{
	if(!bOpen)
		return 0;

	Pause?waveOutPause(hwo):waveOutRestart(hwo);
	return 0;
}
void OW_Flush(void)
{
	if(!bOpen)
		return;

	bFlushing=TRUE;
	waveOutReset(hwo);
	int iPlayingCount;
	do
	{
		iPlayingCount=0;
		for(int b=0;b<bps;b++)
			if(SndBuffers[b].bPlaying)iPlayingCount++;
	}
	while(iPlayingCount);
	for(int b=0;b<bps;b++)
		SndBuffers[b].bFull=FALSE;
	bFlushing=FALSE;
}
int OW_IsOpen()
{
	return bOpen;
}