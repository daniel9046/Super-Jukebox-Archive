// MainWindow.h: interface for the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__CE9216E5_832B_11D4_8925_009027C5CF93__INCLUDED_)
#define AFX_MAINWINDOW_H__CE9216E5_832B_11D4_8925_009027C5CF93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Main.h"
#include "PlayList.h"
#include "FileList.h"
#include "Menu.h"
#include "StatusBar.h"
#include "FindDialog.h"
#include "GotoDialog.h"	// Added by ClassView
#include "ToolBar.h"	// Added by ClassView
#include "VisWindow.h"
#include "PlaylistFilterDialog.h"	// Added by ClassView
#include "ControlPanelDialog.h"
#include "ActiveWallpaper.h"	// Added by ClassView
#include "SourceBar.h"	// Added by ClassView
#include "ReBar.h"	// Added by ClassView
#include "ToolbarCustomizeDialog.h"

class CMainWindow : public CWnd
{
public:
	HMENU m_hSysMenu;
	CToolbarCustomizeDialog m_ToolbarCustomizeDlg;
	CReBar m_ReBar;
	CSourceBar m_SourceBar;
	CActiveWallpaper m_ActiveWallpaper;
	CControlPanelDialog m_ControlPanel;
	BOOL m_bDontPaint;
	void SyncRegistrySettings();
	void RemoveTaskbarButton();
	void AddTaskbarButton();
	BOOL DoSystemTrayMenu(INT nXPos,INT nYPos);
	BOOL RemoveSystemTrayIcon();
	BOOL AddSystemTrayIcon();
	void SetCursor(HCURSOR hCursor);
	void MaintainCursor(BOOL bFlag);
	CSkin& GetActiveSkin();
	BOOL AdjustLayout();
	BOOL UpdateTitleBar();
	BOOL Create(int nCmdShow);
	BOOL RegisterClass(HINSTANCE hInstance);
	CMainWindow();
	virtual ~CMainWindow();
	CVisWindow m_VisWindow;
	CToolBar m_ToolBar;
	CGotoDialog m_GotoDlg;
	CFindDialog m_FindDlg;
	CPlaylistFilterDialog m_PlaylistFilterDlg;
	CPlayList m_PlayList;
	CFileList m_FileList;
	CMenu m_Menu;
	CStatusBar m_StatusBar;
	CSkin m_Skin;
	BOOL m_bAdjustingListViews;
	POINT m_StartPoint;
	RECT m_ClientRect;
	RECT m_SplitterRect;
	HWND m_hWndOldFocus;

protected:
	virtual BOOL OnQueryEndSession(HWND hwnd);
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual void OnDropFiles(HWND hwnd, HDROP hdrop);
	virtual void OnSysCommand(HWND hwnd, UINT cmd, int x, int y);
	virtual void OnNotifyIcon(UINT uID, UINT uMouseMsg);
	virtual BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
	void OnClose(HWND hwnd);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void OnSysColorChange(HWND hwnd);
	HWND OnNextDlgCtl(HWND hwnd, HWND hwndSetFocus, BOOL fNext);
	LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
	LRESULT OnDestroy(HWND hwnd);
	BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	LRESULT OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
	LRESULT OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr);
	LRESULT OnTimer(HWND hwnd, UINT id);
	LRESULT OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags);
	LRESULT OnInitMenu(HWND hwnd, HMENU hMenu);

private:
	BOOL m_bBetaBox;
	BOOL m_bPass;
	INT m_nOldMouseY;
	INT m_nOldMouseX;
	RECT m_OldWindowRect;
	BOOL m_bAdjustLayout;
	HCURSOR m_hCursor;
	BOOL m_bMaintainCursor;
};

#endif // !defined(AFX_MAINWINDOW_H__CE9216E5_832B_11D4_8925_009027C5CF93__INCLUDED_)
