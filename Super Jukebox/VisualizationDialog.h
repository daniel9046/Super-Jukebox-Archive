// VisualizationDialog.h: interface for the CVisualizationDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISUALIZATIONDIALOG_H__943E5C60_A518_11D4_B2F8_00D0B79FA829__INCLUDED_)
#define AFX_VISUALIZATIONDIALOG_H__943E5C60_A518_11D4_B2F8_00D0B79FA829__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CVisualizationDialog : public CWnd  
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CVisualizationDialog();
	virtual ~CVisualizationDialog();
	static BOOL m_bActive;

protected:
	virtual void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT *lpDrawItem);
	virtual void OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	INT m_nMode;
	INT m_nSpectrumSize;
	INT m_nRate;
	INT m_nPriority;
	BOOL m_bShowVisLabels;
	INT m_nVisItem;
	COLORREF m_CustomColors[16];
};

#endif // !defined(AFX_VISUALIZATIONDIALOG_H__943E5C60_A518_11D4_B2F8_00D0B79FA829__INCLUDED_)
