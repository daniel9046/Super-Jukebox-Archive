// StatusBar.cpp: implementation of the CStatusBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StatusBar.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStatusBar::CStatusBar()
{
	m_bMouseOnTimeBar=FALSE;
	m_OldSize.cx=m_OldSize.cy=0;
}

CStatusBar::~CStatusBar()
{

}

BOOL CStatusBar::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(IsWindow(m_hWnd))
		return FALSE;

	if(!RegisterClass(hInstance))
		return FALSE;
	RECT rc={0};
	if(!CWnd::Create("MyStatusBar",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,0,rc,pParentWnd,uID,hInstance))
		return FALSE;
	Update();
	return (m_bActive=TRUE);
}

void CStatusBar::SetText(int nPart, LPCTSTR lpszText)
{
	if(!IsWindow(m_hWnd))
		return;

	SendMessage(m_hWnd,SB_SETTEXT,nPart,(LPARAM)lpszText);
}

void CStatusBar::OnSize(HWND hwnd,UINT state, int cx, int cy)
{
	CWnd::OnSize(hwnd,state,cx,cy);

	HDC hDC=GetDC(m_hWnd);
	SelectObject(hDC,(HFONT)SendMessage(m_hWnd,WM_GETFONT,0,0));
	SIZE sz;
	GetTextExtentPoint32(hDC,"Song: 0000 of 0000",strlen("Song: 0000 of 0000"),&sz);
	m_nParts[0]=sz.cx+8;
	GetTextExtentPoint32(hDC,"Elapsed time: 00:00 of 00:00",strlen("Elapsed time: 00:00 of 00:00"),&sz);
	m_nParts[1]=m_nParts[0]+sz.cx+8;
	m_nParts[2]=m_nParts[1]+100;
	m_nParts[5]=cx;
	GetTextExtentPoint32(hDC,"Preamp: 000",strlen("Preamp: 000"),&sz);
	m_nParts[4]=m_nParts[5]-(sz.cx+21);
	GetTextExtentPoint32(hDC,"Unfiltered",strlen("Unfiltered"),&sz);
	m_nParts[3]=m_nParts[4]-(sz.cx+8);

	SendMessage(m_hWnd,SB_SETPARTS,6,(LPARAM)m_nParts);
	ReleaseDC(m_hWnd,hDC);
}

BOOL CStatusBar::Update()
{
	if(!IsWindow(m_hWnd))
		return FALSE;

	TCHAR szBuf0[500],szBuf1[500];
	if(app.m_Player.IsPlaying())
	{
		CList *PlayingList=app.m_Player.GetPlayingList();
		if(PlayingList)
		{
			sprintf(szBuf0,"Song: %d of %d",PlayingList->MapIndexToExternal(app.m_Player.GetPlayingIndex())+1,PlayingList->GetDisplayedCount());
			SetText(0,szBuf0);
		}

		INT nPlayingTime=app.m_Player.GetPlayTime()/1000;
		app.m_MainWindow.m_PlayList.ConvertTimeDown(nPlayingTime,szBuf1);
		sprintf(szBuf0,"Elapsed time: %s",szBuf1);
		INT nTotalLength=app.m_Player.GetLength();
		if(nTotalLength>0)
		{
			nTotalLength+=app.m_Player.GetFadeLength();
			app.m_MainWindow.m_PlayList.ConvertTimeDown(nTotalLength,szBuf1);
			sprintf(szBuf0,"%s of %s",szBuf0,szBuf1);
		}
		SetText(1,szBuf0);
		sprintf(szBuf0,"Preamp: %u",app.m_Player.GetCurrentPreamp());
		SetText(5,szBuf0);
	}
	else if(app.m_Player.IsStopped())
	{
		SetText(0,NULL);
		SetText(1,NULL);
		SetText(5,NULL);
	}
	SetText(4,app.m_Reg.m_bFilterActive?"Filtered":"Unfiltered");

	SendMessage(m_hWnd,WM_PAINT,0,0);
	return TRUE;
}

