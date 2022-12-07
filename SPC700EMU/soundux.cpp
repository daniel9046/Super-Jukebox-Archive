#include "Main.h"

#define SIXTEEN_BIT_SHIFT 1
#define EIGHT_BIT_SHIFT 9

extern int Echo [15360 + 14];
extern int DummyEchoBuffer [MAX_BUFFER_SIZE];
extern int MixBuffer [MAX_BUFFER_SIZE];
extern int EchoBuffer [MAX_BUFFER_SIZE];
extern int FilterTaps [8];
extern unsigned long Z;
extern int Loop [16];

extern long FilterValues[4][2];
extern int NoiseFreq [32];

#undef ABS
#define ABS(a) ((a) < 0 ? -(a) : (a))

#define AUDIO_CONV(A) (128 + (A >> 8))

#define FIXED_POINT 0x4000
#define FIXED_POINT_REMAINDER 0x3fff
#define FIXED_POINT_SHIFT 14

void S9xSetEnvelopeRate (int channel, unsigned long rate, int direction,
                         int target)
{
    SoundData.channels[channel].envx_target = target;

    if (rate == ~0)
    {
        SoundData.channels[channel].direction = 0;
        rate = 0;
    }
    else
        SoundData.channels[channel].direction = direction;

    int steps = MAX_ENVELOPE_HEIGHT;
    if (rate == 0 || so.playback_rate == 0)
        SoundData.channels[channel].erate = 0;
    else
        SoundData.channels[channel].erate = (unsigned long)
            (((1.0 / ((double) rate / 1000.0 / steps)) * FIXED_POINT) /
                                                        so.playback_rate);
}

void S9xSetSoundVolume (int channel, short volume_left, short volume_right)
{
    Channel *ch = &SoundData.channels[channel];
    ch->volume_left = volume_left;
    ch->volume_right = volume_right;
    if (!so.stereo)
    {
        if (ABS (volume_right) > ABS (volume_left))
            ch->volume_left = ch->volume_right;
        else
            ch->volume_right = ch->volume_left;
    }
    SoundData.channels[channel].left_vol_level =
            (SoundData.channels[channel].envx *
             SoundData.channels[channel].volume_left) >> VOLUME_SHIFT;
    SoundData.channels[channel].right_vol_level =
            (SoundData.channels[channel].envx *
             SoundData.channels[channel].volume_right) >> VOLUME_SHIFT;
}

void S9xSetMasterVolume (short volume_left, short volume_right)
{
    /*if (Settings.DisableMasterVolume)
    {
        SoundData.master_volume_left = 127;
        SoundData.master_volume_right = 127;
    }
    else
    {*/
        SoundData.master_volume_left = volume_left;
        SoundData.master_volume_right = volume_right;
        if (!so.stereo)
        {
            if (ABS (volume_right) > ABS (volume_left))
                SoundData.master_volume_left = SoundData.master_volume_right;
            else
                SoundData.master_volume_right = SoundData.master_volume_left;
        }
    //}
}

void S9xSetEchoVolume (short volume_left, short volume_right)
{
    SoundData.echo_volume_left = volume_left;
    SoundData.echo_volume_right = volume_right;
    if (!so.stereo)
    {
        if (ABS (volume_right) > ABS (volume_left))
            SoundData.echo_volume_left = SoundData.echo_volume_right;
        else
            SoundData.echo_volume_right = SoundData.echo_volume_left;
    }
}

void S9xSetEchoEnable (uint8 byte)
{
    SoundData.echo_channel_enable = byte;
    if (!SoundData.echo_write_enabled)// || Settings.DisableSoundEcho)
        byte = 0;
    if (byte && !SoundData.echo_enable)
        memset (Echo, 0, sizeof (Echo));
    SoundData.echo_enable = byte;
    for (int i = 0; i < 8; i++)
    {
        if (byte & (1 << i))
            SoundData.channels [i].echo_buf_ptr = EchoBuffer;
        else
            SoundData.channels [i].echo_buf_ptr = DummyEchoBuffer;
    }
}

void S9xSetEchoFeedback (int feedback)
{
#define MAX_FEEDBACK 127

    if (feedback > MAX_FEEDBACK)
        feedback = MAX_FEEDBACK;
    else
    if (feedback < -MAX_FEEDBACK)
        feedback = -MAX_FEEDBACK;

    SoundData.echo_feedback = feedback;
}

void S9xSetEchoDelay (int delay)
{
    SoundData.echo_buffer_size = 512 * delay;
    if (so.stereo)
        SoundData.echo_buffer_size <<= 1;
    if (SoundData.echo_buffer_size)
        SoundData.echo_ptr %= SoundData.echo_buffer_size;
    else
        SoundData.echo_ptr = 0;
    S9xSetEchoEnable (SoundData.echo_channel_enable);
}

void S9xSetEchoWriteEnable (uint8 byte)
{
    SoundData.echo_write_enabled = byte;
    S9xSetEchoEnable (SoundData.echo_channel_enable);
}

