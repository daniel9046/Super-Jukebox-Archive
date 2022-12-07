// Wnd.h: interface for the CWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WND_H__67ACD760_8995_11D4_B2F9_00D0B7AE4D79__INCLUDED_)
#define AFX_WND_H__67ACD760_8995_11D4_B2F9_00D0B7AE4D79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CWnd  
{
public:
	virtual void DrawColorButton(const DRAWITEMSTRUCT *lpDrawItem);
	virtual void SetRedraw(BOOL bRedraw);
	virtual BOOL CarryOutActions();
	virtual BOOL Validate();
	virtual HWND Create(UINT uID,CWnd *pParentWnd,HINSTANCE hInstance);
	virtual INT DoModal(UINT uID, CWnd *pParentWnd,HINSTANCE hInstance);
	HDC m_hDC;
	BOOL ScreenToClient(HWND hWnd,LPRECT lpRect);
	BOOL ClientToScreen(HWND hWnd,LPRECT lpRect);
	virtual void IsVisible(BOOL bVisible);
	virtual BOOL IsVisible();
	virtual BOOL IsActive();
	virtual void Destroy();
	virtual BOOL RePaint(CONST RECT *lprcUpdate);
	virtual BOOL Move(LPRECT lprc,BOOL bRepaint);
	virtual UINT GetID();
	HWND GetSafeHWnd();
	virtual BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, DWORD dwExStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, HINSTANCE hInstance);
	virtual LRESULT SubWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	static LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	CWnd();
	virtual ~CWnd();
	HWND m_hWnd;
	CWnd *m_pParentWnd;
	UINT m_uID;
	HINSTANCE m_hInstance;

protected:
	virtual LRESULT OnHelp(HWND hwnd,LPHELPINFO lpHI);
	virtual void OnEndSession(HWND hwnd, BOOL fEnding);
	virtual BOOL OnQueryEndSession(HWND hwnd);
	virtual void OnDropFiles(HWND hwnd, HDROP hdrop);
	virtual void OnSysCommand(HWND hwnd, UINT cmd, int x, int y);
	virtual BOOL OnSizing(HWND hwnd,INT nSide,LPRECT lprc);
	virtual UINT OnNCCalcSize(HWND hwnd, BOOL fCalcValidRects, NCCALCSIZE_PARAMS * lpcsp);
	virtual void OnNCPaint(HWND hwnd, HRGN hrgn);
	virtual void OnWindowPosChanged(HWND hwnd, const LPWINDOWPOS lpwpos);
	virtual BOOL OnWindowPosChanging(HWND hwnd, LPWINDOWPOS lpwpos);
	virtual void OnNotifyIcon(UINT uID,UINT uMouseMsg);
	virtual BOOL OnDeleteItem(INT iItem);
	virtual void OnMove(HWND hwnd, int x, int y);
	virtual void OnChar(HWND hwnd, TCHAR ch, int cRepeat);
	virtual void OnKillFocus(HWND hwnd, HWND hwndNewFocus);
	virtual LRESULT OnCtlColorBtn(HWND hwnd,HDC hdc,HWND hwndButton,INT i);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	virtual BOOL OnEnsureVisible(INT nIndex,BOOL bPartialOK);
	virtual BOOL OnScroll(INT dx,INT dy);
	virtual void OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
	virtual void OnClose(HWND hwnd);
	virtual void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg);
	virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnSysColorChange(HWND hwnd);
	virtual void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
	virtual HWND OnNextDlgCtl(HWND hwnd, HWND hwndSetFocus, BOOL fNext);
	virtual UINT OnGetDlgCode(HWND hwnd, LPMSG lpmsg);
	virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
	virtual void OnSetFocus(HWND hwnd, HWND hwndOldFocus);
	virtual void OnPaint(HWND hwnd);
	virtual LRESULT OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr);
	virtual LRESULT OnInitMenu(HWND hwnd, HMENU hMenu);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual LRESULT OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags);
	virtual LRESULT OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
	virtual LRESULT OnSetDefID(UINT uID);
	virtual LRESULT OnGetDefID();
	virtual LRESULT OnTimer(HWND hwnd, UINT id);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	BOOL m_bActive;
	BOOL m_bRedraw;
	WNDPROC m_OriginalWndProc;
	
private:
	INT m_nModalResult;
	static CWnd* pGlobalWnd;
	BOOL m_bModal;
	BOOL m_bDialog;
	stack<HWND>m_OriginalHWnd;
	stack<UINT>m_OriginalUMsg;
	stack<WPARAM>m_OriginalWParam;
	stack<LPARAM>m_OriginalLParam;	
};

#endif // !defined(AFX_WND_H__67ACD760_8995_11D4_B2F9_00D0B7AE4D79__INCLUDED_)
