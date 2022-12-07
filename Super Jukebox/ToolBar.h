// ToolBar.h: interface for the CToolBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBAR_H__C2338183_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_TOOLBAR_H__C2338183_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"
#include "List.h"
#include "Main.h"

class CToolBar : public CWnd  
{
public:
	LRESULT OnFilterView();
	void EnableButton(UINT uCmdId,BOOL bEnable);
	LRESULT OnAW();
	void CheckButton(UINT uCmdId,BOOL bCheck);
	LRESULT OnWAVDump();
	BOOL SaveReBarInfo();
	BOOL RemoveReBarBand();
	BOOL AddReBarBand();
	void DisplayToolTips(BOOL bDisplay);
	void SetSongList(CList* SongList);
	void IsChanged(BOOL bChanged);
	BOOL GenerateButtons();
	virtual BOOL IsVisible();
	void Destroy();
	void IsVisible(BOOL bVisible);
	BOOL HasPictures(BOOL bHasPictures);
	BOOL HasPictures();
	BOOL HasTextLabels(BOOL bHasTextLabels);
	BOOL HasTextLabels();
	LRESULT OnAdd();
	LRESULT OnRemove();
	LRESULT OnAddAll();
	LRESULT OnRemoveAll();
	LRESULT OnPlay();
	LRESULT OnPause();
	LRESULT OnStop();
	LRESULT OnPrevious();
	LRESULT OnNext();
	LRESULT OnClicked(UINT uID);
	BOOL RegisterClass(HINSTANCE hInstance);
	BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	CToolBar();
	virtual ~CToolBar();
	CWnd m_Pager;

protected:
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

private:
	UINT m_uOldRowCount;
	LRESULT OnVisMode();
	LRESULT OnLayout();
	INT m_nReBarBandIndex;
	CList* m_SongList;
	BOOL m_bChanged;
	BOOL m_bHasPictures,m_bHasTextLabels;;
	HIMAGELIST m_hImlGrayIcons,m_hImlColorIcons;
	struct{TCHAR szName[50];INT idCommand,iBitmap,iString;BYTE bStyle;}m_Buttons[NUMBER_OF_BUTTONS];
};

#endif // !defined(AFX_TOOLBAR_H__C2338183_8FF2_11D4_B2FA_00D0B7AE5907__INCLUDED_)
