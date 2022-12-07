// CBetaBox.cpp: implementation of the CBetaBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BetaBox.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBetaBox::CBetaBox()
{

}

CBetaBox::~CBetaBox()
{

}

BOOL CBetaBox::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	MessageBeep(MB_ICONEXCLAMATION);
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CBetaBox::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDOK:
			if(MessageBox(m_hWnd,"Do you REALLY understand?","NON-PUBLIC BETA RELEASE",MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2)!=IDYES)
			{
				if(app.m_Reg.m_bShowInSystemTray)
					app.m_MainWindow.RemoveSystemTrayIcon();
				ExitProcess(0);
			}
			break;
		case IDCANCEL:
			if(app.m_Reg.m_bShowInSystemTray)
				app.m_MainWindow.RemoveSystemTrayIcon();
			ExitProcess(0);
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}