BOOL CStatusBar::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,STATUSCLASSNAME,&wcx))
		return FALSE;
	m_OriginalWndProc=wcx.lpfnWndProc;
	if(!GetClassInfoEx(hInstance,"MyStatusBar",&wcx))
	{
		wcx.lpszClassName="MyStatusBar";
		wcx.lpfnWndProc=CWnd::WindowProc;
		wcx.style=CS_OWNDC;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

void CStatusBar::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(m_bMouseOnTimeBar)
	{
		m_bMouseOnTimeBar=FALSE;
		SendMessage(m_hWnd,WM_PAINT,0,0);
		ReleaseCapture();
	}

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
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_VIEW_STATUSBAR,0),(LPARAM)m_hWnd);
		break;
	}
	CWnd::OnRButtonUp(hwnd,x,y,keyFlags);
}

void CStatusBar::OnPaint(HWND hwnd)
{
	CWnd::OnPaint(hwnd);

	RECT rc0,rc1;
	SendMessage(m_hWnd,SB_GETRECT,2,(LPARAM)&rc0);
	rc0.left++;rc0.top++;rc0.right--;rc0.bottom--;
	SetRect(&rc1,0,0,rc0.right-rc0.left,rc0.bottom-rc0.top);

	if(m_OldSize.cx!=(rc1.right-rc1.left)||m_OldSize.cy!=(rc1.bottom-rc1.top))
	{
		if(m_hBackDC)
		{
			DeleteDC(m_hBackDC);
			DeleteObject(m_hBackBmp);
			DeleteObject(m_hBackBrush);
		}
		m_hBackDC=CreateCompatibleDC(m_hDC);
		m_hBackBmp=CreateCompatibleBitmap(m_hDC,rc1.right-rc1.left,rc1.bottom-rc1.top);
		SelectObject(m_hBackDC,m_hBackBmp);
		DWORD dwCol=GetSysColor(COLOR_HIGHLIGHT);
		m_hBackBrush=CreateSolidBrush(RGB(~GetRValue(dwCol),~GetGValue(dwCol),~GetBValue(dwCol)));
		SelectObject(m_hBackDC,m_hBackBrush);
		SelectObject(m_hBackDC,(HFONT)SendMessage(m_hWnd,WM_GETFONT,0,0));
		SetTextColor(m_hBackDC,RGB(0,0,0));
		SetBkMode(m_hBackDC,TRANSPARENT);
		m_OldSize.cx=rc1.right-rc1.left;
		m_OldSize.cy=rc1.bottom-rc1.top;
	}

	FillRect(m_hBackDC,&rc1,HBRUSH(COLOR_3DFACE+1));
	if((app.m_Player.IsPlaying()||app.m_Player.IsPaused())&&app.m_Player.GetLength()>0)
	{
		INT nTotalLength=app.m_Player.GetLength()+app.m_Player.GetFadeLength();
		INT nPlayingTime=app.m_Player.GetPlayTime();

		rc1.right=nPlayingTime*rc1.right/(nTotalLength*1000);
		
		FillRect(m_hBackDC,&rc1,m_hBackBrush);
		rc1.right=rc0.right-rc0.left;
	}

	INT nLength=0;
	if(!app.m_Player.IsStopped())
	{
		nLength=app.m_Player.GetLength();
		if(nLength>0)
			nLength+=app.m_Player.GetFadeLength();
	}

	if(m_bMouseOnTimeBar&&nLength>0)
	{		
		POINT pt;
		GetCursorPos(&pt);
		::ScreenToClient(m_hWnd,&pt);
		pt.x-=rc0.left;
		pt.y-=rc0.top;

		INT nTotalLength=app.m_Player.GetLength()+app.m_Player.GetFadeLength();
		TCHAR szBuf[10];
		app.m_MainWindow.m_PlayList.ConvertTimeDown(pt.x*nTotalLength/(rc1.right-rc1.left-1),szBuf);
		
		DrawText(m_hBackDC,szBuf,strlen(szBuf),&rc1,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);

		SelectObject(m_hBackDC,GetStockObject(BLACK_PEN));
		SetROP2(m_hBackDC,R2_COPYPEN);
		MoveToEx(m_hBackDC,pt.x,rc1.top,NULL);
		LineTo(m_hBackDC,pt.x,rc1.bottom);
	}

	if((app.m_Player.IsPlaying()||app.m_Player.IsPaused())&&app.m_Player.GetLength()>0)
	{
		INT nTotalLength=app.m_Player.GetLength()+app.m_Player.GetFadeLength();
		INT nPlayingTime=app.m_Player.GetPlayTime();

		rc1.right=nPlayingTime*rc1.right/(nTotalLength*1000);
		SelectObject(m_hBackDC,GetStockObject(NULL_PEN));
		SetROP2(m_hBackDC,R2_NOT);
		Rectangle(m_hBackDC,rc1.left,rc1.top,rc1.right+1,rc1.bottom+1);
		rc1.right=rc0.right-rc0.left;
	}

	BitBlt(m_hDC,rc0.left,rc0.top,rc0.right-rc0.left,rc0.bottom-rc0.top,m_hBackDC,0,0,SRCCOPY);
}

