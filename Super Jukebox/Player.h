// Player.h: interface for the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYER_H__CCF6F060_8800_11D4_B2F8_00D0B7AE5AEA__INCLUDED_)
#define AFX_PLAYER_H__CCF6F060_8800_11D4_B2F8_00D0B7AE5AEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "List.h"

class CPlayer  
{
public:
	UINT GetCurrentPreamp();
	BOOL HasFailed();
	BOOL SaveID666Tag(LPCTSTR lpFileName, ID666TAG &tag);
	BOOL LoadID666Tag(LPCTSTR lpFileName,ID666TAG &tag);
	BOOL SetWAVOutputFileName(LPCTSTR lpszFileName,BOOL bWithSound);
	BOOL SetPlayTime(UINT uTime,BOOL bFastSeek);
	BOOL SetEchoSS(INT nSep);
	BOOL SetMainSS(UINT uSep);
	BOOL SetSongPitch(UINT uPitch);
	BOOL SetSongSpeed(UINT uSpeed);
	BOOL SetPreAmp(UINT uPreAmp);
	void SetPlayingIndex(INT nIndex);
	CList* GetPlayingList();
	INT GetPlayingIndex();
	void MuteChannels(DWORD dwMask);
	BOOL GetVisParams(UEMATSUVISPARAMS& uvp);
	void SetFadeLength(INT nFadeLength);
	void SetLength(INT nLength);
	void SetThreadPriority(INT nPriorityLevel);
	BOOL GetConfiguration(UEMATSUCONFIG& cfg);
	BOOL IsEngineActive();
	BOOL IsEnginePlaying();
	INT GetFadeLength();
	INT GetLength();
	INT m_nFadeLength;
	INT GetPlayTime();
	BOOL IsPlaying();
	BOOL IsPaused();
	BOOL IsStopped();
	void IsPlaying(BOOL bPlaying);
	void IsPaused(BOOL bPaused);
	void IsStopped(BOOL bStopped);
	BOOL UnPauseSong();
	BOOL PauseSong();
	BOOL StopSong();
	BOOL SetConfiguration(UEMATSUCONFIG& cfg);
	BOOL PlaySong(CListEntry* Entry);
	CPlayer();
	virtual ~CPlayer();

private:
	CList* m_PlayingList;
	INT m_nPlayingIndex;
	INT m_nLength;
	BOOL m_bPlaying,m_bPaused,m_bStopped;
};

#endif // !defined(AFX_PLAYER_H__CCF6F060_8800_11D4_B2F8_00D0B7AE5AEA__INCLUDED_)
