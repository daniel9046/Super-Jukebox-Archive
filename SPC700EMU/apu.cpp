#include "Main.h"

extern int NoiseFreq [32];

void FixEnvelope (int channel, uint8 gain, uint8 adsr1, uint8 adsr2);
void StartSample (int channel);

bool8 S9xInitAPU(){
	IAPU.RAM = new uint8 [0x10000];
	IAPU.ShadowRAM = new uint8 [0x10000];
	IAPU.CachedSamples = new uint8 [0x40000];
	if(!IAPU.RAM||!IAPU.ShadowRAM||!IAPU.CachedSamples)
        return (FALSE);
    return (TRUE);
}

void S9xDeinitAPU(){
	delete IAPU.RAM;
	delete IAPU.ShadowRAM;
	delete IAPU.CachedSamples;
}

extern uint8 APUROM [64];

void S9xResetAPU (){
    memset (IAPU.RAM, 0xff, 0x10000);
    ZeroMemory (IAPU.ShadowRAM, 0x10000);
    ZeroMemory (IAPU.CachedSamples, 0x40000);
    ZeroMemory (APU.OutPorts, 4);
    IAPU.DirectPage = IAPU.RAM;
    memmove (&IAPU.RAM [0xffc0], APUROM, sizeof (APUROM));
    memmove (&APU.ExtraRAM, APUROM, sizeof (APUROM));
    IAPU.PC = IAPU.RAM + IAPU.RAM [0xfffe] + (IAPU.RAM [0xffff] << 8);
    APU.Cycles = 0;//Settings.SPCTo65c816Ratio;
    APURegisters.YA.W = 0;
    APURegisters.X = 0;
    APURegisters.S = 0xff;
    APURegisters.P = 0;
    S9xAPUUnpackStatus ();
    APURegisters.PC = 0;
    IAPU.APUExecuting = TRUE;//Settings.APUEnabled;
#ifdef SPC700_SHUTDOWN
    IAPU.WaitAddress1 = NULL;
    IAPU.WaitAddress2 = NULL;
    IAPU.WaitCounter = 0;
#endif
    APU.ShowROM = TRUE;
    IAPU.RAM [0xf1] = 0x80;

    for (int i = 0; i < 3; i++){
        APU.TimerEnabled [i] = FALSE;
        APU.TimerValueWritten [i] = 0;
        APU.TimerTarget [i] = 0;
        APU.Timer [i] = 0;
    }
    for (int j = 0; j < 0x80; j++)
        APU.DSP [j] = 0;

    APU.DSP [APU_ENDX] = 0xff;
    APU.DSP [APU_KOFF] = 0;
    APU.DSP [APU_KON] = 0;
    APU.DSP [APU_FLG] = APU_MUTE | APU_ECHO_DISABLED;
    APU.KeyedChannels = 0;

    S9xResetSound ();
    S9xSetEchoEnable (0);
}

