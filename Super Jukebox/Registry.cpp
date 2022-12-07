// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Registry.h"
#include "Resource.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{
	SETTING Settings[]=
	{
		"Auto advance",REG_DWORD,sizeof(m_bAutoAdvance),&m_bAutoAdvance,
		"Always on top",REG_DWORD,sizeof(m_bAlwaysOnTop),&m_bAlwaysOnTop,
		"Filter history list",REG_BINARY,sizeof(m_szFilterHistoryList),&m_szFilterHistoryList,
		"System tray double-click command",REG_DWORD,sizeof(m_dwSysTrayCmd),&m_dwSysTrayCmd,
		"Show visualizer labels",REG_DWORD,sizeof(m_bShowVisLabels),&m_bShowVisLabels,
		"Visualizer color scheme",REG_DWORD,sizeof(m_dwVisColorScheme),&m_dwVisColorScheme,
		"Visualizer colors",REG_BINARY,sizeof(m_crVisColors),&m_crVisColors,
		"Register file types on startup",REG_DWORD,sizeof(m_bRegFileTypes),&m_bRegFileTypes,
		"Associated extensions",REG_SZ,sizeof(m_szAssocExt),&m_szAssocExt,
		"Play mode",REG_DWORD,sizeof(m_dwPlayMode),&m_dwPlayMode,
		"Default individual length",REG_DWORD,sizeof(m_dwIndDefaultLength),&m_dwIndDefaultLength,
		"Default individual fade",REG_DWORD,sizeof(m_dwIndDefaultFade),&m_dwIndDefaultFade,
		"Default universal length",REG_DWORD,sizeof(m_dwUniDefaultLength),&m_dwUniDefaultLength,
		"Default universal fade",REG_DWORD,sizeof(m_dwUniDefaultFade),&m_dwUniDefaultFade,
		"Auto rewind",REG_DWORD,sizeof(m_bAutoRewind),&m_bAutoRewind,
		"Randomize",REG_DWORD,sizeof(m_bRandomize),&m_bRandomize,
		"Detect inactivity",REG_DWORD,sizeof(m_bDetectInactivity),&m_bDetectInactivity,
		"Use playlist skin",REG_DWORD,sizeof(m_bUsePlaylistSkin),&m_bUsePlaylistSkin,
		"Playlist skin layout",REG_DWORD,sizeof(m_dwPlaylistSkinLayout),&m_dwPlaylistSkinLayout,
		"Playlist skin file name",REG_SZ,sizeof(m_szPlaylistSkinFile),&m_szPlaylistSkinFile,
		"Blend playlist skin",REG_DWORD,sizeof(m_bBlendPlaylistSkin),&m_bBlendPlaylistSkin,
		"Playlist skin blending color",REG_DWORD,sizeof(m_crPlaylistSkinBlendingColor),&m_crPlaylistSkinBlendingColor,
		"Playlist skin text color",REG_DWORD,sizeof(m_crPlaylistSkinTextColor),&m_crPlaylistSkinTextColor,
		"Playlist skin blending level",REG_DWORD,sizeof(m_dwPlaylistSkinBlendingLevel),&m_dwPlaylistSkinBlendingLevel,
		"Wrap toolbar",REG_DWORD,sizeof(m_bWrapToolBar),&m_bWrapToolBar,
		"List views ratio",REG_BINARY,sizeof(m_dListPaneRatio),&m_dListPaneRatio,
		"Song tempo",REG_DWORD,sizeof(m_dwTempo),&m_dwTempo,
		"Song pitch",REG_DWORD,sizeof(m_dwPitch),&m_dwPitch,
		"Main stereo separation",REG_DWORD,sizeof(m_dwMainSS),&m_dwMainSS,
		"Echo stereo separation",REG_DWORD,sizeof(m_dwEchoSS),&m_dwEchoSS,
		"Toolbar style",REG_DWORD,sizeof(m_dwToolBarStyle),&m_dwToolBarStyle,
		"Location bar style",REG_DWORD,sizeof(m_dwSourceBarStyle),&m_dwSourceBarStyle,
		"Use Active Wallpaper",REG_DWORD,sizeof(m_bUseActiveWallpaper),&m_bUseActiveWallpaper,
		"Use fast seek",REG_DWORD,sizeof(m_bUseFastSeek),&m_bUseFastSeek,
		"Folder histroy list",REG_BINARY,sizeof(m_szFolderHistoryList),&m_szFolderHistoryList,
		"Toolbar band width",REG_DWORD,sizeof(m_dwToolBarBandWidth),&m_dwToolBarBandWidth,
		"Location bar band width",REG_DWORD,sizeof(m_dwSourceBarBandWidth),&m_dwSourceBarBandWidth,
		"Toolbar band index",REG_DWORD,sizeof(m_dwToolBarBandIndex),&m_dwToolBarBandIndex,
		"Location bar band index",REG_DWORD,sizeof(m_dwSourceBarBandIndex),&m_dwSourceBarBandIndex,
		"Show location bar",REG_DWORD,sizeof(m_bShowLocationBar),&m_bShowLocationBar,
		"Warn against editing SPC in archive",REG_DWORD,sizeof(m_bWarnAgainstEditingSPCInArchive),&m_bWarnAgainstEditingSPCInArchive,
		"Display tool tips",REG_DWORD,sizeof(m_bDisplayToolTips),&m_bDisplayToolTips,
		"Last playing song",REG_DWORD,sizeof(m_dwLastPlayingSong),&m_dwLastPlayingSong,
		"Surround sound",REG_DWORD,sizeof(m_bSurroundSound),&m_bSurroundSound,
		"Reverse stereo",REG_DWORD,sizeof(m_bReverseStereo),&m_bReverseStereo,
		"Active Wallpaper thread priority",REG_DWORD,sizeof(m_dwAWThreadPriority),&m_dwAWThreadPriority,
		"Double buffer Active Wallpaper",REG_DWORD,sizeof(m_bDblBufferAW),&m_bDblBufferAW,
		"Output to WAV",REG_DWORD,sizeof(m_bOutputToWAV),&m_bOutputToWAV,
		"Output with sound",REG_DWORD,sizeof(m_bOutputWithSound),&m_bOutputWithSound,
		"WAV output directory",REG_SZ,sizeof(m_szWAVOutputDirectory),&m_szWAVOutputDirectory,
		"Active Wallpaper plugin",REG_SZ,sizeof(m_szActiveWallpaperPlugin),&m_szActiveWallpaperPlugin,
		"List view layout",REG_DWORD,sizeof(m_bLayout),&m_bLayout,
		"Show control panel",REG_DWORD,sizeof(m_bShowControlPanel),&m_bShowControlPanel,
		"Save settings on exit",REG_DWORD,sizeof(m_bSaveSettingsOnExit),&m_bSaveSettingsOnExit,
		"Minimize to system tray",REG_DWORD,sizeof(m_bMinimizeToSystemTray),&m_bMinimizeToSystemTray,
		"Show in taskbar",REG_DWORD,sizeof(m_bShowInTaskbar),&m_bShowInTaskbar,
		"Show in system tray",REG_DWORD,sizeof(m_bShowInSystemTray),&m_bShowInSystemTray,
		"Show menu",REG_DWORD,sizeof(m_bShowMenu),&m_bShowMenu,
		"Show toolbar",REG_DWORD,sizeof(m_bShowToolBar),&m_bShowToolBar,
		"Toolbar has labels",REG_DWORD,sizeof(m_bToolBarHasLabels),&m_bToolBarHasLabels,
		"Toolbar has pictures",REG_DWORD,sizeof(m_bToolBarHasPictures),&m_bToolBarHasPictures,
		"Show status bar",REG_DWORD,sizeof(m_bShowStatusBar),&m_bShowStatusBar,
		"Show file list view",REG_DWORD,sizeof(m_bShowFileList),&m_bShowFileList,
		"Show play list view",REG_DWORD,sizeof(m_bShowPlayList),&m_bShowPlayList,
		"Playlist filter active",REG_DWORD,sizeof(m_bFilterActive),&m_bFilterActive,
		"Playlist filter fields",REG_BINARY,sizeof(m_szFilterFields),&m_szFilterFields,
		"Playlist filter match whole word only",REG_DWORD,sizeof(m_bFilterMatchWholeWordOnly),&m_bFilterMatchWholeWordOnly,
		"Playlist filter match case",REG_DWORD,sizeof(m_bFilterMatchCase),&m_bFilterMatchCase,
		"Playlist filter logical relation",REG_DWORD,sizeof(m_bFilterLogicalRelation),&m_bFilterLogicalRelation,
		"File list sorting column",REG_SZ,sizeof(m_szFileListSortColumn),&m_szFileListSortColumn,
		"File list sorting order",REG_DWORD,sizeof(m_bFileListSortOrder),&m_bFileListSortOrder,
		"Playlist sorting column",REG_SZ,sizeof(m_szPlayListSortColumn),&m_szPlayListSortColumn,
		"Playlist sorting order",REG_DWORD,sizeof(m_bPlayListSortOrder),&m_bPlayListSortOrder,
		"Selected configuration tree view item",REG_DWORD,sizeof(m_dwSelectedConfigurationTreeItem),&m_dwSelectedConfigurationTreeItem,
		"Use skin",REG_DWORD,sizeof(m_bUseSkin),&m_bUseSkin,
		"Skin layout",REG_DWORD,sizeof(m_dwSkinLayout),&m_dwSkinLayout,
		"Skin file name",REG_SZ,sizeof(m_szSkinFileName),&m_szSkinFileName,
		"Blend skin",REG_DWORD,sizeof(m_bBlendSkin),&m_bBlendSkin,
		"Skin blending color",REG_DWORD,sizeof(m_crBlendingColor),&m_crBlendingColor,
		"Skin text color",REG_DWORD,sizeof(m_crTextColor),&m_crTextColor,
		"Skin blending level",REG_DWORD,sizeof(m_dwBlendingLevel),&m_dwBlendingLevel,
		"Always use program skin",REG_DWORD,sizeof(m_bAlwaysUseProgramSkin),&m_bAlwaysUseProgramSkin,
		"Spectrometer size",REG_DWORD,sizeof(m_dwSpectrumSize),&m_dwSpectrumSize,
		"Show title bar",REG_DWORD,sizeof(m_bShowTitleBar),&m_bShowTitleBar,
		"Visualization rate",REG_DWORD,sizeof(m_dwVisRate),&m_dwVisRate,
		"Visualization mode",REG_DWORD,sizeof(m_dwVisMode),&m_dwVisMode,
		"Sound buffer length",REG_DWORD,sizeof(m_dwBufferLength),&m_dwBufferLength,
		"Show visualizer",REG_DWORD,sizeof(m_bShowVisWindow),&m_bShowVisWindow,
		"Active DSP channels",REG_DWORD,sizeof(m_dwActiveChannels),&m_dwActiveChannels,
		"Auto-size playlist columns",REG_DWORD,sizeof(m_bAutoSizeColumns),&m_bAutoSizeColumns,
		"Sample rate",REG_DWORD,sizeof(m_dwSampleRate),&m_dwSampleRate,
		"Sample size",REG_DWORD,sizeof(m_dwSampleSize),&m_dwSampleSize,
		"Number of channels",REG_DWORD,sizeof(m_dwChannels),&m_dwChannels,
		"Interpolation",REG_DWORD,sizeof(m_dwInterpolation),&m_dwInterpolation,
		"Mixing mode",REG_DWORD,sizeof(m_dwMixingMode),&m_dwMixingMode,
		"Preamp",REG_DWORD,sizeof(m_dwPreamp),&m_dwPreamp,
		"Priority class",REG_DWORD,sizeof(m_dwPriorityClass),&m_dwPriorityClass,
		"Main thread priority",REG_DWORD,sizeof(m_dwMainThreadPriority),&m_dwMainThreadPriority,
		"Mixing thread priority",REG_DWORD,sizeof(m_dwMixingThreadPriority),&m_dwMixingThreadPriority,
		"Visualizer thread priority",REG_DWORD,sizeof(m_dwVisThreadPriority),&m_dwVisThreadPriority,
		"Full row select",REG_DWORD,sizeof(m_bFullRowSelect),&m_bFullRowSelect,
		"Show grid lines",REG_DWORD,sizeof(m_bShowGridLines),&m_bShowGridLines,
		"Read ID666 tags",REG_DWORD,sizeof(m_bReadID666),&m_bReadID666,
		"Save ID666 tags",REG_DWORD,sizeof(m_bSaveID666),&m_bSaveID666,
		"APR",REG_DWORD,sizeof(m_dwAPR),&m_dwAPR,
		"Use low-pass filter",REG_DWORD,sizeof(m_bUseLPF),&m_bUseLPF,
		"Use old BRE",REG_DWORD,sizeof(m_bUseOldBRE),&m_bUseOldBRE,
		"Auto-load last playlist",REG_DWORD,sizeof(m_bAutoLoadPlaylist),&m_bAutoLoadPlaylist,
		"Auto-play last song",REG_DWORD,sizeof(m_bAutoPlaySong),&m_bAutoPlaySong,
		"Last active directory",REG_SZ,sizeof(m_szLastDirectory),&m_szLastDirectory,
		"Last used playlist",REG_SZ,sizeof(m_szLastPlaylist),&m_szLastPlaylist,
		"File list columns information",REG_BINARY,sizeof(m_FileListColumnInfos),&m_FileListColumnInfos,
		"Playlist columns information",REG_BINARY,sizeof(m_PlayListColumnInfos),&m_PlayListColumnInfos,
		"Window placement",REG_BINARY,sizeof(m_wp),&m_wp,
	};

	m_dwTotalSettings=sizeof(Settings)/sizeof(Settings[0]);
	m_Settings=new SETTING[m_dwTotalSettings];
	memcpy(m_Settings,Settings,sizeof(Settings));
}

