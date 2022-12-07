#ifndef _SPC700EMU_H_
#define _SPC700EMU_H_

#define SPC_APU 1
#define SPC_SND 2

#ifdef __cplusplus
extern "C"{
#endif

DWORD WINAPI InitSPC(DWORD,DWORD,DWORD,DWORD,DWORD);
void WINAPI DeinitSPC(DWORD);
DWORD WINAPI LoadSPC(LPCSTR);
void WINAPI RestoreSPC();
void WINAPI PlaySPC();
void WINAPI PauseSPC();
void WINAPI StopSPC();

#ifdef __cplusplus
}
#endif
#endif
