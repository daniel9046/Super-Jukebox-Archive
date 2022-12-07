#include "Main.h"

int g_VM=2;

 // Snes9x's Mixer
extern void S9xMixSamples (unsigned char *, int);

 // Mixes Per Second
#define MPS 100
 // Buffers Per Second (Keep this low for Windows)
#define BPS 4
 // Mixes Per Buffer
#define MPB (MPS/BPS)
 // How many 32 Clocks Per Mix
#define C32PM (2048000/MPS)/32

 // Bytes Per Mix
int bpm=0;
 // Bytes Per Buffer
int bpb=0;
 // Samples Per Mix (double for stereo)
int spm=0;

HWAVEOUT hwo=0;
WAVEHDR wh[4];
BOOL playing=FALSE;

SAPURegisters BackupAPURegisters;
unsigned char BackupAPURAM[65536];
unsigned char BackupAPUExtraRAM[64];
unsigned char BackupDSPRAM[128];

extern "C" DWORD WINAPI InitSPC(DWORD dwFlags,DWORD dwChannels,DWORD dwBitsPerSample,DWORD dwSamplesPerSecond,DWORD dwVM)
{
	if(dwFlags&SPC_APU)
		if(S9xInitAPU()!=TRUE)
			return MMSYSERR_NOMEM;

	if(dwFlags&SPC_SND)
	{
		switch(dwVM)
		{
		case 1:g_VM=0;break;
		case 2:g_VM=1;break;
		case 4:g_VM=2;break;
		case 8:g_VM=3;break;
		default:g_VM=0;
		}
		WAVEFORMATEX waveFormatEx;
		waveFormatEx.wFormatTag=WAVE_FORMAT_PCM;
		waveFormatEx.nChannels=(WORD)dwChannels;
		waveFormatEx.wBitsPerSample=(WORD)dwBitsPerSample;
		waveFormatEx.nSamplesPerSec=dwSamplesPerSecond;
		waveFormatEx.nBlockAlign=waveFormatEx.nChannels*waveFormatEx.wBitsPerSample/8;
		waveFormatEx.nAvgBytesPerSec=waveFormatEx.nSamplesPerSec*waveFormatEx.nBlockAlign;
		waveFormatEx.cbSize=0;
		return OpenSoundDevice(&waveFormatEx);
	}
	return MMSYSERR_NOERROR;
}

extern "C" void WINAPI DeinitSPC(DWORD dwFlags)
{
	if(dwFlags&SPC_SND)
		CloseSoundDevice();
	if(dwFlags&SPC_APU)
		S9xDeinitAPU();
}

// The callback

void DoTimer(){
	if (APU.TimerEnabled [2])
	{
		APU.Timer [2]++;
		if (APU.Timer [2] >= APU.TimerTarget [2])
		{
			IAPU.RAM [0xff] = (IAPU.RAM [0xff] + 1) & 0xf;
			APU.Timer [2] = 0;
#ifdef SPC700_SHUTDOWN          
			IAPU.WaitCounter++;
			IAPU.APUExecuting = TRUE;
#endif          

		}
	}
	if (IAPU.TimerErrorCounter>=8)
	{
		IAPU.TimerErrorCounter=0;
		if (APU.TimerEnabled [0])
		{
			APU.Timer [0]++;
			if (APU.Timer [0] >= APU.TimerTarget [0])
			{
				IAPU.RAM [0xfd] = (IAPU.RAM [0xfd] + 1) & 0xf;
				APU.Timer [0] = 0;
#ifdef SPC700_SHUTDOWN          
				IAPU.WaitCounter++;
				IAPU.APUExecuting = TRUE;
#endif          

			}
		}
		if (APU.TimerEnabled [1])
		{
			APU.Timer [1]++;
			if (APU.Timer [1] >= APU.TimerTarget [1])
			{
				IAPU.RAM [0xfe] = (IAPU.RAM [0xfe] + 1) & 0xf;
				APU.Timer [1] = 0;
#ifdef SPC700_SHUTDOWN          
				IAPU.WaitCounter++;
				IAPU.APUExecuting = TRUE;
#endif          
			}
		}
	}
}

