// PlayerDialog.cpp: implementation of the CPlayerDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayerDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayerDialog::CPlayerDialog()
{
	m_uID=IDD_PLAYER;
}

CPlayerDialog::~CPlayerDialog()
{

}

void CPlayerDialog::InitMembers()
{
	m_nSampleRate=app.m_Reg.m_dwSampleRate;
	INT nInterpols[]={INT_NONE,INT_LINEAR,INT_CUBIC,INT_GAUSSIAN};
	for(INT i=0;i<sizeof(nInterpols)/sizeof(nInterpols[0]);i++)
		if(app.m_Reg.m_dwInterpolation==nInterpols[i])
		{
			m_nInterpolation=i;
			break;
		}
	m_nMixingMode=app.m_Reg.m_dwMixingMode;
	m_nPreamp=app.m_Reg.m_dwPreamp;
	m_nBufferLength=app.m_Reg.m_dwBufferLength;
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	for(i=0;i<sizeof(nThreadPriorities)/sizeof(nThreadPriorities[0]);i++)
		if(app.m_Reg.m_dwMixingThreadPriority==nThreadPriorities[i])
		{
			m_nThreadPriority=i;
			break;
		}
	m_nSampleSize=app.m_Reg.m_dwSampleSize;
	m_bChannels=(app.m_Reg.m_dwChannels==2);
	m_dwAPR=app.m_Reg.m_dwAPR;
	m_bUseLPF=app.m_Reg.m_bUseLPF;
	m_bUseOldBRE=app.m_Reg.m_bUseOldBRE;
	m_bSurroundSound=app.m_Reg.m_bSurroundSound;
	m_bReverseStereo=app.m_Reg.m_bReverseStereo;

	m_bOutputToWAV=app.m_Reg.m_bOutputToWAV;
	m_bOutputWithSound=app.m_Reg.m_bOutputWithSound;
	strncpy(m_szWAVOutputDirectory,app.m_Reg.m_szWAVOutputDirectory,sizeof(m_szWAVOutputDirectory));
}

