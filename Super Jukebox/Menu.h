// Menu.h: interface for the CMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENU_H__E60D94C3_83C6_11D4_8925_009027C5CF93__INCLUDED_)
#define AFX_MENU_H__E60D94C3_83C6_11D4_8925_009027C5CF93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "ListEntry.h"

class CMenu  
{
public:
	LRESULT OnEditInvertSelection();
	LRESULT OnHelpContents();
	LRESULT OnFileSaveAsHTML();
	LRESULT OnViewToolbarsWrap();
	LRESULT OnFileExportID666Tags();
	LRESULT OnViewResizeColumns();
	LRESULT OnViewToolbarsSourceBar();
	LRESULT OnFileImportID666Tags();
	LRESULT OnViewShuffle();
	LRESULT OnWindowTileVertically();
	LRESULT OnWindowTileHorizontally();
	LRESULT OnWindowControlPanel();
	LRESULT OnWindowVisualizer();
	LRESULT OnWindowPlayList();
	LRESULT OnWindowFileList();
	LRESULT OnOptionsRestoreDefaultSettings();
	LRESULT OnOptionsSaveSettingsNow();
	LRESULT OnOptionsSaveSettingsOnExit();
	LRESULT OnViewMenu();
	LRESULT OnViewTitleBar();
	LRESULT OnEditEditItem();
	LRESULT OnViewToolbarsCustomize();
	LRESULT OnViewFilter();
	LRESULT OnViewRefresh();
	LRESULT OnOptionsConfigure();
	LRESULT OnEditSelectAll();
	LRESULT OnEditDelete();
	LRESULT OnEditPaste();
	LRESULT OnEditCopy();
	LRESULT OnEditCut();
	LRESULT OnFileProperties();
	LRESULT OnFileExit();
	LRESULT OnFileNew();
	LRESULT OnFileSaveAs();
	LRESULT OnFileSave();
	LRESULT OnFileOpen();
	LRESULT OnViewToolbarsPictures();
	LRESULT OnViewToolbarsTextLabels();
	LRESULT OnViewToolbarsStandardButtons();
	LRESULT OnEditGoTo();
	LRESULT OnEditFind();
	LRESULT OnHelpAboutSuperJukebox();
	BOOL IsActive();
	INT GetSelectedID();
	LRESULT OnViewStatusBar();
	LRESULT OnMenuSelect(HWND hwnd, HMENU hmenu, int item, HMENU hmenuPopup, UINT flags);
	LRESULT OnInitMenu(HWND hwnd, HMENU hMenu);
	BOOL Create(HINSTANCE hInstance,CWnd *pParentWnd,UINT uID);
	CMenu();
	virtual ~CMenu();
	HMENU m_hMenu;

private:
	vector<CListEntry>m_ClipBoard;
	BOOL m_bActive;
	INT m_nSelectedID;
	HINSTANCE m_hInstance;
	CWnd *m_pParentWnd;
	UINT m_uID;	
};

#endif // !defined(AFX_MENU_H__E60D94C3_83C6_11D4_8925_009027C5CF93__INCLUDED_)
