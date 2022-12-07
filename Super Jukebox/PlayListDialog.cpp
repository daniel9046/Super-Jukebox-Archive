// PlayListDialog.cpp: implementation of the CPlayListDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayListDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayListDialog::CPlayListDialog()
{
	memset(m_CustomColors,0,sizeof(m_CustomColors));
}

CPlayListDialog::~CPlayListDialog()
{

}

BOOL CPlayListDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	m_uMode=pl.GetPlayingMode();
	m_uIndSongLength=pl.GetIndSongLength();
	m_uIndSongFadeLength=pl.GetIndSongFadeLength();
	m_uUniSongLength=pl.GetUniSongLength();
	m_uUniSongFadeLength=pl.GetUniSongFadeLength();

	SendDlgItemMessage(m_hWnd,IDC_SPIN_UNI_LENGTH,UDM_SETRANGE32,0,INT_MAX);
	SendDlgItemMessage(m_hWnd,IDC_SPIN_UNI_FADE,UDM_SETRANGE32,0,INT_MAX);

	SendDlgItemMessage(m_hWnd,m_uMode?m_uMode==1?IDC_RADIO_UNIVERSAL:IDC_RADIO_INDEFINITE:IDC_RADIO_INDIVIDUAL,BM_CLICK,0,0);

	CheckDlgButton(m_hWnd,IDC_CHECK_AUTO_REWIND,pl.IsAutoRewind()?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_RANDOMIZE,pl.IsRandomize()?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_DETECT_INACTIVITY,pl.IsDetectInactivity()?BST_CHECKED:BST_UNCHECKED);
	CheckDlgButton(m_hWnd,IDC_CHECK_AUTO_ADVANCE,pl.IsAutoAdvance()?BST_CHECKED:BST_UNCHECKED);

	CheckDlgButton(m_hWnd,IDC_CHECK_USE_SKIN,pl.m_Skin.IsActive()?BST_CHECKED:BST_UNCHECKED);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_ADDSTRING,0,(LPARAM)"Stretch");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_ADDSTRING,0,(LPARAM)"Tile");
	SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_SETCURSEL,pl.m_Skin.GetLayout(),0);
	SetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,pl.m_Skin.GetFileName());

	CheckDlgButton(m_hWnd,IDC_CHECK_BLEND_SKIN,pl.m_Skin.IsBlended()?BST_CHECKED:BST_UNCHECKED);
	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA,pl.m_Skin.GetBlendingColor());
	SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA,pl.m_Skin.GetTextColor());
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_SETRANGE,FALSE,MAKELONG(0,255));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_SETPOS,TRUE,pl.m_Skin.GetBlendingLevel());
	SetDlgItemInt(m_hWnd,IDC_STATIC_BLENDING_LEVEL,pl.m_Skin.GetBlendingLevel(),TRUE);

	EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_SKIN_LAYOUT),pl.m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE),pl.m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSE_SKIN),pl.m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_BLEND_SKIN),pl.m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),pl.m_Skin.IsActive()&&pl.m_Skin.IsBlended());
	EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),pl.m_Skin.IsActive());
	EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_BLENDING_LEVEL),pl.m_Skin.IsActive()&&pl.m_Skin.IsBlended());
	EnableWindow(GetDlgItem(m_hWnd,IDC_STATIC_BLENDING_LEVEL),pl.m_Skin.IsActive()&&pl.m_Skin.IsBlended());
	
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

