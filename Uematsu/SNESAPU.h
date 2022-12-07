/***************************************************************************************************
* Program:    Super Nintendo Entertainment System(tm) Audio Processing Unit Emulator DLL           *
* Platform:   Intel 80386 & MMX                                                                    *
* Programmer: Anti Resonance                                                                       *
*                                                                                                  *
* "SNES" and "Super Nintendo Entertainment System" are trademarks of Nintendo Co., Limited and its *
* subsidiary companies.                                                                            *
*                                                                                                  *
* This program is free software; you can redistribute it and/or modify it under the terms of the   *
* GNU General Public License as published by the Free Software Foundation; either version 2 of     *
* the License, or (at your option) any later version.                                              *
*                                                                                                  *
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;        *
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.        *
* See the GNU General Public License for more details.                                             *
*                                                                                                  *
* You should have received a copy of the GNU General Public License along with this program;       *
* if not, write to the Free Software Foundation, Inc.                                              *
* 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                                        *
*                                                                                                  *
* ------------------------------------------------------------------------------------------------ *
* Revision History:                                                                                *
*                                                                                                  *
* 0.90 01.01.2001 SNESAmp v2.0                                                                     *
*      + Made some changes to the internal interface with the DSP                                  *
*                                                                                                  *
* 0.85 14.12.2000 Super Jukebox v3.0                                                               *
*                                                           Copyright (C)2001 Alpha-II Productions *
***************************************************************************************************/

//**************************************************************************************************
//Type redefinitions

#define	b8		bool

#define	u8		unsigned char
#define	u16	unsigned short
#define	u32	unsigned int
#define	u64	unsigned long int

#define	s8		char
#define	s16	short
#define	s32	int
#define	s64	long int

#define	f32	float
#define	f64	double
#define	f80	long double


//**************************************************************************************************
//Defines

//Compile directives enabled (see SPC700.Asm and DSP.Asm)
#define	SA_DEBUG		0x1
#define	SA_HALFC		0x2
#define	SA_CNTBK		0x4
#define	SA_SPEED		0x8
#define	SA_VMETER	0x100
#define	SA_COFF		0x10000

#define	APU_CLK		24576000               //Number of SPC700 clock cycles in one second

//CPU types --------------------------------
#define	CPU_MMX		1
#define	CPU_3DNOW	2
#define	CPU_SSE		4

//Mixing routines --------------------------
#define	MIX_NONE		0                      //No mixing
#define	MIX_80386	1                      //80386
#define	MIX_MMX		2                      //Multi-Media Extensions
#define	MIX_3DNOW	3                      //3DNow!
#define	MIX_SSE		4                      //Streaming SIMD Extensions

//Interpolation routines -------------------
#define	INT_NONE		0                      //None (fast)
#define	INT_LINEAR	1                      //Linear (fast)
#define	INT_CUBIC	2                      //Cubic (slow)
#define INT_GAUSSIAN 3

//DSP options ------------------------------
#define	DSP_LOW		1                      //Low-pass filter
#define	DSP_OLDSMP	2                      //Old sample decompression routine
#define	DSP_SURND	4                      //"Surround" sound
#define	DSP_REVERSE	8                      //Reverse stereo samples

//SPC700 debugging options -----------------
#define	SPC_RETURN	0x01
#define	SPC_HALT		0x02
#define	SPC_TRACE	0x10


//**************************************************************************************************
//Structures

//Program Status Word ----------------------
struct SPCFlags
{
	u8		C:1;                               //Carry
	u8		Z:1;                               //Zero
	u8		I:1;                               //Interrupts Enabled
	u8		H:1;                               //Half-Carry (Auxiliary)
	u8		B:1;                               //Software Break
	u8		P:1;                               //Direct Page Selector
	u8		V:1;                               //Overflow
	u8		N:1;                               //Negative (Sign)
};

//DSP registers ---------------------------
struct DSPChn
{
	s8		VolL;                              //Volume Left
	s8		VolR;                              //Volume Right
	u16	Pitch;                             //Pitch (Rate/32000) (3.11)
	u8		Wave;                              //Wave form being played back
	u8		ADSR[2];                           //Envelope rates: attack, decay, and sustain
	u8		Gain;                              //Envelope gain rate (if not using ADSR)
	s8		EnvX;                              //Current envelope height (.7)
	s8		OutX;                              //Current sample being output (-.7)
	u16	__r;
};

struct DSPRAM
{
	DSPChn	Chn1;
	s8			MVolL;                          //Main Volume Left (-.7)
	s8			EFB;                            //Echo Feedback (-.7)
	s8			__r1;
	s8			FC1;                            //Filter Coefficient
	DSPChn	Chn2;
	s8			MVolR;                          //Main Volume Right (-.7)
	s8			__r2a;
	s8			__r2;
	s8			FC2;
	DSPChn	Chn3;
	s8			EVolL;                          //Echo Volume Left (-.7)
	u8			PMod;                           //Pitch Modulation on/off for each channel
	s8			__r3;
	s8			FC3;
	DSPChn	Chn4;
	s8			EVolR;                          //Echo Volume Right (-.7)
	u8			NOn;                            //Noise output on/off for each channel
	s8			__r4;
	s8			FC4;
	DSPChn	Chn5;
	u8			KOn;                            //Key On for each channel
	u8			EOn;                            //Echo on/off for each channel
	s8			__r5;
	s8			FC5;
	DSPChn	Chn6;
	u8			KOff;                           //Key Off for each channel (instantiates release mode)
	u8			Dir;                            //Page containing source directory (wave table offsets)
	s8			__r6;
	s8			FC6;
	DSPChn	Chn7;
	u8			Flg;                            //DSP flags and noise frequency
	u8			ESA;                            //Starting page used to store echo waveform
	s8			__r7;
	s8			FC7;
	DSPChn	Chn8;
	u8			EndX;                           //Waveform has ended
	u8			EDel;                           //Echo Delay in ms >> 4
	s8			__r8;
	s8			FC8;
	s8			extra[128];                     //Unused
};

