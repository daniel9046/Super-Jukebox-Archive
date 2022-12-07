// AddFilesDialog.h: interface for the CAddFilesDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADDFILESDIALOG_H__8F192340_F203_11D4_B2FB_00D0B7AE5907__INCLUDED_)
#define AFX_ADDFILESDIALOG_H__8F192340_F203_11D4_B2FB_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CAddFilesDialog : public CWnd  
{
public:
	BOOL IsCanceled();
	CAddFilesDialog();
	virtual ~CAddFilesDialog();

protected:
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
private:
	BOOL m_bCanceled;
};

#endif // !defined(AFX_ADDFILESDIALOG_H__8F192340_F203_11D4_B2FB_00D0B7AE5907__INCLUDED_)
