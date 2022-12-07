// Registry.h: interface for the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRY_H__C4EB86C0_96AF_11D4_B2FA_00D0B7AE5907__INCLUDED_)
#define AFX_REGISTRY_H__C4EB86C0_96AF_11D4_B2FA_00D0B7AE5907__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Main.h"

class CRegistry  
{
public:
	void Reset();
	void Validate();
	BOOL Save();
	BOOL Load();
	CRegistry();
	virtual ~CRegistry();
	BOOL m_bAutoAdvance;
	BOOL m_bAlwaysOnTop;
	TCHAR m_szFilterHistoryList[NUMBER_OF_COLUMNS][10][100];
	DWORD m_dwSysTrayCmd;
	BOOL m_bShowVisLabels;
	DWORD m_dwVisColorScheme;
	COLORREF m_crVisColors[4][26];
	BOOL m_bRegFileTypes;
	TCHAR m_szAssocExt[100];
	BOOL m_bWrapToolBar;
	double m_dListPaneRatio;
	DWORD m_dwPlayMode;
	DWORD m_dwIndDefaultLength;
	DWORD m_dwIndDefaultFade;
	DWORD m_dwUniDefaultLength;
	DWORD m_dwUniDefaultFade;
	BOOL m_bAutoRewind;
	BOOL m_bRandomize;
	BOOL m_bDetectInactivity;
	BOOL m_bUsePlaylistSkin;
	DWORD m_dwPlaylistSkinLayout;
	TCHAR m_szPlaylistSkinFile[MAX_PATH];
	BOOL m_bBlendPlaylistSkin;
	COLORREF m_crPlaylistSkinBlendingColor;
	COLORREF m_crPlaylistSkinTextColor;
	DWORD m_dwPlaylistSkinBlendingLevel;
	DWORD m_dwTempo;
	DWORD m_dwPitch;
	DWORD m_dwMainSS;
	DWORD m_dwEchoSS;
	DWORD m_dwToolBarStyle;
	DWORD m_dwSourceBarStyle;
	BOOL m_bUseFastSeek;
	TCHAR m_szFolderHistoryList[10][MAX_PATH];
	DWORD m_dwToolBarBandWidth;
	DWORD m_dwSourceBarBandWidth;
	DWORD m_dwToolBarBandIndex;
	DWORD m_dwSourceBarBandIndex;
	BOOL m_bShowLocationBar;
	BOOL m_bWarnAgainstEditingSPCInArchive;
	BOOL m_bDisplayToolTips;
	DWORD m_dwLastPlayingSong;
	BOOL m_bSurroundSound;
	BOOL m_bReverseStereo;
	DWORD m_dwAWThreadPriority;
	BOOL m_bDblBufferAW;
	BOOL m_bOutputToWAV;
	BOOL m_bOutputWithSound;
	TCHAR m_szWAVOutputDirectory[MAX_PATH];
	TCHAR m_szActiveWallpaperPlugin[MAX_PATH];
	BOOL m_bUseActiveWallpaper;
	BOOL m_bLayout;
	BOOL m_bShowControlPanel;
	BOOL m_bSaveSettingsOnExit;
	BOOL m_bMinimizeToSystemTray;
	BOOL m_bShowInTaskbar;
	BOOL m_bShowInSystemTray;
	BOOL m_bShowMenu;
	BOOL m_bShowToolBar;
	BOOL m_bToolBarHasLabels;
	BOOL m_bToolBarHasPictures;
	BOOL m_bShowStatusBar;
	BOOL m_bShowFileList;
	BOOL m_bShowPlayList;
	vector<INT>m_ToolBarButtons;
	BOOL m_bFilterActive;
	TCHAR m_szFilterFields[NUMBER_OF_COLUMNS][100];
	BOOL m_bFilterMatchWholeWordOnly;
	BOOL m_bFilterMatchCase;
	BOOL m_bFilterLogicalRelation;
	TCHAR m_szFileListSortColumn[50];
	BOOL m_bFileListSortOrder;
	TCHAR m_szPlayListSortColumn[50];
	BOOL m_bPlayListSortOrder;
	DWORD m_dwSelectedConfigurationTreeItem;
	BOOL m_bUseSkin;
	DWORD m_dwSkinLayout;
	TCHAR m_szSkinFileName[MAX_PATH];
	BOOL m_bBlendSkin;
	COLORREF m_crBlendingColor;
	COLORREF m_crTextColor;
	DWORD m_dwBlendingLevel;
	BOOL m_bAlwaysUseProgramSkin;
	DWORD m_dwSpectrumSize;
	BOOL m_bShowTitleBar;
	DWORD m_dwVisRate;
	DWORD m_dwVisMode;
	DWORD m_dwBufferLength;
	BOOL m_bShowVisWindow;
	DWORD m_dwActiveChannels;
	BOOL m_bAutoSizeColumns;
	DWORD m_dwSampleRate;
	DWORD m_dwSampleSize;
	DWORD m_dwChannels;
	DWORD m_dwInterpolation;
	DWORD m_dwMixingMode;
	DWORD m_dwPreamp;
	DWORD m_dwPriorityClass;
	DWORD m_dwMainThreadPriority;
	DWORD m_dwMixingThreadPriority;
	DWORD m_dwVisThreadPriority;
	BOOL m_bFullRowSelect;
	BOOL m_bShowGridLines;
	BOOL m_bReadID666;
	BOOL m_bSaveID666;
	DWORD m_dwAPR;
	BOOL m_bUseLPF;
	BOOL m_bUseOldBRE;
	BOOL m_bAutoLoadPlaylist;
	BOOL m_bAutoPlaySong;
	TCHAR m_szLastDirectory[MAX_PATH];
	TCHAR m_szLastPlaylist[MAX_PATH];
	struct{TCHAR szName[20];INT nWidth,nOrder;BOOL bActive;}m_FileListColumnInfos[NUMBER_OF_COLUMNS];
	struct{TCHAR szName[20];INT nWidth,nOrder;BOOL bActive;}m_PlayListColumnInfos[NUMBER_OF_COLUMNS];
	WINDOWPLACEMENT m_wp;
	struct SETTING{TCHAR szName[100];DWORD dwType,dwSize;LPVOID lpData;};
	SETTING *m_Settings;
	DWORD m_dwTotalSettings;
};

#endif // !defined(AFX_REGISTRY_H__C4EB86C0_96AF_11D4_B2FA_00D0B7AE5907__INCLUDED_)
