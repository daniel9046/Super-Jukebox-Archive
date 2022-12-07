// PlayListDialog.h: interface for the CPlayListDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYLISTDIALOG_H__91862020_9483_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_PLAYLISTDIALOG_H__91862020_9483_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CPlayListDialog : public CWnd
{
public:
	CPlayListDialog();
	virtual ~CPlayListDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem);
	LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	LRESULT OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr);
	BOOL Validate();
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	UINT m_uMode;
	UINT m_uIndSongLength,m_uUniSongLength;
	UINT m_uIndSongFadeLength,m_uUniSongFadeLength;
	COLORREF m_CustomColors[16];
};

#endif // !defined(AFX_PLAYLISTDIALOG_H__91862020_9483_11D4_B2F9_00D0B7AE5907__INCLUDED_)
