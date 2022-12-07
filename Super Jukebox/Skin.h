// Skin.h: interface for the CSkin class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKIN_H__46A6C6E4_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_SKIN_H__46A6C6E4_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkin  
{
public:
	void IsModified(BOOL bModified);
	BOOL IsModified();
	void Reset();
	INT GetBlendingLevel();
	COLORREF GetBlendingColor();
	BOOL IsBlended();
	void IsBlended(BOOL bBlend);
	COLORREF GetTextColor();
	void SetTextColor(COLORREF cr);
	void SetFileName(LPCTSTR lpszFileName);
	BOOL IsActive(BOOL bActive);
	void Destroy();
	void SetBlendingLevel(INT nBlendingLevel);
	void SetBlendingColor(COLORREF cr);
	LPCTSTR GetFileName();
	INT GetLayout();
	void SetLayout(INT nLayout);
	HDC GetDC();
	BOOL IsActive();
	UINT GetHeight();
	UINT GetWidth();
	BOOL Refresh();
	BOOL Load();
	CSkin();
	virtual ~CSkin();
	HDC m_hDCAdjusted;

private:
	BOOL m_bModified;
	COLORREF m_crTextColor;
	BOOL m_bBlend;
	INT m_nBlendingLevel;
	COLORREF m_crBlendingColor;
	BOOL m_bActive;
	INT m_nLayout;
	TCHAR m_szFileName[MAX_PATH];
	HDC m_hDCOriginal;
	HBITMAP m_hBmpOriginal;
	HBITMAP m_hBmpAdjusted;
	UINT m_uSkinWidth;
	UINT m_uSkinHeight;
};

#endif // !defined(AFX_SKIN_H__46A6C6E4_988E_11D4_B2F9_00D0B7AE5907__INCLUDED_)