void CStatusBar::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	RECT rc;
	SendMessage(m_hWnd,SB_GETRECT,2,(LPARAM)&rc);
	rc.left++;rc.top++;rc.right--;rc.bottom--;
	POINT pt={x,y};
	if(PtInRect(&rc,pt))
	{
		SetCapture(m_hWnd);
		m_bMouseOnTimeBar=TRUE;
		SendMessage(m_hWnd,WM_PAINT,0,0);
	}
	else if(m_bMouseOnTimeBar)
	{
		m_bMouseOnTimeBar=FALSE;
		SendMessage(m_hWnd,WM_PAINT,0,0);
		ReleaseCapture();
	}
	CWnd::OnMouseMove(hwnd,x,y,keyFlags);
}

BOOL CStatusBar::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	m_hDC=GetDC(m_hWnd);
	m_hBackDC=NULL;
	m_hBackBmp=NULL;
	m_hBackBrush=NULL;
	return CWnd::OnCreate(hwnd,lpCreateStruct);
}

LRESULT CStatusBar::OnDestroy(HWND hwnd)
{
	if(m_hBackDC)
	{
		DeleteDC(m_hBackDC);
		DeleteObject(m_hBackBmp);
		DeleteObject(m_hBackBrush);
	}
	ReleaseDC(m_hWnd,m_hDC);
	return CWnd::OnDestroy(hwnd);
}

void CStatusBar::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if(m_bMouseOnTimeBar)
	{
		m_bMouseOnTimeBar=FALSE;
		SendMessage(m_hWnd,WM_PAINT,0,0);
		ReleaseCapture();
	}

	RECT rc;
	SendMessage(m_hWnd,SB_GETRECT,2,(LPARAM)&rc);
	rc.left++;rc.top++;rc.right--;rc.bottom--;
	POINT pt={x,y};
	INT nLength=0;
	if(!app.m_Player.IsStopped())
	{
		nLength=app.m_Player.GetLength();
		if(nLength>0)
			nLength+=app.m_Player.GetFadeLength();
	}
	if(PtInRect(&rc,pt)&&nLength>0)
	{
		app.m_Player.SetPlayTime((x-rc.left)*nLength*1000/(rc.right-rc.left-1),app.m_Reg.m_bUseFastSeek);
		if(!app.m_Player.IsPlaying())
			app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_PLAY);
		if(app.m_Reg.m_bShowVisWindow)
			app.m_MainWindow.m_VisWindow.Redraw();
	}
	CWnd::OnLButtonDown(hwnd,fDoubleClick,x,y,keyFlags);
}

BOOL CStatusBar::GetText(INT nPart, LPTSTR lpszBuf, INT nTextMax)
{
	if(!IsWindow(m_hWnd))
		return FALSE;

	TCHAR szBuf[1000];
	SendMessage(m_hWnd,SB_GETTEXT,nPart,(LPARAM)szBuf);
	strncpy(lpszBuf,szBuf,nTextMax);
	return TRUE;
}
