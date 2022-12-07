#ifndef _MCISPC_H_
#define _MCISPC_H_

#include <mmsystem.h>

#define MCI_CONFIG (MCI_USER_MESSAGES)

typedef struct{
	DWORD dwCallback;
	DWORD dwEngine;
	DWORD dwSampleRate;
	DWORD dwSampleSize;
	DWORD dwChannels;
	DWORD dwVolumeMultiplier;
	DWORD dwPriorityClass;
	DWORD dwSongLength;
}MCI_CONFIG_PARMS,*PMCI_CONFIG_PARMS,FAR *LPMCI_CONFIG_PARMS;

#endif
