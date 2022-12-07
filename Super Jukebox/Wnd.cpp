// Wnd.cpp: implementation of the CWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Wnd.h"
#include "Main.h"
#include "Application.h"

CWnd* CWnd::pGlobalWnd;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWnd::CWnd()
{
	m_bRedraw=TRUE;
	m_OriginalWndProc=DefWindowProc;
	m_hWnd=NULL;
	m_pParentWnd=NULL;
	m_hInstance=0;
	m_uID=0;
	m_bActive=FALSE;
	m_OriginalHWnd.empty();
	m_OriginalUMsg.empty();
	m_OriginalWParam.empty();
	m_OriginalLParam.empty();
}

CWnd::~CWnd()
{

}

BOOL CWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, const RECT &rect, CWnd *pParentWnd, UINT nID,HINSTANCE hInstance)
{
	m_pParentWnd=pParentWnd;
	m_uID=nID;
	m_hInstance=hInstance;

	m_bDialog=m_bModal=FALSE;
	pGlobalWnd=this;
	m_hWnd=CreateWindowEx(dwExStyle,lpszClassName,lpszWindowName,dwStyle,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,m_pParentWnd->GetSafeHWnd(),(HMENU)m_uID,m_hInstance,NULL);
	if(!m_hWnd)
		return FALSE;
	return (m_bActive=TRUE);
}

HWND CWnd::Create(UINT uID, CWnd *pParentWnd, HINSTANCE hInstance)
{
	m_uID=uID;
	m_pParentWnd=pParentWnd;
	m_hInstance=hInstance;

	m_bDialog=TRUE;
	m_bModal=FALSE;
	pGlobalWnd=this;
	m_hWnd=CreateDialogParam(m_hInstance,MAKEINTRESOURCE(m_uID),m_pParentWnd->m_hWnd,(DLGPROC)WindowProc,m_uID);
	m_bActive=(m_hWnd!=NULL);
	return m_hWnd;
}

INT CWnd::DoModal(UINT uID, CWnd *pParentWnd, HINSTANCE hInstance)
{
	m_uID=uID;
	m_pParentWnd=pParentWnd;
	m_hInstance=hInstance;

	m_bDialog=TRUE;
	m_bModal=TRUE;
	m_bActive=TRUE;
	m_nModalResult=IDCANCEL;
	pGlobalWnd=this;
	INT nRes=DialogBoxParam(m_hInstance,MAKEINTRESOURCE(m_uID),m_pParentWnd->m_hWnd,(DLGPROC)WindowProc,m_uID);
	m_bActive=FALSE;
	return nRes;
}

LRESULT CALLBACK CWnd::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	CWnd *pWnd=(CWnd*)GetWindowLong(hWnd,GWL_USERDATA);
	if(pWnd==NULL)
	{
		pWnd=pGlobalWnd;
		SetWindowLong(hWnd,GWL_USERDATA,(LONG)pWnd);
	}

	return pWnd->SubWindowProc(hWnd,uMsg,wParam,lParam);
}

