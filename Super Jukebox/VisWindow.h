// VisWindow.h: interface for the CVisWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VISWINDOW_H__24BA2382_A340_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_VISWINDOW_H__24BA2382_A340_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CVisWindow : public CWnd  
{
public:
	void MuteChannels(int id);
	BOOL Refresh();
	BOOL IsRunning();
	BOOL ResumeThread();
	BOOL SuspendThread();
	void Redraw();
	void SetThreadPriority(INT nPriorityLevel);
	void Stop();
	void Start();
	BOOL RegisterClass(HINSTANCE hInstance);
	BOOL Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID);
	CVisWindow();
	virtual ~CVisWindow();

protected:
	virtual void OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual void OnPaint(HWND hwnd);
	virtual LRESULT OnDestroy(HWND hwnd);
	virtual BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(HWND hwnd, UINT state, int cx, int cy);

private:
	INT Render();
	DWORD m_dwOldActiveChannels;
	UEMATSUVISPARAMS m_UVP,m_OldUVP;
	static void __cdecl Thread(void *p);
	WORD GetSample(LPVOID lpBuf,INT nSmp,INT nChn);
	INT VUMeter(INT nVMax);
	void DrawChannelGauge(HDC pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight,COLORREF clr0,COLORREF clr1);
	volatile BOOL m_bKillThread;
	unsigned long m_uThread;
	volatile BOOL m_bDrawAll;
	INT m_nWidth,m_nHeight;
	INT m_nWaveWidth,m_nWaveHeight;
	struct{TCHAR szName[20];EnvM mode;COLORREF cr;}m_EnvTypes[9];
	struct{TCHAR szName[20];INT x,y;}m_Columns[13];
	HPEN m_Pens[26];
	HBRUSH m_Brushes[26],m_BlueBrush,m_RedBrush,m_BackBrush;
	COLORREF m_Colors[26];
	HBITMAP m_hBmpGroup[9];
	HDC m_hDCGroup[9];
	HFONT m_hFont,m_hOldFont;
};

#endif // !defined(AFX_VISWINDOW_H__24BA2382_A340_11D4_B2F9_00D0B7AE5907__INCLUDED_)