void CALLBACK waveOutProcSPC(HWAVEOUT hWo,UINT uMsg,DWORD dwInstance, 
						  DWORD dwParam1,DWORD dwParam2){
	int i;
	WAVEHDR *whdr=(WAVEHDR*)dwParam1;

	if(!playing)return;
	switch(uMsg){
	  case WOM_DONE:
		whdr->dwUser=TRUE;//Mixing
		waveOutUnprepareHeader(hWo,whdr,sizeof(WAVEHDR));

		for(i=0;i<MPB;i++){
			// APU_LOOP
			for(int c=0;c<C32PM;c++){
				for(int ic=0;ic<32;ic++){
					APU_EXECUTE1();
				}
				IAPU.TimerErrorCounter++;
				DoTimer();
			}

			S9xMixSamples((unsigned char *)(&whdr->lpData[i*bpm]),(spm));
		}
		whdr->dwUser=FALSE; //Not Mixing
		
		if(playing)
		{
			whdr->dwBufferLength=bpb;
			whdr->dwFlags=0;
			waveOutPrepareHeader(hWo,whdr,sizeof(WAVEHDR));
			waveOutWrite(hWo,whdr,sizeof(WAVEHDR));
		}
		break;
	}
}

// Play SPC

extern "C" void WINAPI PlaySPC(){
	int i,b;

	if(hwo&&playing==FALSE){
		for(b=0;b<BPS;b++){
			for(i=0;i<MPB;i++){
				// APU_LOOP
				for(int c=0;c<C32PM;c++){
					for(int ic=0;ic<32;ic++){
						APU_EXECUTE1();
					}
					IAPU.TimerErrorCounter++;
					DoTimer();
				}

				S9xMixSamples((unsigned char *)(&wh[b].lpData[i*bpm]),spm);
			}
		}
		playing=TRUE;
		for(b=0;b<BPS;b++){
			wh[b].dwBufferLength=bpb;
			wh[b].dwFlags=0;
			wh[b].dwUser=FALSE;
			waveOutPrepareHeader(hwo,&wh[b],sizeof(WAVEHDR));
			waveOutWrite(hwo,&wh[b],sizeof(WAVEHDR));
		}
	}
}

// Pause SPC

extern "C" void WINAPI PauseSPC(){
	if(hwo&&playing){
		playing=FALSE;
		waveOutReset(hwo);
	}
}

// Stop SPC

extern "C" void WINAPI StopSPC(){
	PauseSPC();
	RestoreSPC();
}

// Open Sound Device

MMRESULT OpenSoundDevice (WAVEFORMATEX *format)
{
	int b;
	MMRESULT mmResult;

	CloseSoundDevice(); //Close a previously opened Devices

	//SPC mixer information
	so.stereo = (format->nChannels==2)?TRUE:FALSE;//TRUE;//in_stereo;
	so.sixteen_bit = (format->wBitsPerSample==16)?TRUE:FALSE;//TRUE;//sixteen_bit;
	so.playback_rate = format->nSamplesPerSec;//44100;//playback_rate;
	so.buffer_size = format->nSamplesPerSec/MPS;//buffer_size;
	so.encoded = FALSE;
	so.mute_sound = FALSE;

	if((mmResult=waveOutOpen(&hwo,WAVE_MAPPER,format,
	(DWORD)waveOutProcSPC,0,CALLBACK_FUNCTION
	))!=MMSYSERR_NOERROR){
		hwo=0;
		return mmResult;
	}

	//Buffer mixing information
	bpb=format->nAvgBytesPerSec/BPS;
	bpm=format->nAvgBytesPerSec/MPS;
	spm=format->nSamplesPerSec*format->nChannels/MPS;
	for(b=0;b<BPS;b++){
		wh[b].lpData=new char[bpb];
	}

    return MMSYSERR_NOERROR;
}

// Close Sound Device

void CloseSoundDevice(){
	int i;

	if(hwo!=0){
		playing=FALSE;
		waveOutReset(hwo);
		while(waveOutClose(hwo)==WAVERR_STILLPLAYING)Sleep(0);
		for(i=0;i<BPS;i++){
			while(wh[i].dwUser==TRUE);
		}
		hwo=0;
		for(i=0;i<BPS;i++){
			delete [] wh[i].lpData;
		}
	}
}

