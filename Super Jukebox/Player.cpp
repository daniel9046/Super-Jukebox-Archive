// Player.cpp: implementation of the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Player.h"
#include "Extractor.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayer::CPlayer()
{
	m_bPlaying=FALSE;
	m_bPaused=FALSE;
	m_bStopped=TRUE;
	m_nLength=m_nFadeLength=0;
}

CPlayer::~CPlayer()
{

}

BOOL CPlayer::PlaySong(CListEntry* Entry)
{
	if(Entry->GetListID()==IDC_LIST_FILELIST)
		m_PlayingList=&app.m_MainWindow.m_FileList;
	else if(Entry->GetListID()==IDC_LIST_PLAYLIST)
		m_PlayingList=&app.m_MainWindow.m_PlayList;
	else
		return FALSE;

	m_nPlayingIndex=Entry->GetListIndex();

	TCHAR szFileName[MAX_PATH];
	INT nCode=Entry->GetValidFileName(szFileName,sizeof(szFileName));
	if(!nCode)
		return FALSE;

	INT nLength,nFadeLength;
	if(m_PlayingList->GetID()==IDC_LIST_PLAYLIST)
	{
		app.m_Reg.m_dwLastPlayingSong=m_nPlayingIndex;
		CPlayList &pl=app.m_MainWindow.m_PlayList;
		switch(pl.GetPlayingMode())
		{
		case 0:
			nLength=Entry->GetLength();
			if(nLength<=0)
			{
				nLength=pl.GetIndSongLength();
				nFadeLength=pl.GetIndSongFadeLength();
			}
			else
				nFadeLength=Entry->GetFadeLength();
			break;
		case 1:
			nLength=pl.GetUniSongLength();
			nFadeLength=pl.GetUniSongFadeLength();
			break;
		case 2:
			nLength=nFadeLength=0;
			break;
		}
	}
	else
	{
		app.m_Reg.m_dwLastPlayingSong=-1;
		if(app.m_Reg.m_bReadID666&&!Entry->HasID666Tag())
			Entry->LoadID666Tag();
		nLength=Entry->GetLength();
		nFadeLength=Entry->GetFadeLength();
	}
	if(!IsStopped()&&!StopSong())
		return FALSE;

	SetLength(nLength);
	SetFadeLength(nFadeLength);
	BOOL bRes=Uematsu_Play(szFileName);
	if(nCode==1)Entry->ReleaseValidFileName();
	IsPlaying(bRes);
	return bRes;
}

BOOL CPlayer::SetConfiguration(UEMATSUCONFIG& cfg)
{
	return Uematsu_SetConfiguration(&cfg);
}

BOOL CPlayer::StopSong()
{
	if(!IsPlaying()&&!IsPaused())
		return FALSE;

	BOOL bRes=Uematsu_Stop();
	IsStopped(TRUE);
	return bRes;
}

BOOL CPlayer::PauseSong()
{
	if(!IsPlaying())
		return FALSE;

	BOOL bRes=Uematsu_Pause();
	IsPaused(TRUE);
	return bRes;
}

BOOL CPlayer::UnPauseSong()
{
	if(!IsPaused())
		return FALSE;

	BOOL bRes=Uematsu_UnPause();
	IsPlaying(TRUE);
	return bRes;
}

BOOL CPlayer::IsPlaying()
{
	return m_bPlaying;
}

BOOL CPlayer::IsPaused()
{
	return m_bPaused;
}

BOOL CPlayer::IsStopped()
{
	return m_bStopped;
}

void CPlayer::IsPlaying(BOOL bPlaying)
{
	m_bPlaying=bPlaying;
	if(m_bPlaying)
		m_bPaused=m_bStopped=FALSE;
}

void CPlayer::IsPaused(BOOL bPaused)
{
	m_bPaused=bPaused;
	if(m_bPaused)
		m_bPlaying=m_bStopped=FALSE;
}

void CPlayer::IsStopped(BOOL bStopped)
{
	m_bStopped=bStopped;
	if(m_bStopped)
		m_bPlaying=m_bPaused=FALSE;
}

INT CPlayer::GetPlayTime()
{
	return Uematsu_GetPlayTime();
}

INT CPlayer::GetLength()
{
	return m_nLength;
}

INT CPlayer::GetFadeLength()
{
	return m_nFadeLength;
}

BOOL CPlayer::IsEnginePlaying()
{
	return Uematsu_IsPlaying();
}

BOOL CPlayer::IsEngineActive()
{
	return Uematsu_IsActive();
}

BOOL CPlayer::GetConfiguration(UEMATSUCONFIG& cfg)
{
	return Uematsu_GetConfiguration(&cfg);
}

void CPlayer::SetThreadPriority(INT nPriorityLevel)
{
	Uematsu_SetMixingThreadPriority(nPriorityLevel);
}

void CPlayer::SetLength(INT nLength)
{
	m_nLength=nLength;
	Uematsu_SetSongLength(m_nLength*1000,m_nFadeLength*1000);
}

void CPlayer::SetFadeLength(INT nFadeLength)
{
	m_nFadeLength=nFadeLength;
	Uematsu_SetSongLength(m_nLength*1000,m_nFadeLength*1000);
}

BOOL CPlayer::GetVisParams(UEMATSUVISPARAMS &uvp)
{
	return Uematsu_GetVisParams(&uvp);
}

void CPlayer::MuteChannels(DWORD dwMask)
{
	Uematsu_MuteChannels(dwMask);
}

INT CPlayer::GetPlayingIndex()
{
	if(!IsStopped())
		return m_nPlayingIndex;
	return -1;
}

CList* CPlayer::GetPlayingList()
{
	if(!IsStopped())
		return m_PlayingList;
	return NULL;
}

void CPlayer::SetPlayingIndex(INT nIndex)
{
	m_nPlayingIndex=nIndex;
}

BOOL CPlayer::SetPreAmp(UINT uPreAmp)
{
	return Uematsu_SetPreAmp(uPreAmp);
}

BOOL CPlayer::SetSongSpeed(UINT uSpeed)
{
	return Uematsu_SetSongSpeed(uSpeed);
}

BOOL CPlayer::SetSongPitch(UINT uPitch)
{
	return Uematsu_SetSongPitch(uPitch);
}

BOOL CPlayer::SetMainSS(UINT uSep)
{
	return Uematsu_SetMainSS(uSep);
}

BOOL CPlayer::SetEchoSS(INT nSep)
{
	return Uematsu_SetEchoSS(nSep);
}

BOOL CPlayer::SetPlayTime(UINT uTime,BOOL bFastSeek)
{
	return Uematsu_SetPlayTime(uTime,bFastSeek);
}

BOOL CPlayer::SetWAVOutputFileName(LPCTSTR lpszFileName, BOOL bWithSound)
{
	return Uematsu_SetWAVOutputFileName(lpszFileName,bWithSound);
}

BOOL CPlayer::LoadID666Tag(LPCTSTR lpFileName, ID666TAG &tag)
{
	return Uematsu_LoadID666Tag(lpFileName,&tag);
}

BOOL CPlayer::SaveID666Tag(LPCTSTR lpFileName, ID666TAG &tag)
{
	return Uematsu_SaveID666Tag(lpFileName,&tag);
}

BOOL CPlayer::HasFailed()
{
	return Uematsu_HasFailed();
}

UINT CPlayer::GetCurrentPreamp()
{
	return Uematsu_GetCurrentPreamp();
}