LRESULT CWnd::SubWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	m_OriginalHWnd.push(m_hWnd=hWnd);
	m_OriginalUMsg.push(uMsg);
	m_OriginalWParam.push(wParam);
	m_OriginalLParam.push(lParam);

	switch(uMsg)
	{
	case WM_HELP:lResult=OnHelp(hWnd,(LPHELPINFO)lParam);break;
	case WM_ENDSESSION:lResult=HANDLE_WM_ENDSESSION(hWnd,wParam,lParam,OnEndSession);break;
	case WM_QUERYENDSESSION:lResult=HANDLE_WM_QUERYENDSESSION(hWnd,wParam,lParam,OnQueryEndSession);break;
	case WM_DROPFILES:lResult=HANDLE_WM_DROPFILES(hWnd,wParam,lParam,OnDropFiles);break;
	case WM_SYSCOMMAND:lResult=HANDLE_WM_SYSCOMMAND(hWnd,wParam,lParam,OnSysCommand);break;
	case WM_SIZING:lResult=OnSizing(hWnd,(INT)wParam,(LPRECT)lParam);break;
	case WM_NCCALCSIZE:lResult=HANDLE_WM_NCCALCSIZE(hWnd,wParam,lParam,OnNCCalcSize);break;
	case WM_NCPAINT:lResult=HANDLE_WM_NCPAINT(hWnd,wParam,lParam,OnNCPaint);break;
	case WM_WINDOWPOSCHANGED:lResult=HANDLE_WM_WINDOWPOSCHANGED(hWnd,wParam,lParam,OnWindowPosChanged);break;
	case WM_WINDOWPOSCHANGING:lResult=HANDLE_WM_WINDOWPOSCHANGING(hWnd,wParam,lParam,OnWindowPosChanging);break;
	case WM_NOTIFYICON:lResult=(OnNotifyIcon(wParam,lParam),0);break;
	case WM_MOVE:lResult=HANDLE_WM_MOVE(hWnd,wParam,lParam,OnMove);break;
	case WM_CHAR:lResult=HANDLE_WM_CHAR(hWnd,wParam,lParam,OnChar);break;
	case WM_KILLFOCUS:lResult=HANDLE_WM_KILLFOCUS(hWnd,wParam,lParam,OnKillFocus);break;
	case WM_CTLCOLORBTN:lResult=HANDLE_WM_CTLCOLORBTN(hWnd,wParam,lParam,OnCtlColorBtn);break;
	case WM_INITDIALOG:lResult=HANDLE_WM_INITDIALOG(hWnd,wParam,lParam,OnInitDialog);break;
	case LVM_SCROLL:lResult=OnScroll(wParam,lParam);break;
	case LVM_ENSUREVISIBLE:lResult=OnEnsureVisible(wParam,lParam);break;
	case LVM_DELETEITEM:lResult=OnDeleteItem(wParam);break;
	case WM_HSCROLL:lResult=HANDLE_WM_HSCROLL(hWnd,wParam,lParam,OnHScroll);break;
	case WM_VSCROLL:lResult=HANDLE_WM_VSCROLL(hWnd,wParam,lParam,OnVScroll);break;
	case WM_KEYDOWN:lResult=HANDLE_WM_KEYDOWN(hWnd,wParam,lParam,OnKey);break;
	case WM_KEYUP:lResult=HANDLE_WM_KEYUP(hWnd,wParam,lParam,OnKey);break;
	case WM_ERASEBKGND:lResult=HANDLE_WM_ERASEBKGND(hWnd,wParam,lParam,OnEraseBkgnd);break;
	case WM_CLOSE:lResult=HANDLE_WM_CLOSE(hWnd,wParam,lParam,OnClose);break;
	case WM_LBUTTONDOWN:lResult=HANDLE_WM_LBUTTONDOWN(hWnd,wParam,lParam,OnLButtonDown);break;
	case WM_LBUTTONUP:lResult=HANDLE_WM_LBUTTONUP(hWnd,wParam,lParam,OnLButtonUp);break;
	case WM_LBUTTONDBLCLK:lResult=HANDLE_WM_LBUTTONDBLCLK(hWnd,wParam,lParam,OnLButtonDown);break;
	case WM_RBUTTONDOWN:lResult=HANDLE_WM_RBUTTONDOWN(hWnd,wParam,lParam,OnRButtonDown);break;
	case WM_RBUTTONUP:lResult=HANDLE_WM_RBUTTONUP(hWnd,wParam,lParam,OnRButtonUp);break;
	case WM_RBUTTONDBLCLK:lResult=HANDLE_WM_RBUTTONDBLCLK(hWnd,wParam,lParam,OnRButtonDown);break;
	case WM_SETCURSOR:lResult=HANDLE_WM_SETCURSOR(hWnd,wParam,lParam,OnSetCursor);break;
	case WM_MOUSEMOVE:lResult=HANDLE_WM_MOUSEMOVE(hWnd,wParam,lParam,OnMouseMove);break;
	case WM_SYSCOLORCHANGE:lResult=HANDLE_WM_SYSCOLORCHANGE(hWnd,wParam,lParam,OnSysColorChange);break;
	case WM_GETMINMAXINFO:lResult=HANDLE_WM_GETMINMAXINFO(hWnd,wParam,lParam,OnGetMinMaxInfo);break;
	case WM_NEXTDLGCTL:lResult=HANDLE_WM_NEXTDLGCTL(hWnd,wParam,lParam,OnNextDlgCtl);break;
	case WM_GETDLGCODE:lResult=HANDLE_WM_GETDLGCODE(hWnd,wParam,lParam,OnGetDlgCode);break;
	case WM_DRAWITEM:lResult=HANDLE_WM_DRAWITEM(hWnd,wParam,lParam,OnDrawItem);break;
	case WM_SETFOCUS:lResult=HANDLE_WM_SETFOCUS(hWnd,wParam,lParam,OnSetFocus);break;
	case WM_PAINT:lResult=HANDLE_WM_PAINT(hWnd,wParam,lParam,OnPaint);break;
	case WM_CREATE:lResult=HANDLE_WM_CREATE(hWnd,wParam,lParam,OnCreate);break;
	case WM_DESTROY:lResult=HANDLE_WM_DESTROY(hWnd,wParam,lParam,OnDestroy);break;
	case WM_TIMER:lResult=HANDLE_WM_TIMER(hWnd,wParam,lParam,OnTimer);break;
	case WM_ACTIVATE:lResult=HANDLE_WM_ACTIVATE(hWnd,wParam,lParam,OnActivate);break;
	case WM_MENUSELECT:lResult=HANDLE_WM_MENUSELECT(hWnd,wParam,lParam,OnMenuSelect);break;
	case WM_SIZE:lResult=HANDLE_WM_SIZE(hWnd,wParam,lParam,OnSize);break;
	case WM_COMMAND:lResult=HANDLE_WM_COMMAND(hWnd,wParam,lParam,OnCommand);break;
	case WM_INITMENU:lResult=HANDLE_WM_INITMENU(hWnd,wParam,lParam,OnInitMenu);break;
	case WM_NOTIFY:lResult=HANDLE_WM_NOTIFY(hWnd,wParam,lParam,OnNotify);break;
	case DM_GETDEFID:lResult=OnGetDefID();break;
	case DM_SETDEFID:lResult=OnSetDefID(wParam);break;
	default:lResult=(m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,hWnd,uMsg,wParam,lParam));break;
	}
	m_OriginalHWnd.pop();
	m_OriginalUMsg.pop();
	m_OriginalWParam.pop();
	m_OriginalLParam.pop();
	return lResult;
}

