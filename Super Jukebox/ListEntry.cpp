// ListEntry.cpp: implementation of the CListEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListEntry.h"
#include "Extractor.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListEntry::CListEntry()
{
	m_szName[0]='\0';
	m_szFullName[0]='\0';
	m_szDirectory[0]='\0';
	m_szRedirectedFileName[0]='\0';
	m_iIcon=0;
	m_nType=0;
	m_bDisplayed=FALSE;
	m_bArchived=FALSE;
	m_bHasID666Tag=FALSE;
	m_bRedirected=FALSE;
	m_bAdded=FALSE;
	m_bChanged=FALSE;
	memset(&m_ID666Tag,0,sizeof(m_ID666Tag));
	m_uListID=0;
	m_nListIndex=-1;
	m_szValidFileName[0]='\0';
	m_llSize=0;
	m_szType[0]='\0';
	memset(&m_ftModified,0,sizeof(m_ftModified));
}

CListEntry::~CListEntry()
{
}

bool CListEntry::operator<(const CListEntry & obj)
{
	TCHAR szBuf0[1000],szBuf1[1000];

	for(UINT i=0,c=0;i<strlen(m_szName)&&c<sizeof(szBuf0)-1;i++)
	{
		if(isalnum(m_szName[i])||isspace(m_szName[i]))
			szBuf0[c++]=m_szName[i];
	}
	szBuf0[c]='\0';
	for(i=0,c=0;i<strlen(obj.m_szName)&&c<sizeof(szBuf1)-1;i++)
	{
		if(isalnum(obj.m_szName[i])||isspace(obj.m_szName[i]))
			szBuf1[c++]=obj.m_szName[i];
	}
	szBuf1[c]='\0';
	return stricmp(szBuf0,szBuf1)<0;
}

void CListEntry::SetName(LPCTSTR lpszName)
{
	strncpy(m_szName,lpszName,sizeof(m_szName)-1);
	m_szName[sizeof(m_szName)-1]='\0';
}

void CListEntry::GetName(LPTSTR lpszName,INT nTextMax)
{
	strncpy(lpszName,m_szName,nTextMax);
	lpszName[nTextMax-1]='\0';
}

void CListEntry::SetFullName(LPCTSTR lpszFullName)
{
	strncpy(m_szFullName,lpszFullName,sizeof(m_szFullName)-1);
	m_szFullName[sizeof(m_szFullName)-1]='\0';
}

void CListEntry::GetFullName(LPTSTR lpszName,INT nTextMax)
{
	strncpy(lpszName,m_szFullName,nTextMax-1);
	lpszName[nTextMax-1]='\0';
}

void CListEntry::SetDirectory(LPCTSTR lpszDirectory)
{
	strncpy(m_szDirectory,lpszDirectory,sizeof(m_szDirectory)-1);
	m_szDirectory[sizeof(m_szDirectory)-1]='\0';
}

void CListEntry::GetDirectory(LPTSTR lpszName,INT nTextMax)
{
	strncpy(lpszName,m_szDirectory,nTextMax);
	lpszName[nTextMax-1]='\0';
}

void CListEntry::SetIcon(INT iIcon)
{
	m_iIcon=iIcon;
}

INT CListEntry::GetIcon()
{
	return m_iIcon;
}

void CListEntry::SetType(INT nType)
{
	m_nType=nType;
}

INT CListEntry::GetType()
{
	return m_nType;
}

void CListEntry::SetTitle(LPCTSTR lpszTitle)
{
	strncpy(m_ID666Tag.szTitle,lpszTitle,sizeof(m_ID666Tag.szTitle));
	m_ID666Tag.szTitle[sizeof(m_ID666Tag.szTitle)-1]='\0';
}

void CListEntry::GetTitle(LPTSTR lpszTitle,INT nTextMax)
{
	strncpy(lpszTitle,m_ID666Tag.szTitle,nTextMax);
	lpszTitle[nTextMax-1]='\0';
}

void CListEntry::SetGame(LPCTSTR lpszGame)
{
	strncpy(m_ID666Tag.szGame,lpszGame,sizeof(m_ID666Tag.szGame));
	m_ID666Tag.szGame[sizeof(m_ID666Tag.szGame)-1]='\0';
}

void CListEntry::GetGame(LPTSTR lpszGame,INT nTextMax)
{
	strncpy(lpszGame,m_ID666Tag.szGame,nTextMax);
	lpszGame[nTextMax-1]='\0';
}

void CListEntry::SetArtist(LPCTSTR lpszArtist)
{
	strncpy(m_ID666Tag.szArtist,lpszArtist,sizeof(m_ID666Tag.szArtist));
	m_ID666Tag.szArtist[sizeof(m_ID666Tag.szArtist)-1]='\0';
}

