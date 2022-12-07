#include "Main.h"

unsigned long count[256]={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

struct SAPU APU = {
    0
};

struct SIAPU IAPU = {
    0
};

struct SAPURegisters APURegisters = {
    0
};

/*struct SSettings Settings = {
    0
};*/

uint8 W1 = 0, W2 = 0;
uint8 Work8 = 0;
uint16 Work16 = 0;
uint32 Work32 = 0;
signed char Int8 = 0;
short Int16 = 0;
long Int32 = 0;

SoundStatus so = {0};
SSoundData SoundData = {0};
int Echo [15360 + 14] = {0};
int DummyEchoBuffer [MAX_BUFFER_SIZE] = {0};
int MixBuffer [MAX_BUFFER_SIZE] = {0};
int EchoBuffer [MAX_BUFFER_SIZE] = {0};
int FilterTaps [8] = {0};
unsigned long Z = 0;
int Loop [16] = {0};

long FilterValues[4][2] =
{
    {0, 0},
    {240, 0},
    {488, -240},
    {460, -208}
};

int NoiseFreq [32] = {
    0, 16, 32, 25, 31, 42, 50, 63, 84, 100, 125, 167, 200, 250, 333,
    400, 500, 667, 800, 1000, 1300, 1600, 2000, 2700, 3200, 4000,
    5300, 6400, 8000, 10700, 16000, 32000
};

uint8 APUROM [64] =
{
    0xCD,0xEF,0xBD,0xE8,0x00,0xC6,0x1D,0xD0,0xFC,0x8F,0xAA,0xF4,0x8F,
    0xBB,0xF5,0x78,0xCC,0xF4,0xD0,0xFB,0x2F,0x19,0xEB,0xF4,0xD0,0xFC,
    0x7E,0xF4,0xD0,0x0B,0xE4,0xF5,0xCB,0xF4,0xD7,0x00,0xFC,0xD0,0xF3,
    0xAB,0x01,0x10,0xEF,0x7E,0xF4,0x10,0xEB,0xBA,0xF6,0xDA,0x00,0xBA,
    0xF4,0xC4,0xF4,0xDD,0x5D,0xD0,0xDB,0x1F,0x00,0x00,0xC0,0xFF
};

