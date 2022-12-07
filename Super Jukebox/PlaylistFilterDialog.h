// PlaylistFilterDialog.h: interface for the CPlaylistFilterDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYLISTFILTERDIALOG_H__61E10980_A666_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_PLAYLISTFILTERDIALOG_H__61E10980_A666_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "Main.h"

class CPlaylistFilterDialog : public CWnd  
{
public:
	void RefreshComboBoxStates();
	CPlaylistFilterDialog();
	virtual ~CPlaylistFilterDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd, LPHELPINFO lpHI);
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

private:
	BOOL OnRemove();
	BOOL OnApply();
	void RefreshComboBoxContents();
	struct{TCHAR szBuffer[100];UINT uID;}m_Fields[NUMBER_OF_COLUMNS];
};

#endif // !defined(AFX_PLAYLISTFILTERDIALOG_H__61E10980_A666_11D4_B2F9_00D0B7AE5907__INCLUDED_)
