#ifndef _UEMATSU_H_
#define _UEMATSU_H_

#include <windows.h>
#include "SNESAPU.h"

#define MaxAmp 256

//Sub-chunk ID's
#define	XID_SONG		0x01
#define	XID_GAME		0x02
#define	XID_ARTIST	0x03
#define	XID_DUMPER	0x04
#define	XID_DATE		0x05
#define	XID_EMU		0x06
#define	XID_CMNTS	0x07
#define	XID_INTRO	0x30
#define	XID_LOOP		0x31
#define	XID_END		0x32
#define	XID_FADE		0x33
#define	XID_MUTE		0x34
#define	XID_OST		0x10
#define	XID_DISC		0x11
#define	XID_TRACK	0x12
#define	XID_PUB		0x13
#define	XID_COPY		0x14

//Data types
#define	XTYPE_DATA	0x00
#define	XTYPE_STR	0x01
#define	XTYPE_INT	0x04
#define	XTYPE_BCD	0x10

typedef struct _SUBCHK
{
	unsigned char	id;                        //Subchunk ID
	unsigned char	type;                      //Type of data
	unsigned short	data;                      //Data
}SUBCHK,*PSUBCHK,FAR *LPSUBCHK;

typedef struct _SPCREGS
{
	unsigned char	PC[2];
	unsigned char	A;
	unsigned char	X;
	unsigned char	Y;
	unsigned char	PSW;
	unsigned char	SP;
}SPCREGS,*PSPCREGS,FAR *LPSPCREGS;

typedef struct _SPCHEADER
{
	char		FTag[33];                       //File tag
	char		Term[3];                        //Tag terminator
	char		Ver;                            //Version #/100
	SPCREGS		Regs;                           //SPC Registers
	short		__r1;
	char		Title[32];                       //Song title
	char		Game[32];                       //Game title
	char		Dumper[16];                     //Name of dumper
	char		Comment[32];                    //Comments
	char		Date[11];                       //Date dumped
	char		SongLen[3];                     //Song length (number of seconds to play before fading)
	char		FadeLen[5];                     //Fade length (milliseconds)
	char		Artist[32];                     //Artist of song
	char		ChnDis;                         //Channel Disabled
	char		Emulator;                       //Emulator dumped with
	char		__r2[45];
}SPCHEADER,*PSPCHEADER,FAR *LPSPCHEADER;

typedef struct _UEMATSUCONFIG
{
	UINT uSampleRate;			//Sample rate in Hertz (8000-96000)
	UINT uBitsPerSample;		//Number of bits per sample (8, 16 or 32)
	UINT uChannels;				//Number of sound channels (1=mono, 2=stereo)
	UINT uMixingRoutine;		//Sound mixing technique (MIX_NONE,MIX_80386,MIX_MMX,MIX_3DNOW,MIX_SSE)
	UINT uInterpolation;		//Sound interpolation technique (INT_NONE,INT_LINEAR,INT_CUBIC,INT_GAUSSIAN)
	UINT uBufferLength;			//Sound output buffer length in milliseconds (100-10000)
	UINT uVisualizationRate;	//Rate of visualization updating in Hertz (10-120)
	UINT uAPR;					//Automatic Preamplification Reduction (0=off, 1=on, 2=increase)
	UINT uThreshold;			//APR threshold
	UINT fMiscOptions;			//Additional options (DSP_LOW, DSP_OLDSMP, DSP_SURND, DSP_REVERSE)
}UEMATSUCONFIG,*PUEMATSUCONFIG,FAR *LPUEMATSUCONFIG;

