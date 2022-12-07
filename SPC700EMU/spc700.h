
#ifndef _SPC700_H_
#define _SPC700_H_

#define Carry       1
#define Zero        2
#define Interrupt   4
#define HalfCarry   8
#define BreakFlag  16
#define DirectPageFlag 32
#define Overflow   64
#define Negative  128

#define APUClearCarry() (IAPU._Carry = 0)
#define APUSetCarry() (IAPU._Carry = 1)
#define APUSetInterrupt() (APURegisters.P |= Interrupt)
#define APUClearInterrupt() (APURegisters.P &= ~Interrupt)
#define APUSetHalfCarry() (APURegisters.P |= HalfCarry)
#define APUClearHalfCarry() (APURegisters.P &= ~HalfCarry)
#define APUSetBreak() (APURegisters.P |= BreakFlag)
#define APUClearBreak() (APURegisters.P &= ~BreakFlag)
#define APUSetDirectPage() (APURegisters.P |= DirectPageFlag)
#define APUClearDirectPage() (APURegisters.P &= ~DirectPageFlag)
#define APUSetOverflow() (IAPU._Overflow = 1)
#define APUClearOverflow() (IAPU._Overflow = 0)

#define APUCheckZero() (IAPU._Zero == 0)
#define APUCheckCarry() (IAPU._Carry)
#define APUCheckInterrupt() (APURegisters.P & Interrupt)
#define APUCheckHalfCarry() (APURegisters.P & HalfCarry)
#define APUCheckBreak() (APURegisters.P & BreakFlag)
#define APUCheckDirectPage() (APURegisters.P & DirectPageFlag)
#define APUCheckOverflow() (IAPU._Overflow)
#define APUCheckNegative() (IAPU._Zero & 0x80)

#define APUClearFlags(f) (APURegisters.P &= ~(f))
#define APUSetFlags(f)   (APURegisters.P |=  (f))
#define APUCheckFlag(f)  (APURegisters.P &   (f))

union YAndA{
    struct { uint8 A, Y; } B;
    uint16 W;
};

struct SAPURegisters{
    uint8  P;
    YAndA YA;
    uint8  X;
    uint8  S;
    uint16  PC;
};

extern SAPURegisters APURegisters;

#ifdef OPCODES_AS_FUNCTIONS
#define APU_EXECUTE1() (*S9xApuOpcodes[*IAPU.PC])();
#else
#define APU_EXECUTE1() apu();
#endif

/*#define APU_EXECUTE() \
	if(IAPU.APUExecuting){\
		if(--APU.Cycles == 0){\
			APU.Cycles = Settings.SPCTo65c816Ratio;\
			APU_EXECUTE1(); \
		}\
	}*/

#endif