void S9xSetFrequencyModulationEnable (uint8 byte)
{
    SoundData.pitch_mod = byte;
}

void S9xSetSoundKeyOff (int channel)
{
    if (SoundData.channels[channel].state != SOUND_SILENT)
    {
        SoundData.channels[channel].state = SOUND_RELEASE;
        S9xSetEnvelopeRate (channel, SoundData.channels[channel].release_rate, -1, 0);
        SoundData.channels[channel].mode = SOUND_MODE_NONE;
    }
}

void S9xFixSoundAfterSnapshotLoad ()
{
    SoundData.echo_write_enabled = !(APU.DSP [APU_FLG] & 0x20);
    SoundData.echo_channel_enable = APU.DSP [APU_EON];
    S9xSetEchoDelay (APU.DSP [APU_EDL] & 0xf);
    S9xSetEchoFeedback ((signed char) APU.DSP [APU_EFB]);

    S9xSetFilterCoefficient (0, (signed char) APU.DSP [APU_C0]);
    S9xSetFilterCoefficient (1, (signed char) APU.DSP [APU_C1]);
    S9xSetFilterCoefficient (2, (signed char) APU.DSP [APU_C2]);
    S9xSetFilterCoefficient (3, (signed char) APU.DSP [APU_C3]);
    S9xSetFilterCoefficient (4, (signed char) APU.DSP [APU_C4]);
    S9xSetFilterCoefficient (5, (signed char) APU.DSP [APU_C5]);
    S9xSetFilterCoefficient (6, (signed char) APU.DSP [APU_C6]);
    S9xSetFilterCoefficient (7, (signed char) APU.DSP [APU_C7]);
    for (int i = 0; i < 8; i++)
        SoundData.channels[i].needs_decode = TRUE;
}

void S9xSetFilterCoefficient (int tap, int value)
{
    FilterTaps [tap & 7] = value;
}

void S9xSetSoundADSR (int channel, int attack_rate, int decay_rate,
                   int sustain_rate, int sustain_level, int release_rate)
{
    SoundData.channels[channel].attack_rate = attack_rate;
    SoundData.channels[channel].decay_rate = decay_rate;
    SoundData.channels[channel].sustain_rate = sustain_rate;
    SoundData.channels[channel].release_rate = release_rate;
    SoundData.channels[channel].sustain_level = sustain_level;
}

void S9xSetEnvelopeHeight (int channel, int level)
{
    S9xSetEnvelopeRate (channel, ~0, 0, 0);
    SoundData.channels[channel].envx = level >> (7 - ENVELOPE_SHIFT);
    SoundData.channels[channel].left_vol_level =
            (SoundData.channels[channel].envx *
             SoundData.channels[channel].volume_left) >> VOLUME_SHIFT;
    SoundData.channels[channel].right_vol_level =
            (SoundData.channels[channel].envx *
             SoundData.channels[channel].volume_right) >> VOLUME_SHIFT;
}

int S9xGetEnvelopeHeight (int channel)
{
    if (//Settings.SoundEnvelopeHeightReading &&
        (APU.KeyedChannels & (1 << channel)))
        return (SoundData.channels[channel].envx);
    else
        return (0);
}

void S9xSetSoundSample (int channel, uint16 sample_number)
{
#if 0
    register Channel *ch = &SoundData.channels[channel];
    if (ch->state != SOUND_SILENT && 
        sample_number != ch->sample_number)
    {
        int keep = ch->state;
        ch->state = SOUND_SILENT;

        ch->sample_number = sample_number;
        ch->loop = FALSE;
        ch->needs_decode = TRUE;
        ch->last_block = FALSE;
        ch->previous [0] = ch->previous[1] = 0;
        uint8 *dir = IAPU.RAM + (((APU.DSP [APU_DIR] << 8) +
                            (sample_number << 2)) & 0xffff);
        ch->block_pointer = *dir + (*(dir + 1) << 8);
        ch->sample_pointer = 0;
        ch->state = keep;
    }
#endif
}

void S9xSetSoundDecayMode (int channel)
{
    register Channel *ch = &SoundData.channels[channel];
    if (ch->state != SOUND_SILENT)
    {
        S9xSetEnvelopeRate (channel, ch->decay_rate, -1, 0);
        ch->state = SOUND_DECAY;
    }
}

int S9xSetSoundMode (int channel, int mode)
{
//    if (APU.KeyedChannels & (1 << channel))
    {
        switch (mode)
        {
        case SOUND_MODE_ADSR:
            if (SoundData.channels [channel].mode == SOUND_MODE_NONE)
                SoundData.channels [channel].mode = SOUND_MODE_ADSR;
            break;
        case SOUND_MODE_GAIN:
            switch (SoundData.channels [channel].mode)
            {
            case SOUND_MODE_NONE:
            case SOUND_MODE_INCREASE:
            case SOUND_MODE_DECREASE:
            case SOUND_MODE_ADSR:
                SoundData.channels [channel].mode = SOUND_MODE_GAIN;
                break;
            }
            break;
        case SOUND_MODE_INCREASE:
            if (SoundData.channels [channel].mode == SOUND_MODE_NONE ||
                SoundData.channels [channel].mode == SOUND_MODE_DECREASE)
                SoundData.channels [channel].mode = SOUND_MODE_INCREASE;
            break;
        case SOUND_MODE_DECREASE:
            SoundData.channels [channel].mode = SOUND_MODE_DECREASE;
            break;
        case SOUND_MODE_NONE:
            SoundData.channels [channel].mode = SOUND_MODE_NONE;
            break;
        }
    }
    return (SoundData.channels [channel].mode);
}

