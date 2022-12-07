// ConfigurationDialog.h: interface for the CConfigurationDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGURATIONDIALOG_H__F95698A2_95DD_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_CONFIGURATIONDIALOG_H__F95698A2_95DD_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ActiveWallpaperDialog.h"	// Added by ClassView
#include "MiscDialog.h"	// Added by ClassView
#include "PlayerDialog.h"	// Added by ClassView
#include "SkinDialog.h"	// Added by ClassView
#include "VisualizationDialog.h"	// Added by ClassView
#include "ViewDialog.h"
#include "ProgramDialog.h"	// Added by ClassView

class CConfigurationDialog : public CWnd
{
public:
	void SetInitialSelection(INT nInitialSelection);
	CConfigurationDialog();
	virtual ~CConfigurationDialog();

protected:
	virtual LRESULT OnHelp(HWND hwnd,LPHELPINFO lpHI);
	virtual void OnPaint(HWND hwnd);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	LRESULT OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr);
	BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	CActiveWallpaperDialog m_ActiveWallpaperDlg;
	BOOL m_bPass;
	CProgramDialog m_ProgramDlg;
	HWND m_hWndTree;
	CMiscDialog m_MiscDlg;
	void DrawBanner(HDC hDC,INT nWidth,INT nHeight);
	RECT m_rcChildDialog,m_rcBanner;
	HDC m_hDCBanner;
	HBITMAP m_hBmpBanner;
	INT m_nInitialSelection;
	BOOL LoadChildDialog(CWnd *dlg);
	TVITEM m_ActiveItem;
	CWnd *m_ActiveDlg;
	CPlayerDialog m_PlayerDlg;	
	CSkinDialog m_SkinDlg;
	CVisualizationDialog m_VisualizationDlg;
	CViewDialog m_ViewDlg;
};

#endif // !defined(AFX_CONFIGURATIONDIALOG_H__F95698A2_95DD_11D4_B2FA_00D0B7AE5907__INCLUDED_)