void S9xSetAPUDSP (uint8 byte)
{
    uint8 reg = IAPU.RAM [0xf2] & 0x7f;

    switch (reg)
    {
    case APU_FLG:
        if (byte & APU_SOFT_RESET)
        {
            APU.DSP [reg] = APU_MUTE | APU_ECHO_DISABLED | (byte & 0x1f);
            APU.DSP [APU_ENDX] = 0xff;
            APU.DSP [APU_KOFF] = 0;
            APU.DSP [APU_KON] = 0;
            S9xSetEchoWriteEnable (FALSE);
            // Kill sound
            S9xResetSound ();
        }
        else
        {
            if (byte & APU_ECHO_DISABLED)
                S9xSetEchoWriteEnable (FALSE);
            else
                S9xSetEchoWriteEnable (TRUE);

            if (byte & APU_MUTE)
            {
                S9xSetSoundMute (TRUE);
            }
            else
                S9xSetSoundMute (FALSE);
        }
        break;
    case APU_NON:
        if (byte != APU.DSP [APU_NON])
        {
            uint8 mask = 1;
            for (int c = 0; c < 8; c++, mask <<= 1)
            {
                int type;
                if (byte & mask)
                {
                    type = SOUND_NOISE;
                }
                else
                {
                    type = SOUND_SAMPLE;
                }
                S9xSetSoundType (c, type);
            }
        }
        break;
    case APU_MVOL_LEFT:
        if (byte != APU.DSP [APU_MVOL_LEFT])
        {
            S9xSetMasterVolume ((signed char) byte,
                             (signed char) APU.DSP [APU_MVOL_RIGHT]);
        }
        break;
    case APU_MVOL_RIGHT:
        if (byte != APU.DSP [APU_MVOL_RIGHT])
        {
            S9xSetMasterVolume ((signed char) APU.DSP [APU_MVOL_LEFT],
                             (signed char) byte);
        }
        break;
    case APU_EVOL_LEFT:
        if (byte != APU.DSP [APU_EVOL_LEFT])
        {
            S9xSetEchoVolume ((signed char) byte,
							(signed char) APU.DSP [APU_EVOL_RIGHT]);
        }
        break;
    case APU_EVOL_RIGHT:
        if (byte != APU.DSP [APU_EVOL_RIGHT])
        {
			S9xSetEchoVolume ((signed char) APU.DSP [APU_EVOL_LEFT],
							(signed char) byte);
        }
        break;
    case APU_ENDX:
        byte = 0;
        break;

    case APU_KOFF:
        if (byte)
        {
            uint8 mask = 1;
            for (int c = 0; c < 8; c++, mask <<= 1)
            {
                if ((byte & mask) != 0)
                {
                    if (APU.KeyedChannels & mask)
                    {
                        APU.KeyedChannels &= ~mask;
                        APU.DSP [APU_KON] &= ~mask;
                        APU.DSP [APU_KOFF] |= mask;
                        S9xSetSoundKeyOff (c);
                    }
                }
            }
        }
        APU.DSP [APU_KOFF] = byte;
        return;
    case APU_KON:
        if (byte)
        {
            uint8 mask = 1;
            for (int c = 0; c < 8; c++, mask <<= 1)
            {
                if ((byte & mask) != 0)
                {
                    APU.KeyedChannels |= mask;
                    APU.DSP [APU_KON] |= mask;
                    StartSample (c);
                }
            }
        }
        return;
        
    case APU_VOL_LEFT + 0x00:
    case APU_VOL_LEFT + 0x10:
    case APU_VOL_LEFT + 0x20:
    case APU_VOL_LEFT + 0x30:
    case APU_VOL_LEFT + 0x40:
    case APU_VOL_LEFT + 0x50:
    case APU_VOL_LEFT + 0x60:
    case APU_VOL_LEFT + 0x70:
        if (byte != APU.DSP [reg])
        {
            S9xSetSoundVolume (reg >> 4, (signed char) byte,
                            (signed char) APU.DSP [reg + 1]);
        }
        break;
    case APU_VOL_RIGHT + 0x00:
    case APU_VOL_RIGHT + 0x10:
    case APU_VOL_RIGHT + 0x20:
    case APU_VOL_RIGHT + 0x30:
    case APU_VOL_RIGHT + 0x40:
    case APU_VOL_RIGHT + 0x50:
    case APU_VOL_RIGHT + 0x60:
    case APU_VOL_RIGHT + 0x70:
        if (byte != APU.DSP [reg])
        {
            S9xSetSoundVolume (reg >> 4, (signed char) APU.DSP [reg - 1],
                            (signed char) byte);
        }
        break;

    case APU_P_LOW + 0x00:
    case APU_P_LOW + 0x10:
    case APU_P_LOW + 0x20:
    case APU_P_LOW + 0x30:
    case APU_P_LOW + 0x40:
    case APU_P_LOW + 0x50:
    case APU_P_LOW + 0x60:
    case APU_P_LOW + 0x70:
        S9xSetSoundFrequency (reg >> 4, ((byte + (APU.DSP [reg + 1] << 8)) & FREQUENCY_MASK) * 8);
        break;

    case APU_P_HIGH + 0x00:
    case APU_P_HIGH + 0x10:
    case APU_P_HIGH + 0x20:
    case APU_P_HIGH + 0x30:
    case APU_P_HIGH + 0x40:
    case APU_P_HIGH + 0x50:
    case APU_P_HIGH + 0x60:
    case APU_P_HIGH + 0x70:
        S9xSetSoundFrequency (reg >> 4, (((byte << 8) + APU.DSP [reg - 1]) & FREQUENCY_MASK) * 8);
        break;

    case APU_SRCN + 0x00:
    case APU_SRCN + 0x10:
    case APU_SRCN + 0x20:
    case APU_SRCN + 0x30:
    case APU_SRCN + 0x40:
    case APU_SRCN + 0x50:
    case APU_SRCN + 0x60:
    case APU_SRCN + 0x70:
        if (byte != APU.DSP [reg])
        {
            S9xSetSoundSample (reg >> 4, byte);
        }
        break;
        
    case APU_ADSR1 + 0x00:
    case APU_ADSR1 + 0x10:
    case APU_ADSR1 + 0x20:
    case APU_ADSR1 + 0x30:
    case APU_ADSR1 + 0x40:
    case APU_ADSR1 + 0x50:
    case APU_ADSR1 + 0x60:
    case APU_ADSR1 + 0x70:
        if (byte != APU.DSP [reg])
        {
            FixEnvelope (reg >> 4, APU.DSP [reg + 2], byte, APU.DSP [reg + 1]);
        }
        break;

    case APU_ADSR2 + 0x00:
    case APU_ADSR2 + 0x10:
    case APU_ADSR2 + 0x20:
    case APU_ADSR2 + 0x30:
    case APU_ADSR2 + 0x40:
    case APU_ADSR2 + 0x50:
    case APU_ADSR2 + 0x60:
    case APU_ADSR2 + 0x70:
        if (byte != APU.DSP [reg])
        {
            FixEnvelope (reg >> 4, APU.DSP [reg + 1], APU.DSP [reg - 1], byte);
        }
        break;

    case APU_GAIN + 0x00:
    case APU_GAIN + 0x10:
    case APU_GAIN + 0x20:
    case APU_GAIN + 0x30:
    case APU_GAIN + 0x40:
    case APU_GAIN + 0x50:
    case APU_GAIN + 0x60:
    case APU_GAIN + 0x70:
        if (byte != APU.DSP [reg])
        {
            FixEnvelope (reg >> 4, byte, APU.DSP [reg - 2], APU.DSP [reg - 1]);
        }
        break;

    case APU_ENVX + 0x00:
    case APU_ENVX + 0x10:
    case APU_ENVX + 0x20:
    case APU_ENVX + 0x30:
    case APU_ENVX + 0x40:
    case APU_ENVX + 0x50:
    case APU_ENVX + 0x60:
    case APU_ENVX + 0x70:
        break;

    case APU_OUTX + 0x00:
    case APU_OUTX + 0x10:
    case APU_OUTX + 0x20:
    case APU_OUTX + 0x30:
    case APU_OUTX + 0x40:
    case APU_OUTX + 0x50:
    case APU_OUTX + 0x60:
    case APU_OUTX + 0x70:
        break;
    
    case APU_DIR:
        break;

    case APU_PMON:
        if (byte != APU.DSP [reg])
        {
            S9xSetFrequencyModulationEnable (byte);
        }
        break;

    case APU_EON:
        if (byte != APU.DSP [APU_EON])
        {
            S9xSetEchoEnable (byte);
        }
        break;

    case APU_EFB:
        S9xSetEchoFeedback ((signed char) byte);
        break;

    case APU_ESA:
        break;

    case APU_EDL:
        if ((byte & 0xf) != APU.DSP [APU_EDL])
            S9xSetEchoDelay (byte & 0xf);
        break;

    case APU_C0:
    case APU_C1:
    case APU_C2:
    case APU_C3:
    case APU_C4:
    case APU_C5:
    case APU_C6:
    case APU_C7:
        S9xSetFilterCoefficient (reg >> 4, (signed char) byte);
        break;
    default:
        break;
    }

    APU.DSP [reg] = byte;
}