void S9xSetSoundFrequency (int channel, int hertz)
{
    SoundData.channels[channel].frequency = hertz;
}

void S9xSetSoundType (int channel, int type_of_sound)
{
    SoundData.channels[channel].type = type_of_sound;
}

bool8 S9xSetSoundMute (bool8 mute){
	bool8 old = so.mute_sound;
	so.mute_sound = mute;
	return (old);
}

inline void DecodeBlock (Channel *ch)
{
    signed long out;
    unsigned char filter;
    unsigned char shift;
    signed char sample1, sample2;
    unsigned char i;

    if (ch->block_pointer > 0x10000 - 9)
    {
        ch->last_block = TRUE;
        ch->loop = FALSE;
        ch->block = ch->decoded;
        return;
    }
    signed char *compressed = (signed char *) &IAPU.RAM [ch->block_pointer];

    filter = *compressed;
    if (filter & 1)
    {
        ch->last_block = TRUE;
        if (filter & 2)
            ch->loop = TRUE;
    }

    // If enabled, results in 'tick' sound on some samples that repeat by
    // re-using part of the original sample but generate a slightly different
    // waveform.
    /*if (!Settings.DisableSampleCaching &&
        memcmp ((uint8 *) compressed, &IAPU.ShadowRAM [ch->block_pointer], 9) == 0)
    {
        ch->block = (signed short *) (IAPU.CachedSamples + (ch->block_pointer << 2));
        ch->previous [0] = ch->block [15];
        ch->previous [1] = ch->block [14];
    }
    else
    {
        if (!Settings.DisableSampleCaching)
            memcpy (&IAPU.ShadowRAM [ch->block_pointer], (uint8 *) compressed, 9);*/
        compressed++;
        signed short *raw = ch->block = ch->decoded;

        shift = filter >> 4;
        filter = ((filter >> 2) & 3);
        long prev0 = ch->previous[0];
        long prev1 = ch->previous[1];
        long *f = (long *) &FilterValues[filter];

        for (i = 8; i != 0; i--)
        {
            sample1 = *compressed++;
            sample2 = sample1 << 4;
            //Sample 2 = Bottom Nibble, Sign Extended.
            sample2 >>= 4;
            //Sample 1 = Top Nibble, shifted down and Sign Extended.
            sample1 >>= 4;
            out = (sample1 << shift) + 
                   (((prev0 * f[0]) + (prev1 * f[1])) >> 8);

            if (out < -32768) out = -32768;
            if (out > 32767) out = 32767;
            prev1 = prev0;
            prev0 = out;
            *raw++ = (signed short) out;

            out = (sample2 << shift) +
                   (((prev0 * f[0]) + (prev1 * f[1])) >> 8);

            if (out < -32768) out = -32768;
            if (out > 32767) out = 32767;
            prev1 = prev0;
            prev0 = out;
            *raw++ = (signed short) out;
        }
        ch->previous [0] = (signed short)prev0;
        ch->previous [1] = (signed short)prev1;
        /*if (!Settings.DisableSampleCaching)
            memcpy (IAPU.CachedSamples + (ch->block_pointer << 2),
                    (uint8 *) ch->decoded, 32);
    }*/

    if ((ch->block_pointer += 9) >= 0x10000 - 9)
    {
        ch->last_block = TRUE;
        ch->loop = FALSE;
        ch->block_pointer -= 0x10000 - 9;
    }
}

int envelope_shift;

