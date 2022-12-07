// Skin.cpp: implementation of the CSkin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Skin.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkin::CSkin()
{
	m_bActive=FALSE;
	m_nLayout=0;
	m_szFileName[0]='\0';
	m_bBlend=FALSE;
	m_crBlendingColor=RGB(255,255,255);
	m_crTextColor=RGB(0,0,0);
	m_nBlendingLevel=128;
	m_hDCAdjusted=NULL;
	m_hBmpAdjusted=NULL;
	m_hDCOriginal=NULL;
	m_hBmpOriginal=NULL;
	m_bModified=FALSE;
}

CSkin::~CSkin()
{

}

BOOL CSkin::Load()
{
	if(m_bActive)
	{
		HDC hDC=::GetDC(app.m_MainWindow.m_hWnd);
		if(m_hDCOriginal)DeleteDC(m_hDCOriginal);
		m_hDCOriginal=CreateCompatibleDC(hDC);
		if(!m_hDCOriginal)return IsActive(FALSE);
		if(m_hBmpOriginal)DeleteObject(m_hBmpOriginal);
		m_hBmpOriginal=(HBITMAP)LoadImage(app.m_hInstance,m_szFileName,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if(!m_hBmpOriginal)return IsActive(FALSE);
		if(!SelectObject(m_hDCOriginal,m_hBmpOriginal))return IsActive(FALSE);
		BITMAP bitmap;
		if(!GetObject(m_hBmpOriginal,sizeof(BITMAP),&bitmap))return IsActive(FALSE);
		m_uSkinWidth=bitmap.bmWidth;
		m_uSkinHeight=bitmap.bmHeight;
		if(m_bBlend)
		{
			HDC hDC0=CreateCompatibleDC(hDC);
			if(!hDC0)return IsActive(FALSE);
			HBITMAP hBmp0=CreateCompatibleBitmap(hDC,m_uSkinWidth,m_uSkinHeight);
			if(!hBmp0)return IsActive(FALSE);
			if(!SelectObject(hDC0,hBmp0))return IsActive(FALSE);
			HBRUSH br=CreateSolidBrush(m_crBlendingColor);
			if(!br)return IsActive(FALSE);
			RECT rt={0,0,m_uSkinWidth,m_uSkinHeight};
			FillRect(hDC0,&rt,br);
			DeleteObject(br);
			BLENDFUNCTION bf;
			bf.BlendOp=AC_SRC_OVER;
			bf.BlendFlags=0;
			bf.SourceConstantAlpha=m_nBlendingLevel;
			bf.AlphaFormat=0;
			if(!AlphaBlend(m_hDCOriginal,0,0,m_uSkinWidth,m_uSkinHeight,hDC0,0,0,m_uSkinWidth,m_uSkinHeight,bf))return IsActive(FALSE);
			DeleteDC(hDC0);
			DeleteObject(hBmp0);
		}
		ReleaseDC(app.m_MainWindow.m_hWnd,hDC);
	}

	return TRUE;
}

BOOL CSkin::Refresh()
{
	if(!this)return FALSE;

	if(m_bActive)
	{
		if(app.m_MainWindow.m_ClientRect.right-app.m_MainWindow.m_ClientRect.left<0||app.m_MainWindow.m_ClientRect.bottom-app.m_MainWindow.m_ClientRect.top<0)
			return FALSE;
		HDC hDC=::GetDC(app.m_MainWindow.m_hWnd);
		if(m_hDCAdjusted)DeleteDC(m_hDCAdjusted);
		m_hDCAdjusted=CreateCompatibleDC(hDC);
		if(!m_hDCAdjusted)return IsActive(FALSE);
		if(m_hBmpAdjusted)DeleteObject(m_hBmpAdjusted);
		m_hBmpAdjusted=CreateCompatibleBitmap(hDC,app.m_MainWindow.m_ClientRect.right-app.m_MainWindow.m_ClientRect.left,app.m_MainWindow.m_ClientRect.bottom-app.m_MainWindow.m_ClientRect.top);
		if(!m_hBmpAdjusted)return IsActive(FALSE);
		if(!SelectObject(m_hDCAdjusted,m_hBmpAdjusted))return IsActive(FALSE);
		if(m_nLayout)
		{
			INT cbx=(app.m_MainWindow.m_ClientRect.right-app.m_MainWindow.m_ClientRect.left)/m_uSkinWidth;
			INT cby=(app.m_MainWindow.m_ClientRect.bottom-app.m_MainWindow.m_ClientRect.top)/m_uSkinHeight;
			for(int y=0;y<=cby;y++)
				for(int x=0;x<=cbx;x++)
					if(!BitBlt(m_hDCAdjusted,x*m_uSkinWidth,y*m_uSkinHeight,m_uSkinWidth,m_uSkinHeight,m_hDCOriginal,0,0,SRCCOPY))return IsActive(FALSE);
		}
		else
		{
			if(!StretchBlt(m_hDCAdjusted,0,0,app.m_MainWindow.m_ClientRect.right-app.m_MainWindow.m_ClientRect.left,app.m_MainWindow.m_ClientRect.bottom-app.m_MainWindow.m_ClientRect.top,m_hDCOriginal,0,0,m_uSkinWidth,m_uSkinHeight,SRCCOPY))return IsActive(FALSE);
		}
		ReleaseDC(app.m_MainWindow.m_hWnd,hDC);
	}
	return TRUE;
}

UINT CSkin::GetWidth()
{
	return m_uSkinWidth;
}

UINT CSkin::GetHeight()
{
	return m_uSkinHeight;
}

BOOL CSkin::IsActive()
{
	return m_bActive;
}

HDC CSkin::GetDC()
{
	return m_hDCAdjusted;
}

void CSkin::SetLayout(INT nLayout)
{
	if(m_nLayout!=nLayout)
		m_bModified=TRUE;
	m_nLayout=nLayout;
	if(m_nLayout>1)
		m_nLayout=1;
}

INT CSkin::GetLayout()
{
	return m_nLayout;
}

LPCTSTR CSkin::GetFileName()
{
	return m_szFileName;
}

void CSkin::SetBlendingColor(COLORREF cr)
{
	if(m_crBlendingColor!=cr)
		m_bModified=TRUE;
	m_crBlendingColor=cr;
}

void CSkin::SetBlendingLevel(INT nBlendingLevel)
{
	if(m_nBlendingLevel!=nBlendingLevel)
		m_bModified=TRUE;
	m_nBlendingLevel=nBlendingLevel;
}

void CSkin::Destroy()
{
	if(m_bActive)
	{
		if(m_hDCOriginal)DeleteDC(m_hDCOriginal);
		if(m_hBmpOriginal)DeleteObject(m_hBmpOriginal);
		if(m_hDCAdjusted)DeleteDC(m_hDCAdjusted);
		if(m_hBmpAdjusted)DeleteObject(m_hBmpAdjusted);
		m_hDCOriginal=NULL;
		m_hBmpOriginal=NULL;
		m_hDCAdjusted=NULL;
		m_hBmpAdjusted=NULL;
		IsActive(FALSE);
	}
}

BOOL CSkin::IsActive(BOOL bActive)
{
	if(m_bActive!=bActive)
		m_bModified=TRUE;
	m_bActive=bActive;
	return m_bActive;
}

void CSkin::SetFileName(LPCTSTR lpszFileName)
{
	if(stricmp(m_szFileName,lpszFileName))
		m_bModified=TRUE;
	strncpy(m_szFileName,lpszFileName,sizeof(m_szFileName));
}

void CSkin::SetTextColor(COLORREF cr)
{
	if(m_crTextColor!=cr)
		m_bModified=TRUE;
	m_crTextColor=cr;
}

COLORREF CSkin::GetTextColor()
{
	return m_crTextColor;
}

void CSkin::IsBlended(BOOL bBlend)
{
	if(m_bBlend!=bBlend)
		m_bModified=TRUE;
	m_bBlend=bBlend;
}

BOOL CSkin::IsBlended()
{
	return m_bBlend;
}

COLORREF CSkin::GetBlendingColor()
{
	return m_crBlendingColor;
}

INT CSkin::GetBlendingLevel()
{
	return m_nBlendingLevel;
}

void CSkin::Reset()
{
	Destroy();
	m_nLayout=0;
	m_szFileName[0]='\0';
	m_bBlend=FALSE;
	m_crBlendingColor=RGB(255,255,255);
	m_crTextColor=RGB(0,0,0);
	m_nBlendingLevel=128;
	m_bModified=FALSE;
}

BOOL CSkin::IsModified()
{
	return m_bModified;
}

void CSkin::IsModified(BOOL bModified)
{
	m_bModified=bModified;
}