typedef struct _UEMATSUVISPARAMS
{
	DSPRAM DSP;				//Data for the DSP (read SNESAPU.H for description)
	Channel Mix[8];			//Data for the 8 DSP channels (read SNESAPU.H for description)
	INT nVMMaxL,nVMMaxR;	//Maximum output sample values for the main left and right channels
	INT nDecibelList[18];	//Array of 18 dB values, 2 for each 8 DSP channels and 2 for the main (left-right)
	LPVOID lpPCMBuffer;		//Pointer to the PCM sound data
	UINT uPCMBufferLength;	//Length of the PCM sound data in bytes
	UINT uCount;			//Keeps count of the visualization records
	UINT uT64Cnt;			//Time stamp (64 kHz)
	UINT uPreAmp;			//Current pre-amp level
}UEMATSUVISPARAMS,*PUEMATSUVISPARAMS,FAR *LPUEMATSUVISPARAMS;

typedef struct _ID666TAG
{
	TCHAR szTitle[256];			//Title of song
	TCHAR szGame[256];			//Name of game
	TCHAR szArtist[256];		//Name of artist
	TCHAR szPublisher[256];		//Game publisher
	TCHAR szDumper[256];		//Name of dumper
	TCHAR szDate[256];			//Date dumped
	TCHAR szComment[256];		//Optional comment
	TCHAR szOST[256];			//Original soundtrack title
	UINT uSong_ms;				//Length of song
	UINT uLoop_ms;				//Length of loop
	UINT uEnd_ms;				//Length of end
	UINT uFade_ms;				//Length of fadeout
	BYTE bDisc;					//OST disc number
	WORD wTrack;				//OST track number
	WORD wCopyright;			//Game copyright date
	BYTE bMute;					//Bitmask for channel states	
	BYTE bEmulator;				//Emulator used to dump
}ID666TAG,*PID666TAG,FAR *LPID666TAG;

#ifdef __cplusplus
extern "C"{
#endif

BOOL WINAPI Uematsu_Init();
BOOL WINAPI Uematsu_DeInit();
BOOL WINAPI Uematsu_SetWindowHandle(HWND hWnd);
BOOL WINAPI Uematsu_GetConfiguration(LPUEMATSUCONFIG lpuc);
BOOL WINAPI Uematsu_SetConfiguration(LPUEMATSUCONFIG lpuc);
BOOL WINAPI Uematsu_Play(LPCTSTR lpszFileName);
BOOL WINAPI Uematsu_Stop();
BOOL WINAPI Uematsu_Pause();
BOOL WINAPI Uematsu_UnPause();
BOOL WINAPI Uematsu_SetWAVOutputFileName(LPCTSTR lpszFileName,BOOL bWithSound);
UINT WINAPI Uematsu_GetPlayTime();
BOOL WINAPI Uematsu_SetPlayTime(UINT uTime,BOOL bFastSeek);
BOOL WINAPI Uematsu_SetSongLength(UINT uSong,UINT uFade);
BOOL WINAPI Uematsu_SetSongSpeed(UINT uSpeed);
BOOL WINAPI Uematsu_SetSongPitch(UINT uPitch);
BOOL WINAPI Uematsu_SetMainSS(UINT uSeparation);
BOOL WINAPI Uematsu_SetEchoSS(INT nSeparation);
BOOL WINAPI Uematsu_SetPreAmp(UINT uPreAmp);
BOOL WINAPI Uematsu_SetMixingThreadPriority(INT nPriority);
BOOL WINAPI Uematsu_IsActive();
BOOL WINAPI Uematsu_IsPlaying();
BOOL WINAPI Uematsu_HasFailed();
BOOL WINAPI Uematsu_MuteChannels(INT nChannels);
BOOL WINAPI Uematsu_GetVisParams(LPUEMATSUVISPARAMS lpuvp);
BOOL WINAPI Uematsu_LoadID666Tag(LPCTSTR lpszFileName,LPID666TAG lpTag);
BOOL WINAPI Uematsu_SaveID666Tag(LPCTSTR lpszFileName,LPID666TAG lpTag);
BOOL WINAPI Uematsu_GetCoreInfo(LPUINT lpuVersion,LPUINT lpuMinVersion,LPUINT lpuOptions);
UINT WINAPI Uematsu_GetCurrentPreamp();

#ifdef __cplusplus
}
#endif
#endif