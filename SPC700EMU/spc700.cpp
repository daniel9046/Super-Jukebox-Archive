#include "Main.h"

#define SPC700_WRAP_IN_PAGE0

#ifdef SPC700_WRAP_IN_PAGE0
 #define PAGEZERO_ADDRESS_TYPE uint8
#else
 #define PAGEZERO_ADDRESS_TYPE uint16
#endif

#ifdef NO_INLINE_SET_GET
 uint8 S9xAPUGetByteZ (PAGEZERO_ADDRESS_TYPE);
 uint8 S9xAPUGetByte (uint32);
 void S9xAPUSetByteZ (uint8, PAGEZERO_ADDRESS_TYPE);
 void S9xAPUSetByte (uint8, uint32);
#else
 #undef INLINE
 #define INLINE inline
 #include "apumem.h"
#endif

extern uint8 Work8;
extern uint16 Work16;
extern uint32 Work32;
extern signed char Int8;
extern short Int16;
extern long Int32;
extern short Int16;
extern uint8 W1;
extern uint8 W2;

#define OP1 (*(IAPU.PC + 1))
#define OP2 (*(IAPU.PC + 2))

#ifdef SPC700_SHUTDOWN
 #define APUShutdown() \
    if ((IAPU.PC == IAPU.WaitAddress1 || IAPU.PC == IAPU.WaitAddress2)) \
    { \
        if (IAPU.WaitCounter == 0) \
        { \
            IAPU.APUExecuting = FALSE; \
        } \
        else \
        if (IAPU.WaitCounter >= 2) \
            IAPU.WaitCounter = 1; \
        else \
            IAPU.WaitCounter--; \
    }
#else
 #define APUShutdown()
#endif

#define APUSetZN8(b)\
    IAPU._Zero = (b);

#define APUSetZN16(w)\
    IAPU._Zero = ((w) != 0) | ((w) >> 8);

void STOP(char *s){
    //char buffer [100];
    //sprintf (String, "Sound CPU in unknown state executing %s at %04X\n%s\n", s, IAPU.PC - IAPU.RAM, buffer);
    //S9xMessage (S9X_ERROR, S9X_APU_STOPED, String);
    APU.TimerEnabled [0] = APU.TimerEnabled [1] = APU.TimerEnabled [2] = FALSE;
    IAPU.APUExecuting = FALSE;
    //S9xExit ();
}

#define TCALL(n)\
{\
	PushW (IAPU.PC - IAPU.RAM + 1); \
	IAPU.PC = IAPU.RAM + (APU.ExtraRAM [((15 - n) << 1)] + \
		(APU.ExtraRAM [((15 - n) << 1) + 1] << 8)); \
}

// XXX: HalfCarry
#define SBC(a,b)\
 Int16 = (short) (a) - (short) (b) + (short) (APUCheckCarry ()) - 1;\
 APUClearHalfCarry ();\
 IAPU._Carry = Int16 >= 0;\
 if ((((a) ^ (b)) & 0x80) && (((a) ^ (uint8) Int16) & 0x80))\
    APUSetOverflow ();\
 else \
    APUClearOverflow (); \
 (a) = (uint8) Int16;\
 APUSetZN8 ((uint8) Int16);

// XXX: HalfCarry
#define ADC(a,b)\
 Work16 = (a) + (b) + APUCheckCarry();\
 APUClearHalfCarry ();\
 IAPU._Carry = Work16 >= 0x100; \
 if (~((a) ^ (b)) & ((b) ^ (uint8) Work16) & 0x80)\
    APUSetOverflow ();\
 else \
    APUClearOverflow (); \
 (a) = (uint8) Work16;\
 APUSetZN8 ((uint8) Work16);

#define CMP(a,b)\
 Int16 = (short) (a) - (short) (b);\
 IAPU._Carry = Int16 >= 0;\
 APUSetZN8 ((uint8) Int16);

#define ASL(b)\
    IAPU._Carry = ((b) & 0x80) != 0; \
    (b) <<= 1;\
    APUSetZN8 (b);
#define LSR(b)\
    IAPU._Carry = (b) & 1;\
    (b) >>= 1;\
    APUSetZN8 (b);
#define ROL(b)\
    Work16 = ((b) << 1) | APUCheckCarry (); \
    IAPU._Carry = Work16 >= 0x100; \
    (b) = (uint8) Work16; \
    APUSetZN8 (b);
#define ROR(b)\
    Work16 = (b) | ((uint16) APUCheckCarry () << 8); \
    IAPU._Carry = (uint8) Work16 & 1; \
    Work16 >>= 1; \
    (b) = (uint8) Work16; \
    APUSetZN8 (b);

#define Push(b)\
    *(IAPU.RAM + 0x100 + APURegisters.S) = b;\
    APURegisters.S--;

#define Pop(b)\
    APURegisters.S++;\
    (b) = *(IAPU.RAM + 0x100 + APURegisters.S);

#define PushW(w)\
    *(uint16 *) (IAPU.RAM + 0xff + APURegisters.S) = w;\
    APURegisters.S -= 2;
#define PopW(w)\
    APURegisters.S += 2;\
    w = *(uint16 *) (IAPU.RAM + 0xff + APURegisters.S);

#define Relative()\
    Int8 = OP1;\
    Int16 = (int) (IAPU.PC + 2 - IAPU.RAM) + Int8;

#define Relative2()\
    Int8 = OP2;\
    Int16 = (int) (IAPU.PC + 3 - IAPU.RAM) + Int8;

#ifdef SPC700_WRAP_IN_PAGE0
 #define IndexedXIndirect()\
    IAPU.Address = *(uint16 *) (IAPU.DirectPage + ((OP1 + APURegisters.X) & 0xff));
#else
 #define IndexedXIndirect()\
    IAPU.Address = *(uint16 *) (IAPU.DirectPage + OP1 + APURegisters.X);
#endif


#define Absolute()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1);

#define AbsoluteX()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1) + APURegisters.X;

#define AbsoluteY()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1) + APURegisters.YA.B.Y;