inline void MixStereo (int sample_count)
{
    int J;
    int K;
    int I;
    int prev_pitch_mod;
	envelope_shift=ENVELOPE_SHIFT-g_VM;

    for (J = 0; J < NUM_CHANNELS; J++)
    {
        register Channel *ch = &SoundData.channels[J];

        if (SoundData.pitch_mod & (1 << (J + 1)))
            memset (SoundData.channels[J].wave, 0,
                    sizeof (SoundData.channels[0].wave));

        int VL;
        int VR;
        int freq;
        
        // Stereo sound
        if (ch->state != SOUND_SILENT &&
            (so.sound_switch & (1 << J)))
        {
            if (ch->type == SOUND_NOISE)
                freq = NoiseFreq [APU.DSP [APU_FLG] & 0x1f];
            else
                freq = ch->frequency;

            if (freq)
            {
                if (J > 0 && SoundData.pitch_mod & (1 << J) &&
                    ch->type != SOUND_NOISE)
                {
                    prev_pitch_mod = SoundData.channels [J - 1].wave [0];
                    freq = (ch->frequency * (prev_pitch_mod + 1)) >> 8;
                }
                K = (unsigned long) (((double) freq) *
                                     FIXED_POINT / so.playback_rate);
            }
            else
                K = 0;
            
            if (ch->needs_decode)
            {
                DecodeBlock (ch);
                ch->needs_decode = FALSE;
                ch->sample_pointer = 1;
                ch->sample = ch->block [0];
            }
            if (ch->type == SOUND_NOISE)
            {
                VL = (((so.noise_gen << 17) >> 17) * ch->left_vol_level) >> envelope_shift;
                VR = (((so.noise_gen << 17) >> 17) * ch->right_vol_level) >> envelope_shift;
            }
            else
            {
                VL = (ch->sample * ch->left_vol_level) >> envelope_shift;
                VR = (ch->sample * ch->right_vol_level) >> envelope_shift;
            }

            for (I = 0; I < sample_count; I += 2)
            {
                ch->env_error += ch->erate;
                if (ch->env_error >= FIXED_POINT)
                {
                    uint32 step = (ch->env_error >> FIXED_POINT_SHIFT) - 1;
                    if (step)
                    {
                        if (ch->direction < 0)
                        {
                            if (ch->envx - (int) step <= ch->envx_target)
                                ch->envx = ch->envx_target;
                            else
                                ch->envx -= step;
                        }
                        else
                        {
                            if (ch->envx + (int) step >= ch->envx_target)
                                ch->envx = ch->envx_target;
                            else
                                ch->envx += step;
                        }
                        ch->env_error &= FIXED_POINT_REMAINDER;
                    }
                    else
                        ch->env_error -= FIXED_POINT;

                    if (ch->envx == ch->envx_target || ch->envx < 0)
                    {
                        if (ch->envx <= 0)
                        {
                            ch->state = SOUND_SILENT;
                            S9xAPUSetEndOfSample (J);
                            APU.KeyedChannels &= ~(1 << J);
                            goto stereo_exit;
                        }
                        else
                        switch (++ch->state)
                        {
                        case SOUND_DECAY:
                            if (ch->sustain_level != 7)
                            {
                                S9xSetEnvelopeRate (J, ch->decay_rate, -1,
                                                 (MAX_ENVELOPE_HEIGHT * (ch->sustain_level + 1)) >> 3);
                                break;
                            }
                            ch->state++;
                            // Fall ...
                        case SOUND_SUSTAIN:
                            S9xSetEnvelopeRate (J, ch->sustain_rate, -1, 0);
                            break;
                        case SOUND_RELEASE:
                            S9xSetEnvelopeRate (J, ch->release_rate, -1, 0);
                            break;
                        default:
                            ch->state = SOUND_SILENT;
                            S9xAPUSetEndOfSample (J);
                            APU.KeyedChannels &= ~(1 << J);
                            goto stereo_exit;
                        }
                    }
                    else
                        ch->envx += ch->direction;

                    ch->left_vol_level = (ch->envx * ch->volume_left) >> VOLUME_SHIFT;
                    ch->right_vol_level = (ch->envx * ch->volume_right) >> VOLUME_SHIFT;
                }

                ch->count += K;
                if (ch->count >= FIXED_POINT)
                {
                    ch->sample_pointer += (ch->count >> FIXED_POINT_SHIFT) - 1;
                    ch->count &= FIXED_POINT_REMAINDER;
                    if (ch->sample_pointer >= 16)
                    {
                        ch->sample_pointer -= 16;
                        if (ch->last_block)
                        {
                            S9xAPUSetEndX (J);
                            if (!ch->loop)
                            {
                                ch->state = SOUND_SILENT;
                                APU.KeyedChannels &= ~(1 << J);
                                S9xAPUSetEndOfSample (J);
                                goto stereo_exit;
                            }
                            ch->last_block = FALSE;
                            uint8 *dir = IAPU.RAM + (((APU.DSP [APU_DIR] << 8) +
                                                    (ch->sample_number << 2)) & 0xffff);
                            ch->block_pointer = *(dir + 2) + (*(dir + 3) << 8);
                        }
                        DecodeBlock (ch);
                    }
                    ch->sample = ch->block [ch->sample_pointer++];
                    if (ch->type == SOUND_NOISE)
                    {
                        if ((so.noise_gen <<= 1) & 0x80000000L)
                            so.noise_gen ^= 0x0040001L;
                        VL = (((so.noise_gen << 17) >> 17) * ch->left_vol_level) >> envelope_shift;
                        VR = (((so.noise_gen << 17) >> 17) * ch->right_vol_level) >> envelope_shift;
                    }
                    else
                    {
                        VL = (ch->sample * ch->left_vol_level) >> envelope_shift;
                        VR = (ch->sample * ch->right_vol_level) >> envelope_shift;
                    }
                }
                MixBuffer [I    ] += VL;
                MixBuffer [I + 1] += VR;
                ch->echo_buf_ptr [I    ] += VL;
                ch->echo_buf_ptr [I + 1] += VR;

                if (SoundData.pitch_mod)
                {
                    if (SoundData.pitch_mod & (1 << (J + 1)))
                        ch->wave [I] = ((ch->sample * ch->envx) >> (ENVELOPE_SHIFT + 8)) & 0x7f;
                
                    if (J > 0 && (SoundData.pitch_mod & (1 << J)) &&
                        ch->type != SOUND_NOISE)
                    {
                        int new_pitch_mod = SoundData.channels [J - 1].wave [I];
                        if (new_pitch_mod != prev_pitch_mod)
                        {
                            prev_pitch_mod = new_pitch_mod;
                            freq = (ch->frequency * (new_pitch_mod + 1)) >> 8;
                            K = (unsigned long) (((double) freq) *
                                                 FIXED_POINT / so.playback_rate);
                        }
                    }
                }
            }
stereo_exit: ;
        }
    }
}