CRegistry::~CRegistry()
{
	delete[]m_Settings;
}

void CRegistry::Reset()
{
	m_bAutoAdvance=TRUE;
	m_bAlwaysOnTop=FALSE;
	memset(&m_szFilterHistoryList,0,sizeof(m_szFilterHistoryList));
	m_dwSysTrayCmd=1;
	m_bShowVisLabels=TRUE;
	m_dwVisColorScheme=0;
	COLORREF crVisColors[4][26]=
	{
		{RGB(0,0,0),RGB(255,255,255),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,128,0),RGB(0,255,0),RGB(0,255,0),RGB(255,0,0),RGB(0,255,0),RGB(255,0,0),RGB(0,255,0),RGB(255,0,0),RGB(0,0,160),RGB(0,0,255),RGB(160,0,0),RGB(255,0,0)},
		{RGB(0,0,0),RGB(255,128,128),RGB(230,115,0),RGB(255,182,108),RGB(255,0,255),RGB(255,113,255),RGB(0,164,164),RGB(102,255,255),RGB(164,53,0),RGB(255,152,102),RGB(0,0,164),RGB(102,102,255),RGB(164,0,164),RGB(255,102,255),RGB(164,164,0),RGB(255,255,102),RGB(16,0,0),RGB(255,0,0),RGB(0,16,0),RGB(0,255,0),RGB(0,0,16),RGB(0,0,255),RGB(164,164,0),RGB(255,255,102),RGB(164,0,164),RGB(255,102,255)},
		{RGB(64,128,128),RGB(255,255,255),RGB(0,121,0),RGB(0,255,0),RGB(0,0,174),RGB(128,128,255),RGB(121,61,0),RGB(255,128,0),RGB(132,132,0),RGB(255,255,0),RGB(117,0,117),RGB(255,0,255),RGB(0,0,132),RGB(0,0,255),RGB(121,0,0),RGB(255,0,0),RGB(100,0,100),RGB(255,255,255),RGB(0,0,100),RGB(255,255,255),RGB(0,100,0),RGB(255,255,255),RGB(121,61,0),RGB(255,128,0),RGB(73,0,147),RGB(128,0,255)},
		{RGB(64,69,77),RGB(192,192,192),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(128,128,128),RGB(255,255,255),RGB(255,255,255),RGB(128,128,128)}
	};
	memcpy(&m_crVisColors,&crVisColors,sizeof(m_crVisColors));
	m_bRegFileTypes=FALSE;
	strcpy(m_szAssocExt,"PL");
	m_dwPlayMode=0;
	m_dwIndDefaultLength=0;
	m_dwIndDefaultFade=0;
	m_dwUniDefaultLength=0;
	m_dwUniDefaultFade=0;
	m_bAutoRewind=FALSE;
	m_bRandomize=FALSE;
	m_bDetectInactivity=FALSE;
	m_bUsePlaylistSkin=FALSE;
	m_dwPlaylistSkinLayout=0;
	m_szPlaylistSkinFile[0]='\0';
	m_bBlendPlaylistSkin=FALSE;
	m_crPlaylistSkinBlendingColor=RGB(255,255,255);
	m_crPlaylistSkinTextColor=RGB(0,0,0);
	m_dwPlaylistSkinBlendingLevel=128;
	m_bWrapToolBar=FALSE;
	m_dListPaneRatio=0.25;
	m_dwTempo=0;
	m_dwPitch=0;
	m_dwMainSS=50;
	m_dwEchoSS=100;
	m_dwToolBarStyle=RBBS_CHILDEDGE;
	m_dwSourceBarStyle=RBBS_CHILDEDGE|RBBS_BREAK;
	m_bUseFastSeek=TRUE;
	memset(m_szFolderHistoryList,0,sizeof(m_szFolderHistoryList));
	m_dwToolBarBandWidth=0;
	m_dwSourceBarBandWidth=0;
	m_dwToolBarBandIndex=0;
	m_dwSourceBarBandIndex=1;
	m_bShowLocationBar=TRUE;
	m_bWarnAgainstEditingSPCInArchive=TRUE;
	m_bDisplayToolTips=TRUE;
	m_dwLastPlayingSong=-1;
	m_bSurroundSound=FALSE;
	m_bReverseStereo=FALSE;
	m_dwAWThreadPriority=THREAD_PRIORITY_NORMAL;
	m_bDblBufferAW=TRUE;
	m_bOutputToWAV=FALSE;
	m_bOutputWithSound=TRUE;
	m_szWAVOutputDirectory[0]='\0';
	strcpy(m_szActiveWallpaperPlugin,"(none)");
	m_bUseActiveWallpaper=FALSE;
	m_bLayout=FALSE;
	m_bSaveSettingsOnExit=TRUE;
	m_bMinimizeToSystemTray=FALSE;
	m_bShowInTaskbar=TRUE;
	m_bShowInSystemTray=TRUE;
	m_bShowMenu=TRUE;
	m_bShowToolBar=TRUE;
	m_bToolBarHasLabels=TRUE;
	m_bToolBarHasPictures=TRUE;
	m_bShowStatusBar=TRUE;
	m_bShowFileList=TRUE;
	m_bShowPlayList=TRUE;
	m_bShowControlPanel=FALSE;
	INT nButtons[]={8,9,10,11,0,12,13,14,0,15,16,0,19,30,31,0,33};
	m_ToolBarButtons.clear();
	for(INT i=0;i<sizeof(nButtons)/sizeof(nButtons[0]);i++)
		m_ToolBarButtons.push_back(nButtons[i]);
	m_bFilterActive=FALSE;
	memset(m_szFilterFields,0,sizeof(m_szFilterFields));
	m_bFilterMatchWholeWordOnly=FALSE;
	m_bFilterMatchCase=FALSE;
	m_bFilterLogicalRelation=FALSE;
	m_szFileListSortColumn[0]='\0';
	m_bFileListSortOrder=TRUE;
	m_szPlayListSortColumn[0]='\0';
	m_bPlayListSortOrder=TRUE;
	m_dwSelectedConfigurationTreeItem=0;
	m_dwSpectrumSize=50;
	m_bShowTitleBar=TRUE;
	m_dwVisRate=30;
	m_dwVisMode=0;
	m_dwBufferLength=1000;
	m_dwActiveChannels=0xFF;
	m_dwSampleRate=44100;
	m_dwSampleSize=16;
	m_dwChannels=2;
	m_dwInterpolation=INT_NONE;
	m_dwMixingMode=1;
	m_dwPreamp=50;
	m_dwPriorityClass=NORMAL_PRIORITY_CLASS;
	m_dwMainThreadPriority=THREAD_PRIORITY_NORMAL;
	m_dwMixingThreadPriority=THREAD_PRIORITY_NORMAL;
	m_dwVisThreadPriority=THREAD_PRIORITY_NORMAL;
	m_dwAPR=0;
	m_bUseLPF=FALSE;
	m_bUseOldBRE=FALSE;
	m_bReadID666=TRUE;
	m_bSaveID666=FALSE;
	m_bFullRowSelect=TRUE;
	m_bShowGridLines=FALSE;
	m_bAutoLoadPlaylist=FALSE;
	m_bAutoPlaySong=TRUE;
	m_bAutoSizeColumns=TRUE;
	m_bShowVisWindow=TRUE;
	GetCurrentDirectory(sizeof(m_szLastDirectory),m_szLastDirectory);
	m_szLastPlaylist[0]='\0';
	m_bUseSkin=FALSE;
	m_dwSkinLayout=0;
	m_szSkinFileName[0]='\0';
	m_bBlendSkin=FALSE;
	m_crBlendingColor=RGB(255,255,255);
	m_crTextColor=RGB(0,0,0);
	m_dwBlendingLevel=128;

	m_wp.length=sizeof(m_wp);
	m_wp.flags=0;
	m_wp.showCmd=SW_SHOWNORMAL;
	m_wp.ptMaxPosition.x=-1;
	m_wp.ptMaxPosition.y=-1;
	m_wp.ptMinPosition.x=-1;
	m_wp.ptMinPosition.y=-1;
	SystemParametersInfo(SPI_GETWORKAREA,0,&m_wp.rcNormalPosition,0);
	m_wp.rcNormalPosition.left+=20;
	m_wp.rcNormalPosition.top+=20;
	m_wp.rcNormalPosition.right-=20;
	m_wp.rcNormalPosition.bottom-=20;

	LPCTSTR lpszFileListNames[]={"Name","Size","Type","Modified"};
	for(i=0;i<sizeof(lpszFileListNames)/sizeof(lpszFileListNames[0]);i++)
	{
		strncpy(m_FileListColumnInfos[i].szName,lpszFileListNames[i],sizeof(m_FileListColumnInfos[i].szName));
		m_FileListColumnInfos[i].nWidth=-1;
		m_FileListColumnInfos[i].nOrder=i;
		m_FileListColumnInfos[i].bActive=TRUE;
	}	
	LPCTSTR lpszPlayListNames[]={"File","Title","Game","Artist","Publisher","Dumper","Date","Comment","Length","Fade"};
	for(i=0;i<sizeof(lpszPlayListNames)/sizeof(lpszPlayListNames[0]);i++)
	{
		strncpy(m_PlayListColumnInfos[i].szName,lpszPlayListNames[i],sizeof(m_PlayListColumnInfos[i].szName));
		m_PlayListColumnInfos[i].nWidth=-1;
		m_PlayListColumnInfos[i].nOrder=i;
		m_PlayListColumnInfos[i].bActive=TRUE;
	}	
}

