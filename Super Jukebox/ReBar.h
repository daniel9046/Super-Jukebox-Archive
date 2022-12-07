// ReBar.h: interface for the CReBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REBAR_H__10B6C3E4_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_REBAR_H__10B6C3E4_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CReBar : public CWnd  
{
public:
	BOOL GetBandInfo(UINT nIndex, LPREBARBANDINFO lprbbi);
	BOOL RegisterClass(HINSTANCE hInstance);
	BOOL SetBandInfo(UINT nIndex,LPREBARBANDINFO lprbbi);
	BOOL ShowBand(INT nBand,BOOL bShow);
	INT GetBandIndex(UINT uID);
	BOOL DeleteBand(UINT uIndex);
	BOOL AddBand(UINT uIndex,LPREBARBANDINFO lprbBand);
	BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	CReBar();
	virtual ~CReBar();

protected:
	virtual LRESULT OnNotify(HWND hwnd,INT nIdCtrl,NMHDR FAR* lpNMHdr);
};

#endif // !defined(AFX_REBAR_H__10B6C3E4_BBA8_11D4_B2FA_00D0B7AE5907__INCLUDED_)