inline void MixMono (int sample_count)
{
    int J;
    int K;
    int I;
    //int prev_pitch_mod;
	envelope_shift=ENVELOPE_SHIFT-g_VM;

    for (J = 0; J < NUM_CHANNELS; J++)
    {
        Channel *ch = &SoundData.channels[J];

        if (SoundData.pitch_mod & (1 << (J + 1)))
            memset (SoundData.channels[J].wave, 0,
                    sizeof (SoundData.channels[0].wave));

        // Mono sound
        int V;
        if (ch->state != SOUND_SILENT && (so.sound_switch & (1 << J)))
        {
            int freq;

            if (ch->type == SOUND_NOISE)
                freq = NoiseFreq [APU.DSP [APU_FLG] & 0x1f];
            else
                freq = ch->frequency;

            if (freq)
                K = (unsigned long) (((double) freq + 1) *
                                     FIXED_POINT / so.playback_rate);
            else
                K = 0;

            if (ch->needs_decode)
            {
                DecodeBlock (ch);
                ch->needs_decode = FALSE;
                ch->sample_pointer = 1;
                ch->sample = ch->block [0];
            }
            if (ch->type == SOUND_NOISE)
                V = (((so.noise_gen << 17) >> 17) * ch->left_vol_level) >> envelope_shift;
            else
                V = (ch->sample * ch->left_vol_level) >> envelope_shift;

            for (I = 0; I < sample_count; I++)
            {
                ch->env_error += ch->erate;
                if (ch->env_error >= FIXED_POINT)
                {
                    uint32 step = (ch->env_error >> FIXED_POINT_SHIFT) - 1;
                    if (step)
                    {
                        if (ch->direction < 0)
                        {
                            if (ch->envx - (int) step <= ch->envx_target)
                                ch->envx = ch->envx_target;
                            else
                                ch->envx -= step;
                        }
                        else
                        {
                            if (ch->envx + (int) step >= ch->envx_target)
                                ch->envx = ch->envx_target;
                            else
                                ch->envx += step;
                        }
                        ch->env_error &= FIXED_POINT_REMAINDER;
                    }
                    else
                        ch->env_error -= FIXED_POINT;

                    if (ch->envx == ch->envx_target || ch->envx < 0)
                    {
                        if (ch->envx <= 0)
                        {
                            ch->state = SOUND_SILENT;
                            S9xAPUSetEndOfSample (J);
                            APU.KeyedChannels &= ~(1 << J);
                            goto mono_exit;
                        }
                        else
                        switch (++ch->state)
                        {
                        case SOUND_DECAY:
                            if (ch->sustain_level != 7)
                            {
                                S9xSetEnvelopeRate (J, ch->decay_rate, -1,
                                                 (MAX_ENVELOPE_HEIGHT * (ch->sustain_level + 1)) >> 3);
                                break;
                            }
                            ch->state++;
                            // Fall ...
                        case SOUND_SUSTAIN:
                            S9xSetEnvelopeRate (J, ch->sustain_rate, -1, 0);
                            break;
                        case SOUND_RELEASE:
                            S9xSetEnvelopeRate (J, ch->release_rate, -1, 0);
                            break;
                        default:
                            ch->state = SOUND_SILENT;
                            S9xAPUSetEndOfSample (J);
                            APU.KeyedChannels &= ~(1 << J);
                            goto mono_exit;
                        }
                    }
                    else
                        ch->envx += ch->direction;

                    ch->left_vol_level = (ch->envx * ch->volume_left) >> VOLUME_SHIFT;
                }

                ch->count += K;
                if (ch->count >= FIXED_POINT)
                {
                    uint32 step = ch->count >> FIXED_POINT_SHIFT;
                    ch->count &= FIXED_POINT_REMAINDER;
                    ch->sample_pointer += step - 1;
                    if (ch->sample_pointer >= 16)
                    {
                        ch->sample_pointer -= 16;
                        if (ch->last_block)
                        {
                            S9xAPUSetEndX (J);
                            if (!ch->loop)
                            {
                                ch->state = SOUND_SILENT;
                                APU.KeyedChannels &= ~(1 << J);
                                S9xAPUSetEndOfSample (J);
                                goto mono_exit;
                            }
                            ch->last_block = FALSE;
                            uint8 *dir = IAPU.RAM + (((APU.DSP [APU_DIR] << 8) +
                                                    (ch->sample_number << 2)) & 0xffff);
                            ch->block_pointer = *(dir + 2) + (*(dir + 3) << 8);
                        }
                        DecodeBlock (ch);
                    }
                    ch->sample = ch->block [ch->sample_pointer++];
                    if (ch->type == SOUND_NOISE)
                    {
                        if ((so.noise_gen <<= 1) & 0x80000000L)
                            so.noise_gen ^= 0x0040001L;
                        V = (((so.noise_gen << 17) >> 17) * ch->left_vol_level) >> envelope_shift;
                    }
                    else
                        V = (ch->sample * ch->left_vol_level) >> envelope_shift;
                }
                MixBuffer [I] += V;
                ch->echo_buf_ptr [I] += V;
            }
mono_exit: ;
        }
    }
}