void FixEnvelope (int channel, uint8 gain, uint8 adsr1, uint8 adsr2)
{
    if (adsr1 & 0x80)
    {
        // ADSR mode
        static unsigned long AttackRate [16] = {
            4100, 2600, 1500, 1000, 640, 380, 260, 160,
            96, 64, 40, 24, 16, 10, 6, 0
        };
        static unsigned long DecayRate [8] = {
            1200, 740, 440, 290, 180, 110, 74, 37
        };
        static unsigned long SustainRate [32] = {
            ~0, 38000, 28000, 24000, 19000, 14000, 12000, 9400,
            7100, 5900, 4700, 3500, 2900, 2400, 1800, 1500,
            1200, 880, 740, 590, 440, 370, 290, 220,
            180, 150, 110, 92, 74, 55, 37, 18
        };
        if (S9xSetSoundMode (channel, SOUND_MODE_ADSR) == SOUND_MODE_ADSR)
        {
            S9xSetSoundADSR (channel, AttackRate [adsr1 & 0xf],
                          DecayRate [(adsr1 >> 4) & 7],
                          SustainRate [adsr2 & 0x1f],
                          (adsr2 >> 5) & 7, 8);
        }
    }
    else
    {
        // Gain mode
        if ((gain & 0x80) == 0)
        {
            if (S9xSetSoundMode (channel, SOUND_MODE_GAIN) == SOUND_MODE_GAIN)
            {
                S9xSetSoundADSR (channel, 0, 0, ~0, 7, 8);
                S9xSetEnvelopeHeight (channel, gain & 0x7f);
            }
        }
        else
        {
            static unsigned long IncreaseRate [32] = {
                ~0, 4100, 3100, 2600, 2000, 1500, 1300, 1000,
                770, 640, 510, 380, 320, 260, 190, 160,
                130, 96, 80, 64, 48, 40, 32, 24,
                20, 16, 12, 10, 8, 6, 4, 2
            };
            static unsigned long DecreaseRateExp [32] = {
                ~0, 38000, 28000, 24000, 19000, 14000, 12000, 9400,
                7100, 5900, 4700, 3500, 2900, 2400, 1800, 1500,
                1200, 880, 740, 590, 440, 370, 290, 220,
                180, 150, 110, 92, 74, 55, 37, 18
            };
            if (gain & 0x40)
            {
                // Increase mode
                // XXX: Bent curve or straight line
                if (S9xSetSoundMode (channel, SOUND_MODE_INCREASE) == SOUND_MODE_INCREASE)
                {
                    S9xSetSoundADSR (channel, IncreaseRate [gain & 0x1f],
                                  ~0, ~0, 7, 8);
                }
            }
            else
            {
                // XXX: Bent curve or straight line
                // Switch to decrease mode from ADSR mode
                if (S9xSetSoundMode (channel, SOUND_MODE_DECREASE) == SOUND_MODE_DECREASE)
                {
                    S9xSetSoundADSR (channel, 0,
                                  (gain & 0x20) != 0 ? DecreaseRateExp [gain & 0x1f] :
                                  IncreaseRate [gain & 0x1f], ~0, 7, 8);
                    S9xSetSoundDecayMode (channel);
                }
            }
        }
    }
}