BOOL CWnd::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top())?FALSE:TRUE;
}

LRESULT CWnd::OnDestroy(HWND hwnd)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnTimer(HWND hwnd, UINT id)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if(m_bDialog)
	{
		BOOL bResult=FALSE;

		switch(codeNotify)
		{
		case BN_CLICKED:
			m_nModalResult=id;
			switch(id)
			{
			case IDOK:
				if(Validate())
					Destroy();
				bResult=TRUE;
				break;
			case IDCANCEL:
				Destroy();
				bResult=TRUE;
				break;
			}
			break;
		}
		return bResult;
	}
	else
	{
		return CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
	}
}

LRESULT CWnd::OnInitMenu(HWND hwnd, HMENU hMenu)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnGetDefID()
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnSetDefID(UINT uID)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}


HWND CWnd::GetSafeHWnd()
{
	return this?m_hWnd:NULL;
}

void CWnd::OnPaint(HWND hwnd)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

UINT CWnd::OnGetDlgCode(HWND hwnd, LPMSG lpmsg)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

UINT CWnd::GetID()
{
	return m_uID;
}

HWND CWnd::OnNextDlgCtl(HWND hwnd, HWND hwndSetFocus, BOOL fNext)
{
	return m_bDialog?FALSE:(HWND)CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::Move(LPRECT lprc,BOOL bRepaint)
{
	if(!IsWindow(m_hWnd))
		return FALSE;

	return MoveWindow(m_hWnd,lprc->left,lprc->top,lprc->right-lprc->left,lprc->bottom-lprc->top,bRepaint);
}

void CWnd::OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::RePaint(CONST RECT *lprcUpdate)
{
	if(!IsWindow(m_hWnd))
		return FALSE;

	return RedrawWindow(m_hWnd,lprcUpdate,NULL,RDW_ERASE|RDW_INVALIDATE|RDW_ALLCHILDREN|RDW_UPDATENOW);
}

void CWnd::Destroy()
{
	if(!IsWindow(m_hWnd))
		return;

	(m_bDialog&&m_bModal)?EndDialog(m_hWnd,m_nModalResult):DestroyWindow(m_hWnd);
	m_hWnd=NULL;
	m_bActive=FALSE;
}

BOOL CWnd::IsActive()
{
	return m_bActive;
}

BOOL CWnd::IsVisible()
{
	return IsWindowVisible(m_hWnd);
}

void CWnd::IsVisible(BOOL bVisible)
{
	//if(IsVisible()!=bVisible)
		ShowWindow(m_hWnd,bVisible?SW_SHOW:SW_HIDE);
}

void CWnd::OnSysColorChange(HWND hwnd)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::ClientToScreen(HWND hWnd, LPRECT lpRect)
{
	POINT pt;
	pt.x=lpRect->left;
	pt.y=lpRect->top;
	if(!::ClientToScreen(hWnd,&pt))
		return FALSE;
	lpRect->left=pt.x;
	lpRect->top=pt.y;

	pt.x=lpRect->right;
	pt.y=lpRect->bottom;
	if(!::ClientToScreen(hWnd,&pt))
		return FALSE;
	lpRect->right=pt.x;
	lpRect->bottom=pt.y;
	return TRUE;
}

BOOL CWnd::ScreenToClient(HWND hWnd, LPRECT lpRect)
{
	POINT pt;
	pt.x=lpRect->left;
	pt.y=lpRect->top;
	if(!::ScreenToClient(hWnd,&pt))
		return FALSE;
	lpRect->left=pt.x;
	lpRect->top=pt.y;

	pt.x=lpRect->right;
	pt.y=lpRect->bottom;
	if(!::ScreenToClient(hWnd,&pt))
		return FALSE;
	lpRect->right=pt.x;
	lpRect->bottom=pt.y;
	return TRUE;
}

void CWnd::OnClose(HWND hwnd)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
	return m_bDialog?FALSE:m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnScroll(INT dx, INT dy)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnEnsureVisible(INT nIndex, BOOL bPartialOK)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnDeleteItem(INT iItem)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	HWND hwndOwner;
	RECT rc,rcDlg,rcOwner;

	if((hwndOwner=GetParent(m_hWnd))==NULL)
		hwndOwner=GetDesktopWindow();

	GetWindowRect(hwndOwner,&rcOwner);
	GetWindowRect(m_hWnd,&rcDlg);
	CopyRect(&rc,&rcOwner);

	OffsetRect(&rcDlg,-rcDlg.left,-rcDlg.top);
	OffsetRect(&rc,-rc.left,-rc.top);
	OffsetRect(&rc,-rcDlg.right,-rcDlg.bottom);

	SetWindowPos(m_hWnd,HWND_TOP,rcOwner.left+(rc.right/2),rcOwner.top+(rc.bottom/2),0,0,SWP_NOSIZE);
	return TRUE;
}