void S9xMixSamples (uint8 *buffer, int sample_count)
{
    int J;
    int K;
    int I;
    //int prev_pitch_mod;

    if (!so.mute_sound)
    {
        memset (MixBuffer, 0, sample_count * sizeof (MixBuffer [0]));
        if (SoundData.echo_enable)
            memset (EchoBuffer, 0, sample_count * sizeof (EchoBuffer [0]));

        if (so.stereo)
            MixStereo (sample_count);
        else
            MixMono (sample_count);
    }

    /* Mix and convert waveforms */
    if (so.sixteen_bit)
    {
        // 16-bit sound
        if (so.mute_sound)
        {
            for (J = 0; J < sample_count; J++)
                ((signed short *) buffer)[J] = 0;
        }
        else
        {
            if (SoundData.echo_enable)
            {
                if (so.stereo)
                {
                    // 16-bit stereo sound with echo enabled
                    for (J = 0; J < sample_count; J++)
                    {
                        I = MixBuffer [J] >> SIXTEEN_BIT_SHIFT;
                        K = EchoBuffer [J] >> SIXTEEN_BIT_SHIFT;

                        int E = 0;
                        if (SoundData.echo_buffer_size)
                            E = Echo [SoundData.echo_ptr];

                        Loop [(Z - 0) & 15] = E;
                        E =  E                    * FilterTaps [0];
                        E += Loop [(Z -  2) & 15] * FilterTaps [1];
                        E += Loop [(Z -  4) & 15] * FilterTaps [2];
                        E += Loop [(Z -  6) & 15] * FilterTaps [3];
                        E += Loop [(Z -  8) & 15] * FilterTaps [4];
                        E += Loop [(Z - 10) & 15] * FilterTaps [5];
                        E += Loop [(Z - 12) & 15] * FilterTaps [6];
                        E += Loop [(Z - 14) & 15] * FilterTaps [7];
                        E >>= 7;
                        Z++;
                        if (E > 32767)
                            E = 32767;
                        else
                        if (E < -32768)
                            E = -32768;

                        Echo [SoundData.echo_ptr] = ((E * SoundData.echo_feedback) >> VOLUME_SHIFT) + K;

                        if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
                            SoundData.echo_ptr = 0;

                        if (!(J & 1))
                        {
                            I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                            I += (E * SoundData.echo_volume_left) >> VOLUME_SHIFT;
                        }
                        else
                        {
                            I = (I * SoundData.master_volume_right) >> VOLUME_SHIFT;
                            I += (E * SoundData.echo_volume_right) >> VOLUME_SHIFT;
                        }
                        if (I > 32767)
                            I = 32767;
                        else
                        if (I < -32768)
                            I = -32768;
                        ((signed short *)buffer)[J] = I;
                    }
                }
                else
                {
                    // 16-bit mono sound with echo enabled
                    for (J = 0; J < sample_count; J++)
                    {
                        I = MixBuffer [J] >> SIXTEEN_BIT_SHIFT;
                        K = EchoBuffer [J] >> SIXTEEN_BIT_SHIFT;

                        int E = 0;
                        if (SoundData.echo_buffer_size)
                            E = Echo [SoundData.echo_ptr];

                        Loop [(Z - 0) & 7] = E;
                        E =  E                  * FilterTaps [0];
                        E += Loop [(Z - 1) & 7] * FilterTaps [1];
                        E += Loop [(Z - 2) & 7] * FilterTaps [2];
                        E += Loop [(Z - 3) & 7] * FilterTaps [3];
                        E += Loop [(Z - 4) & 7] * FilterTaps [4];
                        E += Loop [(Z - 5) & 7] * FilterTaps [5];
                        E += Loop [(Z - 6) & 7] * FilterTaps [6];
                        E += Loop [(Z - 7) & 7] * FilterTaps [7];
                        E >>= 7;
                        Z++;
                        if (E > 32767)
                            E = 32767;
                        else
                        if (E < -32768)
                            E = -32768;

                        Echo [SoundData.echo_ptr] = ((E * SoundData.echo_feedback) >> VOLUME_SHIFT) + K;

                        if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
                            SoundData.echo_ptr = 0;

                        I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                        I += (E * SoundData.echo_volume_left) >> VOLUME_SHIFT;

                        if (I > 32767)
                            I = 32767;
                        else
                        if (I < -32768)
                            I = -32768;
                        ((signed short *) buffer)[J] = I;
                    }
                }
            }
            else
            {
                // 16-bit mono or stereo sound, no echo
                for (J = 0; J < sample_count; J++)
                {
                    I = MixBuffer [J] >> SIXTEEN_BIT_SHIFT;

                    if (!(J & 1))
                        I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                    else
                        I = (I * SoundData.master_volume_right) >> VOLUME_SHIFT;

                    if (I > 32767)
                        I = 32767;
                    else
                    if (I < -32768)
                        I = -32768;
                    ((signed short *) buffer)[J] = I;
                }
            }
        }
    }
    else
    {
        // 8-bit sound
        if (so.mute_sound)
        {
            for (J = 0; J < sample_count; J++)
                buffer [J] = 128;
        }
        else
        {
            if (SoundData.echo_enable)
            {
                if (so.stereo)
                {
                    // 8-bit stereo sound with echo enabled
                    for (J = 0; J < sample_count; J++)
                    {
                        I = MixBuffer [J] >> EIGHT_BIT_SHIFT;
                        K = EchoBuffer [J] >> EIGHT_BIT_SHIFT;

                        int E = 0;
                        if (SoundData.echo_buffer_size)
                            E = Echo [SoundData.echo_ptr];

                        Loop [(Z - 0) & 15] = E;
                        E =  E                    * FilterTaps [0];
                        E += Loop [(Z -  2) & 15] * FilterTaps [1];
                        E += Loop [(Z -  4) & 15] * FilterTaps [2];
                        E += Loop [(Z -  6) & 15] * FilterTaps [3];
                        E += Loop [(Z -  8) & 15] * FilterTaps [4];
                        E += Loop [(Z - 10) & 15] * FilterTaps [5];
                        E += Loop [(Z - 12) & 15] * FilterTaps [6];
                        E += Loop [(Z - 14) & 15] * FilterTaps [7];
                        E >>= 7;
                        Z++;
                        if (E > 127)
                            E = 127;
                        else
                        if (E < -128)
                            E = -128;

                        Echo [SoundData.echo_ptr] = ((E * SoundData.echo_feedback) >> VOLUME_SHIFT) + K;

                        if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
                            SoundData.echo_ptr = 0;

                        if (!(J & 1))
                        {
                            I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                            I += (E * SoundData.echo_volume_left) >> VOLUME_SHIFT;
                        }
                        else
                        {
                            I = (I * SoundData.master_volume_right) >> VOLUME_SHIFT;
                            I += (E * SoundData.echo_volume_right) >> VOLUME_SHIFT;
                        }
                        if (I > 127)
                            I = 127;
                        else
                        if (I < -128)
                            I = -128;
                        buffer [J] = I + 128;
                    }
                }
                else
                {
                    // 8-bit mono sound with echo enabled
                    for (J = 0; J < sample_count; J++)
                    {
                        I = MixBuffer [J] >> EIGHT_BIT_SHIFT;
                        K = EchoBuffer [J] >> EIGHT_BIT_SHIFT;

                        int E = 0;
                        if (SoundData.echo_buffer_size)
                            E = Echo [SoundData.echo_ptr];

                        Loop [(Z - 0) & 7] = E;
                        E =  E                  * FilterTaps [0];
                        E += Loop [(Z - 1) & 7] * FilterTaps [1];
                        E += Loop [(Z - 2) & 7] * FilterTaps [2];
                        E += Loop [(Z - 3) & 7] * FilterTaps [3];
                        E += Loop [(Z - 4) & 7] * FilterTaps [4];
                        E += Loop [(Z - 5) & 7] * FilterTaps [5];
                        E += Loop [(Z - 6) & 7] * FilterTaps [6];
                        E += Loop [(Z - 7) & 7] * FilterTaps [7];
                        E >>= 7;
                        Z++;
                        if (E > 127)
                            E = 127;
                        else
                        if (E < -128)
                            E = -128;

                        Echo [SoundData.echo_ptr] = ((E * SoundData.echo_feedback) >> VOLUME_SHIFT) + K;

                        if ((SoundData.echo_ptr += 1) >= SoundData.echo_buffer_size)
                            SoundData.echo_ptr = 0;

                        I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                        I += (E * SoundData.echo_volume_left) >> VOLUME_SHIFT;

                        if (I > 127)
                            I = 127;
                        else
                        if (I < -128)
                            I = -128;
                        buffer [J] = I + 128;
                    }
                }
            }
            else
            {
                // 8-bit mono or stereo sound, no echo
                for (J = 0; J < sample_count; J++)
                {
                    I = MixBuffer [J] >> EIGHT_BIT_SHIFT;

                    if (!(J & 1))
                        I = (I * SoundData.master_volume_left) >> VOLUME_SHIFT;
                    else
                        I = (I * SoundData.master_volume_right) >> VOLUME_SHIFT;

                    if (I > 127)
                        I = 127;
                    else
                    if (I < -128)
                        I = -128;
                    buffer [J] = I + 128;
                }
            }
        }
    }
}