BOOL CPlayerDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	UINT uSampleRates[]={8000,11025,16000,22050,32000,44100,48000,64000,72000,88200,96000};
	TCHAR szBuf[100];
	for(INT i=0;i<sizeof(uSampleRates)/sizeof(uSampleRates[0]);i++)
	{
		itoa(uSampleRates[i],szBuf,10);
		SendDlgItemMessage(m_hWnd,IDC_COMBO_SAMPLE_RATE,CB_ADDSTRING,0,(LPARAM)szBuf);
	}
	itoa(m_nSampleRate,szBuf,10);
	SetDlgItemText(m_hWnd,IDC_COMBO_SAMPLE_RATE,szBuf);
	LPCTSTR lpszInterpolations[]={"None","Linear","Cubic","Gaussian"};
	for(i=0;i<sizeof(lpszInterpolations)/sizeof(lpszInterpolations[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_INTERPOLATION,CB_ADDSTRING,0,(LPARAM)lpszInterpolations[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_INTERPOLATION,CB_SETCURSEL,m_nInterpolation,0);
	LPCTSTR lpszMixers[]={"None","386","MMX","3DNow!","SSE"};
	for(i=0;i<sizeof(lpszMixers)/sizeof(lpszMixers[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_MIXING_MODE,CB_ADDSTRING,0,(LPARAM)lpszMixers[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_MIXING_MODE,CB_SETCURSEL,m_nMixingMode,0);
	SetDlgItemInt(m_hWnd,IDC_EDIT_PREAMP,m_nPreamp,TRUE);
	SendDlgItemMessage(m_hWnd,IDC_SPIN_PREAMP,UDM_SETRANGE32,0,MaxAmp);
	SetDlgItemInt(m_hWnd,IDC_EDIT_BUFFER_LENGTH,m_nBufferLength,TRUE);
	SendDlgItemMessage(m_hWnd,IDC_SPIN_BUFFER_LENGTH,UDM_SETRANGE32,100,10000);
	LPCTSTR lpszPriorities[]={"Idle","Lowest","Below Normal","Normal","Above Normal","Highest","Time Critical"};
	for(i=0;i<sizeof(lpszPriorities)/sizeof(lpszPriorities[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_PRIORITY_LEVEL,CB_ADDSTRING,0,(LPARAM)lpszPriorities[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_PRIORITY_LEVEL,CB_SETCURSEL,m_nThreadPriority,0);

	CheckRadioButton(m_hWnd,IDC_RADIO_8BIT,IDC_RADIO_32BIT,m_nSampleSize==8?IDC_RADIO_8BIT:m_nSampleSize==16?IDC_RADIO_16BIT:IDC_RADIO_32BIT);
	CheckRadioButton(m_hWnd,IDC_RADIO_MONO,IDC_RADIO_STEREO,m_bChannels?IDC_RADIO_STEREO:IDC_RADIO_MONO);
	CheckRadioButton(m_hWnd,IDC_RADIO_APR_OFF,IDC_RADIO_APR_INC,m_dwAPR?m_dwAPR==2?IDC_RADIO_APR_INC:IDC_RADIO_APR_ON:IDC_RADIO_APR_OFF);
	CheckDlgButton(m_hWnd,IDC_CHECK_USE_LOW_PASS_FILTER,m_bUseLPF?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_USE_OLD_SCHOOL_BRE,m_bUseOldBRE?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_SURROUNDSOUND,m_bSurroundSound?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_REVERSE_STEREO,m_bReverseStereo?BST_CHECKED:BST_UNCHECKED);

	CheckDlgButton(m_hWnd,IDC_CHECK_OUTPUTTOWAV,m_bOutputToWAV?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_OUTPUTWITHSOUND,m_bOutputWithSound?BST_CHECKED:BST_UNCHECKED);
	SendDlgItemMessage(m_hWnd,IDC_EDIT_WAVDIR,EM_LIMITTEXT,sizeof(m_szWAVOutputDirectory)-1,0);
	SetDlgItemText(m_hWnd,IDC_EDIT_WAVDIR,m_szWAVOutputDirectory);

	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_OUTPUTWITHSOUND),m_bOutputToWAV);
	EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_WAVDIR),m_bOutputToWAV);
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSEFORWAVDIR),m_bOutputToWAV);
	
	return FALSE;
}

BOOL CPlayerDialog::Validate()
{
	TCHAR szBuf[100];
	GetDlgItemText(m_hWnd,IDC_COMBO_SAMPLE_RATE,szBuf,sizeof(szBuf));
	m_nSampleRate=atoi(szBuf);
	m_nInterpolation=SendDlgItemMessage(m_hWnd,IDC_COMBO_INTERPOLATION,CB_GETCURSEL,0,0);
	m_nMixingMode=SendDlgItemMessage(m_hWnd,IDC_COMBO_MIXING_MODE,CB_GETCURSEL,0,0);
	m_nPreamp=GetDlgItemInt(m_hWnd,IDC_EDIT_PREAMP,NULL,TRUE);
	m_nBufferLength=GetDlgItemInt(m_hWnd,IDC_EDIT_BUFFER_LENGTH,NULL,TRUE);
	m_nThreadPriority=SendDlgItemMessage(m_hWnd,IDC_COMBO_PRIORITY_LEVEL,CB_GETCURSEL,0,0);
	m_nSampleSize=IsDlgButtonChecked(m_hWnd,IDC_RADIO_8BIT)==BST_CHECKED?8:IsDlgButtonChecked(m_hWnd,IDC_RADIO_16BIT)==BST_CHECKED?16:32;
	m_bChannels=IsDlgButtonChecked(m_hWnd,IDC_RADIO_STEREO)==BST_CHECKED;
	IsDlgButtonChecked(m_hWnd,IDC_RADIO_APR_INC)==BST_CHECKED?m_dwAPR=2:IsDlgButtonChecked(m_hWnd,IDC_RADIO_APR_ON)==BST_CHECKED?m_dwAPR=1:m_dwAPR=0;
	m_bUseLPF=IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_LOW_PASS_FILTER)==BST_CHECKED;
	m_bUseOldBRE=IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_OLD_SCHOOL_BRE)==BST_CHECKED;
	m_bSurroundSound=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SURROUNDSOUND)==BST_CHECKED;
	m_bReverseStereo=IsDlgButtonChecked(m_hWnd,IDC_CHECK_REVERSE_STEREO)==BST_CHECKED;
	GetDlgItemText(m_hWnd,IDC_EDIT_WAVDIR,m_szWAVOutputDirectory,sizeof(m_szWAVOutputDirectory));

	if(m_nSampleRate<8000||m_nSampleRate>96000)
	{
		TCHAR szBuf[100];
		sprintf(szBuf,"You must enter a value between 8000 and 96000 for the 'Sample rate' field.");
		MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONWARNING);
		SetFocus(GetDlgItem(m_hWnd,IDC_COMBO_SAMPLE_RATE));
		return FALSE;
	}
	if(m_nPreamp<0||m_nPreamp>MaxAmp)
	{
		TCHAR szBuf[100];
		sprintf(szBuf,"You must enter a value between 0 and %d for the 'Preamp' field.",MaxAmp);
		MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONWARNING);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_PREAMP));
		return FALSE;
	}
	if(m_nBufferLength<100||m_nBufferLength>10000)
	{
		TCHAR szBuf[100];
		sprintf(szBuf,"You must enter a value between 100 and 10000 for the 'Buffer length' field.");
		MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONWARNING);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_BUFFER_LENGTH));
		return FALSE;
	}
	if(m_bOutputToWAV)
	{
		if(!(strlen(m_szWAVOutputDirectory)<=3&&m_szWAVOutputDirectory[1]==':'))
		{
			if(m_szWAVOutputDirectory[strlen(m_szWAVOutputDirectory)-1]=='\\')
				m_szWAVOutputDirectory[strlen(m_szWAVOutputDirectory)-1]='\0';
			if(strlen(m_szWAVOutputDirectory)>2)
			{
				HANDLE hFindFile;
				WIN32_FIND_DATA fd;
				if((hFindFile=FindFirstFile(m_szWAVOutputDirectory,&fd))==INVALID_HANDLE_VALUE)
				{
					DisplayError(m_hWnd,FALSE,"The specified WAV output directory does not exist. Enter a directory that exists.");
					SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_WAVDIR));
					return FALSE;
				}
				FindClose(hFindFile);
			}
		}
	}

	return TRUE;
}