#define MemBit()\
    IAPU.Address = *(uint16 *) (IAPU.PC + 1);\
    IAPU.Bit = (uint8)(IAPU.Address >> 13);\
    IAPU.Address &= 0x1fff;

#define IndirectIndexedY()\
    IAPU.Address = *(uint16 *) (IAPU.DirectPage + OP1) + APURegisters.YA.B.Y;

#ifndef OPCODES_AS_FUNCTIONS
 void apu (){
	//if(APU.Flags & TRACE_FLAG) S9xTraceAPU();
	switch (*IAPU.PC){
#endif

APU_START(0x00)
// NOP
IAPU.PC++;
APU_END

APU_START(0x01)
// TCALL 0
TCALL(0)
APU_END

APU_START(0x11)
// TCALL 1
TCALL(1)
APU_END

APU_START(0x21)
// TCALL 2
TCALL(2)
APU_END

APU_START(0x31)
// TCALL 3
TCALL(3)
APU_END

APU_START(0x41)
// TCALL 4
TCALL(4)
APU_END

APU_START(0x51)
// TCALL 5
TCALL(5)
APU_END

APU_START(0x61)
// TCALL 6
TCALL(6)
APU_END

APU_START(0x71)
// TCALL 7
TCALL(7)
APU_END

APU_START(0x81)
// TCALL 8
TCALL(8)
APU_END

APU_START(0x91)
// TCALL 9
TCALL(9)
APU_END

APU_START(0xA1)
// TCALL 10
TCALL(10)
APU_END

APU_START(0xB1)
// TCALL 11
TCALL(11)
APU_END

APU_START(0xC1)
// TCALL 12
TCALL(12)
APU_END

APU_START(0xD1)
// TCALL 13
TCALL(13)
APU_END

APU_START(0xE1)
// TCALL 14
TCALL(14)
APU_END

APU_START(0xF1)
// TCALL 15
TCALL(15)
APU_END

APU_START(0x3F)
// CALL abs
Absolute ();
PushW (IAPU.PC + 3 - IAPU.RAM);
IAPU.PC = IAPU.RAM + IAPU.Address;
APU_END

APU_START(0x4F)
// PCALL $xx
Work8 = OP1;
PushW (IAPU.PC + 2 - IAPU.RAM);
IAPU.PC = IAPU.RAM + 0xff00 + Work8;
APU_END

#define SET(b) \
S9xAPUSetByteZ (S9xAPUGetByteZ (OP1) | (uint8)(1 << (b)), OP1); \
IAPU.PC += 2

APU_START(0x02)
SET(0);
APU_END

APU_START(0x22)
SET(1);
APU_END

APU_START(0x42)
SET(2);
APU_END

APU_START(0x62)
SET(3);
APU_END

APU_START(0x82)
SET(4);
APU_END

APU_START(0xA2)
SET(5);
APU_END

APU_START(0xC2)
SET(6);
APU_END

APU_START(0xE2)
SET(7);
APU_END

#define CLR(b) \
S9xAPUSetByteZ (S9xAPUGetByteZ (OP1) & (uint8)~(1 << (b)), OP1); \
IAPU.PC += 2;

APU_START(0x12)
CLR(0);
APU_END

APU_START(0x32)
CLR(1);
APU_END

APU_START(0x52)
CLR(2);
APU_END

APU_START(0x72)
CLR(3);
APU_END

APU_START(0x92)
CLR(4);
APU_END

APU_START(0xB2)
CLR(5);
APU_END

APU_START(0xD2)
CLR(6);
APU_END

APU_START(0xF2)
CLR(7);
APU_END

#define BBS(b) \
Work8 = OP1; \
Relative2 (); \
if (S9xAPUGetByteZ (Work8) & (1 << (b))) \
    IAPU.PC = IAPU.RAM + (uint16) Int16; \
else \
    IAPU.PC += 3

APU_START(0x03)
BBS(0);
APU_END

APU_START(0x23)
BBS(1);
APU_END

APU_START(0x43)
BBS(2);
APU_END

APU_START(0x63)
BBS(3);
APU_END

APU_START(0x83)
BBS(4);
APU_END

APU_START(0xA3)
BBS(5);
APU_END

APU_START(0xC3)
BBS(6);
APU_END

APU_START(0xE3)
BBS(7);
APU_END

#define BBC(b) \
Work8 = OP1; \
Relative2 (); \
if (!(S9xAPUGetByteZ (Work8) & (1 << (b)))) \
    IAPU.PC = IAPU.RAM + (uint16) Int16; \
else \
    IAPU.PC += 3

APU_START(0x13)
BBC(0);
APU_END

APU_START(0x33)
BBC(1);
APU_END

APU_START(0x53)
BBC(2);
APU_END

APU_START(0x73)
BBC(3);
APU_END

APU_START(0x93)
BBC(4);
APU_END

APU_START(0xB3)
BBC(5);
APU_END

APU_START(0xD3)
BBC(6);
APU_END

APU_START(0xF3)
BBC(7);
APU_END

APU_START(0x04)
// OR A,dp
APURegisters.YA.B.A |= S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x05)
// OR A,abs
Absolute ();
APURegisters.YA.B.A |= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x06)
// OR A,(X)
APURegisters.YA.B.A |= S9xAPUGetByteZ (APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x07)
// OR A,(dp+X)
IndexedXIndirect();
APURegisters.YA.B.A |= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x08)
// OR A,#00
APURegisters.YA.B.A |= OP1;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x09)
// OR dp(dest),dp(src)
Work8 = S9xAPUGetByteZ (OP1);
Work8 |= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x14)
// OR A,dp+X
APURegisters.YA.B.A |= S9xAPUGetByteZ (OP1 + APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x15)
// OR A,abs+X
AbsoluteX ();
APURegisters.YA.B.A |= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x16)
// OR A,abs+Y
AbsoluteY ();
APURegisters.YA.B.A |= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x17)
// OR A,(dp)+Y
IndirectIndexedY ();
APURegisters.YA.B.A |= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x18)
// OR dp,#00
Work8 = OP1;
Work8 |= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x19)
// OR (X),(Y)
Work8 = S9xAPUGetByteZ (APURegisters.X) | S9xAPUGetByteZ (APURegisters.YA.B.Y);
APUSetZN8 (Work8);
S9xAPUSetByteZ (Work8, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0x0A)
// OR1 C,membit
MemBit ();
if (APUCheckCarry ())
{
    if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
        APUSetCarry ();
}
IAPU.PC += 3;
APU_END

