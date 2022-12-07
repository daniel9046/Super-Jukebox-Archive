// SourceBar.h: interface for the CSourceBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOURCEBAR_H__10B6C3E2_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_SOURCEBAR_H__10B6C3E2_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CSourceBar : public CWnd  
{
public:
	void ValidateHistoryList();
	void DisplayToolTips(BOOL bDisplay);
	BOOL RefreshComboBoxContents();
	void Focus();
	BOOL SetLocationText(LPCTSTR szText);
	void IsVisible(BOOL bVisible);
	BOOL SaveReBarInfo();
	BOOL RemoveReBarBand();
	BOOL AddReBarBand();
	BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	BOOL RegisterClass(HINSTANCE hInstance);
	CSourceBar();
	virtual ~CSourceBar();

protected:
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual LRESULT OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr);
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	virtual void OnEditKillFocus(HWND hwnd, HWND hwndNewFocus);
	virtual void OnEditSetFocus(HWND hwnd, HWND hwndOldFocus);
	virtual void OnEditChar(HWND hwnd, TCHAR ch, int cRepeat);
	virtual void OnEditKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual LRESULT OnDestroy(HWND hwnd);

private:
	BOOL m_bDropped;
	INT m_nReBarBandIndex;
	static LRESULT CALLBACK EditWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	WNDPROC m_OldEditWndProc;
	HWND m_hWndToolBar,m_hWndComboBox,m_hWndEdit;
	HFONT m_hFont;
	HWND m_hWndStatic;
	HIMAGELIST m_hImlGrayOneUp,m_hImlColorOneUp;
	stack<HWND>m_OriginalHWnd;
	stack<UINT>m_OriginalUMsg;
	stack<WPARAM>m_OriginalWParam;
	stack<LPARAM>m_OriginalLParam;	
};

#endif // !defined(AFX_SOURCEBAR_H__10B6C3E2_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_)
