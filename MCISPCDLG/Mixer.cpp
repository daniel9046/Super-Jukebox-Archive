#include <windows.h>
#include <mmsystem.h>

UINT GetMixerIDForWaveMapper()
{
	HWAVEOUT hWaveOut;
	WAVEFORMATEX waveFormatEx;
	UINT uMixerID;

	waveFormatEx.wFormatTag=WAVE_FORMAT_PCM;
	waveFormatEx.nChannels=1;
	waveFormatEx.wBitsPerSample=8;
	waveFormatEx.nSamplesPerSec=11025;
	waveFormatEx.nBlockAlign=waveFormatEx.nChannels*waveFormatEx.wBitsPerSample/8;
	waveFormatEx.nAvgBytesPerSec=waveFormatEx.nSamplesPerSec*waveFormatEx.nBlockAlign;
	waveFormatEx.cbSize=0;
	if(waveOutOpen(&hWaveOut,WAVE_MAPPER,&waveFormatEx,0,0,0)!=MMSYSERR_NOERROR)return (UINT)-1;
	if(mixerGetID((HMIXEROBJ)hWaveOut,&uMixerID,MIXER_OBJECTF_HWAVEOUT)!=MMSYSERR_NOERROR)return (UINT)-1;
	waveOutClose(hWaveOut);
	return uMixerID;
}

DWORD GetMixerControlID(UINT uMixerID,DWORD dwComponentType,DWORD dwControlType)
{
	MIXERLINE mxrLine;
	MIXERLINECONTROLS mxrLineControls;
	MIXERCONTROL mxrControl;

	if(mixerGetNumDevs()<1)return (DWORD)-1;

	mxrLine.dwComponentType=dwComponentType;
	mxrLine.cbStruct=sizeof(mxrLine);
	if(mixerGetLineInfo((HMIXEROBJ)uMixerID,&mxrLine,MIXER_GETLINEINFOF_COMPONENTTYPE)!=MMSYSERR_NOERROR)return (DWORD)-1;
	mxrLineControls.dwLineID=mxrLine.dwLineID;
	mxrLineControls.cbmxctrl=sizeof(MIXERCONTROL);
	mxrLineControls.pamxctrl=&mxrControl;
	mxrLineControls.dwControlType=dwControlType;
	mxrLineControls.cbStruct=sizeof(mxrLineControls);
	if(mixerGetLineControls((HMIXEROBJ)uMixerID,&mxrLineControls,MIXER_GETLINECONTROLSF_ONEBYTYPE)!=MMSYSERR_NOERROR)return (DWORD)-1;
	return mxrControl.dwControlID;
}

DWORD GetMixerControlValue(UINT uMixerID,DWORD dwControlID)
{
	MIXERCONTROLDETAILS mxrCntrlDtls;
	MIXERCONTROLDETAILS_UNSIGNED mxrCntrlDtlsUnsigned[10];

	mxrCntrlDtls.cbStruct=sizeof(mxrCntrlDtls);
	mxrCntrlDtls.dwControlID=dwControlID;
	mxrCntrlDtls.cChannels=2;
	mxrCntrlDtls.hwndOwner=NULL;
	mxrCntrlDtls.cMultipleItems=0;
	mxrCntrlDtls.cbDetails=sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxrCntrlDtls.paDetails=mxrCntrlDtlsUnsigned;
	if(mixerGetControlDetails((HMIXEROBJ)uMixerID,&mxrCntrlDtls,MIXER_GETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)return (DWORD)-1;

	return mxrCntrlDtlsUnsigned[0].dwValue;
}

DWORD SetMixerControlValue(UINT uMixerID,DWORD dwControlID,DWORD dwControlValue)
{
	MIXERCONTROLDETAILS mxrCntrlDtls;
	MIXERCONTROLDETAILS_UNSIGNED mxrCntrlDtlsUnsigned[10];

	mxrCntrlDtlsUnsigned[0].dwValue=dwControlValue;
	mxrCntrlDtls.cbStruct=sizeof(mxrCntrlDtls);
	mxrCntrlDtls.dwControlID=dwControlID;
	mxrCntrlDtls.cChannels=1;
	mxrCntrlDtls.hwndOwner=NULL;
	mxrCntrlDtls.cMultipleItems=0;
	mxrCntrlDtls.cbDetails=sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mxrCntrlDtls.paDetails=mxrCntrlDtlsUnsigned;
	if(mixerSetControlDetails((HMIXEROBJ)uMixerID,&mxrCntrlDtls,MIXER_SETCONTROLDETAILSF_VALUE)!=MMSYSERR_NOERROR)return (DWORD)-1;

	return 0;
}