APU_START(0x2A)
// OR1 C,not membit
MemBit ();
if (!APUCheckCarry ())
{
    if (!(S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
        APUSetCarry ();
}
IAPU.PC += 3;
APU_END

APU_START(0x4A)
// AND1 C,membit
MemBit ();
if (APUCheckCarry ())
{
    if (!(S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
        APUClearCarry ();
}
IAPU.PC += 3;
APU_END

APU_START(0x6A)
// AND1 C, not membit
MemBit ();
if (APUCheckCarry ())
{
    if ((S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit)))
        APUClearCarry ();
}
IAPU.PC += 3;
APU_END

APU_START(0x8A)
// EOR1 C, membit
MemBit ();
if (APUCheckCarry ())
{
    if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
        APUClearCarry ();
}
else
{
    if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
        APUSetCarry ();
}
IAPU.PC += 3;
APU_END

APU_START(0xAA)
// MOV1 C,membit
MemBit ();
if (S9xAPUGetByte (IAPU.Address) & (1 << IAPU.Bit))
    APUSetCarry ();
else
    APUClearCarry ();
IAPU.PC += 3;
APU_END

APU_START(0xCA)
// MOV1 membit,C
MemBit ();
if (APUCheckCarry ())
{
    S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) | (1 << IAPU.Bit), IAPU.Address);
}
else
{
    S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) & ~(1 << IAPU.Bit), IAPU.Address);
}
IAPU.PC += 3;
APU_END

APU_START(0xEA)
// NOT1 membit
MemBit ();
S9xAPUSetByte (S9xAPUGetByte (IAPU.Address) ^ (1 << IAPU.Bit), IAPU.Address);
IAPU.PC += 3;
APU_END

// HERE
APU_START(0x0B)
// ASL dp
Work8 = S9xAPUGetByteZ (OP1);
ASL (Work8);
S9xAPUSetByteZ (Work8, OP1);
IAPU.PC += 2;
APU_END

APU_START(0x0C)
// ASL abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
ASL (Work8);
S9xAPUSetByte (Work8, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0x1B)
// ASL dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
ASL (Work8);
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0x1C)
// ASL A
ASL (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x0D)
// PUSH PSW
S9xAPUPackStatus ();
Push (APURegisters.P);
IAPU.PC++;
APU_END

APU_START(0x2D)
// PUSH A
Push (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x4D)
// PUSH X
Push (APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0x6D)
// PUSH Y
Push (APURegisters.YA.B.Y);
IAPU.PC++;
APU_END

APU_START(0x8E)
// POP PSW
Pop (APURegisters.P);
S9xAPUUnpackStatus ();
if (APUCheckDirectPage ())
    IAPU.DirectPage = IAPU.RAM + 0x100;
else
    IAPU.DirectPage = IAPU.RAM;
IAPU.PC++;
APU_END

APU_START(0xAE)
// POP A
Pop (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xCE)
// POP X
Pop (APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0xEE)
// POP Y
Pop (APURegisters.YA.B.Y);
IAPU.PC++;
APU_END

APU_START(0x0E)
// TSET1 abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
S9xAPUSetByte (Work8 | APURegisters.YA.B.A, IAPU.Address);
Work8 &= APURegisters.YA.B.A;
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x4E)
// TCLR1 abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
S9xAPUSetByte (Work8 & ~APURegisters.YA.B.A, IAPU.Address);
Work8 &= APURegisters.YA.B.A;
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x0F)
// BRK

#if 0
STOP ("BRK");
#else
PushW (IAPU.PC + 1 - IAPU.RAM);
S9xAPUPackStatus ();
Push (APURegisters.P);
APUSetBreak ();
APUClearInterrupt ();
// XXX:Where is the BRK vector ???
IAPU.PC = IAPU.RAM + APU.ExtraRAM [0x20] + (APU.ExtraRAM [0x21] << 8);
#endif
APU_END

APU_START(0xEF)
// SLEEP
// XXX: sleep
//STOP ("SLEEP");
IAPU.APUExecuting = FALSE;
IAPU.PC++;
APU_END

APU_START(0xFF)
// STOP
//STOP ("STOP");
IAPU.APUExecuting = FALSE;
IAPU.PC++;
APU_END

