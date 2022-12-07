// SkinDialog.cpp: implementation of the CSkinDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkinDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkinDialog::CSkinDialog()
{
	m_uID=IDD_SKIN;
	memset(m_CustomColors,0,sizeof(m_CustomColors));
}

CSkinDialog::~CSkinDialog()
{

}

void CSkinDialog::InitMembers()
{
	m_Skin=app.m_MainWindow.m_Skin;
	m_bAlwaysUseProgramSkin=app.m_Reg.m_bAlwaysUseProgramSkin;
}

BOOL CSkinDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	CheckDlgButton(m_hWnd,IDC_CHECK_USE_SKIN,m_Skin.IsActive()?BST_CHECKED:BST_UNCHECKED);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_ADDSTRING,0,(LPARAM)"Stretch");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_ADDSTRING,0,(LPARAM)"Tile");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_SETCURSEL,m_Skin.GetLayout(),0);
	SetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,m_Skin.GetFileName());

	CheckDlgButton(m_hWnd,IDC_CHECK_BLEND_SKIN,m_Skin.IsBlended()?BST_CHECKED:BST_UNCHECKED);
	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA,m_Skin.GetBlendingColor());
	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA,m_Skin.GetTextColor());
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_SETRANGE,FALSE,MAKELONG(0,255));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_SETPOS,TRUE,m_Skin.GetBlendingLevel());
	SetDlgItemInt(m_hWnd,IDC_STATIC_BLENDING_LEVEL,m_Skin.GetBlendingLevel(),TRUE);

	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_SKIN_LAYOUT),m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE),m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSE_SKIN),m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_BLEND_SKIN),m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),m_Skin.IsActive()&&m_Skin.IsBlended());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_BLENDING_LEVEL),m_Skin.IsActive()&&m_Skin.IsBlended());
	EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_BLENDING_LEVEL),m_Skin.IsActive()&&m_Skin.IsBlended());

	CheckDlgButton(m_hWnd,IDC_CHECK_ALWAYS_USE_PROGRAM_SKIN,m_bAlwaysUseProgramSkin?BST_CHECKED:BST_UNCHECKED);

	return FALSE;
}

void CSkinDialog::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	TCHAR szBuf[100];
	itoa(SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_GETPOS,0,0),szBuf,10);
	SetDlgItemText(m_hWnd,IDC_STATIC_BLENDING_LEVEL,szBuf);
}

BOOL CSkinDialog::Validate()
{
	TCHAR szBuf[MAX_PATH];
	GetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,szBuf,sizeof(szBuf));
	if(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED&&!strlen(szBuf))
	{
		MessageBox(m_hWnd,"You must enter a value for the Skin File edit field.",app.m_szAppName,MB_ICONWARNING);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE));
		return FALSE;
	}
	CSkin skin;
	skin.IsActive(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED);
	skin.SetFileName(szBuf);
	if(!skin.Load())
	{
		MessageBox(m_hWnd,"Failed to load the bitmap file.",app.m_szAppName,MB_ICONERROR);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE));
		return FALSE;
	}
	skin.Destroy();

	m_Skin.IsActive(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED);
	m_Skin.SetLayout(SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_GETCURSEL,0,0));
	m_Skin.SetFileName(szBuf);
	m_Skin.IsBlended(IsDlgButtonChecked(m_hWnd,IDC_CHECK_BLEND_SKIN)==BST_CHECKED);
	m_Skin.SetBlendingColor(GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA));
	m_Skin.SetTextColor(GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA));
	m_Skin.SetBlendingLevel(SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_GETPOS,0,0));
	m_bAlwaysUseProgramSkin=IsDlgButtonChecked(m_hWnd,IDC_CHECK_ALWAYS_USE_PROGRAM_SKIN)==BST_CHECKED?TRUE:FALSE;

	return TRUE;
}

BOOL CSkinDialog::CarryOutActions()
{
	app.m_Reg.m_bAlwaysUseProgramSkin=m_bAlwaysUseProgramSkin;
	app.m_MainWindow.m_Skin.IsActive(m_Skin.IsActive());
	app.m_MainWindow.m_Skin.SetLayout(m_Skin.GetLayout());
	app.m_MainWindow.m_Skin.SetFileName(m_Skin.GetFileName());
	app.m_MainWindow.m_Skin.IsBlended(m_Skin.IsBlended());
	app.m_MainWindow.m_Skin.SetBlendingColor(m_Skin.GetBlendingColor());
	app.m_MainWindow.m_Skin.SetTextColor(m_Skin.GetTextColor());
	app.m_MainWindow.m_Skin.SetBlendingLevel(m_Skin.GetBlendingLevel());
	app.m_MainWindow.m_Skin.Load();
	app.m_MainWindow.AdjustLayout();
	app.m_MainWindow.RePaint(NULL);
	return TRUE;
}

LRESULT CSkinDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_CHECK_USE_SKIN:
		case IDC_CHECK_BLEND_SKIN:
			BOOL b0,b1;
			b0=IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED;
			b1=IsDlgButtonChecked(m_hWnd,IDC_CHECK_BLEND_SKIN)==BST_CHECKED;

			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_SKIN_LAYOUT),b0);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE),b0);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSE_SKIN),b0);
			EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_BLEND_SKIN),b0);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),b0&&b1);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),b0);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_BLENDING_LEVEL),b0&&b1);
			EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_BLENDING_LEVEL),b0&&b1);
			break;
		case IDC_BUTTON_BROWSE_SKIN:
			OPENFILENAME ofn;
			memset(&ofn,0,sizeof(ofn));
			ofn.lStructSize=sizeof(ofn);
			ofn.hwndOwner=m_hWnd;
			ofn.lpstrFilter="Bitmap Files (.BMP)\0*.BMP\0All Files (*.*)\0*.*\0";
			TCHAR szFileName[MAX_PATH];
			strcpy(szFileName,m_Skin.GetFileName());
			ofn.lpstrFile=szFileName;
			ofn.nMaxFile=sizeof(szFileName);
			ofn.lpstrDefExt="BMP";
			ofn.Flags=OFN_FILEMUSTEXIST|OFN_HIDEREADONLY|OFN_NOCHANGEDIR;
			if(GetOpenFileName(&ofn))
				SetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,szFileName);
			break;
		case IDC_BUTTON_BLENDING_COLOR:
		case IDC_BUTTON_TEXT_COLOR:
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
			}
			break;
		}
		break;
	}
	return 0;
}

void CSkinDialog::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem)
{
	switch(lpDrawItem->CtlType)
	{
	case ODT_BUTTON:
		switch(lpDrawItem->CtlID)
		{
		case IDC_BUTTON_BLENDING_COLOR:
		case IDC_BUTTON_TEXT_COLOR:
			DrawColorButton(lpDrawItem);
			break;
		}
		break;
	}
	CWnd::OnDrawItem(hwnd,lpDrawItem);
}