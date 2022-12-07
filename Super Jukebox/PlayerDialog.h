// PlayerDialog.h: interface for the CPlayerDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERDIALOG_H__F024D8A2_9600_11D4_B2F9_00D0B7AE5907__INCLUDED_)
#define AFX_PLAYERDIALOG_H__F024D8A2_9600_11D4_B2F9_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Wnd.h"

class CPlayerDialog : public CWnd
{
public:
	BOOL CarryOutActions();
	BOOL Validate();
	void InitMembers();
	CPlayerDialog();
	virtual ~CPlayerDialog();

protected:
	virtual LRESULT OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
	virtual BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

private:
	INT m_nSampleRate;
	INT m_nSampleSize;
	BOOL m_bChannels;
	INT m_nInterpolation;
	INT m_nMixingMode;
	INT m_nPreamp;
	INT m_nBufferLength;
	INT m_nThreadPriority;
	DWORD m_dwAPR;
	BOOL m_bUseLPF;
	BOOL m_bUseOldBRE;
	BOOL m_bSurroundSound;
	BOOL m_bReverseStereo;
	BOOL m_bOutputToWAV;
	BOOL m_bOutputWithSound;
	TCHAR m_szWAVOutputDirectory[MAX_PATH];
};

#endif // !defined(AFX_PLAYERDIALOG_H__F024D8A2_9600_11D4_B2F9_00D0B7AE5907__INCLUDED_)