APU_START(0x10)
// BPL
Relative ();
if (!APUCheckNegative ())
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0x30)
// BMI
Relative ();
if (APUCheckNegative ())
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0x90)
// BCC
Relative ();
if (!APUCheckCarry ())
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0xB0)
// BCS
Relative ();
if (APUCheckCarry ())
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0xD0)
// BNE
if (!APUCheckZero ())
{
	IAPU.PC += ((int)((int8*)IAPU.PC)[1])+2;
	//IAPU.PC += ((int8*)IAPU.PC)[1];
    //APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0xF0)
// BEQ
if(APUCheckZero()){
	IAPU.PC += ((int)((int8*)IAPU.PC)[1])+2;
	//IAPU.PC += ((int8*)IAPU.PC)[1];
    //APUShutdown();
}
else
    IAPU.PC += 2;
APU_END

APU_START(0x50)
// BVC
Relative ();
if (!APUCheckOverflow ())
    IAPU.PC = IAPU.RAM + (uint16) Int16;
else
    IAPU.PC += 2;
APU_END

APU_START(0x70)
// BVS
Relative ();
if (APUCheckOverflow ())
    IAPU.PC = IAPU.RAM + (uint16) Int16;
else
    IAPU.PC += 2;
APU_END

APU_START(0x2F)
// BRA
Relative ();
IAPU.PC = IAPU.RAM + (uint16) Int16;
APU_END

APU_START(0x80)
// SETC
APUSetCarry ();
IAPU.PC++;
APU_END

APU_START(0xED)
// NOTC
IAPU._Carry ^= 1;
IAPU.PC++;
APU_END

APU_START(0x40)
// SETP
APUSetDirectPage ();
IAPU.DirectPage = IAPU.RAM + 0x100;
IAPU.PC++;
APU_END

APU_START(0x1A)
// DECW dp
Work16 = S9xAPUGetByteZ (OP1) + (S9xAPUGetByteZ (OP1 + 1) << 8);
Work16--;
S9xAPUSetByteZ ((uint8)Work16, OP1);
S9xAPUSetByteZ (Work16 >> 8, OP1 + 1);
APUSetZN16 (Work16);
IAPU.PC += 2;
APU_END

APU_START(0x5A)
// CMPW YA,dp
Work16 = S9xAPUGetByteZ (OP1) + (S9xAPUGetByteZ (OP1 + 1) << 8);
Int32 = (long) APURegisters.YA.W - (long) Work16;
IAPU._Carry = Int32 >= 0;
APUSetZN16 ((uint16) Int32);    
IAPU.PC += 2;
APU_END

APU_START(0x3A)
// INCW dp
Work16 = S9xAPUGetByteZ (OP1) + (S9xAPUGetByteZ (OP1 + 1) << 8);
Work16++;
S9xAPUSetByteZ ((uint8)Work16, OP1);
S9xAPUSetByteZ (Work16 >> 8, OP1 + 1);
APUSetZN16 (Work16);
IAPU.PC += 2;
APU_END

APU_START(0x7A)
// ADDW YA,dp
Work16 = S9xAPUGetByteZ (OP1) + (S9xAPUGetByteZ (OP1 + 1) << 8);
Work32 = (uint32) APURegisters.YA.W + Work16;
APUClearHalfCarry ();
IAPU._Carry = Work32 >= 0x10000;
if (~(APURegisters.YA.W ^ Work16) & (Work16 ^ (uint16) Work32) & 0x8000)
    APUSetOverflow ();
else
    APUClearOverflow ();
APURegisters.YA.W = (uint16) Work32;
APUSetZN16 (APURegisters.YA.W);
IAPU.PC += 2;
APU_END

APU_START(0x9A)
// SUBW YA,dp
Work16 = S9xAPUGetByteZ (OP1) + (S9xAPUGetByteZ (OP1 + 1) << 8);
Int32 = (long) APURegisters.YA.W - (long) Work16;
APUClearHalfCarry ();
IAPU._Carry = Int32 >= 0;
if (((APURegisters.YA.W ^ Work16) & 0x8000) &&
    ((APURegisters.YA.W ^ (uint16) Int32) & 0x8000))
    APUSetOverflow ();
else
    APUClearOverflow ();
if (((APURegisters.YA.W ^ Work16) & 0x0080) &&
    ((APURegisters.YA.W ^ (uint16) Int32) & 0x0080))
    APUSetHalfCarry ();
APURegisters.YA.W = (uint16) Int32;
APUSetZN16 (APURegisters.YA.W);
IAPU.PC += 2;
APU_END

APU_START(0xBA)
// MOVW YA,dp
APURegisters.YA.B.A = S9xAPUGetByteZ (OP1);
APURegisters.YA.B.Y = S9xAPUGetByteZ (OP1 + 1);
APUSetZN16 (APURegisters.YA.W);
IAPU.PC += 2;
APU_END

APU_START(0xDA)
// MOVW dp,YA
S9xAPUSetByteZ (APURegisters.YA.B.A, OP1);
S9xAPUSetByteZ (APURegisters.YA.B.Y, OP1 + 1);
IAPU.PC += 2;
APU_END

//HERE
APU_START(0x64)
// CMP A,dp
Work8 = S9xAPUGetByteZ (OP1);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x65)
// CMP A,abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x66)
// CMP A,(X)
Work8 = S9xAPUGetByteZ (APURegisters.X);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC++;
APU_END

APU_START(0x67)
// CMP A,(dp+X)
IndexedXIndirect ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x68)
// CMP A,#00
Work8 = OP1;
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x69)
// CMP dp(dest), dp(src)
W1 = S9xAPUGetByteZ (OP1);
Work8 = S9xAPUGetByteZ (OP2);
CMP (Work8, W1);
IAPU.PC += 3;
APU_END

APU_START(0x74)
// CMP A, dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x75)
// CMP A,abs+X
AbsoluteX ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x76)
// CMP A, abs+Y
AbsoluteY ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x77)
// CMP A,(dp)+Y
IndirectIndexedY ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x78)
// CMP dp,#00
Work8 = OP1;
W1 = S9xAPUGetByteZ (OP2);
CMP (W1, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x79)
// CMP (X),(Y)
W1 = S9xAPUGetByteZ (APURegisters.X);
Work8 = S9xAPUGetByteZ (APURegisters.YA.B.Y);
CMP (W1, Work8);
IAPU.PC++;
APU_END

APU_START(0x1E)
// CMP X,abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.X, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x3E)
// CMP X,dp
Work8 = S9xAPUGetByteZ (OP1);
CMP (APURegisters.X, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xC8)
// CMP X,#00
CMP (APURegisters.X, OP1);
IAPU.PC += 2;
APU_END

APU_START(0x5E)
// CMP Y,abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
CMP (APURegisters.YA.B.Y, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x7E)
// CMP Y,dp
Work8 = S9xAPUGetByteZ (OP1);
CMP (APURegisters.YA.B.Y, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xAD)
// CMP Y,#00
Work8 = OP1;
CMP (APURegisters.YA.B.Y, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x1F)
// JMP (abs+X)
Absolute ();
IAPU.PC = IAPU.RAM + S9xAPUGetByte (IAPU.Address + APURegisters.X) +
         (S9xAPUGetByte (IAPU.Address + APURegisters.X + 1) << 8);
// XXX: HERE:
//APU.Flags |= TRACE_FLAG;
APU_END

APU_START(0x5F)
// JMP abs
Absolute ();
IAPU.PC = IAPU.RAM + IAPU.Address;
APU_END

APU_START(0x20)
// CLRP
APUClearDirectPage ();
IAPU.DirectPage = IAPU.RAM;
IAPU.PC++;
APU_END

APU_START(0x60)
// CLRC
APUClearCarry ();
IAPU.PC++;
APU_END