BOOL CPlayerDialog::CarryOutActions()
{
	BOOL bChanged=FALSE;

	if(app.m_Reg.m_dwSampleRate!=m_nSampleRate)
	{
		app.m_Reg.m_dwSampleRate=m_nSampleRate;
		bChanged=TRUE;
	}
	INT nInterpols[]={INT_NONE,INT_LINEAR,INT_CUBIC,INT_GAUSSIAN};
	if(app.m_Reg.m_dwInterpolation!=nInterpols[m_nInterpolation])
	{
		app.m_Reg.m_dwInterpolation=nInterpols[m_nInterpolation];
		bChanged=TRUE;
	}
	if(app.m_Reg.m_dwMixingMode!=m_nMixingMode)
	{
		app.m_Reg.m_dwMixingMode=m_nMixingMode;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_dwPreamp!=m_nPreamp)
	{
		app.m_Reg.m_dwPreamp=m_nPreamp;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_dwBufferLength!=m_nBufferLength)
	{
		app.m_Reg.m_dwBufferLength=m_nBufferLength;
		bChanged=TRUE;
	}
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	if(app.m_Reg.m_dwMixingThreadPriority!=nThreadPriorities[m_nThreadPriority])
	{
		app.m_Reg.m_dwMixingThreadPriority=nThreadPriorities[m_nThreadPriority];
		app.m_Player.SetThreadPriority(app.m_Reg.m_dwMixingThreadPriority);
	}
	if(app.m_Reg.m_dwSampleSize!=m_nSampleSize)
	{
		app.m_Reg.m_dwSampleSize=m_nSampleSize;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_dwChannels!=(m_bChannels?2:1))
	{
		app.m_Reg.m_dwChannels=m_bChannels?2:1;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_dwAPR!=m_dwAPR)
	{
		app.m_Reg.m_dwAPR=m_dwAPR;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bUseLPF!=m_bUseLPF)
	{
		app.m_Reg.m_bUseLPF=m_bUseLPF;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bUseOldBRE!=m_bUseOldBRE)
	{
		app.m_Reg.m_bUseOldBRE=m_bUseOldBRE;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bSurroundSound!=m_bSurroundSound)
	{
		app.m_Reg.m_bSurroundSound=m_bSurroundSound;
		bChanged=TRUE;
	}
	if(app.m_Reg.m_bReverseStereo!=m_bReverseStereo)
	{
		app.m_Reg.m_bReverseStereo=m_bReverseStereo;
		bChanged=TRUE;
	}
	BOOL bWavChanged=FALSE;
	if(app.m_Reg.m_bOutputToWAV!=m_bOutputToWAV)
	{
		app.m_Reg.m_bOutputToWAV=m_bOutputToWAV;		
		bWavChanged=TRUE;
	}
	if(app.m_Reg.m_bOutputWithSound!=m_bOutputWithSound)
	{
		app.m_Reg.m_bOutputWithSound=m_bOutputWithSound;
		bWavChanged=TRUE;
	}
	if(stricmp(app.m_Reg.m_szWAVOutputDirectory,m_szWAVOutputDirectory))
	{
		strncpy(app.m_Reg.m_szWAVOutputDirectory,m_szWAVOutputDirectory,sizeof(app.m_Reg.m_szWAVOutputDirectory));
		bWavChanged=TRUE;
	}

	if(bChanged||bWavChanged)
	{
		BOOL b=app.IsCallInternal(TRUE);
		app.m_MainWindow.m_VisWindow.Stop();
		app.m_MainWindow.m_ActiveWallpaper.Stop();
		app.IsCallInternal(b);
		if(bWavChanged)
		{
			app.m_MainWindow.m_ToolBar.CheckButton(IDC_BUTTON_WAVDUMP,app.m_Reg.m_bOutputToWAV);
			BOOL b=app.IsCallInternal(TRUE);
			app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_WAVDUMP);
			app.IsCallInternal(b);
		}
		if(bChanged)
		{
			app.m_Player.SetPreAmp(app.m_Reg.m_dwPreamp);
			UEMATSUCONFIG cfg;
			app.m_Player.GetConfiguration(cfg);
			cfg.uSampleRate=app.m_Reg.m_dwSampleRate;
			cfg.uInterpolation=app.m_Reg.m_dwInterpolation;
			INT nMixers[]={MIX_NONE,MIX_80386,MIX_MMX,MIX_3DNOW,MIX_SSE};
			cfg.uMixingRoutine=nMixers[app.m_Reg.m_dwMixingMode];
			cfg.uBufferLength=app.m_Reg.m_dwBufferLength;
			cfg.uBitsPerSample=app.m_Reg.m_dwSampleSize;
			cfg.uChannels=app.m_Reg.m_dwChannels;
			cfg.uAPR=app.m_Reg.m_dwAPR;
			cfg.fMiscOptions=(app.m_Reg.m_bUseLPF?DSP_LOW:0)|(app.m_Reg.m_bUseOldBRE?DSP_OLDSMP:0)|(app.m_Reg.m_bSurroundSound?DSP_SURND:0)|(app.m_Reg.m_bReverseStereo?DSP_REVERSE:0);
			if(!app.m_Player.SetConfiguration(cfg))
			{
				if(!app.m_Player.IsStopped())
					app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_STOP);
				DisplayError(app.m_MainWindow.m_hWnd,FALSE,"Failed to set the player configuration.");
			}
		}
		if(app.m_Reg.m_bShowVisWindow)
			app.m_MainWindow.m_VisWindow.Start();
		if(app.m_Reg.m_bUseActiveWallpaper)
			app.m_MainWindow.m_ActiveWallpaper.Start();
	}
	return TRUE;
}

