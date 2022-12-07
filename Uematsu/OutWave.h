#ifndef _OUT_WAVE_H_
#define _OUT_WAVE_H_

int OW_Open(int SampleRate, int NumChannels, int BitsPerSmp, int BufferLenms, int VisRate);
void OW_Close();
int OW_Write(char *Buf, int Len);
DWORD OW_CanWrite();
int OW_IsPlaying();
int OW_Pause(int Pause);
void OW_Flush();
int OW_IsOpen();

#endif