APU_START(0xE0)
// CLRV
APUClearHalfCarry ();
APUClearOverflow ();
IAPU.PC++;
APU_END

APU_START(0x24)
// AND A,dp
APURegisters.YA.B.A &= S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x25)
// AND A,abs
Absolute ();
APURegisters.YA.B.A &= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x26)
// AND A,(X)
APURegisters.YA.B.A &= S9xAPUGetByteZ (APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x27)
// AND A,(dp+X)
IndexedXIndirect();
APURegisters.YA.B.A &= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x28)
// AND A,#00
APURegisters.YA.B.A &= OP1;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x29)
// AND dp(dest),dp(src)
Work8 = S9xAPUGetByteZ (OP1);
Work8 &= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x34)
// AND A,dp+X
APURegisters.YA.B.A &= S9xAPUGetByteZ (OP1 + APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x35)
// AND A,abs+X
AbsoluteX ();
APURegisters.YA.B.A &= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x36)
// AND A,abs+Y
AbsoluteY ();
APURegisters.YA.B.A &= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x37)
// AND A,(dp)+Y
IndirectIndexedY ();
APURegisters.YA.B.A &= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x38)
// AND dp,#00
Work8 = OP1;
Work8 &= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x39)
// AND (X),(Y)
Work8 = S9xAPUGetByteZ (APURegisters.X) & S9xAPUGetByteZ (APURegisters.YA.B.Y);
APUSetZN8 (Work8);
S9xAPUSetByteZ (Work8, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0x2B)
// ROL dp
Work8 = S9xAPUGetByteZ (OP1);
ROL (Work8);
S9xAPUSetByteZ (Work8, OP1);
IAPU.PC += 2;
APU_END

APU_START(0x2C)
// ROL abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
ROL (Work8);
S9xAPUSetByte (Work8, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0x3B)
// ROL dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
ROL (Work8);
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0x3C)
// ROL A
ROL (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x2E)
// CBNE dp,rel
Work8 = OP1;
Relative2 ();
if (S9xAPUGetByteZ (Work8) != APURegisters.YA.B.A)
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 3;
APU_END

APU_START(0xDE)
// CBNE dp+X,rel
Work16 = OP1 + APURegisters.X;
Relative2 ();
if (S9xAPUGetByteZ ((uint8)Work16) != APURegisters.YA.B.A)
{
    IAPU.PC = IAPU.RAM + (uint16) Int16;
    APUShutdown();
}
else
    IAPU.PC += 3;
APU_END

APU_START(0x3D)
// INC X
APURegisters.X++;
APUSetZN8 (APURegisters.X);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0xFC)
// INC Y
APURegisters.YA.B.Y++;
APUSetZN8 (APURegisters.YA.B.Y);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0x1D)
// DEC X
APURegisters.X--;
APUSetZN8 (APURegisters.X);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0xDC)
// DEC Y
APURegisters.YA.B.Y--;
APUSetZN8 (APURegisters.YA.B.Y);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0xAB)
// INC dp
Work8 = S9xAPUGetByteZ (OP1 ) + 1;
S9xAPUSetByteZ (Work8, OP1);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 2;
APU_END

APU_START(0xAC)
// INC abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address) + 1;
S9xAPUSetByte (Work8, IAPU.Address);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 3;
APU_END

APU_START(0xBB)
// INC dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X) + 1;
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 2;
APU_END

APU_START(0xBC)
// INC A
APURegisters.YA.B.A++;
APUSetZN8 (APURegisters.YA.B.A);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0x8B)
// DEC dp
Work8 = S9xAPUGetByteZ (OP1) - 1;
S9xAPUSetByteZ (Work8, OP1);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 2;
APU_END

APU_START(0x8C)
// DEC abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address) - 1;
S9xAPUSetByte (Work8, IAPU.Address);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 3;
APU_END

APU_START(0x9B)
// DEC dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X) - 1;
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
APUSetZN8 (Work8);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC += 2;
APU_END

APU_START(0x9C)
// DEC A
APURegisters.YA.B.A--;
APUSetZN8 (APURegisters.YA.B.A);
#ifdef SPC700_SHUTDOWN
IAPU.WaitCounter++;
#endif
IAPU.PC++;
APU_END

APU_START(0x44)
// EOR A,dp
APURegisters.YA.B.A ^= S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x45)
// EOR A,abs
Absolute ();
APURegisters.YA.B.A ^= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x46)
// EOR A,(X)
APURegisters.YA.B.A ^= S9xAPUGetByteZ (APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x47)
// EOR A,(dp+X)
IndexedXIndirect();
APURegisters.YA.B.A ^= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x48)
// EOR A,#00
APURegisters.YA.B.A ^= OP1;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x49)
// EOR dp(dest),dp(src)
Work8 = S9xAPUGetByteZ (OP1);
Work8 ^= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x54)
// EOR A,dp+X
APURegisters.YA.B.A ^= S9xAPUGetByteZ (OP1 + APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x55)
// EOR A,abs+X
AbsoluteX ();
APURegisters.YA.B.A ^= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x56)
// EOR A,abs+Y
AbsoluteY ();
APURegisters.YA.B.A ^= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0x57)
// EOR A,(dp)+Y
IndirectIndexedY ();
APURegisters.YA.B.A ^= S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0x58)
// EOR dp,#00
Work8 = OP1;
Work8 ^= S9xAPUGetByteZ (OP2);
S9xAPUSetByteZ (Work8, OP2);
APUSetZN8 (Work8);
IAPU.PC += 3;
APU_END

APU_START(0x59)
// EOR (X),(Y)
Work8 = S9xAPUGetByteZ (APURegisters.X) ^ S9xAPUGetByteZ (APURegisters.YA.B.Y);
APUSetZN8 (Work8);
S9xAPUSetByteZ (Work8, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0x4B)
// LSR dp
Work8 = S9xAPUGetByteZ (OP1);
LSR (Work8);
S9xAPUSetByteZ (Work8, OP1);
IAPU.PC += 2;
APU_END

