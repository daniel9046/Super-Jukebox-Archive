#ifndef _PORT_H_
#define _PORT_H_

struct SSetDSP{
    unsigned char reg;
    unsigned char data;
};

struct SDSPBuffer{
    unsigned long length;
    SSetDSP *setdsp;
};

struct SDSPStream{
    unsigned long length;
    SDSPBuffer *dspbuffer;
};

extern SDSPStream dspstream;
extern unsigned long curdspbuffer;

BOOL LoadDSPStream(char *fname);

extern unsigned long count[256];

#ifndef snes9x_types_defined
 #define snes9x_types_defined
 typedef unsigned char uint8;
 typedef unsigned short uint16;
 typedef unsigned long uint32;
 typedef unsigned char bool8;
 typedef signed char int8;
 typedef short int16;
 typedef int int32;
#endif

#include <mmsystem.h> //link winmm.lib

extern BOOL playing;

MMRESULT OpenSoundDevice (WAVEFORMATEX *format);
void CloseSoundDevice();

BOOL LoadUnknown(char *fname);

extern int g_VM;
extern char *filename;

#endif