void S9xResetSound (){
	for (int i = 0; i < 8; i++){
		SoundData.channels[i].state = SOUND_SILENT;
		SoundData.channels[i].type = SOUND_SAMPLE;
		SoundData.channels[i].volume_left = 0;
		SoundData.channels[i].volume_right = 0;
		SoundData.channels[i].frequency = 0;
		SoundData.channels[i].count = 0;
		SoundData.channels[i].loop = FALSE;
		SoundData.channels[i].envx_target = 0;
		SoundData.channels[i].env_error = 0;
		SoundData.channels[i].erate = 0;
		SoundData.channels[i].envx = 0;
		SoundData.channels[i].left_vol_level = 0;
		SoundData.channels[i].right_vol_level = 0;
		SoundData.channels[i].direction = 0;
		SoundData.channels[i].attack_rate = 0;
		SoundData.channels[i].decay_rate = 0;
		SoundData.channels[i].sustain_rate = 0;
		SoundData.channels[i].release_rate = 0;
		SoundData.channels[i].sustain_level = 0;
		SoundData.channels[i].mode = SOUND_MODE_NONE;
	}
	SoundData.echo_ptr = 0;
	SoundData.echo_feedback = 0;
	SoundData.echo_buffer_size = 1;
	so.mute_sound = TRUE;
	so.noise_gen = 1;
	so.sound_switch = 255;
}