void CListEntry::GetArtist(LPTSTR lpszArtist,INT nTextMax)
{
	strncpy(lpszArtist,m_ID666Tag.szArtist,nTextMax);
	lpszArtist[nTextMax-1]='\0';
}

void CListEntry::SetDumper(LPCTSTR lpszDumper)
{
	strncpy(m_ID666Tag.szDumper,lpszDumper,sizeof(m_ID666Tag.szDumper));
	m_ID666Tag.szDumper[sizeof(m_ID666Tag.szDumper)-1]='\0';
}

void CListEntry::GetDumper(LPTSTR lpszDumper,INT nTextMax)
{
	strncpy(lpszDumper,m_ID666Tag.szDumper,nTextMax);
	lpszDumper[nTextMax-1]='\0';
}

void CListEntry::SetDate(LPCTSTR lpszDate)
{
	strncpy(m_ID666Tag.szDate,lpszDate,sizeof(m_ID666Tag.szDate));
	m_ID666Tag.szDate[sizeof(m_ID666Tag.szDate)-1]='\0';
}

void CListEntry::GetDate(LPTSTR lpszDate,INT nTextMax)
{
	strncpy(lpszDate,m_ID666Tag.szDate,nTextMax);
	lpszDate[nTextMax-1]='\0';
}

void CListEntry::SetComment(LPCTSTR lpszComment)
{
	strncpy(m_ID666Tag.szComment,lpszComment,sizeof(m_ID666Tag.szComment));
	m_ID666Tag.szComment[sizeof(m_ID666Tag.szComment)-1]='\0';
}

void CListEntry::GetComment(LPTSTR lpszComment,INT nTextMax)
{
	strncpy(lpszComment,m_ID666Tag.szComment,nTextMax);
	lpszComment[nTextMax-1]='\0';
}

void CListEntry::SetLength(INT nLength)
{
	if(nLength>959)
		nLength=959;
	m_ID666Tag.uSong_ms=nLength*1000;
}

INT CListEntry::GetLength()
{
	return m_ID666Tag.uSong_ms/1000;
}

void CListEntry::SetFadeLength(INT nFadeLength)
{
	if(nFadeLength>99)
		nFadeLength=99;
	m_ID666Tag.uFade_ms=nFadeLength*1000;
}

INT CListEntry::GetFadeLength()
{
	return m_ID666Tag.uFade_ms/1000;
}

BOOL CListEntry::IsDisplayed()
{
	return m_bDisplayed;
}

void CListEntry::IsDisplayed(BOOL bDisplayed)
{
	m_bDisplayed=bDisplayed;
}

void CListEntry::Clear()
{
	m_szName[0]='\0';
	m_szFullName[0]='\0';
	m_szDirectory[0]='\0';
	m_szRedirectedFileName[0]='\0';
	m_iIcon=0;
	m_nType=0;
	m_bDisplayed=FALSE;
	m_bArchived=FALSE;
	m_bHasID666Tag=FALSE;
	m_bRedirected=FALSE;
	m_bAdded=FALSE;
	m_bChanged=FALSE;
	memset(&m_ID666Tag,0,sizeof(ID666TAG));
	m_uListID=0;
	m_nListIndex=-1;
	m_szValidFileName[0]='\0';
	m_llSize=0;
	m_szType[0]='\0';
	memset(&m_ftModified,0,sizeof(m_ftModified));
}

BOOL CListEntry::SetID666Tag(ID666TAG &Tag)
{
	m_ID666Tag=Tag;
	return (m_bHasID666Tag=TRUE);
}

void CListEntry::GetID666Tag(ID666TAG &Tag)
{
	Tag=m_ID666Tag;
}

BOOL CListEntry::IsArchived()
{
	return m_bArchived;
}

void CListEntry::IsArchived(BOOL bArchived)
{
	m_bArchived=bArchived;
}

BOOL CListEntry::HasID666Tag()
{
	return m_bHasID666Tag;
}

void CListEntry::HasID666Tag(BOOL bHasID666Tag)
{
	m_bHasID666Tag=bHasID666Tag;
}

BOOL CListEntry::LoadID666Tag()
{
	TCHAR szFileName[MAX_PATH];
	INT nCode=GetValidFileName(szFileName,sizeof(szFileName));
	if(!nCode)
		return FALSE;
	ID666TAG tag;
	BOOL bRes=app.m_Player.LoadID666Tag(szFileName,tag);
	if(nCode==1)ReleaseValidFileName();
	if(bRes)
		SetID666Tag(tag);
	return bRes;
}