// Restore SPC Load State

void FixEnvelope (int channel, uint8 gain, uint8 adsr1, uint8 adsr2);
void StartSample (int channel);

extern "C" void WINAPI RestoreSPC(){
	int i;

	APURegisters.PC = BackupAPURegisters.PC;
	APURegisters.YA.B.A = BackupAPURegisters.YA.B.A;
	APURegisters.X = BackupAPURegisters.X;
	APURegisters.YA.B.Y = BackupAPURegisters.YA.B.Y;
	APURegisters.P = BackupAPURegisters.P;
	APURegisters.S = BackupAPURegisters.S;
	memcpy(IAPU.RAM,BackupAPURAM,65536);
	memcpy(APU.ExtraRAM,BackupAPUExtraRAM,64);
	memcpy(APU.DSP,BackupDSPRAM,128);

	for(i=0;i<4;i++){
		APU.OutPorts [i] = IAPU.RAM[0xf4+i];
	}
	IAPU.TimerErrorCounter=0;
	for (i=0;i<3;i++){
		if (IAPU.RAM[0xfa+i] == 0)
			APU.TimerTarget[i] = 0x100;
		else
			APU.TimerTarget[i] = IAPU.RAM[0xfa+i];
	}
	S9xSetAPUControl(IAPU.RAM[0xf1]);
	S9xSetSoundMute (FALSE);
	IAPU.PC = IAPU.RAM + APURegisters.PC;
	S9xAPUUnpackStatus();
	if (APUCheckDirectPage ())
		IAPU.DirectPage = IAPU.RAM + 0x100;
	else
		IAPU.DirectPage = IAPU.RAM;
	IAPU.APUExecuting = TRUE;

	S9xFixSoundAfterSnapshotLoad();

	S9xSetFrequencyModulationEnable(APU.DSP[APU_PMON]);
	S9xSetMasterVolume(APU.DSP[APU_MVOL_LEFT],APU.DSP[APU_MVOL_RIGHT]);
	S9xSetEchoVolume(APU.DSP[APU_EVOL_LEFT],APU.DSP[APU_EVOL_RIGHT]);
	uint8 mask = 1;
	int type;
	for(i=0;i<8;i++,mask<<=1){
		FixEnvelope (i,
			APU.DSP[APU_GAIN+(i<<4)],
			APU.DSP[APU_ADSR1+(i<<4)],
			APU.DSP[APU_ADSR2+(i<<4)]);
		S9xSetSoundVolume (i,
			APU.DSP[APU_VOL_LEFT+(i<<4)],
			APU.DSP[APU_VOL_RIGHT+(i<<4)]);
        S9xSetSoundFrequency (i,
			((APU.DSP[APU_P_LOW+(i<<4)]+(APU.DSP[APU_P_HIGH+(i<<4)]<< 8))&FREQUENCY_MASK)*8);
		if (APU.DSP [APU_NON] & mask){
			type = SOUND_NOISE;
		}else{
			type = SOUND_SAMPLE;
		}
		S9xSetSoundType (i, type);
		if ((APU.DSP[APU_KON] & mask) != 0){
			APU.KeyedChannels |= mask;
			StartSample (i);
		}
	}
}

// Load SPC file

extern "C" DWORD WINAPI LoadSPC(PCSTR pszFileName)
{
	FILE *file;
	char temp[64];

	if((file=fopen(pszFileName,"rb"))!=NULL)
	{
		S9xResetAPU();

		fseek(file,37,SEEK_SET);
		fread(&BackupAPURegisters.PC,2,1,file);
		fread(&BackupAPURegisters.YA.B.A,1,1,file);
		fread(&BackupAPURegisters.X,1,1,file);
		fread(&BackupAPURegisters.YA.B.Y,1,1,file);
		fread(&BackupAPURegisters.P,1,1,file);
		fread(&BackupAPURegisters.S,1,1,file);
		fseek(file,256,SEEK_SET);
		fread(BackupAPURAM,65536,1,file);
		fread(BackupDSPRAM,128,1,file);
		fread(temp,64,1,file);
		fread(BackupAPUExtraRAM,64,1,file);

		fclose(file);

		RestoreSPC();
		return 0;
	}
	return 1;
}