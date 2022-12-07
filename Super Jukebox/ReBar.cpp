// ReBar.cpp: implementation of the CReBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReBar.h"
#include "Resource.h"
#include "Main.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReBar::CReBar()
{

}

CReBar::~CReBar()
{

}

BOOL CReBar::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,REBARCLASSNAME,&wcx))
		return FALSE;
	m_OriginalWndProc=wcx.lpfnWndProc;
	if(!GetClassInfoEx(hInstance,"MyReBar",&wcx))
	{
		wcx.lpszClassName="MyReBar";
		wcx.lpfnWndProc=CWnd::WindowProc;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

BOOL CReBar::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(!RegisterClass(hInstance))
		return FALSE;
	RECT rc={0};
	if(!CWnd::Create("MyReBar",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_BORDER|WS_OVERLAPPED|RBS_VARHEIGHT|RBS_BANDBORDERS|RBS_AUTOSIZE|RBS_DBLCLKTOGGLE|CCS_NODIVIDER|CCS_NOPARENTALIGN|CCS_TOP,WS_EX_TOOLWINDOW|WS_EX_CONTROLPARENT,rc,pParentWnd,uID,hInstance))
		return FALSE;

	REBARINFO rbi;
	rbi.cbSize=sizeof(rbi);
	rbi.fMask=0;
	rbi.himl=NULL;
	if(!SendMessage(m_hWnd,RB_SETBARINFO,0,(LPARAM)&rbi))
		return FALSE;

	SendMessage(m_hWnd,RB_SETBKCOLOR,0,(LPARAM)GetSysColor(COLOR_3DFACE));

	app.m_MainWindow.m_ToolBar.HasTextLabels(app.m_Reg.m_bToolBarHasLabels);
	app.m_MainWindow.m_ToolBar.HasPictures(app.m_Reg.m_bToolBarHasPictures);
	if(!app.m_Reg.m_dwToolBarBandIndex)
	{
		if(!app.m_MainWindow.m_ToolBar.Create(m_hInstance,this,IDC_TOOL_BAR))return FALSE;
		if(!app.m_MainWindow.m_SourceBar.Create(m_hInstance,this,IDC_SOURCE_BAR))return FALSE;
	}
	else
	{
		if(!app.m_MainWindow.m_SourceBar.Create(m_hInstance,this,IDC_SOURCE_BAR))return FALSE;
		if(!app.m_MainWindow.m_ToolBar.Create(m_hInstance,this,IDC_TOOL_BAR))return FALSE;
	}
		
	return TRUE;
}

BOOL CReBar::AddBand(UINT uIndex,LPREBARBANDINFO lprbBand)
{
	if(uIndex>SendMessage(m_hWnd,RB_GETBANDCOUNT,0,0))
		uIndex=SendMessage(m_hWnd,RB_GETBANDCOUNT,0,0);
	return SendMessage(m_hWnd,RB_INSERTBAND,uIndex,(LPARAM)lprbBand);
}

BOOL CReBar::DeleteBand(UINT uIndex)
{
	return SendMessage(m_hWnd,RB_DELETEBAND,uIndex,0);
}

INT CReBar::GetBandIndex(UINT uID)
{
	return SendMessage(m_hWnd,RB_IDTOINDEX,uID,0);
}

BOOL CReBar::ShowBand(INT nBand, BOOL bShow)
{
	BOOL bRes=SendMessage(m_hWnd,RB_SHOWBAND,nBand,bShow);
	UINT uCount=SendMessage(m_hWnd,RB_GETBANDCOUNT,0,0);
	BOOL bHide=TRUE;
	for(UINT i=0;i<uCount;i++)
	{
		REBARBANDINFO rbBand;
		rbBand.cbSize=sizeof(rbBand);
		rbBand.fMask=RBBIM_STYLE;
		SendMessage(m_hWnd,RB_GETBANDINFO,i,(LPARAM)&rbBand);
		if(!(rbBand.fStyle&RBBS_HIDDEN))
			bHide=FALSE;
	}
	IsVisible(!bHide);
	return bRes;
}

BOOL CReBar::SetBandInfo(UINT nIndex, LPREBARBANDINFO lprbbi)
{
	return SendMessage(m_hWnd,RB_SETBANDINFO,nIndex,(LPARAM)lprbbi);
}

BOOL CReBar::GetBandInfo(UINT nIndex, LPREBARBANDINFO lprbbi)
{
	return SendMessage(m_hWnd,RB_GETBANDINFO,nIndex,(LPARAM)lprbbi);
}

LRESULT CReBar::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	switch(lpNMHdr->code)
	{
	case TBN_HOTITEMCHANGE:
		switch(lpNMHdr->idFrom)
		{
		case IDC_TOOL_BAR:
			NMTBHOTITEM *lpNMTBHotItem=(NMTBHOTITEM*)lpNMHdr;
			if(!(lpNMTBHotItem->dwFlags&HICF_LEAVING))
			{
				TCHAR szBuf[500];
				LoadString(app.m_hInstance,lpNMTBHotItem->idNew,szBuf,sizeof(szBuf));
				app.m_MainWindow.m_StatusBar.SetText(3,szBuf);
			}
			else
				app.m_MainWindow.m_StatusBar.SetText(3,"");
			break;
		}
		break;
	}
	return CWnd::OnNotify(hwnd,nIdCtrl,lpNMHdr);
}
