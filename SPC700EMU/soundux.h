#ifndef _SOUND_H_
#define _SOUND_H_

enum { SOUND_SAMPLE = 0, SOUND_NOISE };
enum { SOUND_SILENT, SOUND_ATTACK, SOUND_DECAY, SOUND_SUSTAIN, SOUND_RELEASE };
enum { SOUND_MODE_NONE, SOUND_MODE_ADSR, SOUND_MODE_GAIN,
		SOUND_MODE_INCREASE, SOUND_MODE_DECREASE };

#define MAX_ENVELOPE_HEIGHT 127
#define ENVELOPE_SHIFT 7
#define MAX_VOLUME 127
#define VOLUME_SHIFT 7

#define NUM_CHANNELS    8       /* Number of channels, 8                  */
#define MAX_BUFFER_SIZE (1024 * 16)

#define SOUND_BUFS      3       /* Number of buffers, >= 2                */

struct SoundStatus{
	int sound_fd;
	int sound_switch;
	int playback_rate;
	int buffer_size;
	int noise_gen;
	bool8 mute_sound;
	int stereo;
	bool8 sixteen_bit;
	bool8 encoded;
};

extern SoundStatus so;

struct Channel{
    int state;
    int type;
    short volume_left;
    short volume_right;
    int frequency;
    int count;
    signed short wave [MAX_BUFFER_SIZE];
    bool8 loop;
    int envx;
    short left_vol_level;
    short right_vol_level;
    short envx_target;
    unsigned long int env_error;
    unsigned long erate;
    int direction;
    unsigned long attack_rate;
    unsigned long decay_rate;
    unsigned long sustain_rate;
    unsigned long release_rate;
    unsigned long sustain_level;
    signed short sample;
    signed short decoded [16];
    signed short previous [2];
    signed short *block;
    uint16 sample_number;
    bool8 last_block;
    bool8 needs_decode;
    uint32 block_pointer;
    uint32 sample_pointer;
    int *echo_buf_ptr;
    int mode;
    // Just incase they are needed in the future, for snapshot compatibility.
    uint32 dummy [8];
};

struct SSoundData{
    short master_volume_left;
    short master_volume_right;
    short echo_volume_left;
    short echo_volume_right;
    int echo_enable;
    int echo_feedback;
    int echo_ptr;
    int echo_buffer_size;
    int echo_write_enabled;
    int echo_channel_enable;
    int pitch_mod;
    // Just incase they are needed in the future, for snapshot compatibility.
    uint32 dummy [3];
    Channel channels [NUM_CHANNELS];
};

extern SSoundData SoundData;

void S9xSetSoundVolume (int channel, short volume_left, short volume_right);
void S9xSetSoundFrequency (int channel, int hertz);
void S9xSetSoundType (int channel, int type_of_sound);
void S9xSetMasterVolume (short master_volume_left, short master_volume_right);
void S9xSetEchoVolume (short echo_volume_left, short echo_volume_right);
void S9xSetSoundControl (int sound_switch);
bool8 S9xSetSoundMute (bool8 mute);
void S9xSetEnvelopeHeight (int channel, int height);
void S9xSetSoundADSR (int channel, int attack, int decay, int sustain,
                      int sustain_level, int release);
void S9xSetSoundKeyOff (int channel);
void S9xSetSoundDecayMode (int channel);
void S9xSetSoundAttachMode (int channel);
int S9xSetSoundMode (int channel, int mode);
void S9xSoundStartEnvelope (int channel);
void S9xSetSoundSample (int channel, uint16 sample_number);
void S9xSetEchoFeedback (int echo_feedback);
void S9xSetEchoEnable (uint8 byte);
void S9xSetEchoDelay (int byte);
void S9xSetEchoWriteEnable (uint8 byte);
void S9xSetFilterCoefficient (int tap, int value);
void S9xSetFrequencyModulationEnable (uint8 byte);
void S9xSetEnvelopeRate (int channel, unsigned long rate, int direction,
                         int target);

int S9xGetEnvelopeHeight (int channel);
void S9xResetSound ();
void S9xFixSoundAfterSnapshotLoad ();

void S9xPlaySample (int channel, int type, short volume_left, short volume_right,
                 int frequency, uint16 sample_number);
void S9xMixSamples (uint8 *buffer, int sample_count);

#endif
