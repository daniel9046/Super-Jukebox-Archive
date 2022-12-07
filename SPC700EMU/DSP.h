/***************************************************************************************************
* Program:    SNES Digital Signal Processor Emulator
* Platform:   80386 / MMX
* Programmer: Anti Resonance
* 
* Adapted from SPC Tool for use in Snes9x
* 
* Permission to use, copy, modify and distribute Snes9x in both binary and source form, for non-
* commercial purposes, is hereby granted without fee, providing that this license information and
* copyright notice appear with all copies and any derived work.
* 
* This software is provided 'as-is', without any express or implied warranty. In no event shall the
* authors be held liable for any damages arising from the use of this software.
* 
* Snes9x is freeware for PERSONAL USE only. Commercial users should seek permission of the copyright
* holders first. Commercial use includes charging money for Snes9x or software derived from Snes9x.
* 
* The copyright holders request that bug fixes and improvements to the code should be forwarded to
* them so everyone can benefit from the modifications in future versions.
* 
* Super NES and Super Nintendo Entertainment System are trademarks of Nintendo Co., Limited and its
* subsidiary companies.
* 
* --------------------------------------------------------------------------------------------------
* Revision History:
*
* 1.4  22.05.2000 SNESAmp v1.2
*      + Added a low-pass filter
*      + Added output monitor
*      + Optimized the waveform code in EmuDSPN
*      + Added FixSeek
*      + SetDSPVol now uses a 16-bit value
*      - FixDSP was marking channels as keyed even if the pitch was 0
*      - Correctly implemented echo volume (should be perfect now)
*
* 1.1  05.04.2000 SNESAmp v1.0
*      - Channels with a pitch of 0 can't be keyed on
*      - EnvH becomes 0 when channel is keyed off
*      - EnvH is set to 0 when envelope is in gain mode
*      - Account for sample blocks with a range greater than 16-bits (BRE still not correct)
*
* 1.0  17.03.2000 SNESAmp v0.9
*                                                             Copyright (C)2000 Alpha-II Productions
***************************************************************************************************/

//**************************************************************************************************
//Public Variables

//DSP Options ------------------------------
enum	Inter
{
	IntN=1,                                  //None
	IntL,                                    //Linear
	IntC                                     //Cubic
};

struct DOpt
{
	char	IntType:2;                         //Interpolation type
	char	Smp8bit:1;                         //Output 8-bit samples
	char	SmpMono:1;                         //Output monaural samples
	char	OldBRE:1;                          //Use old school method for bit-rate expansion
	char	MixRout:2;                         //Mixing routine: 0-No mixing  1-Integer  2-MMX
	char	LowPass:1;                         //Use low pass filter
};

//DSP registers ---------------------------
struct DSPChn
{
	char	VolL;                              //Volume Left
	char	VolR;                              //Volume Right
	short Pitch;                             //Pitch (Rate/32000) (3.11)
	char	Wave;                              //Wave form being played back
	char	ADSR[2];                           //Envelope rates: attack, decay, and sustain
	char	Gain;                              //Envelope gain rate (if not using ADSR)
	char	EnvX;                              //Current envelope height (.7)
	char	OutX;                              //Current sample being output (-.7)
	short	__r;
};

struct DSPRAM
{
	DSPChn	Chn1;
	char		MVolL;                          //Main Volume Left (-.7)
	char		EFB;                            //Echo Feedback (-.7)
	char		__r1;
	char		FC1;                            //Filter Coefficient
	DSPChn	Chn2;
	char		MVolR;                          //Main Volume Right (-.7)
	char		__r2a;
	char		__r2;
	char		FC2;
	DSPChn	Chn3;
	char		EVolL;                          //Echo Volume Left (-.7)
	char		PMod;                           //Pitch Modulation on/off for each channel
	char		__r3;
	char		FC3;
	DSPChn	Chn4;
	char		EVolR;                          //Echo Volume Right (-.7)
	char		NOn;                            //Noise output on/off for each channel
	char		__r4;
	char		FC4;
	DSPChn	Chn5;
	char		KOn;                            //Key On for each channel
	char		EOn;                            //Echo on/off for each channel
	char		__r5;
	char		FC5;
	DSPChn	Chn6;
	char		KOff;                           //Key Off for each channel (instantiates release mode)
	char		Dir;                            //Page containing source directory (wave table offsets)
	char		__r6;
	char		FC6;
	DSPChn	Chn7;
	char		Flg;                            //DSP flags and noise frequency
	char		ESA;                            //Starting page used to store echo waveform
	char		__r7;
	char		FC7;
	DSPChn	Chn8;
	char		EndX;                           //Waveform has ended
	char		EDel;                           //Echo Delay in ms >> 4
	char		__r8;
	char		FC8;
	char		extra[128];                     //Unused
};

//Internal mixing settings -----------------
enum	EnvM {Dec,Exp,Inc,Bent=6,Release=8,Sustain,Attack,Decay=13,Direct=15};

