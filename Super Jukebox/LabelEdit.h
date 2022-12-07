// LabelEdit.h: interface for the CLabelEdit class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LABELEDIT_H__1E9DAA20_9B5D_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_LABELEDIT_H__1E9DAA20_9B5D_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CLabelEdit : public CWnd  
{
public:
	virtual void Destroy();
	BOOL IsAdvancing();
	BOOL AdvanceListItem();
	BOOL WriteListItem();
	BOOL ReadListItem(INT iItem,INT iSubItem);
	BOOL RegisterClass(HINSTANCE hInstance);
	BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	CLabelEdit();
	virtual ~CLabelEdit();

protected:
	virtual void OnChar(HWND hwnd, TCHAR ch, int cRepeat);
	virtual void OnKillFocus(HWND hwnd, HWND hwndNewFocus);
	virtual void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

private:
	TCHAR m_szOriginalText[256];
	BOOL m_bAdvancing;
	INT m_iItem,m_iSubItem;
	BOOL m_bDestroying;
};

#endif // !defined(AFX_LABELEDIT_H__1E9DAA20_9B5D_11D4_B2FA_00D0B7AE5907__INCLUDED_)