LRESULT CPlayerDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_CHECK_OUTPUTTOWAV:
		case IDC_CHECK_OUTPUTWITHSOUND:
			m_bOutputToWAV=IsDlgButtonChecked(m_hWnd,IDC_CHECK_OUTPUTTOWAV)==BST_CHECKED;
			m_bOutputWithSound=IsDlgButtonChecked(m_hWnd,IDC_CHECK_OUTPUTWITHSOUND)==BST_CHECKED;
			EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_OUTPUTWITHSOUND),m_bOutputToWAV);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_WAVDIR),m_bOutputToWAV);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSEFORWAVDIR),m_bOutputToWAV);
			break;
		case IDC_BUTTON_BROWSEFORWAVDIR:
			BROWSEINFO bi;
			LPITEMIDLIST lpIDL;
			bi.hwndOwner=m_hWnd;
			bi.pidlRoot=NULL;
			bi.pszDisplayName=m_szWAVOutputDirectory;
			bi.lpszTitle="Select output directory for WAV files...";
			bi.ulFlags=BIF_EDITBOX|BIF_RETURNONLYFSDIRS;
			bi.lpfn=NULL;
			bi.lParam=0;
			if((lpIDL=SHBrowseForFolder(&bi))!=NULL)
			{
				SHGetPathFromIDList(lpIDL,m_szWAVOutputDirectory);
				SetDlgItemText(m_hWnd,IDC_EDIT_WAVDIR,m_szWAVOutputDirectory);
				LPMALLOC pMalloc=NULL;
				SHGetMalloc(&pMalloc);
				pMalloc->Free(lpIDL);
				pMalloc->Release();
			}
			break;
		}
		break;
	}
	return 0;
}