//Internal mixing settings -----------------
struct MixO
{
	b8		Mute:1;                            //Channel is muted
	u8		__r1:1;
	b8		KeyOff:1;                          //Channel is keying off
	b8		Inactive:1;                        //Channel is inactive
	u8		__r2:2;
	u8		IntType:2;                         //Interpolation type (not used)
};

enum	EnvM {Dec,Exp,Inc,Bent=6,Release=8,Sustain,Attack,Decay=13,Direct=15};

struct Channel
{
	//Waveform ---------18
	s32	SP1;                               //Last sample decompressed (prev1)
	s32	SP2;                               //Second to last sample (prev2)
	void	*BCur;                             //-> current block
	void	*BLoop;                            //-> loop start block
	u8		BHdr;                              //Block Header for current block
	u8		MFlg;                              //Mixing options (MixO)
	//Envelope ---------22
	u8		EMode;                             //Current mode (EnvM)
	u8		ERIdx;                             //Index in RateTab (0-31)
	u32	ERate;                             //Rate of envelope adjustment (16.16)
	u32	EDec;                              //Rate Decimal (.16)
	u32	EVal;                              //Current envelope value
	s32	EAdj;                              //Amount to adjust envelope height
	u32	EDest;                             //Envelope Destination (16.16)
	//Mixing -----------20
	s32	ChnL;                              //Channel Volume (-24.7)
	s32	ChnR;                              // "  "
	u32	PRate;                             //Pitch Rate after modulation (16.16)
	u32	PDec;                              //Pitch Decimal (.16) (used as delta for interpolation)
	u32	PDSP;                              //Pitch rate converted from the DSP (16.16)
	//Visualization ----8
	s32	VMaxL;                             //Maximum absolute sample output
	s32	VMaxR;
	//Samples ----------56
	s16	*SIdx;                             //-> current sample in SmpRAM
	s32	SOut;                              //Last sample output before chn vol (used for pitch mod)
	s32	__r;                               //reserved
	s16	SInt[4];                           //Samples used for interpolation
	s16	SRAM2[4];                          //Last 4 samples from previous block
	s16	SRAM[16];                          //32 bytes for decompressed sample blocks
};


//**************************************************************************************************
//External Functions - See other header files for exported function descriptions

typedef u32 __cdecl DebugSPC(u8 *pc, u16 ya, u8 x, SPCFlags psw, u8 *sp);

#define	import	__declspec(dllimport)

#ifdef __cplusplus
extern "C" {
#endif

//**************************************************************************************************
// SNESAPU Info
//
// Desc:
//    Returns the version and compile information about the library
//
// In:
//    Ver -> Version
//           bits 7-0   Optional letter (a-z, 0x00 otherwise)
//           bits 15-8  Minor version (stored as BCD)
//           bits 23-16 Major version (stored as BCD)
//           bits 31-24 0x00
//    Min -> Minimum version, the oldest version this library is backwards compatible with
//    Opt -> Compile directives
//           see defines at top

import	void	__stdcall SNESAPUInfo(u32 *Ver, u32 *Min, u32 *Opt);

import	void	__stdcall GetAPUData	\
								 (void **pSPCRAM, u8 **pExtraRAM, u8 **pSPCOut, u32 **pT64Cnt, \
								  DSPRAM **pDSP, Channel **pMix, u32 **pVMMaxL, u32 **pVMMaxR);
import	void	__stdcall ResetAPU(u32 amp);
import	void	__stdcall FixAPU(u16 pc, u8 a, u8 y, u8 x, u8 psw, u8 sp);
import	void	__stdcall SetAPUOpt(u32 mix, u32 chn, u32 smp, u32 rate, u32 inter, u32 opts);
import	void	__stdcall SetAPUSmpClk(u32 speed);
import	void	__stdcall SetAPULength(u32 song, u32 fade);
import	void*	__stdcall EmuAPU(void *pBuf, u32 len, b8 type);
import	void	__stdcall SeekAPU(u32 time, b8 fast);
import	void	__stdcall SetSPCDbg(DebugSPC *pTrace, u32 opts);
import	void	__stdcall GetSPCRegs(u16 &pc, u8 &a, u8 &y, u8 &x, u8 &psw, u8 &sp);
import	void	__stdcall SPCIn(u32 port, u32 val);
import	void	__stdcall SPCInW(u32 port, u32 val);
import	s32	__stdcall EmuSPC(u32 cyc);
import	u32	__stdcall GetProcInfo();
import	void	__stdcall DSPIn();
import	void	__stdcall SetDSPPitch(u32 base);
import	void	__stdcall SetDSPAmp(u32 amp);
import	void	__stdcall SetDSPStereo(u32 sep);
import	void	__stdcall SetDSPEFBCT(s32 leak);
import	void*	__stdcall EmuDSP(void *pBuf, s32 size);
import	u32	__stdcall UnpackWave(s16 *pBuf, s8 *pBlk, u32 num, u32 opts, s32 &prev1, s32 &prev2);
import	void	__stdcall VMax2dB(void *pList, b8 fp);


#ifdef __cplusplus
}
#endif