APU_START(0x4C)
// LSR abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
LSR (Work8);
S9xAPUSetByte (Work8, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0x5B)
// LSR dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
LSR (Work8);
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0x5C)
// LSR A
LSR (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x7D)
// MOV A,X
APURegisters.YA.B.A = APURegisters.X;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xDD)
// MOV A,Y
APURegisters.YA.B.A = APURegisters.YA.B.Y;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x5D)
// MOV X,A
APURegisters.X = APURegisters.YA.B.A;
APUSetZN8 (APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0xFD)
// MOV Y,A
APURegisters.YA.B.Y = APURegisters.YA.B.A;
APUSetZN8 (APURegisters.YA.B.Y);
IAPU.PC++;
APU_END

APU_START(0x9D)
//MOV X,SP
APURegisters.X = APURegisters.S;
APUSetZN8 (APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0xBD)
// MOV SP,X
APURegisters.S = APURegisters.X;
IAPU.PC++;
APU_END

APU_START(0x6B)
// ROR dp
Work8 = S9xAPUGetByteZ (OP1);
ROR (Work8);
S9xAPUSetByteZ (Work8, OP1);
IAPU.PC += 2;
APU_END

APU_START(0x6C)
// ROR abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
ROR (Work8);
S9xAPUSetByte (Work8, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0x7B)
// ROR dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
ROR (Work8);
S9xAPUSetByteZ (Work8, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0x7C)
// ROR A
ROR (APURegisters.YA.B.A);
IAPU.PC ++;
APU_END

APU_START(0x6E)
// DBNZ dp,rel
Work8 = OP1;
Relative2 ();
W1 = S9xAPUGetByteZ (Work8) - 1;
S9xAPUSetByteZ (W1, Work8);
if (W1 != 0)
    IAPU.PC = IAPU.RAM + (uint16) Int16;
else
    IAPU.PC += 3;
APU_END

APU_START(0xFE)
// DBNZ Y,rel
Relative ();
APURegisters.YA.B.Y--;
if (APURegisters.YA.B.Y != 0)
    IAPU.PC = IAPU.RAM + (uint16) Int16;
else
    IAPU.PC += 2;
APU_END

APU_START(0x6F)
// RET
APURegisters.S += 2;
IAPU.PC = IAPU.RAM + *(uint16*)(IAPU.RAM + 0xff + APURegisters.S);
APU_END

APU_START(0x7F)
// RETI
//STOP ("RETI");
Pop (APURegisters.P);
S9xAPUUnpackStatus ();
PopW (APURegisters.PC);
IAPU.PC = IAPU.RAM + APURegisters.PC;
APU_END

APU_START(0x84)
// ADC A,dp
Work8 = S9xAPUGetByteZ (OP1);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x85)
// ADC A, abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x86)
// ADC A,(X)
Work8 = S9xAPUGetByteZ (APURegisters.X);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC++;
APU_END

APU_START(0x87)
// ADC A,(dp+X)
IndexedXIndirect ();
Work8 = S9xAPUGetByte (IAPU.Address);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x88)
// ADC A,#00
Work8 = OP1;
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x89)
// ADC dp(dest),dp(src)
Work8 = S9xAPUGetByteZ (OP1);
W1 = S9xAPUGetByteZ (OP2);
ADC (W1, Work8);
S9xAPUSetByteZ (W1, OP2);
IAPU.PC += 3;
APU_END

APU_START(0x94)
// ADC A,dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x95)
// ADC A, abs+X
AbsoluteX ();
Work8 = S9xAPUGetByte (IAPU.Address);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x96)
// ADC A, abs+Y
AbsoluteY ();
Work8 = S9xAPUGetByte (IAPU.Address);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0x97)
// ADC A, (dp)+Y
IndirectIndexedY ();
Work8 = S9xAPUGetByte (IAPU.Address);
ADC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0x98)
// ADC dp,#00
Work8 = OP1;
W1 = S9xAPUGetByteZ (OP2);
ADC (W1, Work8);
S9xAPUSetByteZ (W1, OP2);
IAPU.PC += 3;
APU_END

APU_START(0x99)
// ADC (X),(Y)
W1 = S9xAPUGetByteZ (APURegisters.X);
Work8 = S9xAPUGetByteZ (APURegisters.YA.B.Y);
ADC (W1, Work8);
S9xAPUSetByteZ (W1, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0x8D)
// MOV Y,#00
APURegisters.YA.B.Y = OP1;
APUSetZN8 (APURegisters.YA.B.Y);
IAPU.PC += 2;
APU_END

APU_START(0x8F)
// MOV dp,#00
Work8 = OP1;
S9xAPUSetByteZ (Work8, OP2);
IAPU.PC += 3;
APU_END

APU_START(0x9E)
// DIV YA,X
if (APURegisters.X == 0)
{
    APUSetOverflow ();
    APURegisters.YA.B.Y = 0xff;
    APURegisters.YA.B.A = 0xff;
}
else
{
    APUClearOverflow ();
    Work8 = APURegisters.YA.W / APURegisters.X;
    APURegisters.YA.B.Y = APURegisters.YA.W % APURegisters.X;
    APURegisters.YA.B.A = Work8;
}
// XXX How should Overflow, Half Carry, Zero and Negative flags be set??
//APUSetZN16 (APURegisters.YA.W);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0x9F)
// XCN A
APURegisters.YA.B.A = (APURegisters.YA.B.A >> 4) | (APURegisters.YA.B.A << 4);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xA4)
// SBC A, dp
Work8 = S9xAPUGetByteZ (OP1);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xA5)
// SBC A, abs
Absolute ();
Work8 = S9xAPUGetByte (IAPU.Address);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0xA6)
// SBC A, (X)
Work8 = S9xAPUGetByteZ (APURegisters.X);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC++;
APU_END

APU_START(0xA7)
// SBC A,(dp+X)
IndexedXIndirect ();
Work8 = S9xAPUGetByte (IAPU.Address);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xA8)
// SBC A,#00
Work8 = OP1;
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xA9)
// SBC dp(dest), dp(src)
Work8 = S9xAPUGetByteZ (OP1);
W1 = S9xAPUGetByteZ (OP2);
SBC (W1, Work8);
S9xAPUSetByteZ (W1, OP2);
IAPU.PC += 3;
APU_END

