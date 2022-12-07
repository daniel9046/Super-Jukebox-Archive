// LabelEdit.cpp: implementation of the CLabelEdit class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LabelEdit.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLabelEdit::CLabelEdit()
{
	m_bDestroying=FALSE;
	m_bAdvancing=FALSE;
}

CLabelEdit::~CLabelEdit()
{

}

BOOL CLabelEdit::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(IsWindow(m_hWnd))
		return FALSE;

	//if(!RegisterClass(hInstance))
	//	return FALSE;
	RECT rc={0,0,0,0};
	if(!CWnd::Create("EDIT",NULL,WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,0,rc,pParentWnd,uID,hInstance))
		return FALSE;
	m_OriginalWndProc=(WNDPROC)GetWindowLong(m_hWnd,GWL_WNDPROC);
	SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)CWnd::WindowProc);
	SendMessage(m_hWnd,WM_SETFONT,SendMessage(app.m_MainWindow.m_PlayList.m_hWnd,WM_GETFONT,0,0),0);
	SetFocus(m_hWnd);
	app.EnableAccels(FALSE);
	return (m_bActive=TRUE);
}

BOOL CLabelEdit::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,"EDIT",&wcx))
		return FALSE;
	m_OriginalWndProc=wcx.lpfnWndProc;
	if(!GetClassInfoEx(hInstance,"MyEdit",&wcx))
	{
		wcx.lpszClassName="MyEdit";
		wcx.lpfnWndProc=CWnd::WindowProc;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

void CLabelEdit::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch(vk)
	{
	case VK_TAB:
		if(fDown)
		{
			WriteListItem();
			AdvanceListItem();
		}
		break;
	case VK_ESCAPE:
		if(fDown)
			Destroy();
		break;
	case VK_RETURN:
		if(fDown)
		{
			WriteListItem();
			Destroy();
		}
		break;
	}
	CWnd::OnKey(hwnd,vk,fDown,cRepeat,flags);
}

void CLabelEdit::OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	if(LOBYTE(VkKeyScan(ch))==VK_TAB) //Remove infernal beeping!!!
		return;
	CWnd::OnChar(hwnd,ch,cRepeat);
}

void CLabelEdit::OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	if(!m_bDestroying)
	{
		WriteListItem();
		Destroy();
	}

	CWnd::OnKillFocus(hwnd,hwndNewFocus);
}

BOOL CLabelEdit::ReadListItem(INT iItem, INT iSubItem)
{
	m_iItem=iItem;m_iSubItem=iSubItem;
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	m_bAdvancing=TRUE;
	pl.EnsureVisible(m_iItem,m_iSubItem);
	m_bAdvancing=FALSE;

	ListView_GetItemText(pl.m_hWnd,m_iItem,m_iSubItem,m_szOriginalText,sizeof(m_szOriginalText));
	RECT rc;
	ListView_GetSubItemRect(pl.m_hWnd,m_iItem,m_iSubItem,LVIR_LABEL,&rc);

	SetWindowText(m_hWnd,NULL);
	Move(&rc,TRUE);
	SendMessage(m_hWnd,EM_LIMITTEXT,sizeof(m_szOriginalText)-1,0);
	SetWindowText(m_hWnd,m_szOriginalText);
	SendMessage(m_hWnd,EM_SETSEL,0,-1);
	RedrawWindow(pl.m_hWnd,NULL,NULL,RDW_UPDATENOW);

	return TRUE;
}

BOOL CLabelEdit::WriteListItem()
{
	TCHAR szBuf[500];
	GetWindowText(m_hWnd,szBuf,sizeof(szBuf));
	if(strcmp(szBuf,m_szOriginalText))
	{
		TCHAR szColumnName[200];
		app.m_MainWindow.m_PlayList.GetColumnNameFromIndex(m_iSubItem,szColumnName,sizeof(szColumnName));
		app.m_MainWindow.m_PlayList.EditListEntry(m_iItem,szColumnName,szBuf);
	}

	return TRUE;
}

BOOL CLabelEdit::AdvanceListItem()
{
	BOOL bShift=GetKeyState(VK_SHIFT)&0x8000;
	BOOL bCtrl=GetKeyState(VK_CONTROL)&0x8000;

	INT nCols=app.m_MainWindow.m_PlayList.GetColumnCount();
	INT nOrder[NUMBER_OF_COLUMNS];
	Header_GetOrderArray(app.m_MainWindow.m_PlayList.m_hWndHeader,nCols,nOrder);
	for(INT c=0;c<nCols;c++)
		if(nOrder[c]==m_iSubItem)
		{
			m_iSubItem=c;
			break;
		}

	if(!bCtrl)
		m_iSubItem+=(bShift?-1:1);
	else
	{
		m_iItem+=(bShift?-1:1);
		if(m_iItem==(bShift?-1:app.m_MainWindow.m_PlayList.GetDisplayedCount()))
			m_iItem=(bShift?app.m_MainWindow.m_PlayList.GetDisplayedCount()-1:0);
	}
	if(m_iSubItem==(bShift?-1:nCols))
	{
		m_iItem+=(bShift?-1:1);
		if(m_iItem==(bShift?-1:app.m_MainWindow.m_PlayList.GetDisplayedCount()))
			m_iItem=(bShift?app.m_MainWindow.m_PlayList.GetDisplayedCount()-1:0);
		m_iSubItem=(bShift?nCols-1:0);
	}

	ReadListItem(m_iItem,nOrder[m_iSubItem]);
	return TRUE;
}

BOOL CLabelEdit::IsAdvancing()
{
	return m_bAdvancing;
}

void CLabelEdit::Destroy()
{
	m_bDestroying=TRUE;
	CWnd::Destroy();
	m_bDestroying=FALSE;
	app.EnableAccels(TRUE);
}