void S9xSetAPUControl (uint8 byte)
{
    if ((byte & 1) != 0 && !APU.TimerEnabled [0])
    {
        APU.Timer [0] = 0;
        IAPU.RAM [0xfd] = 0;
        if ((APU.TimerTarget [0] = IAPU.RAM [0xfa]) == 0)
            APU.TimerTarget [0] = 0x100;
    }
    if ((byte & 2) != 0 && !APU.TimerEnabled [1])
    {
        APU.Timer [1] = 0;
        IAPU.RAM [0xfe] = 0;
        if ((APU.TimerTarget [1] = IAPU.RAM [0xfb]) == 0)
            APU.TimerTarget [1] = 0x100;
    }
    if ((byte & 4) != 0 && !APU.TimerEnabled [2])
    {
        APU.Timer [2] = 0;
        IAPU.RAM [0xff] = 0;
        if ((APU.TimerTarget [2] = IAPU.RAM [0xfc]) == 0)
            APU.TimerTarget [2] = 0x100;
    }
    APU.TimerEnabled [0] = byte & 1;
    APU.TimerEnabled [1] = (byte & 2) >> 1;
    APU.TimerEnabled [2] = (byte & 4) >> 2;

    if (byte & 0x10)
        IAPU.RAM [0xF4] = IAPU.RAM [0xF5] = 0;

    if (byte & 0x20)
        IAPU.RAM [0xF6] = IAPU.RAM [0xF7] = 0;

    if (byte & 0x80)
        APU.ShowROM = TRUE;
    else
        APU.ShowROM = FALSE;
    IAPU.RAM [0xf1] = byte;
}