BOOL CPlayListDialog::Validate()
{
	TCHAR szBuf[MAX_PATH];
	GetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,szBuf,sizeof(szBuf));
	if(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED&&!strlen(szBuf))
	{
		MessageBox(m_hWnd,"You must enter a value for the 'Skin file' edit field.",app.m_szAppName,MB_ICONWARNING);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE));
		return FALSE;
	}
	CSkin skin;
	skin.IsActive(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED);
	skin.SetFileName(szBuf);
	if(!skin.Load())
	{
		MessageBox(m_hWnd,"Failed to load the skin bitmap file.",app.m_szAppName,MB_ICONERROR);
		SetFocus(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE));
		return FALSE;
	}
	skin.Destroy();

	CPlayList &pl=app.m_MainWindow.m_PlayList;

	pl.SetPlayingMode(m_uMode);
	pl.SetIndSongLength(m_uIndSongLength);
	pl.SetIndSongFadeLength(m_uIndSongFadeLength);
	pl.SetUniSongLength(m_uUniSongLength);
	pl.SetUniSongFadeLength(m_uUniSongFadeLength);

	pl.IsAutoRewind(IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTO_REWIND)==BST_CHECKED);
	pl.IsRandomize(IsDlgButtonChecked(m_hWnd,IDC_CHECK_RANDOMIZE)==BST_CHECKED);
	pl.IsDetectInactivity(IsDlgButtonChecked(m_hWnd,IDC_CHECK_DETECT_INACTIVITY)==BST_CHECKED);
	pl.IsAutoAdvance(IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTO_ADVANCE)==BST_CHECKED);

	pl.m_Skin.IsActive(IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED);
	pl.m_Skin.SetLayout(SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_GETCURSEL,0,0));
	GetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,szBuf,sizeof(szBuf));
	pl.m_Skin.SetFileName(szBuf);
	pl.m_Skin.IsBlended(IsDlgButtonChecked(m_hWnd,IDC_CHECK_BLEND_SKIN)==BST_CHECKED);
	pl.m_Skin.SetBlendingColor(GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA));
	pl.m_Skin.SetTextColor(GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA));
	pl.m_Skin.SetBlendingLevel(SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_GETPOS,0,0));
	pl.m_Skin.Load();
	pl.m_Skin.Refresh();
	app.m_MainWindow.RePaint(NULL);
	app.m_MainWindow.UpdateTitleBar();

	CList *List=app.m_Player.GetPlayingList();
	if(List==&pl)
	{
		UINT uLength,uFadeLength;
		switch(pl.GetPlayingMode())
		{
		case 0:
			CListEntry *Entry;
			Entry=pl.GetListEntryUnmapped(app.m_Player.GetPlayingIndex());
			uLength=Entry->GetLength();
			if(uLength<=0)
			{
				uLength=pl.GetIndSongLength();
				uFadeLength=pl.GetIndSongFadeLength();
			}
			else
				uFadeLength=Entry->GetFadeLength();
			break;
		case 1:
			uLength=pl.GetUniSongLength();
			uFadeLength=pl.GetUniSongFadeLength();
			break;
		case 2:
			uLength=uFadeLength=0;
			break;
		}
		app.m_Player.SetLength(uLength);
		app.m_Player.SetFadeLength(uFadeLength);
	}

	return TRUE;
}

LRESULT CPlayListDialog::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	switch(lpNMHdr->code)
	{
	case UDN_DELTAPOS:
		switch(lpNMHdr->idFrom)
		{
		case IDC_SPIN_UNI_LENGTH:
		case IDC_SPIN_UNI_FADE:
			LPNMUPDOWN lpNMUpDown=(LPNMUPDOWN)lpNMHdr;
			TCHAR szBuf[100];
			GetDlgItemText(m_hWnd,lpNMHdr->idFrom,szBuf,sizeof(szBuf));
			pl.ConvertTimeDown(pl.ConvertTimeUp(szBuf)+lpNMUpDown->iDelta,szBuf);
			SetDlgItemText(m_hWnd,lpNMHdr->idFrom,szBuf);
			break;
		}
		break;
	}
	return 0;
}