APU_START(0xB4)
// SBC A, dp+X
Work8 = S9xAPUGetByteZ (OP1 + APURegisters.X);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xB5)
// SBC A,abs+X
AbsoluteX ();
Work8 = S9xAPUGetByte (IAPU.Address);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0xB6)
// SBC A,abs+Y
AbsoluteY ();
Work8 = S9xAPUGetByte (IAPU.Address);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 3;
APU_END

APU_START(0xB7)
// SBC A,(dp)+Y
IndirectIndexedY ();
Work8 = S9xAPUGetByte (IAPU.Address);
SBC (APURegisters.YA.B.A, Work8);
IAPU.PC += 2;
APU_END

APU_START(0xB8)
// SBC dp,#00
Work8 = OP1;
W1 = S9xAPUGetByteZ (OP2);
SBC (W1, Work8);
S9xAPUSetByteZ (W1, OP2);
IAPU.PC += 3;
APU_END

APU_START(0xB9)
// SBC (X),(Y)
W1 = S9xAPUGetByteZ (APURegisters.X);
Work8 = S9xAPUGetByteZ (APURegisters.YA.B.Y);
SBC (W1, Work8);
S9xAPUSetByteZ (W1, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0xAF)
// MOV (X)+, A
S9xAPUSetByteZ (APURegisters.YA.B.A, APURegisters.X++);
IAPU.PC++;
APU_END

APU_START(0xBE)
// DAS
// XXX:
IAPU.PC++;
APU_END

APU_START(0xBF)
// MOV A,(X)+
APURegisters.YA.B.A = S9xAPUGetByteZ (APURegisters.X++);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xC0)
// DI
APUClearInterrupt ();
IAPU.PC++;
APU_END

APU_START(0xA0)
// EI
APUSetInterrupt ();
IAPU.PC++;
APU_END

APU_START(0xC4)
// MOV dp,A
S9xAPUSetByteZ (APURegisters.YA.B.A, OP1);
IAPU.PC += 2;
APU_END

APU_START(0xC5)
// MOV abs,A
Absolute ();
S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0xC6)
// MOV (X), A
S9xAPUSetByteZ (APURegisters.YA.B.A, APURegisters.X);
IAPU.PC++;
APU_END

APU_START(0xC7)
// MOV (dp+X),A
IndexedXIndirect ();
S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
IAPU.PC += 2;
APU_END

APU_START(0xC9)
// MOV abs,X
Absolute ();
S9xAPUSetByte (APURegisters.X, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0xCB)
// MOV dp,Y
S9xAPUSetByteZ (APURegisters.YA.B.Y, OP1);
IAPU.PC += 2;
APU_END

APU_START(0xCC)
// MOV abs,Y
Absolute ();
S9xAPUSetByte (APURegisters.YA.B.Y, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0xCD)
// MOV X,#00
APURegisters.X = OP1;
APUSetZN8 (APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0xCF)
// MUL YA
APURegisters.YA.W = (uint16) APURegisters.YA.B.A * APURegisters.YA.B.Y;
APUSetZN16 (APURegisters.YA.W);
IAPU.PC++;
APU_END

APU_START(0xD4)
// MOV dp+X, A
S9xAPUSetByteZ (APURegisters.YA.B.A, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0xD5)
// MOV abs+X,A
AbsoluteX ();
S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0xD6)
// MOV abs+Y,A
AbsoluteY ();
S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
IAPU.PC += 3;
APU_END

APU_START(0xD7)
// MOV (dp)+Y,A
IndirectIndexedY ();
S9xAPUSetByte (APURegisters.YA.B.A, IAPU.Address);
IAPU.PC += 2;
APU_END

APU_START(0xD8)
// MOV dp,X
S9xAPUSetByteZ (APURegisters.X, OP1);
IAPU.PC += 2;
APU_END

APU_START(0xD9)
// MOV dp+Y,X
S9xAPUSetByteZ (APURegisters.X, OP1 + APURegisters.YA.B.Y);
IAPU.PC += 2;
APU_END

APU_START(0xDB)
// MOV dp+X,Y
S9xAPUSetByteZ (APURegisters.YA.B.Y, OP1 + APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0xDF)
// DAA
W1 = APURegisters.YA.B.A & 0xf;
W2 = APURegisters.YA.B.A >> 4;
APUClearCarry ();
if (W1 > 9)
{
    W1 -= 10;
    W2++;
}
if (W2 > 9)
{
    W2 -= 10;
    APUSetCarry ();
}
APURegisters.YA.B.A = W1 | (W2 << 4);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xE4)
// MOV A, dp
APURegisters.YA.B.A = S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0xE5)
// MOV A,abs
Absolute ();
APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0xE6)
// MOV A,(X)
APURegisters.YA.B.A = S9xAPUGetByteZ (APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC++;
APU_END

APU_START(0xE7)
// MOV A,(dp+X)
IndexedXIndirect();
APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0xE8)
// MOV A,#00
APURegisters.YA.B.A = OP1;
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0xE9)
// MOV X, abs
Absolute ();
APURegisters.X = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.X);
IAPU.PC += 3;
APU_END

APU_START(0xEB)
// MOV Y,dp
APURegisters.YA.B.Y = S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.YA.B.Y);
IAPU.PC += 2;
APU_END

APU_START(0xEC)
// MOV Y,abs
Absolute ();
APURegisters.YA.B.Y = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.Y);
IAPU.PC += 3;
APU_END

APU_START(0xF4)
// MOV A, dp+X
APURegisters.YA.B.A = S9xAPUGetByteZ (OP1 + APURegisters.X);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0xF5)
// MOV A, abs+X
AbsoluteX ();
APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0xF6)
// MOV A, abs+Y
AbsoluteY ();
APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 3;
APU_END

APU_START(0xF7)
// MOV A, (dp)+Y
IndirectIndexedY ();
APURegisters.YA.B.A = S9xAPUGetByte (IAPU.Address);
APUSetZN8 (APURegisters.YA.B.A);
IAPU.PC += 2;
APU_END

