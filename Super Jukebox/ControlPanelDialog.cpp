// ControlPanelDialog.cpp: implementation of the CControlPanelDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlPanelDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControlPanelDialog::CControlPanelDialog()
{
}

CControlPanelDialog::~CControlPanelDialog()
{

}

BOOL CControlPanelDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	TCHAR szBuf[100];

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_SPEED,TBM_SETRANGE,FALSE,MAKELONG(-50,50));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_SPEED,TBM_SETPOS,TRUE,app.m_Reg.m_dwTempo);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_SPEED,TBM_SETTICFREQ,5,0);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_SPEED,TBM_SETPAGESIZE,0,5);
	sprintf(szBuf,"%.2fx",pow(2,(INT)app.m_Reg.m_dwTempo/50.0));
	SetDlgItemText(m_hWnd,IDC_STATIC_CPL_SPEED,szBuf);

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_PITCH,TBM_SETRANGE,FALSE,MAKELONG(-50,50));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_PITCH,TBM_SETPOS,TRUE,app.m_Reg.m_dwPitch);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_PITCH,TBM_SETTICFREQ,5,0);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_PITCH,TBM_SETPAGESIZE,0,5);
	sprintf(szBuf,"%.2f",(INT)app.m_Reg.m_dwPitch/4.0);
	SetDlgItemText(m_hWnd,IDC_STATIC_CPL_PITCH,szBuf);

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_MAINSS,TBM_SETRANGE,FALSE,MAKELONG(0,100));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_MAINSS,TBM_SETPOS,TRUE,app.m_Reg.m_dwMainSS);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_MAINSS,TBM_SETTICFREQ,5,0);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_MAINSS,TBM_SETPAGESIZE,0,5);
	sprintf(szBuf,"%u",app.m_Reg.m_dwMainSS);
	SetDlgItemText(m_hWnd,IDC_STATIC_CPL_MAINSS,szBuf);

	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_ECHOSS,TBM_SETRANGE,FALSE,MAKELONG(-100,100));
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_ECHOSS,TBM_SETPOS,TRUE,app.m_Reg.m_dwEchoSS);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_ECHOSS,TBM_SETTICFREQ,10,0);
	SendDlgItemMessage(m_hWnd,IDC_SLIDER_CPL_ECHOSS,TBM_SETPAGESIZE,0,10);
	INT nPos=(INT)app.m_Reg.m_dwEchoSS;
	if(nPos>=0)nPos=100-nPos;
	else nPos=(100+nPos)*-1;
	sprintf(szBuf,"%d%%",nPos);
	SetDlgItemText(m_hWnd,IDC_STATIC_CPL_ECHOSS,szBuf);
	return FALSE;
}

void CControlPanelDialog::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	TCHAR szBuf[100];
	UINT uID=GetWindowLong(hwndCtl,GWL_ID);

	INT nPos=SendDlgItemMessage(m_hWnd,uID,TBM_GETPOS,0,0);
	switch(uID)
	{
	case IDC_SLIDER_CPL_SPEED:
		app.m_Reg.m_dwTempo=nPos;
		sprintf(szBuf,"%.2fx",pow(2,(INT)app.m_Reg.m_dwTempo/50.0));
		SetDlgItemText(m_hWnd,IDC_STATIC_CPL_SPEED,szBuf);		
		app.m_Player.SetSongSpeed(pow(2,(INT)app.m_Reg.m_dwTempo/50.0)*65536.0);
		break;
	case IDC_SLIDER_CPL_PITCH:
		if(nPos>48)nPos=48;
		else if(nPos<-48)nPos=-48;
		app.m_Reg.m_dwPitch=nPos;
		sprintf(szBuf,"%.2f",(INT)app.m_Reg.m_dwPitch/4.0);
		SetDlgItemText(m_hWnd,IDC_STATIC_CPL_PITCH,szBuf);		
		app.m_Player.SetSongPitch((UINT)(pow(1.0145453349375236,(INT)app.m_Reg.m_dwPitch)*32000));
		break;
	case IDC_SLIDER_CPL_MAINSS:
		app.m_Reg.m_dwMainSS=nPos;
		sprintf(szBuf,"%u",app.m_Reg.m_dwMainSS);
		SetDlgItemText(m_hWnd,IDC_STATIC_CPL_MAINSS,szBuf);
		app.m_Player.SetMainSS((app.m_Reg.m_dwMainSS<<16)/100);
		break;
	case IDC_SLIDER_CPL_ECHOSS:
		app.m_Reg.m_dwEchoSS=nPos;
		if(nPos>=0)nPos=100-nPos;
		else nPos=(100+nPos)*-1;
		sprintf(szBuf,"%d%%",nPos);
		SetDlgItemText(m_hWnd,IDC_STATIC_CPL_ECHOSS,szBuf);
		app.m_Player.SetEchoSS(((INT)app.m_Reg.m_dwEchoSS<<15)/100);
		break;
	}
}

void CControlPanelDialog::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HMENU hMenu=CreatePopupMenu();
	if(!hMenu)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return;
	}
	AppendMenu(hMenu,MF_STRING,1,"&Hide");
	POINT pt={x,y};
	::ClientToScreen(m_hWnd,&pt);
	INT nSelection=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,0,m_hWnd,NULL);
	DestroyMenu(hMenu);

	switch(nSelection)
	{
	case 1:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_CONTROLPANEL,0),(LPARAM)m_hWnd);
		break;
	}
}