LRESULT CPlayListDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	LRESULT lResult=1;
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	switch(codeNotify)
	{
	case EN_CHANGE:
		switch(id)
		{
		case IDC_EDIT_UNI_LENGTH:
			TCHAR szBuf[50];
			GetDlgItemText(m_hWnd,IDC_EDIT_UNI_LENGTH,szBuf,sizeof(szBuf));
			if(m_uMode==0)
				m_uIndSongLength=pl.ConvertTimeUp(szBuf);
			else if(m_uMode==1)
				m_uUniSongLength=pl.ConvertTimeUp(szBuf);
			break;
		case IDC_EDIT_UNI_FADE:
			GetDlgItemText(m_hWnd,IDC_EDIT_UNI_FADE,szBuf,sizeof(szBuf));
			if(m_uMode==0)
				m_uIndSongFadeLength=pl.ConvertTimeUp(szBuf);
			else if(m_uMode==1)
				m_uUniSongFadeLength=pl.ConvertTimeUp(szBuf);
			break;
		}
		break;
	case BN_CLICKED:
		switch(id)
		{
		case IDC_RADIO_INDIVIDUAL:
			m_uMode=0;
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_LENGTH),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_LENGTH),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_FADE),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_FADE),TRUE);
			TCHAR szBuf[100];
			pl.ConvertTimeDown(m_uIndSongLength,szBuf);
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_LENGTH,szBuf);
			pl.ConvertTimeDown(m_uIndSongFadeLength,szBuf);
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_FADE,szBuf);
			break;
		case IDC_RADIO_UNIVERSAL:
			m_uMode=1;
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_LENGTH),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_LENGTH),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_FADE),TRUE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_FADE),TRUE);
			pl.ConvertTimeDown(m_uUniSongLength,szBuf);
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_LENGTH,szBuf);
			pl.ConvertTimeDown(m_uUniSongFadeLength,szBuf);
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_FADE,szBuf);
			break;
		case IDC_RADIO_INDEFINITE:
			m_uMode=2;
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_LENGTH),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_LENGTH),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_UNI_FADE),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SPIN_UNI_FADE),FALSE);
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_LENGTH,"");
			SetDlgItemText(m_hWnd,IDC_EDIT_UNI_FADE,"");
			break;
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
			strcpy(szFileName,app.m_MainWindow.m_PlayList.m_Skin.GetFileName());
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
		case IDC_BUTTON_RESET:
			m_uMode=0;
			m_uIndSongLength=m_uIndSongFadeLength=0;
			m_uUniSongLength=m_uUniSongFadeLength=0;
			SendDlgItemMessage(m_hWnd,IDC_RADIO_INDIVIDUAL,BM_CLICK,0,0);

			CheckDlgButton(m_hWnd,IDC_CHECK_AUTO_REWIND,BST_UNCHECKED);
			CheckDlgButton(m_hWnd,IDC_CHECK_RANDOMIZE,BST_UNCHECKED);
			CheckDlgButton(m_hWnd,IDC_CHECK_DETECT_INACTIVITY,BST_UNCHECKED);
			CheckDlgButton(m_hWnd,IDC_CHECK_AUTO_ADVANCE,BST_CHECKED);

			CheckDlgButton(m_hWnd,IDC_CHECK_USE_SKIN,BST_UNCHECKED);
			SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_SETCURSEL,0,0);
			SetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,NULL);

			CheckDlgButton(m_hWnd,IDC_CHECK_BLEND_SKIN,BST_UNCHECKED);
			SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA,RGB(255,255,255));
			SetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA,RGB(0,0,0));
			SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_SETPOS,TRUE,128);
			SetDlgItemInt(m_hWnd,IDC_STATIC_BLENDING_LEVEL,128,TRUE);

			EnableWindow(GetDlgItem(m_hWnd,IDC_COMBO_SKIN_LAYOUT),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_EDIT_SKIN_FILE),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BROWSE_SKIN),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_CHECK_BLEND_SKIN),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),FALSE);
			EnableWindow(GetDlgItem(m_hWnd,IDC_SLIDER_BLENDING_LEVEL),FALSE);
			break;
		case IDC_BUTTON_SETDEFAULT:
			app.m_Reg.m_dwPlayMode=m_uMode;
			app.m_Reg.m_dwIndDefaultLength=m_uIndSongLength;
			app.m_Reg.m_dwIndDefaultFade=m_uIndSongFadeLength;
			app.m_Reg.m_dwUniDefaultLength=m_uUniSongLength;
			app.m_Reg.m_dwUniDefaultFade=m_uUniSongFadeLength;

			app.m_Reg.m_bAutoRewind=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTO_REWIND)==BST_CHECKED;
			app.m_Reg.m_bRandomize=IsDlgButtonChecked(m_hWnd,IDC_CHECK_RANDOMIZE)==BST_CHECKED;
			app.m_Reg.m_bDetectInactivity=IsDlgButtonChecked(m_hWnd,IDC_CHECK_DETECT_INACTIVITY)==BST_CHECKED;
			app.m_Reg.m_bAutoAdvance=IsDlgButtonChecked(m_hWnd,IDC_CHECK_AUTO_ADVANCE)==BST_CHECKED;

			app.m_Reg.m_bUsePlaylistSkin=IsDlgButtonChecked(m_hWnd,IDC_CHECK_USE_SKIN)==BST_CHECKED;
			app.m_Reg.m_dwPlaylistSkinLayout=SendDlgItemMessage(m_hWnd,IDC_COMBO_SKIN_LAYOUT,CB_GETCURSEL,0,0);
			GetDlgItemText(m_hWnd,IDC_EDIT_SKIN_FILE,app.m_Reg.m_szPlaylistSkinFile,sizeof(app.m_Reg.m_szPlaylistSkinFile));
			app.m_Reg.m_bBlendPlaylistSkin=IsDlgButtonChecked(m_hWnd,IDC_CHECK_BLEND_SKIN)==BST_CHECKED;
			app.m_Reg.m_crPlaylistSkinBlendingColor=GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_BLENDING_COLOR),GWL_USERDATA);
			app.m_Reg.m_crPlaylistSkinTextColor=GetWindowLong(GetDlgItem(m_hWnd,IDC_BUTTON_TEXT_COLOR),GWL_USERDATA);
			app.m_Reg.m_dwPlaylistSkinBlendingLevel=SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_GETPOS,0,0);
			break;
		}
		break;
	}
	if(lResult)
		return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
	return lResult;
}

void CPlayListDialog::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem)
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

void CPlayListDialog::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	TCHAR szBuf[100];
	itoa(SendDlgItemMessage(m_hWnd,IDC_SLIDER_BLENDING_LEVEL,TBM_GETPOS,0,0),szBuf,10);
	SetDlgItemText(m_hWnd,IDC_STATIC_BLENDING_LEVEL,szBuf);
}

LRESULT CPlayListDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
