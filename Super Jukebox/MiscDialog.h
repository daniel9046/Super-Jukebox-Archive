// MiscDialog.h: interface for the CMiscDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MISCDIALOG_H__9ED54BC2_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_MISCDIALOG_H__9ED54BC2_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CMiscDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CMiscDialog();
	virtual ~CMiscDialog();

protected:
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	BOOL m_bUseFastSeek;
	BOOL m_bAutoLoadLastPlayList;
	BOOL m_bAutoPlayLastSong;
	BOOL m_bAutoSizePlayListColumns;
	BOOL m_bMinimizeToSysTray;
	BOOL m_bReadID666Tags;
	BOOL m_bSaveID666Tags;
	BOOL m_bDisplayToolTips;
	DWORD m_dwSysTrayCmd;
};

#endif // !defined(AFX_MISCDIALOG_H__9ED54BC2_AE37_11D4_B2F9_00D0B7AE5907__INCLUDED_)
