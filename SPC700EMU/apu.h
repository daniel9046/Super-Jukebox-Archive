#ifndef _apu_h_
#define _apu_h_

#define OPCODES_AS_FUNCTIONS

#ifdef OPCODES_AS_FUNCTIONS
 #define APU_START(OP) DELAYED_APU_START(OP)
 #define DELAYED_APU_START(OP) void Apu##OP () {
 #define APU_END }
 #define APU_EXIT return;
#else
 #define APU_START(OP) DELAYED_APU_START(OP)
 #define DELAYED_APU_START(OP) case OP: {
 #define APU_END break; }
 #define APU_EXIT break;
#endif

#include "spc700.h"

struct SIAPU{
	uint8 *PC;
	uint8 *RAM;
	uint8 *DirectPage;
	bool8 APUExecuting;
	uint8 Bit;
	uint32 Address;
	uint8 *WaitAddress1;
	uint8 *WaitAddress2;
	uint32 WaitCounter;
	uint8 *ShadowRAM;
	uint8 *CachedSamples;
	uint8 _Carry;
	uint8 _Zero;
	uint8 _Overflow;
	uint32 TimerErrorCounter;
};

struct SAPU{
    uint32 Cycles;
    bool8 ShowROM;
    uint8 Flags;
    uint8 KeyedChannels;
    uint8 OutPorts [4];
    uint8 DSP [0x80];
    uint8 ExtraRAM [64];
    uint16 Timer [3];
    uint16 TimerTarget [3];
    bool8 TimerEnabled [3];
    bool8 TimerValueWritten [3];
};

extern SAPU APU;
extern SIAPU IAPU;

inline void S9xAPUUnpackStatus(){
    IAPU._Zero = ((APURegisters.P & Zero) == 0) | (APURegisters.P & Negative);
    IAPU._Carry = (APURegisters.P & Carry);
    IAPU._Overflow = (APURegisters.P & Overflow) >> 6;
}

inline void S9xAPUPackStatus(){
    APURegisters.P &= ~(Zero | Negative | Carry | Overflow);
    APURegisters.P |= IAPU._Carry | ((IAPU._Zero == 0) << 1) |
                      (IAPU._Zero & 0x80) | (IAPU._Overflow << 6);
}

void S9xResetAPU (void);
bool8 S9xInitAPU ();
void S9xDeinitAPU ();
void S9xDecacheSamples ();
void S9xSetAPUControl (uint8 byte);
void S9xSetAPUDSP (uint8 byte);
uint8 S9xGetAPUDSP ();
void S9xSetAPUTimer (uint16 Address, uint8 byte);
bool8 S9xInitSound (int quality, bool8 stereo, int buffer_size);
void S9xAPUSetEndOfSample (int channel);
void S9xAPUSetEndX (int channel);

#ifdef OPCODES_AS_FUNCTIONS
extern void (*S9xApuOpcodes [256]) ();
#endif
#define APU_VOL_LEFT    0x00
#define APU_VOL_RIGHT   0x01
#define APU_P_LOW   0x02
#define APU_P_HIGH  0x03
#define APU_SRCN 0x04
#define APU_ADSR1 0x05
#define APU_ADSR2 0x06
#define APU_GAIN 0x07
#define APU_ENVX 0x08
#define APU_OUTX 0x09

#define APU_MVOL_LEFT 0x0c
#define APU_MVOL_RIGHT 0x1c
#define APU_EVOL_LEFT 0x2c
#define APU_EVOL_RIGHT 0x3c
#define APU_KON 0x4c
#define APU_KOFF 0x5c
#define APU_FLG 0x6c
#define APU_ENDX 0x7c

#define APU_EFB 0x0d
#define APU_PMON 0x2d
#define APU_NON 0x3d
#define APU_EON 0x4d
#define APU_DIR 0x5d
#define APU_ESA 0x6d
#define APU_EDL 0x7d

#define APU_C0 0x0f
#define APU_C1 0x1f
#define APU_C2 0x2f
#define APU_C3 0x3f
#define APU_C4 0x4f
#define APU_C5 0x5f
#define APU_C6 0x6f
#define APU_C7 0x7f

#define APU_SOFT_RESET 0x80
#define APU_MUTE 0x40
#define APU_ECHO_DISABLED 0x20

#define FREQUENCY_MASK 0x3fff

void apu ();

#endif
