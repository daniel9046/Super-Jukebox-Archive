#ifndef _apumemory_h_
#define _apumemory_h_

extern uint8 W4;
extern uint8 APUROM[64];

/*INLINE uint8 S9xAPUGetByteZ (uint8 Address){
    if (Address >= 0xf0 && IAPU.DirectPage == IAPU.RAM){
        if (Address >= 0xf4 && Address <= 0xf7){
#ifdef SPC700_SHUTDOWN
			IAPU.WaitAddress2 = IAPU.WaitAddress1;
			IAPU.WaitAddress1 = IAPU.PC;
#endif      
            return (IAPU.RAM [Address]);
        }
        if (Address >= 0xfd)
        {
#ifdef SPC700_SHUTDOWN
            IAPU.WaitAddress2 = IAPU.WaitAddress1;
            IAPU.WaitAddress1 = IAPU.PC;
#endif
            uint8 t = IAPU.RAM [Address];
            IAPU.RAM [Address] = 0;
            return (t);
        }
        else
		if (Address == 0xf3)
			return (S9xGetAPUDSP ());
        return (IAPU.RAM [Address]);
    }
    else
        return (IAPU.DirectPage [Address]);
}*/

INLINE uint8 S9xAPUGetByteZ (uint8 Address){
    if (Address == 0xf3)
        return (S9xGetAPUDSP ());
    if (Address >= 0xfd && IAPU.DirectPage == IAPU.RAM){
        uint8 t = IAPU.RAM [Address];
        IAPU.RAM [Address] = 0;
        return (t);
    }
    return (IAPU.DirectPage [Address]);
}

INLINE void S9xAPUSetByteZ (uint8 byte, uint8 Address)
{
    if (Address >= 0xf0 && IAPU.DirectPage == IAPU.RAM)
    {
        if (Address == 0xf3)
            S9xSetAPUDSP (byte);
        else
        if (Address >= 0xf4 && Address <= 0xf7)
            APU.OutPorts [Address - 0xf4] = byte;
        else
        if (Address == 0xf1)
            S9xSetAPUControl (byte);
        else
        if (Address < 0xfd)
        {
            IAPU.RAM [Address] = byte;
            if (Address >= 0xfa)
            {
                if (byte == 0)
                    APU.TimerTarget [Address - 0xfa] = 0x100;
                else
                    APU.TimerTarget [Address - 0xfa] = byte;
            }
        }
    }
    else
        IAPU.DirectPage [Address] = byte;
}

/*INLINE uint8 S9xAPUGetByte (uint32 Address)
{
    Address &= 0xffff;
    
    if (Address <= 0xff && Address >= 0xf0)
    {
        if (Address >= 0xf4 && Address <= 0xf7)
        {
#ifdef SPC700_SHUTDOWN
			IAPU.WaitAddress2 = IAPU.WaitAddress1;
			IAPU.WaitAddress1 = IAPU.PC;
#endif
            return (IAPU.RAM [Address]);
        }
        else
        if (Address == 0xf3)
            return (S9xGetAPUDSP ());
        if (Address >= 0xfd)
        {
#ifdef SPC700_SHUTDOWN
            IAPU.WaitAddress2 = IAPU.WaitAddress1;
            IAPU.WaitAddress1 = IAPU.PC;
#endif
            uint8 t = IAPU.RAM [Address];
            IAPU.RAM [Address] = 0;
            return (t);
        }
        return (IAPU.RAM [Address]);
    }
    else
    if (Address >= 0xffc0)
    {
        if (APU.ShowROM)
            return (APUROM [Address - 0xffc0]);
        else
            return (APU.ExtraRAM [Address - 0xffc0]);
    }
    else
        return (IAPU.RAM [Address]);
}*/

INLINE uint8 S9xAPUGetByte (uint32 Address)
{
    Address &= 0xffff;
    
    if (Address == 0xf3)
        return (S9xGetAPUDSP ());
    if (Address <= 0xff && Address >= 0xfd)
    {
        uint8 t = IAPU.RAM [Address];
        IAPU.RAM [Address] = 0;
        return (t);
    }
    if (Address >= 0xffc0)
    {
        if (APU.ShowROM)
            return (APUROM [Address - 0xffc0]);
        return (APU.ExtraRAM [Address - 0xffc0]);
    }
    return (IAPU.RAM [Address]);
}

INLINE void S9xAPUSetByte (uint8 byte, uint32 Address)
{
    Address &= 0xffff;
    
    if (Address <= 0xff && Address >= 0xf0)
    {
        if (Address == 0xf3)
            S9xSetAPUDSP (byte);
        else
        if (Address >= 0xf4 && Address <= 0xf7)
            APU.OutPorts [Address - 0xf4] = byte;
        else
        if (Address == 0xf1)
            S9xSetAPUControl (byte);
        else
        if (Address < 0xfd)
        {
            IAPU.RAM [Address] = byte;
            if (Address >= 0xfa)
            {
                if (byte == 0)
                    APU.TimerTarget [Address - 0xfa] = 0x100;
                else
                    APU.TimerTarget [Address - 0xfa] = byte;
            }
        }
    }
    else
    {
        if (Address < 0xffc0)
            IAPU.RAM [Address] = byte;
        else
            APU.ExtraRAM [Address - 0xffc0] = byte;
    }
}
#endif