BOOL CWnd::Validate()
{
	return TRUE;
}

BOOL CWnd::CarryOutActions()
{
	return TRUE;
}

LRESULT CWnd::OnCtlColorBtn(HWND hwnd, HDC hdc, HWND hwndButton, INT i)
{
	return FALSE;
}

void CWnd::OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::SetRedraw(BOOL bRedraw)
{
	SendMessage(m_hWnd,WM_SETREDRAW,(WPARAM)(m_bRedraw=bRedraw),0);
}

void CWnd::OnMove(HWND hwnd, int x, int y)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnNotifyIcon(UINT uID, UINT uMouseMsg)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnWindowPosChanged(HWND hwnd, const LPWINDOWPOS lpwpos)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnNCPaint(HWND hwnd, HRGN hrgn)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

UINT CWnd::OnNCCalcSize(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS *lpcsp)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

BOOL CWnd::OnSizing(HWND hwnd,INT nSide, LPRECT lprc)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnSysCommand(HWND hwnd, UINT cmd, int x, int y)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnDropFiles(HWND hwnd, HDROP hdrop)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::DrawColorButton(const DRAWITEMSTRUCT *lpDrawItem)
{
	BOOL bSelected=lpDrawItem->itemState&ODS_SELECTED;
	BOOL bDisabled=lpDrawItem->itemState&ODS_DISABLED;
	BOOL bDefault=lpDrawItem->itemState&ODS_DEFAULT;
	BOOL bFocus=lpDrawItem->itemState&ODS_FOCUS;

	HDC hDC=lpDrawItem->hDC;
	RECT rc=lpDrawItem->rcItem;

	DrawFrameControl(hDC,&rc,DFC_BUTTON,DFCS_BUTTONPUSH|(bSelected?DFCS_PUSHED:0)|(bDisabled?DFCS_INACTIVE:0)|(bDefault?DFCS_MONO:0));

	HPEN old_pn=(HPEN)SelectObject(hDC,GetStockObject(BLACK_PEN));
	HBRUSH br=CreateSolidBrush(GetWindowLong(lpDrawItem->hwndItem,GWL_USERDATA));
	HBRUSH old_br=(HBRUSH)SelectObject(hDC,br);
	rc.left+=5;rc.top+=4;
	rc.right-=5;rc.bottom-=4;
	Rectangle(hDC,rc.left+bSelected,rc.top+bSelected,rc.right+bSelected,rc.bottom+bSelected);
	SelectObject(hDC,old_pn);
	SelectObject(hDC,old_br);
	DeleteObject(br);

	if(bFocus)
	{
		rc.left-=2;rc.top--;
		rc.right+=2;rc.bottom++;
		DrawFocusRect(hDC,&rc);
	}
}

BOOL CWnd::OnQueryEndSession(HWND hwnd)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CWnd::OnEndSession(HWND hwnd, BOOL fEnding)
{
	if(!m_bDialog)
		CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CWnd::OnHelp(HWND hwnd,LPHELPINFO lpHI)
{
	return m_bDialog?FALSE:CallWindowProc(m_OriginalWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}