struct Channel
{
	//Waveform ---------18
	int	SP1;                               //Last sample decompressed (prev1)
	int	SP2;                               //Second to last sample (prev2)
	void	*BCur;                             //-> current block
	void	*BLoop;                            //-> loop start block
	char	BHdr;                              //Block Header for current block
	char	MOpt;                              //Mixing options (MixO)
	//Envelope ---------22
	char	EMode;                             //Current mode (EnvM)
	char	ERIdx;                             //Index in RateTab (0-31)
	int	ERate;                             //Rate of envelope adjustment (16.16)
	int	EDec;                              //Rate Decimal (.16)
	int	EVal;                              //Current envelope value
	int	EAdj;                              //Amount to adjust envelope height
	int	EDest;                             //Envelope Destination (16.16)
	//Mixing -----------20
	int	ChnL;                              //Channel Volume (-24.7)
	int	ChnR;                              // "  "
	int	PRate;                             //Pitch Rate after modulation (16.16)
	int	PDec;                              //Pitch Decimal (.16) (used as delta for interpolation)
	int	PDSP;                              //Pitch rate converted from the DSP (16.16)
	//Visualization ----12
	short	VU[2];                             //VU meter level (not used)
	int	VMaxL;                             //Max wave height before 16-bit clipping
	int	VMaxR;
	//Samples ----------56
	short	*SIdx;                             //-> current sample in SmpRAM
	int	SOut;                              //Last sample output before chn vol (used for pitch mod)
	short SInt[4];                           //Samples used for interpolation
	short	SRAM2[4];                          //Last 4 samples from previous block
	short	SRAM[16];                          //32 bytes for decompressed sample blocks
};

extern "C" DSPRAM DSP;                      //1 page of DSP RAM
extern "C" Channel Mix[8];


//*************************************************************************************************
//External Functions

//**************************************************************************************************
// Initialize DSP
//
// Desc:
//    Initializes the DSP.  Creates the derivative table for cubic spline interpolation.
// In:
//    -> 64KB of SPC RAM aligned on a 64K boundary
// Out:
//    nothing
// Destroys:
//    ST(0-7)

extern "C" void InitDSP(void*);


//**************************************************************************************************
// Reset DSP
//
// Desc:
//    Resets the DSP registers, erases internal variables, and resets the volume
// In:
//    nothing
// Out:
//    nothing
// Destroys:
//    EAX

extern "C" void ResetDSP();


//**************************************************************************************************
// Set DSP Options
//
// Desc:
//    Recalculates tables, changes the output sample rate, and sets up the mixing routine.
// In:
//    Rate = Sample Rate (8000-64000)
//    Opt  = DOpt
// Out:
//    nothing
// Destroys:
//    EAX,EDX

extern "C" void SetDSPOpt(int Rate,DOpt Opt);


//**************************************************************************************************
// Fix DSP After Loading Saved State
//
// Desc:
//    Initializes the internal mixer variables
// In:
//    nothing
// Out:
//    nothing
// Destroys:
//    EAX,EDX

extern "C" void FixDSP();


//**************************************************************************************************
// Fix DSP After Seeking
// 
// Desc:
//    Puts all DSP channels in a key off state and erases echo region.  Call after using EmuDSPN.
// In:
//    nothing
// Out:
//    nothing
// Destroys:
//    nothing

extern "C" void FixSeek();


//**************************************************************************************************
// Set DSP Pre-amplification Level
//
// Desc:
//    Sets the amplification level
// In:
//    Amp = Amplification level (0-100)
// Out:
//    nothing
// Destroys:
//    EAX,EDX

extern "C" void SetDSPAmp(int Amp);


//**************************************************************************************************
// Set DSP Volume
//
// Desc:
//    Sets the volume level for fading
// In:
//    Vol = Volume (0-65536)
// Out:
//    nothing
// Destroys:
//    EAX,EDX

extern "C" void SetDSPVol(int Vol);


//**************************************************************************************************
// DSP Data Port
//
// Desc:
//    Writes a value to a specified DSP register and alters the DSP accordingly
// In:
//    BL = DSP Address
//    BH = DSP Data
// Out:
//    nothing
// Destroys:
//    EBX

extern "C" void DSPDataIn();


//**************************************************************************************************
// Emulate DSP
//
// Desc:
//    Emulates the DSP functions of the SNES
// In:
//    Buf-> Buffer to store output
//    Smp = Number of samples to create (0-2048)
// Out:
//    -> End of buffer
// Destroys:
//    EDX

extern "C" void* EmuDSP(void *Buf,int Smp);


//**************************************************************************************************
// Emulate DSP (use MMX instructions)
//
// Desc:
//    Emulates the DSP functions of the SNES
// In:
//    Buf-> Buffer to store output
//    Smp = Number of samples to create (0-2048)
// Out:
//    -> End of buffer
// Destroys:
//    EDX,MM4-MM7

extern "C" void* EmuDSPX(void *Buf,int Smp);


//**************************************************************************************************
// Emulate DSP (no mixing)
//
// Desc:
//    Emulates the DSP functions of the SNES, except sample decompression and mixing.
//    Buffer will not be changed, and OutW will not be modified.
// In:
//    EmuPtr-> Buffer to store output
//    EmuSmp = Number of samples to create (0-2048)
// Out:
//    EAX -> End of buffer
// Destroys:
//    EDX

extern "C" void* EmuDSPN(void *Buf,int Smp);
