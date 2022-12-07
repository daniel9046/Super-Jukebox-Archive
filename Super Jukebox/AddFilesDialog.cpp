// AddFilesDialog.cpp: implementation of the CAddFilesDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AddFilesDialog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAddFilesDialog::CAddFilesDialog()
{
	m_bCanceled=FALSE;
}

CAddFilesDialog::~CAddFilesDialog()
{

}

BOOL CAddFilesDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_bCanceled=FALSE;
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CAddFilesDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDCANCEL:
			m_bCanceled=TRUE;
			return 0;
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}

BOOL CAddFilesDialog::IsCanceled()
{
	return m_bCanceled;
}