BOOL CListEntry::SaveID666Tag()
{
	TCHAR szFileName[MAX_PATH];
	if(!IsArchived())
	{
		TCHAR szBuf0[MAX_PATH];
		GetDirectory(szBuf0,MAX_PATH);
		strcpy(szFileName,szBuf0);
		if(szFileName[strlen(szFileName)-1]!='\\')
			strcat(szFileName,"\\");
		GetFullName(szBuf0,MAX_PATH);
		strcat(szFileName,szBuf0);

		return app.m_Player.SaveID666Tag(szFileName,m_ID666Tag);
	}
	return FALSE;
}

void CListEntry::RedirectFile(LPCTSTR lpszFileName)
{
	strncpy(m_szRedirectedFileName,lpszFileName,MAX_PATH);
	m_szRedirectedFileName[MAX_PATH-1]='\0';
	m_bRedirected=TRUE;
}

BOOL CListEntry::IsRedirected()
{
	return m_bRedirected;
}

void CListEntry::IsRedirected(BOOL bRedirected)
{
	if(!this)return;
	m_bRedirected=bRedirected;
}

BOOL CListEntry::IsAdded()
{
	return m_bAdded;
}

void CListEntry::IsAdded(BOOL bAdded)
{
	m_bAdded=bAdded;
}