BOOL CRegistry::Load()
{
	HKEY hKey;

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,KEY_ALL_ACCESS,&hKey)!=ERROR_SUCCESS)
		return FALSE;

	for(INT i=0;i<m_dwTotalSettings;i++)
		RegQueryValueEx(hKey,m_Settings[i].szName,NULL,&m_Settings[i].dwType,(LPBYTE)m_Settings[i].lpData,&m_Settings[i].dwSize);

	DWORD dwNumButtons,dwSize=sizeof(dwNumButtons),dwType=REG_DWORD;
	if(RegQueryValueEx(hKey,"Number of toolbar buttons",NULL,&dwType,(LPBYTE)&dwNumButtons,&dwSize)==ERROR_SUCCESS)
	{
		INT *nButtons=new INT[dwNumButtons];
		dwSize=sizeof(INT)*dwNumButtons;
		dwType=REG_BINARY;
		RegQueryValueEx(hKey,"Toolbar buttons",NULL,&dwType,(LPBYTE)nButtons,&dwSize);
		m_ToolBarButtons.clear();
		for(INT i=0;i<dwNumButtons;i++)
			m_ToolBarButtons.push_back(nButtons[i]);
		delete[]nButtons;
	}

	RegCloseKey(hKey);
	
	Validate();

	return TRUE;
}