APU_START(0xF8)
// MOV X,dp
APURegisters.X = S9xAPUGetByteZ (OP1);
APUSetZN8 (APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0xF9)
// MOV X,dp+Y
APURegisters.X = S9xAPUGetByteZ (OP1 + APURegisters.YA.B.Y);
APUSetZN8 (APURegisters.X);
IAPU.PC += 2;
APU_END

APU_START(0xFA)
// MOV dp(dest),dp(src)
S9xAPUSetByteZ (S9xAPUGetByteZ (OP1), OP2);
IAPU.PC += 3;
APU_END

APU_START(0xFB)
// MOV Y,dp+X
APURegisters.YA.B.Y = S9xAPUGetByteZ (OP1 + APURegisters.X);
APUSetZN8 (APURegisters.YA.B.Y);
IAPU.PC += 2;
APU_END

#ifdef NO_INLINE_SET_GET
 #undef INLINE
 #define INLINE
 #include "apumem.h"
#endif

#ifndef OPCODES_AS_FUNCTIONS
}
}
#else
void (*S9xApuOpcodes [256]) () = {
Apu0x00, Apu0x01, Apu0x02, Apu0x03, Apu0x04, Apu0x05, Apu0x06, Apu0x07, 
Apu0x08, Apu0x09, Apu0x0A, Apu0x0B, Apu0x0C, Apu0x0D, Apu0x0E, Apu0x0F, 
Apu0x10, Apu0x11, Apu0x12, Apu0x13, Apu0x14, Apu0x15, Apu0x16, Apu0x17, 
Apu0x18, Apu0x19, Apu0x1A, Apu0x1B, Apu0x1C, Apu0x1D, Apu0x1E, Apu0x1F, 
Apu0x20, Apu0x21, Apu0x22, Apu0x23, Apu0x24, Apu0x25, Apu0x26, Apu0x27, 
Apu0x28, Apu0x29, Apu0x2A, Apu0x2B, Apu0x2C, Apu0x2D, Apu0x2E, Apu0x2F, 
Apu0x30, Apu0x31, Apu0x32, Apu0x33, Apu0x34, Apu0x35, Apu0x36, Apu0x37, 
Apu0x38, Apu0x39, Apu0x3A, Apu0x3B, Apu0x3C, Apu0x3D, Apu0x3E, Apu0x3F, 
Apu0x40, Apu0x41, Apu0x42, Apu0x43, Apu0x44, Apu0x45, Apu0x46, Apu0x47, 
Apu0x48, Apu0x49, Apu0x4A, Apu0x4B, Apu0x4C, Apu0x4D, Apu0x4E, Apu0x4F, 
Apu0x50, Apu0x51, Apu0x52, Apu0x53, Apu0x54, Apu0x55, Apu0x56, Apu0x57, 
Apu0x58, Apu0x59, Apu0x5A, Apu0x5B, Apu0x5C, Apu0x5D, Apu0x5E, Apu0x5F, 
Apu0x60, Apu0x61, Apu0x62, Apu0x63, Apu0x64, Apu0x65, Apu0x66, Apu0x67, 
Apu0x68, Apu0x69, Apu0x6A, Apu0x6B, Apu0x6C, Apu0x6D, Apu0x6E, Apu0x6F, 
Apu0x70, Apu0x71, Apu0x72, Apu0x73, Apu0x74, Apu0x75, Apu0x76, Apu0x77, 
Apu0x78, Apu0x79, Apu0x7A, Apu0x7B, Apu0x7C, Apu0x7D, Apu0x7E, Apu0x7F, 
Apu0x80, Apu0x81, Apu0x82, Apu0x83, Apu0x84, Apu0x85, Apu0x86, Apu0x87, 
Apu0x88, Apu0x89, Apu0x8A, Apu0x8B, Apu0x8C, Apu0x8D, Apu0x8E, Apu0x8F, 
Apu0x90, Apu0x91, Apu0x92, Apu0x93, Apu0x94, Apu0x95, Apu0x96, Apu0x97, 
Apu0x98, Apu0x99, Apu0x9A, Apu0x9B, Apu0x9C, Apu0x9D, Apu0x9E, Apu0x9F, 
Apu0xA0, Apu0xA1, Apu0xA2, Apu0xA3, Apu0xA4, Apu0xA5, Apu0xA6, Apu0xA7, 
Apu0xA8, Apu0xA9, Apu0xAA, Apu0xAB, Apu0xAC, Apu0xAD, Apu0xAE, Apu0xAF, 
Apu0xB0, Apu0xB1, Apu0xB2, Apu0xB3, Apu0xB4, Apu0xB5, Apu0xB6, Apu0xB7, 
Apu0xB8, Apu0xB9, Apu0xBA, Apu0xBB, Apu0xBC, Apu0xBD, Apu0xBE, Apu0xBF, 
Apu0xC0, Apu0xC1, Apu0xC2, Apu0xC3, Apu0xC4, Apu0xC5, Apu0xC6, Apu0xC7, 
Apu0xC8, Apu0xC9, Apu0xCA, Apu0xCB, Apu0xCC, Apu0xCD, Apu0xCE, Apu0xCF, 
Apu0xD0, Apu0xD1, Apu0xD2, Apu0xD3, Apu0xD4, Apu0xD5, Apu0xD6, Apu0xD7, 
Apu0xD8, Apu0xD9, Apu0xDA, Apu0xDB, Apu0xDC, Apu0xDD, Apu0xDE, Apu0xDF, 
Apu0xE0, Apu0xE1, Apu0xE2, Apu0xE3, Apu0xE4, Apu0xE5, Apu0xE6, Apu0xE7, 
Apu0xE8, Apu0xE9, Apu0xEA, Apu0xEB, Apu0xEC, Apu0xED, Apu0xEE, Apu0xEF, 
Apu0xF0, Apu0xF1, Apu0xF2, Apu0xF3, Apu0xF4, Apu0xF5, Apu0xF6, Apu0xF7, 
Apu0xF8, Apu0xF9, Apu0xFA, Apu0xFB, Apu0xFC, Apu0xFD, Apu0xFE, Apu0xFF
};
#endif
