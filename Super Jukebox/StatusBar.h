// StatusBar.h: interface for the CStatusBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSBAR_H__E60D94C0_83C6_11D4_8925_009027C5CF93__INCLUDED_)
#define AFX_STATUSBAR_H__E60D94C0_83C6_11D4_8925_009027C5CF93__INCLUDED_

#include "Wnd.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStatusBar : public CWnd
{
public:
	BOOL GetText(INT nPart,LPTSTR lpszBuf,INT nTextMax);
	BOOL Update();
	void SetText(int nPart,LPCTSTR lpszText);
	BOOL Create(HINSTANCE hInstance,CWnd *pParentWnd,UINT uID);
	BOOL RegisterClass(HINSTANCE hInstance);
	CStatusBar();
	virtual ~CStatusBar();

protected:
	virtual void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	virtual void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnPaint(HWND hwnd);
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnSize(HWND hwnd,UINT state,int cx,int cy);

private:
	SIZE m_OldSize;
	HDC m_hBackDC;
	HBRUSH m_hBackBrush;
	HBITMAP m_hBackBmp;
	BOOL m_bMouseOnTimeBar;
	UINT m_nParts[6];
};

#endif // !defined(AFX_STATUSBAR_H__E60D94C0_83C6_11D4_8925_009027C5CF93__INCLUDED_)
