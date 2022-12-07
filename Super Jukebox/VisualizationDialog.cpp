// VisualizationDialog.cpp: implementation of the CVisualizationDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisualizationDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

BOOL CVisualizationDialog::m_bActive;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisualizationDialog::CVisualizationDialog()
{
	m_uID=IDD_VISUALIZATION;
	memset(m_CustomColors,0,sizeof(m_CustomColors));
}

CVisualizationDialog::~CVisualizationDialog()
{

}

void CVisualizationDialog::InitMembers()
{
	m_bActive=app.m_Reg.m_bShowVisWindow;
	m_bShowVisLabels=app.m_Reg.m_bShowVisLabels;
	m_nMode=app.m_Reg.m_dwVisMode;
	m_nSpectrumSize=app.m_Reg.m_dwSpectrumSize;
	m_nRate=app.m_Reg.m_dwVisRate;
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	for(INT i=0;i<sizeof(nThreadPriorities)/sizeof(nThreadPriorities[0]);i++)
		if(app.m_Reg.m_dwVisThreadPriority==nThreadPriorities[i])
		{
			m_nPriority=i;
			break;
		}
	m_nVisItem=0;
}

BOOL CVisualizationDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	CheckDlgButton(m_hWnd,IDC_CHECK_SHOW_VISUALIZATION,m_bActive?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_SHOWVISLABELS,m_bShowVisLabels?BST_CHECKED:BST_UNCHECKED);
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_SHOWVISLABELS),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VIS_MODE),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_SPECTRUM_SIZE),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_VIS_RATE),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_VIS_RATE),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VIS_PRIORITY),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VISCOLSCHEME),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VISCOLITEM),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL1),m_bActive);
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL2),m_bActive);

	SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_MODE,CB_ADDSTRING,0,(LPARAM)"Oscilloscope");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_MODE,CB_ADDSTRING,0,(LPARAM)"Spectrometer");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_MODE,CB_SETCURSEL,m_nMode,0);

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE,TBM_SETRANGE,FALSE,MAKELONG(1,100));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE,TBM_SETPOS,TRUE,m_nSpectrumSize);
	SetDlgItemInt(m_hWnd,IDC_STATIC_SPECTRUM_SIZE,m_nSpectrumSize,TRUE);

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_VIS_RATE,TBM_SETRANGE,FALSE,MAKELONG(10,120));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_VIS_RATE,TBM_SETPOS,TRUE,m_nRate);
	SetDlgItemInt(m_hWnd,IDC_STATIC_VIS_RATE,m_nRate,TRUE);

	LPCTSTR lpszPriorities[]={"Idle","Lowest","Below Normal","Normal","Above Normal","Highest","Time Critical"};
	for(INT i=0;i<sizeof(lpszPriorities)/sizeof(lpszPriorities[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_PRIORITY,CB_ADDSTRING,0,(LPARAM)lpszPriorities[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_PRIORITY,CB_SETCURSEL,m_nPriority,0);

	for(i=0;i<4;i++)
	{
		TCHAR szBuf[20];
		sprintf(szBuf,"Scheme %d",i);
		SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLSCHEME,CB_ADDSTRING,0,(LPARAM)szBuf);
	}
	SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLSCHEME,CB_SETCURSEL,app.m_Reg.m_dwVisColorScheme,0);
	LPCTSTR lpszVisItems[]={"Background/Labels","Channel","Voice","Key On","Echo","Pitch Mod.","Noise","Surround","Envelope","Pitch","Volume","Left","Right"};
	for(i=0;i<sizeof(lpszVisItems)/sizeof(lpszVisItems[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLITEM,CB_ADDSTRING,0,(LPARAM)lpszVisItems[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLITEM,CB_SETCURSEL,m_nVisItem,0);

	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL1),GWL_USERDATA,(LONG)app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme][m_nVisItem*2]);
	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL2),GWL_USERDATA,(LONG)app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme][m_nVisItem*2+1]);

	return FALSE;
}

BOOL CVisualizationDialog::Validate()
{
	m_bActive=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SHOW_VISUALIZATION)==BST_CHECKED;
	m_bShowVisLabels=IsDlgButtonChecked(m_hWnd,IDC_CHECK_SHOWVISLABELS)==BST_CHECKED;
	m_nMode=SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_MODE,CB_GETCURSEL,0,0);
	m_nSpectrumSize=SendDlgItemMessage(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE,TBM_GETPOS,0,0);
	m_nRate=SendDlgItemMessage(m_hWnd,IDC_SLIDER_VIS_RATE,TBM_GETPOS,0,0);
	m_nPriority=SendDlgItemMessage(m_hWnd,IDC_COMBO_VIS_PRIORITY,CB_GETCURSEL,0,0);
	m_nVisItem=SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLITEM,CB_GETCURSEL,0,0);

	return TRUE;
}