BOOL CListEntry::GetProperty(LPCTSTR lpszPropertyName, LPTSTR lpszBuffer, INT nTextMax)
{
	lpszBuffer[0]='\0';

	if(!stricmp(lpszPropertyName,"File")||!stricmp(lpszPropertyName,"Name"))
		GetName(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Title"))
		GetTitle(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Game"))
		GetGame(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Artist"))
		GetArtist(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Publisher"))
		GetPublisher(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Dumper"))
		GetDumper(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Date"))
		GetDate(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Comment"))
		GetComment(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Length"))
		app.m_MainWindow.m_PlayList.ConvertTimeDown(GetLength(),lpszBuffer);
	else if(!stricmp(lpszPropertyName,"Fade"))
		app.m_MainWindow.m_PlayList.ConvertTimeDown(GetFadeLength(),lpszBuffer);
	else if(!stricmp(lpszPropertyName,"Size")&&m_nType!=LIST_ID_DIR)
	{
		if(m_nType!=LIST_ID_DRV||(stricmp(m_szType,"Floppy Disk")&&stricmp(m_szType,"CD-ROM Disc")))
		{
			if(m_llSize>=1073741824)
				sprintf(lpszBuffer,"%.2fGB",(double)m_llSize/1073741824);
			else if(m_llSize>=1048576)
				sprintf(lpszBuffer,"%.2fMB",(double)m_llSize/1048576);
			else if(m_llSize>=1024)
				sprintf(lpszBuffer,"%I64uKB",m_llSize/1024);
			else
				sprintf(lpszBuffer,"%I64u",m_llSize);
		}
	}
	else if(!stricmp(lpszPropertyName,"Type"))
		GetType(lpszBuffer,nTextMax);
	else if(!stricmp(lpszPropertyName,"Modified")&&m_nType!=LIST_ID_DRV&&stricmp(m_szName,".."))
	{
		SYSTEMTIME st;
		FileTimeToSystemTime(&m_ftModified,&st);

		TCHAR szDate[100],szTime[100];
		GetDateFormat(LOCALE_USER_DEFAULT,DATE_SHORTDATE,&st,NULL,szDate,sizeof(szDate));
		GetTimeFormat(LOCALE_USER_DEFAULT,TIME_NOSECONDS,&st,NULL,szTime,sizeof(szTime));
		sprintf(lpszBuffer,"%s %s",szDate,szTime);
	}

	return TRUE;
}

BOOL CListEntry::SetProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszNewValue)
{
	CPlayList pl;

	if(!stricmp(lpszPropertyName,"File"))
	{
		SetName(lpszNewValue);
		SetFullName(lpszNewValue);
	}
	else if(!stricmp(lpszPropertyName,"Title"))
		SetTitle(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Game"))
		SetGame(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Artist"))
		SetArtist(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Publisher"))
		SetPublisher(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Dumper"))
		SetDumper(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Date"))
		SetDate(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Comment"))
		SetComment(lpszNewValue);
	else if(!stricmp(lpszPropertyName,"Length"))
		SetLength(pl.ConvertTimeUp(lpszNewValue));
	else if(!stricmp(lpszPropertyName,"Fade"))
		SetFadeLength(pl.ConvertTimeUp(lpszNewValue));

	m_bChanged=TRUE;

	return TRUE;
}

BOOL CListEntry::IsChanged()
{
	return m_bChanged;
}

void CListEntry::IsChanged(BOOL bChanged)
{
	m_bChanged=bChanged;
}

void CListEntry::SetListID(UINT uListID)
{
	m_uListID=uListID;
}

UINT CListEntry::GetListID()
{
	return m_uListID;
}

void CListEntry::SetListIndex(INT nIndex)
{
	m_nListIndex=nIndex;
}

INT CListEntry::GetListIndex()
{
	return m_nListIndex;
}

void CListEntry::GetRedirectedName(LPTSTR lpszName, INT nTextMax)
{
	strncpy(lpszName,m_szRedirectedFileName,nTextMax);
	lpszName[nTextMax-1]='\0';
}

INT CListEntry::GetValidFileName(LPTSTR lpszFileName, INT nTextMax)
{
	if(strlen(m_szValidFileName))
	{
		strncpy(lpszFileName,m_szValidFileName,nTextMax);
		lpszFileName[nTextMax-1]='\0';
		return 2;
	}

	if(!IsArchived())
	{
		TCHAR szBuf[MAX_PATH];
		GetPath(szBuf,sizeof(szBuf));
		if(!app.m_MainWindow.m_PlayList.LocateFile(szBuf,szBuf))
			return 0;
		strncpy(m_szValidFileName,szBuf,sizeof(m_szValidFileName));
	}
	else
	{
		GetRedirectedName(m_szValidFileName,sizeof(m_szValidFileName));
		if(!IsRedirected())
		{
			TCHAR szBuf0[MAX_PATH];
			GetDirectory(szBuf0,sizeof(szBuf0));
			TCHAR szArchive[MAX_PATH],szSubPath[MAX_PATH];
			app.m_MainWindow.m_FileList.IsPathInArchive(szBuf0,szArchive,szSubPath);
			if(!app.m_MainWindow.m_PlayList.LocateFile(szArchive,szArchive))
				return 0;
			TCHAR szOldStatus[500];
			app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
			app.m_MainWindow.m_StatusBar.SetText(3,"Extracting song from archive...");
			app.m_MainWindow.MaintainCursor(TRUE);
			app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
			CExtractor Extractor;
			BOOL bRes=TRUE;
			if(!Extractor.Open(szArchive,FALSE))
				bRes=FALSE;
			if(!Extractor.SetCurrentDirectory(szSubPath))
				bRes=FALSE;
			TCHAR szBuf1[MAX_PATH];
			GetFullName(szBuf1,sizeof(szBuf1));
			char *szTempName=_tempnam(NULL,NULL);
			strcpy(m_szValidFileName,szTempName);
			free(szTempName);
			if(!Extractor.ExtractSpecificFile(szBuf1,m_szValidFileName))
				bRes=FALSE;
			if(!Extractor.Close())
				bRes=FALSE;

			app.m_MainWindow.MaintainCursor(FALSE);
			app.m_MainWindow.SetCursor(NULL);
			app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
			if(!bRes)
			{
				m_szValidFileName[0]='\0';
				return 0;
			}
		}
	}
	strncpy(lpszFileName,m_szValidFileName,nTextMax);
	lpszFileName[nTextMax-1]='\0';
	return 1;
}

void CListEntry::ReleaseValidFileName()
{
	if(IsArchived()&&!IsRedirected())
		DeleteFile(m_szValidFileName);
	m_szValidFileName[0]='\0';
}

BOOL CListEntry::GetPath(LPTSTR lpszPath, INT nTextMax)
{
	GetDirectory(lpszPath,nTextMax);
	TCHAR szFileName[MAX_PATH];
	GetFullName(szFileName,sizeof(szFileName));
	if(lpszPath[strlen(lpszPath)-1]!='\\')
		strcat(lpszPath,"\\");
	strcat(lpszPath,szFileName);
	return TRUE;
}

void CListEntry::SetSize(LONGLONG llSize)
{
	m_llSize=llSize;
}

LONGLONG CListEntry::GetSize()
{
	return m_llSize;
}

void CListEntry::SetType(LPCTSTR lpszType)
{
	strncpy(m_szType,lpszType,sizeof(m_szType));
}

void CListEntry::GetType(LPTSTR lpszType, INT nTextMax)
{
	strncpy(lpszType,m_szType,nTextMax);
}

void CListEntry::SetModified(FILETIME ftModified)
{
	m_ftModified=ftModified;
}

FILETIME CListEntry::GetModified()
{
	return m_ftModified;
}

void CListEntry::SetPublisher(LPCTSTR lpszPublisher)
{
	strncpy(m_ID666Tag.szPublisher,lpszPublisher,sizeof(m_ID666Tag.szPublisher));
}

void CListEntry::GetPublisher(LPTSTR lpszPublisher,INT nTextMax)
{
	strncpy(lpszPublisher,m_ID666Tag.szPublisher,nTextMax);
}