BOOL CRegistry::Save()
{
	Validate();

	HKEY hKey;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Marius Fodor\\Super Jukebox",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,NULL)!=ERROR_SUCCESS)
		return FALSE;

	for(INT i=0;i<m_dwTotalSettings;i++)
		RegSetValueEx(hKey,m_Settings[i].szName,0,m_Settings[i].dwType,(LPBYTE)m_Settings[i].lpData,m_Settings[i].dwSize);

	DWORD dwNumButtons=m_ToolBarButtons.size();
	RegSetValueEx(hKey,"Number of toolbar buttons",0,REG_DWORD,(LPBYTE)&dwNumButtons,sizeof(dwNumButtons));
	INT *nButtons=new INT[dwNumButtons];
	for(i=0;i<dwNumButtons;i++)
		nButtons[i]=m_ToolBarButtons[i];
	RegSetValueEx(hKey,"Toolbar buttons",0,REG_BINARY,(LPBYTE)nButtons,sizeof(INT)*dwNumButtons);
	delete[]nButtons;

	RegCloseKey(hKey);

	return TRUE;
}

void CRegistry::Validate()
{
	if(m_dwSampleRate<8000)m_dwSampleRate=8000;
	else if(m_dwSampleRate>96000)m_dwSampleRate=96000;
	if(m_dwPreamp<0)m_dwPreamp=0;
	else if(m_dwPreamp>MaxAmp)m_dwPreamp=MaxAmp;
}