bool8 S9xInitSound (int mode, bool8 stereo, int buffer_size){
	so.sound_fd = -1;
	so.sound_switch = 255;
	so.playback_rate = 0;
	so.buffer_size = 0;
	so.stereo = FALSE;
	so.sixteen_bit = FALSE;
	so.encoded = FALSE;
    
	S9xResetSound ();
    
	//if(!(mode & 7))return (1);

	S9xSetSoundMute (TRUE);
	//if(!S9xOpenSoundDevice (mode, stereo, buffer_size)){
		//S9xMessage (S9X_ERROR, S9X_SOUND_DEVICE_OPEN_FAILED,
			//"Sound device open failed");
		return (0);
	//}
	//return (1);
}


void S9xSetSoundControl (int sound_switch)
{
    so.sound_switch = sound_switch;
}

void S9xPlaySample (int channel, int type, short volume_left,
                 short volume_right, int frequency, uint16 sample_number)
{
    Channel *ch = &SoundData.channels[channel];
    
    ch->state = SOUND_SILENT;
    ch->type = type;
    ch->volume_left = volume_left;
    ch->volume_right = volume_right;
    if (!so.stereo)
    {
        if (ABS (volume_right) > ABS (volume_left))
            ch->volume_left = ch->volume_right;
        else
            ch->volume_right = ch->volume_left;
    }
    ch->frequency = frequency;
    ch->sample_number = sample_number;
    ch->loop = FALSE;
    ch->needs_decode = TRUE;
    ch->last_block = FALSE;
    ch->previous [0] = ch->previous[1] = 0;
    uint8 *dir = IAPU.RAM + (((APU.DSP [APU_DIR] << 8) +
                            (sample_number << 2)) & 0xffff);
    ch->block_pointer = *dir + (*(dir + 1) << 8);
    ch->sample_pointer = 0;
    ch->env_error = 0;
    S9xSoundStartEnvelope (channel);
}

void S9xSoundStartEnvelope (int channel)
{
    Channel *ch = &SoundData.channels[channel];
    if (ch->attack_rate == 0)
    {
        if (ch->decay_rate == 0 || ch->sustain_level == 7)
        {
            if (ch->mode != SOUND_MODE_GAIN)
                ch->envx = (MAX_ENVELOPE_HEIGHT * (ch->sustain_level + 1)) >> 3;
            ch->left_vol_level = (ch->envx * ch->volume_left) >> VOLUME_SHIFT;
            ch->right_vol_level = (ch->envx * ch->volume_right) >> VOLUME_SHIFT;
            S9xSetEnvelopeRate (channel, ch->sustain_rate, -1, 0);
            ch->state = SOUND_SUSTAIN;
        }
        else
        {
            ch->envx = MAX_ENVELOPE_HEIGHT;
            ch->left_vol_level = (ch->envx * ch->volume_left) >> VOLUME_SHIFT;
            ch->right_vol_level = (ch->envx * ch->volume_right) >> VOLUME_SHIFT;
            S9xSetEnvelopeRate (channel, ch->decay_rate, -1, 
                             (MAX_ENVELOPE_HEIGHT * (ch->sustain_level + 1)) >> 3);
            ch->state = SOUND_DECAY;
        }
    }
    else
    {
        ch->envx = 0;
        ch->left_vol_level = 0;
        ch->right_vol_level = 0;
        S9xSetEnvelopeRate (channel, ch->attack_rate, 1, MAX_ENVELOPE_HEIGHT);
        ch->state = SOUND_ATTACK;
    }
}

