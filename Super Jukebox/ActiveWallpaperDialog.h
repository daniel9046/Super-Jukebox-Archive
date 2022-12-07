// ActiveWallpaperDialog.h: interface for the CActiveWallpaperDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVEWALLPAPERDIALOG_H__6F7CE9C0_B48E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_ACTIVEWALLPAPERDIALOG_H__6F7CE9C0_B48E_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "ActiveWallpaper.h"

class CActiveWallpaperDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CActiveWallpaperDialog();
	virtual ~CActiveWallpaperDialog();

protected:
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	CActiveWallpaper m_aw;
	vector<TCHAR*>m_Names;
	vector<TCHAR*>m_DllNames;
	TCHAR m_szCurrentPlugin[MAX_PATH];
	INT m_nPriority;
	BOOL m_bDblBuffer;
};

#endif // !defined(AFX_ACTIVEWALLPAPERDIALOG_H__6F7CE9C0_B48E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