void S9xSetAPUTimer (uint16 Address, uint8 byte)
{
    IAPU.RAM [Address] = byte;

    switch (Address)
    {
    case 0xfa:
//      if (!APU.TimerEnabled [0] || !APU.TimerValueWritten [0])
        {
            if ((APU.TimerTarget [0] = IAPU.RAM [0xfa]) == 0)
                APU.TimerTarget [0] = 0x100;
            APU.TimerValueWritten [0] = TRUE;
        }
        break;
    case 0xfb:
//      if (!APU.TimerEnabled [1] || !APU.TimerValueWritten [1])
        {
            if ((APU.TimerTarget [1] = IAPU.RAM [0xfb]) == 0)
                APU.TimerTarget [1] = 0x100;
            APU.TimerValueWritten [1] = TRUE;
        }
        break;
    case 0xfc:
//      if (!APU.TimerEnabled [2] || !APU.TimerValueWritten [2])
        {
            if ((APU.TimerTarget [2] = IAPU.RAM [0xfc]) == 0)
                APU.TimerTarget [2] = 0x100;
            APU.TimerValueWritten [2] = TRUE;
        }
        break;
    }
}

uint8 S9xGetAPUDSP ()
{
    uint8 reg = IAPU.RAM [0xf2] & 0x7f;
    uint8 byte = APU.DSP [reg];

    switch (reg)
    {
    case APU_OUTX + 0x00:
    case APU_OUTX + 0x10:
    case APU_OUTX + 0x20:
    case APU_OUTX + 0x30:
    case APU_OUTX + 0x40:
    case APU_OUTX + 0x50:
    case APU_OUTX + 0x60:
    case APU_OUTX + 0x70:
        return (byte);
        break;

    case APU_ENVX + 0x00:
    case APU_ENVX + 0x10:
    case APU_ENVX + 0x20:
    case APU_ENVX + 0x30:
    case APU_ENVX + 0x40:
    case APU_ENVX + 0x50:
    case APU_ENVX + 0x60:
    case APU_ENVX + 0x70:
        return ((uint8) S9xGetEnvelopeHeight (reg >> 4));

    case APU_ENDX:
        APU.DSP [APU_ENDX] = 0;
        break;
    default:
        break;
    }
    return (byte);
}

void S9xAPUSetEndOfSample (int ch)
{
    APU.DSP [APU_ENDX] |= 1 << ch;
    APU.DSP [APU_KON] &= ~(1 << ch);
    APU.DSP [APU_KOFF] &= ~(1 << ch);
}

void S9xAPUSetEndX (int ch)
{
    APU.DSP [APU_ENDX] |= 1 << ch;
}

void StartSample (int c)
{
    // Convert sample and start playing sound
    uint16 sample_number = APU.DSP [APU_SRCN + c * 0x10];
    int freq;
    int type;
    if (APU.DSP [APU_NON] & (1 << c))
    {
        type = SOUND_NOISE;
        freq = NoiseFreq [APU.DSP [APU_FLG] & 0x1f];
    }
    else
    {
        type = SOUND_SAMPLE;
        freq = ((APU.DSP [APU_P_LOW + c * 0x10] +
                (APU.DSP [APU_P_HIGH + c * 0x10] << 8)) & FREQUENCY_MASK) * 8;
    }
    S9xSetSoundMode (c, SOUND_MODE_NONE);
    FixEnvelope (c, 
                 APU.DSP [APU_GAIN  + (c << 4)], 
                 APU.DSP [APU_ADSR1 + (c << 4)],
                 APU.DSP [APU_ADSR2 + (c << 4)]);

    S9xPlaySample (c, type,
                (signed char) APU.DSP [APU_VOL_LEFT + c * 0x10],
                (signed char) APU.DSP [APU_VOL_RIGHT + c * 0x10],
                freq, sample_number);
}