BOOL CVisualizationDialog::CarryOutActions()
{
	if(app.m_Reg.m_bShowVisWindow!=m_bActive)
	{
		app.m_Reg.m_bShowVisWindow=m_bActive;
		if(app.m_Reg.m_bShowVisWindow)
		{
			app.m_MainWindow.m_VisWindow.Create(app.m_hInstance,&app.m_MainWindow,IDC_VIS_WINDOW);
			if(app.m_Player.IsPlaying()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
				app.m_MainWindow.m_VisWindow.Start();
		}
		else
			app.m_MainWindow.m_VisWindow.Destroy();
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
	}
	if(app.m_Reg.m_bShowVisLabels!=m_bShowVisLabels)
	{
		app.m_Reg.m_bShowVisLabels=m_bShowVisLabels;
		app.m_MainWindow.m_VisWindow.Refresh();
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
	}
	app.m_Reg.m_dwVisMode=m_nMode;
	app.m_Reg.m_dwSpectrumSize=m_nSpectrumSize;
	if(app.m_Reg.m_dwVisRate!=m_nRate)
	{
		app.m_Reg.m_dwVisRate=m_nRate;
		UEMATSUCONFIG Cfg;
		app.m_Player.GetConfiguration(Cfg);
		Cfg.uVisualizationRate=app.m_Reg.m_dwVisRate;
		app.m_Player.SetConfiguration(Cfg);
	}
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	if(app.m_Reg.m_dwVisThreadPriority!=nThreadPriorities[m_nPriority])
	{
		app.m_Reg.m_dwVisThreadPriority=nThreadPriorities[m_nPriority];
		app.m_MainWindow.m_VisWindow.SetThreadPriority(app.m_Reg.m_dwVisThreadPriority);
	}

	return TRUE;
}

LRESULT CVisualizationDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_CHECK_SHOW_VISUALIZATION:
			BOOL bEnable;
			bEnable=IsDlgButtonChecked(m_hWnd,id)==BST_CHECKED;
			EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_SHOWVISLABELS),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VIS_MODE),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_SPECTRUM_SIZE),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_VIS_RATE),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_VIS_RATE),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VIS_PRIORITY),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VISCOLSCHEME),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_VISCOLITEM),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL1),bEnable);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL2),bEnable);
			break;
		case IDC_BUTTON_VISCOL1:
		case IDC_BUTTON_VISCOL2:
			CHOOSECOLOR cc;
			memset(&cc,0,sizeof(cc));
			cc.lStructSize=sizeof(cc);
			cc.hwndOwner=m_hWnd;
			cc.rgbResult=GetWindowLong(hwndCtl,GWL_USERDATA);
			cc.lpCustColors=m_CustomColors;
			cc.Flags=CC_RGBINIT;
			if(ChooseColor(&cc))
			{
				SetWindowLong(hwndCtl,GWL_USERDATA,cc.rgbResult);
				InvalidateRect(hwndCtl,NULL,TRUE);
				app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme][m_nVisItem*2+(id==IDC_BUTTON_VISCOL2?1:0)]=cc.rgbResult;
				app.m_MainWindow.m_VisWindow.Refresh();
			}
			break;
		}
		break;
	case CBN_SELENDOK:
		switch(id)
		{
		case IDC_COMBO_VISCOLSCHEME:
			app.m_Reg.m_dwVisColorScheme=SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLSCHEME,CB_GETCURSEL,0,0);
			app.m_MainWindow.m_VisWindow.Refresh();
		case IDC_COMBO_VISCOLITEM:
			m_nVisItem=SendDlgItemMessage(m_hWnd,IDC_COMBO_VISCOLITEM,CB_GETCURSEL,0,0);
			SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL1),GWL_USERDATA,(LONG)app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme][m_nVisItem*2]);
			SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL2),GWL_USERDATA,(LONG)app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme][m_nVisItem*2+1]);
			InvalidateRect(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL1),NULL,TRUE);
			InvalidateRect(GetDlgItem(m_hWnd,IDC_BUTTON_VISCOL2),NULL,TRUE);
			break;
		}
		break;
	}
	return 0;
}

void CVisualizationDialog::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	TCHAR szBuf[100];
	if(hwndCtl==GetDlgItem(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE))
	{
		itoa(SendDlgItemMessage(m_hWnd,IDC_SLIDER_SPECTRUM_SIZE,TBM_GETPOS,0,0),szBuf,10);
		SetDlgItemText(m_hWnd,IDC_STATIC_SPECTRUM_SIZE,szBuf);
	}
	else if(hwndCtl==GetDlgItem(m_hWnd,IDC_SLIDER_VIS_RATE))
	{
		itoa(SendDlgItemMessage(m_hWnd,IDC_SLIDER_VIS_RATE,TBM_GETPOS,0,0),szBuf,10);
		SetDlgItemText(m_hWnd,IDC_STATIC_VIS_RATE,szBuf);
	}
}

void CVisualizationDialog::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem)
{
	switch(lpDrawItem->CtlType)
	{
	case ODT_BUTTON:
		switch(lpDrawItem->CtlID)
		{
		case IDC_BUTTON_VISCOL1:
		case IDC_BUTTON_VISCOL2:
			DrawColorButton(lpDrawItem);
			break;
		}
		break;
	}
	CWnd::OnDrawItem(hwnd,lpDrawItem);
}
