// PlayList.cpp: implementation of the CPlayList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PlayList.h"
#include "Resource.h"
#include "AfxRes.h"
#include "Extractor.h"
#include "WarningDialog.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlayList::CPlayList()
{
	m_bMapped=TRUE;
	m_uMaxColumnCount=NUMBER_OF_COLUMNS;
	Reset();
}

CPlayList::~CPlayList()
{

}

BOOL CPlayList::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	return CList::Create(hInstance,pParentWnd,uID);
}

BOOL CPlayList::Reset()
{
	Clear();
	strcpy(m_szFileName,"Untitled.pl");
	m_uMode=app.m_Reg.m_dwPlayMode;
	m_uIndSongLength=app.m_Reg.m_dwIndDefaultLength;
	m_uIndSongFadeLength=app.m_Reg.m_dwIndDefaultFade;
	m_uUniSongLength=app.m_Reg.m_dwUniDefaultLength;
	m_uUniSongFadeLength=app.m_Reg.m_dwUniDefaultFade;
	m_bAutoRewind=app.m_Reg.m_bAutoRewind;
	m_bRandomize=app.m_Reg.m_bRandomize;
	m_bDetectInactivity=app.m_Reg.m_bDetectInactivity;
	m_bAutoAdvance=app.m_Reg.m_bAutoAdvance;
	m_bSaved=TRUE;
	m_iItem=m_iSubItem=-1;
	m_Skin.IsActive(app.m_Reg.m_bUsePlaylistSkin);
	m_Skin.SetLayout(app.m_Reg.m_dwPlaylistSkinLayout);
	m_Skin.SetFileName(app.m_Reg.m_szPlaylistSkinFile);
	m_Skin.IsBlended(app.m_Reg.m_bBlendPlaylistSkin);
	m_Skin.SetBlendingColor(app.m_Reg.m_crPlaylistSkinBlendingColor);
	m_Skin.SetTextColor(app.m_Reg.m_crPlaylistSkinTextColor);
	m_Skin.SetBlendingLevel(app.m_Reg.m_dwPlaylistSkinBlendingLevel);
	m_Skin.Load();
	m_bExists=FALSE;
	return TRUE;
}

BOOL CPlayList::Save(LPCTSTR lpszFileName)
{
	if(!lpszFileName||!strlen(lpszFileName))
		return FALSE;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Saving playlist...");
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
 
 	FILE *OutFile;
	OutFile=fopen(lpszFileName,"wb");
	if(!OutFile)
	{
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
		return FALSE;
	}

	TCHAR szBuffer[MAX_PATH];
	sprintf(szBuffer,"SJPL\r\n");
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"Version=%d\r\n",PLAYLIST_VERSION);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"Mode=%d\r\n",m_uMode);
	fputs(szBuffer,OutFile);
	TCHAR szBuf[MAX_PATH];
	ConvertTimeDown(m_uIndSongLength,szBuf);
	sprintf(szBuffer,"Individual Length=%s\r\n",szBuf);
	fputs(szBuffer,OutFile);
	ConvertTimeDown(m_uIndSongFadeLength,szBuf);
	sprintf(szBuffer,"Individual Fade=%s\r\n",szBuf);
	fputs(szBuffer,OutFile);
	ConvertTimeDown(m_uUniSongLength,szBuf);
	sprintf(szBuffer,"Universal Length=%s\r\n",szBuf);
	fputs(szBuffer,OutFile);
	ConvertTimeDown(m_uUniSongFadeLength,szBuf);
	sprintf(szBuffer,"Universal Fade=%s\r\n",szBuf);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"AutoRewind=%d\r\n",m_bAutoRewind);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"Randomize=%d\r\n",m_bRandomize);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"DetectInactivity=%d\r\n",m_bDetectInactivity);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"AutoAdvance=%d\r\n",m_bAutoAdvance);
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"UseSkin=%d\r\n",m_Skin.IsActive());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"SkinLayout=%d\r\n",m_Skin.GetLayout());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"%s\r\n",m_Skin.GetFileName());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"BlendSkin=%d\r\n",m_Skin.IsBlended());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"BlendingColor=%u\r\n",m_Skin.GetBlendingColor());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"TextColor=%u\r\n",m_Skin.GetTextColor());
	fputs(szBuffer,OutFile);
	sprintf(szBuffer,"BlendingLevel=%d\r\n",m_Skin.GetBlendingLevel());
	fputs(szBuffer,OutFile);
	for(INT i=0;i<m_Entries.size();i++)
	{
		m_Entries[i].GetName(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetFullName(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetDirectory(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetTitle(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetGame(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetArtist(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetPublisher(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetDumper(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetDate(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		m_Entries[i].GetComment(szBuf,sizeof(szBuf));
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		ConvertTimeDown(m_Entries[i].GetLength(),szBuf);
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
		ConvertTimeDown(m_Entries[i].GetFadeLength(),szBuf);
		sprintf(szBuffer,"%s\r\n",szBuf);
		fputs(szBuffer,OutFile);
	}
	fclose(OutFile);

	strncpy(m_szFileName,lpszFileName,sizeof(m_szFileName));
	m_szFileName[sizeof(m_szFileName)-1]='\0';
	m_Skin.IsModified(FALSE);
	m_bExists=m_bSaved=TRUE;
	app.m_MainWindow.UpdateTitleBar();

	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return TRUE;
}

BOOL CPlayList::Load(LPCTSTR lpszFileName)
{
	if(!lpszFileName||!strlen(lpszFileName))
		return FALSE;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Loading playlist...");
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));

	FILE *InFile;
	InFile=fopen(lpszFileName,"rb");
	if(!InFile)
	{
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
		return FALSE;
	}

	char szBuffer[MAX_PATH];
	fgets(szBuffer,sizeof(szBuffer),InFile);
	TrimString(szBuffer);
	if(stricmp(szBuffer,"SJPL"))
	{
		fclose(InFile);
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
		return FALSE;
	}
	fgets(szBuffer,sizeof(szBuffer),InFile);
	TrimString(szBuffer);
	int iVersion=GetSettingValue(szBuffer);
	if(iVersion>PLAYLIST_VERSION)
	{
		fclose(InFile);
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
		return FALSE;
	}
	if(iVersion>=2&&iVersion<=6)
		fgets(szBuffer,sizeof(szBuffer),InFile);
	fgets(szBuffer,sizeof(szBuffer),InFile);
	TrimString(szBuffer);
	m_uMode=GetSettingValue(szBuffer);
	if(m_uMode!=0&&m_uMode!=1&&m_uMode!=2)
	{
		fclose(InFile);
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
		return FALSE;
	}
	fgets(szBuffer,sizeof(szBuffer),InFile);
	TrimString(szBuffer);
	if(iVersion>=10)
	{
		for(int i=0;i<strlen(szBuffer);i++)
			if(szBuffer[i]=='=')break;
		strcpy(szBuffer,&szBuffer[i+1]);
		m_uIndSongLength=ConvertTimeUp(szBuffer);
	}
	else 
		m_uIndSongLength=GetSettingValue(szBuffer);
	if(m_uIndSongLength<0)m_uIndSongLength=0;
	if(iVersion>=8)
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		if(iVersion>=10)
		{
			for(int i=0;i<strlen(szBuffer);i++)
				if(szBuffer[i]=='=')break;
			strcpy(szBuffer,&szBuffer[i+1]);
			m_uIndSongFadeLength=ConvertTimeUp(szBuffer);
		}
		else 
			m_uIndSongFadeLength=GetSettingValue(szBuffer);
		if(m_uIndSongFadeLength<0)m_uIndSongFadeLength=0;
	}
	if(iVersion<11)
		fgets(szBuffer,sizeof(szBuffer),InFile);
	else
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		for(int i=0;i<strlen(szBuffer);i++)
			if(szBuffer[i]=='=')break;
		strcpy(szBuffer,&szBuffer[i+1]);
		m_uUniSongLength=ConvertTimeUp(szBuffer);
		if(m_uUniSongLength<0)m_uUniSongLength=0;

		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		for(i=0;i<strlen(szBuffer);i++)
			if(szBuffer[i]=='=')break;
		strcpy(szBuffer,&szBuffer[i+1]);
		m_uUniSongFadeLength=ConvertTimeUp(szBuffer);
		if(m_uUniSongFadeLength<0)m_uUniSongFadeLength=0;
	}
	fgets(szBuffer,sizeof(szBuffer),InFile);
	TrimString(szBuffer);
	m_bAutoRewind=GetSettingValue(szBuffer)?1:0;
	if(iVersion>=3&&iVersion<=10)
		fgets(szBuffer,sizeof(szBuffer),InFile);
	if(iVersion>=4)
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		m_bRandomize=GetSettingValue(szBuffer)?1:0;
	}
	else m_bRandomize=0;
	if(iVersion>=5)
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		m_bDetectInactivity=GetSettingValue(szBuffer)?1:0;
	}
	else m_bDetectInactivity=0;
	if(iVersion>=12)
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		m_bAutoAdvance=GetSettingValue(szBuffer)?1:0;
	}
	else m_bDetectInactivity=0;
	if(iVersion>=6)
	{
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		m_Skin.IsActive(GetSettingValue(szBuffer));
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		m_Skin.SetLayout(GetSettingValue(szBuffer));
		fgets(szBuffer,sizeof(szBuffer),InFile);
		TrimString(szBuffer);
		TCHAR szTemp[MAX_PATH];
		strcpy(szTemp,m_szFileName);
		strncpy(m_szFileName,lpszFileName,sizeof(m_szFileName));
		LocateFile(szBuffer,szBuffer);
		strncpy(m_szFileName,szTemp,sizeof(m_szFileName));
		m_Skin.SetFileName(szBuffer);
		if(iVersion>=11)
		{
			fgets(szBuffer,sizeof(szBuffer),InFile);
			TrimString(szBuffer);
			m_Skin.IsBlended(GetSettingValue(szBuffer));
			fgets(szBuffer,sizeof(szBuffer),InFile);
			TrimString(szBuffer);
			m_Skin.SetBlendingColor(GetSettingValue(szBuffer));
			fgets(szBuffer,sizeof(szBuffer),InFile);
			TrimString(szBuffer);
			m_Skin.SetTextColor(GetSettingValue(szBuffer));
			fgets(szBuffer,sizeof(szBuffer),InFile);
			TrimString(szBuffer);
			m_Skin.SetBlendingLevel(GetSettingValue(szBuffer));
		}
		if(!m_Skin.Load())
		{
			sprintf(szTemp,"Failed to load skin file \"%s\".",m_Skin.GetFileName());
			DisplayError(m_hWnd,FALSE,szTemp);
		}
		m_Skin.Refresh();
	}
	m_Skin.IsModified(FALSE);

	fpos_t pos;
	CListEntry Entry;
	SetRedraw(FALSE);
	Clear();
	while(!fgetpos(InFile,&pos)&&pos<filelength(fileno(InFile)))
	{
		if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
		TrimString(szBuffer);
		Entry.SetName(szBuffer);
		if(iVersion>=9)
		{
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
		}
		Entry.SetFullName(szBuffer);
		if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
		TrimString(szBuffer);
		Entry.SetDirectory(szBuffer);
		Entry.IsArchived(app.m_MainWindow.m_FileList.IsPathInArchive(szBuffer,NULL,NULL));
		if(iVersion>=8)
		{
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetTitle(szBuffer);
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetGame(szBuffer);
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetArtist(szBuffer);
			if(iVersion>=12)
			{
				if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
				TrimString(szBuffer);
				Entry.SetPublisher(szBuffer);
			}
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetDumper(szBuffer);
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetDate(szBuffer);
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			Entry.SetComment(szBuffer);
		}
		if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
		TrimString(szBuffer);
		if(iVersion>=10)
			Entry.SetLength(ConvertTimeUp(szBuffer));
		else
			Entry.SetLength(atoi(szBuffer));
		if(iVersion>=8)
		{
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
			TrimString(szBuffer);
			if(iVersion>=10)
				Entry.SetFadeLength(ConvertTimeUp(szBuffer));
			else
				Entry.SetFadeLength(atoi(szBuffer));
		}
		if(iVersion<11)
		{
			if(fgets(szBuffer,sizeof(szBuffer),InFile)==NULL)break;
		}

		Entry.SetType(LIST_ID_SPC);
		Entry.HasID666Tag(TRUE);
		InsertEntry(Entry);
	}
	fclose(InFile);

	FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
	SortItems(m_szSortColumn,m_bSortOrder);
	if(app.m_Reg.m_bAutoSizeColumns)
		ResizeColumns();
	SetRedraw(TRUE);
	InvalidateRect(m_hWnd,NULL,TRUE);
	SelectEntry(0);

	strncpy(m_szFileName,lpszFileName,sizeof(m_szFileName));
	m_szFileName[sizeof(m_szFileName)-1]='\0';
	m_bExists=m_bSaved=TRUE;
	app.m_MainWindow.UpdateTitleBar();
	app.m_MainWindow.RePaint(NULL);

	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return TRUE;
}

void CPlayList::TrimString(LPTSTR lpszString)
{
	for(INT i=0;i<strlen(lpszString);i++)
		if(lpszString[i]=='\n'||lpszString[i]=='\r')
		{lpszString[i]='\0';break;}
}

UINT CPlayList::GetSettingValue(LPCTSTR lpszSetting)
{
	for(INT nIndex=strlen(lpszSetting)-1;nIndex>=0;nIndex--)
		if(lpszSetting[nIndex]=='=')
			break;
	TCHAR szBuffer[200];
	INT nIndex1=0;
	while(nIndex<strlen(lpszSetting))
		szBuffer[nIndex1++]=lpszSetting[++nIndex];
	return atoi(szBuffer);
}

void CPlayList::GetName(LPTSTR lpszFileName,INT nTextMax)
{
	strncpy(lpszFileName,m_szFileName,nTextMax);
	lpszFileName[nTextMax-1]='\0';
}

BOOL CPlayList::ProcessEntry(INT nIndex)
{
	app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_STOP);
	app.m_MainWindow.m_ToolBar.SetSongList(this);
	app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_PLAY);
	return TRUE;
}

BOOL CPlayList::InsertEntry(CListEntry &ListEntry,INT nIndex)
{
	if(&ListEntry!=NULL&&ListEntry.GetType()==LIST_ID_SPC)
	{
		if(app.m_Reg.m_bReadID666&&!ListEntry.HasID666Tag())
			ListEntry.LoadID666Tag();
		CList::InsertEntry(ListEntry,nIndex);
		m_bSaved=FALSE;
		return TRUE;
	}
	return FALSE;
}

BOOL CPlayList::RemoveEntry(INT nIndex)
{
	if(nIndex>=0)
	{
		INT nInternalIndex=MapIndexToInternal(nIndex);
		CList::RemoveEntry(nIndex);
		if(app.m_Player.GetPlayingList()==this)
		{
			INT nPlayingIndex=app.m_Player.GetPlayingIndex();			
			if(nInternalIndex<nPlayingIndex)
				app.m_Player.SetPlayingIndex(nPlayingIndex-1);
			else if(nInternalIndex==nPlayingIndex)
				app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_STOP);
		}
		m_bSaved=FALSE;
		return TRUE;
	}
	return FALSE;
}

void CPlayList::Clear()
{
	CList::Clear();
	m_bSaved=TRUE;
}

BOOL CPlayList::IsSaved()
{
	if(GetEntryCount())
	{
		if(m_Skin.IsModified())
			return FALSE;
		return m_bSaved;
	}
	return TRUE;
}

UINT CPlayList::GetPlayingMode()
{
	return m_uMode;
}

void CPlayList::SetPlayingMode(UINT uMode)
{
	if(m_uMode!=uMode)
		m_bSaved=FALSE;
	m_uMode=uMode;
}

UINT CPlayList::GetIndSongLength()
{
	return m_uIndSongLength;
}

UINT CPlayList::GetIndSongFadeLength()
{
	return m_uIndSongFadeLength;
}

UINT CPlayList::GetUniSongLength()
{
	return m_uUniSongLength;
}

UINT CPlayList::GetUniSongFadeLength()
{
	return m_uUniSongFadeLength;
}

BOOL CPlayList::IsAutoRewind()
{
	return m_bAutoRewind;
}

BOOL CPlayList::IsRandomize()
{
	return m_bRandomize;
}

BOOL CPlayList::IsDetectInactivity()
{
	return m_bDetectInactivity;
}

void CPlayList::SetIndSongLength(UINT uSongLength)
{
	if(m_uIndSongLength!=uSongLength)
		m_bSaved=FALSE;
	m_uIndSongLength=uSongLength;
	if(m_uIndSongLength<0)m_uIndSongLength=0;
}

void CPlayList::SetIndSongFadeLength(UINT uSongFadeLength)
{
	if(m_uIndSongFadeLength!=uSongFadeLength)
		m_bSaved=FALSE;
	m_uIndSongFadeLength=uSongFadeLength;
	if(m_uIndSongFadeLength<0)m_uIndSongFadeLength;
}

void CPlayList::SetUniSongLength(UINT uSongLength)
{
	if(m_uUniSongLength!=uSongLength)
		m_bSaved=FALSE;
	m_uUniSongLength=uSongLength;
	if(m_uUniSongLength<0)m_uUniSongLength=0;
}

void CPlayList::SetUniSongFadeLength(UINT uSongFadeLength)
{
	if(m_uUniSongFadeLength!=uSongFadeLength)
		m_bSaved=FALSE;
	m_uUniSongFadeLength=uSongFadeLength;
	if(m_uUniSongFadeLength<0)m_uUniSongFadeLength;
}

void CPlayList::IsAutoRewind(BOOL bAutoRewind)
{
	if(m_bAutoRewind!=bAutoRewind)
		m_bSaved=FALSE;
	m_bAutoRewind=bAutoRewind;
}

void CPlayList::IsRandomize(BOOL bRandomize)
{
	if(m_bRandomize!=bRandomize)
		m_bSaved=FALSE;
	m_bRandomize=bRandomize;
}

void CPlayList::IsDetectInactivity(BOOL bDetectInactivity)
{
	if(m_bDetectInactivity!=bDetectInactivity)
		m_bSaved=FALSE;
	m_bDetectInactivity=bDetectInactivity;
}

void CPlayList::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	if(fDoubleClick)
	{
		KillTimer(m_hWnd,1);
		CList::OnLButtonDown(hwnd,fDoubleClick,x,y,keyFlags);
		return;
	}

	BOOL bEditing;
	if((bEditing=m_LabelEdit.IsActive()))
	{
		m_LabelEdit.WriteListItem();
		m_LabelEdit.Destroy();
	}

	LVHITTESTINFO lvhti;
	memset(&lvhti,0,sizeof(lvhti));
	lvhti.pt.x=x;lvhti.pt.y=y;
	m_iItem=m_iSubItem=-1;
	if(ListView_SubItemHitTest(m_hWnd,&lvhti)!=-1&&(ListView_GetItemState(m_hWnd,lvhti.iItem,LVIS_SELECTED)==LVIS_SELECTED||bEditing)&&GetFocus()==m_hWnd)
	{
		m_iItem=lvhti.iItem;
		m_iSubItem=lvhti.iSubItem;
	}

	CList::OnLButtonDown(hwnd,fDoubleClick,x,y,keyFlags);

	if(m_iItem>=0&&m_iSubItem>=0)
		SetTimer(m_hWnd,1,GetDoubleClickTime(),NULL);
}

void CPlayList::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(m_LabelEdit.IsActive())
	{
		m_LabelEdit.WriteListItem();
		m_LabelEdit.Destroy();
	}
	CList::OnHScroll(hwnd,hwndCtl,code,pos);
}

void CPlayList::OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(m_LabelEdit.IsActive())
	{
		m_LabelEdit.WriteListItem();
		m_LabelEdit.Destroy();
	}
	CList::OnVScroll(hwnd,hwndCtl,code,pos);
}

LRESULT CPlayList::OnTimer(HWND hwnd, UINT id)
{
	if(id==1)
	{
		KillTimer(m_hWnd,1);
		if(!(GetKeyState(VK_LBUTTON)&0x8000)&&m_iItem!=-1&&m_iSubItem!=-1)
			EditItem(m_iItem,m_iSubItem);
		return 0;
	}
	
	return CList::OnTimer(hwnd,id);
}

BOOL CPlayList::EditListEntry(INT nIndex,LPCTSTR lpszField, LPCTSTR lpszNewValue)
{
	TCHAR szBuf[1000];
	INT nMappedIndex=MapIndexToInternal(nIndex);
	INT nColumnIndex=GetColumnIndexFromName(lpszField);

	m_Entries[nMappedIndex].GetProperty(lpszField,szBuf,sizeof(szBuf));
	if(strcmp(szBuf,lpszNewValue))
	{
		m_Entries[nMappedIndex].SetProperty(lpszField,lpszNewValue);
		m_Entries[nMappedIndex].GetProperty(lpszField,szBuf,sizeof(szBuf));

		ListView_SetItemText(m_hWnd,nIndex,nColumnIndex,szBuf);
		if(app.m_Reg.m_bAutoSizeColumns)
			ResizeColumns(nColumnIndex);
		if(app.m_Player.GetPlayingList()==this&&app.m_Player.GetPlayingIndex()==nMappedIndex&&GetPlayingMode()==0)
		{
			if(!stricmp(lpszField,"Length")||!stricmp(lpszField,"Fade"))
			{
				UINT uLength,uFadeLength;
				uLength=m_Entries[nMappedIndex].GetLength();
				if(uLength<=0)
				{
					uLength=GetIndSongLength();
					uFadeLength=GetIndSongFadeLength();
				}
				else
					uFadeLength=m_Entries[nMappedIndex].GetFadeLength();
				app.m_Player.SetLength(uLength);
				app.m_Player.SetFadeLength(uFadeLength);
			}			
		}
		if(app.m_Reg.m_bSaveID666)
		{
			if(m_Entries[nMappedIndex].IsArchived()&&app.m_Reg.m_bWarnAgainstEditingSPCInArchive)
			{
				CWarningDialog dlg;
				dlg.SetWarningText("You are editing an SPC file that is inside an archive, therefore, ID666 tag information cannot be saved for this SPC file.");
				dlg.DoModal(IDD_WARNING,this,m_hInstance);
				app.m_Reg.m_bWarnAgainstEditingSPCInArchive=dlg.IsNoMoreChecked()^TRUE;
			}
			else if(!m_Entries[nMappedIndex].IsArchived()&&!m_Entries[nMappedIndex].SaveID666Tag())
			{
				TCHAR szFileName[MAX_PATH],szBuf[5000];
				m_Entries[nMappedIndex].GetPath(szFileName,sizeof(szFileName));
				sprintf(szBuf,"Failed to update ID666 tag for \"%s\".",szFileName);
				DisplayError(m_hWnd,FALSE,szBuf);
			}
		}
		m_bSaved=FALSE;
		app.m_MainWindow.UpdateTitleBar();
	}

	return TRUE;
}

INT CPlayList::FilterItems(TCHAR szFields[NUMBER_OF_COLUMNS][100], BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bLogicalRelation)
{
	if(!app.m_Reg.m_bFilterActive)
		return 0;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Filtering playlist entries...");
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));

	app.m_Reg.m_bFilterMatchWholeWordOnly=bMatchWholeWord;
	app.m_Reg.m_bFilterMatchCase=bMatchCase;
	app.m_Reg.m_bFilterLogicalRelation=bLogicalRelation;
	for(INT x=0;x<NUMBER_OF_COLUMNS;x++)
		strncpy(app.m_Reg.m_szFilterFields[x],szFields[x],sizeof(app.m_Reg.m_szFilterFields[x]));

	INT nOldPlayingIndex=app.m_Player.GetPlayingIndex();
	INT nOldSelectedIndex=MapIndexToInternal(GetSelectedEntry());
	IsMapped(FALSE);
	BOOL *bMatches=new BOOL[m_Entries.size()];
	for(INT y=0;y<m_Entries.size();y++)
	{
		for(INT x=0;x<m_uColumnCount;x++)
		{
			TCHAR szColumnName[1000];
			GetColumnNameFromIndex(x,szColumnName,sizeof(szColumnName));
			for(INT iField=0;iField<NUMBER_OF_COLUMNS;iField++)
			{
				if(!stricmp(m_ColumnInfos[iField].szName,szColumnName))
					break;
			}
			if(iField<NUMBER_OF_COLUMNS&&strlen(szFields[iField]))
			{
				if(!SearchEntry(y,szColumnName,szFields[iField],bMatchWholeWord,bMatchCase))
				{
					bMatches[y]=FALSE;
					if(!bLogicalRelation)
						break;
				}
				else
				{
					bMatches[y]=TRUE;
					if(bLogicalRelation)
						break;
				}
			}
		}
	}
	ListView_DeleteAllItems(m_hWnd);
	INT nTotalMatches=0;
	for(y=0;y<m_Entries.size();y++)
	{
		if(bMatches[y])
		{
			DisplayEntries(y,y);
			nTotalMatches++;
		}
	}
	delete[]bMatches;
	IsMapped(TRUE);
	if(app.m_Player.GetPlayingList()==this)
	{
		if(MapIndexToExternal(nOldPlayingIndex)<0)
			app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_STOP);		
	}
	SelectEntry(MapIndexToExternal(nOldSelectedIndex));
	if(GetSelectedEntry()<0)
		SelectEntry(0);
	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);

	return nTotalMatches;
}

void CPlayList::EditItem(INT nIndex,INT nSubIndex)
{		
	if(nIndex>=0)
	{
		if(m_LabelEdit.IsActive())
			m_LabelEdit.Destroy();
		m_LabelEdit.Create(m_hInstance,this,IDC_EDIT_PLAYLIST_LABEL);
		m_LabelEdit.ReadListItem(nIndex,nSubIndex);
	}
}

void CPlayList::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	CList::OnRButtonDown(hwnd,fDoubleClick,x,y,keyFlags);

	if(!(GetKeyState(VK_RBUTTON)&0x8000))
		DoContextMenu(x,y);
}

BOOL CPlayList::DoContextMenu(INT nXPos, INT nYPos)
{
	HMENU hMenu=CreatePopupMenu();
	if(!hMenu)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return FALSE;
	}
	HMENU hMenu1=CreatePopupMenu();
	if(!hMenu1)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return FALSE;
	}
	BOOL bActiveCols[]={GetColumnIndexFromName("File")!=-1,GetColumnIndexFromName("Title")!=-1,GetColumnIndexFromName("Game")!=-1,GetColumnIndexFromName("Artist")!=-1,GetColumnIndexFromName("Publisher")!=-1,GetColumnIndexFromName("Dumper")!=-1,GetColumnIndexFromName("Date")!=-1,GetColumnIndexFromName("Comment")!=-1,GetColumnIndexFromName("Length")!=-1,GetColumnIndexFromName("Fade")!=-1};
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[0]?MF_CHECKED:0),5,"&File");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[1]?MF_CHECKED:0),6,"&Title");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[2]?MF_CHECKED:0),7,"&Game");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[3]?MF_CHECKED:0),8,"&Artist");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[4]?MF_CHECKED:0),9,"&Publisher");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[5]?MF_CHECKED:0),10,"Du&mper");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[6]?MF_CHECKED:0),11,"&Date");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[7]?MF_CHECKED:0),12,"&Comment");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[8]?MF_CHECKED:0),13,"&Length");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[9]?MF_CHECKED:0),14,"&Fade");

	HMENU hMenu2=CreatePopupMenu();
	if(!hMenu2)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return FALSE;
	}
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"File")?MF_CHECKED:0),15,"&File");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Title")?MF_CHECKED:0),16,"&Title");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Game")?MF_CHECKED:0),17,"&Game");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Artist")?MF_CHECKED:0),18,"&Artist");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Publisher")?MF_CHECKED:0),19,"&Publisher");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Dumper")?MF_CHECKED:0),20,"Du&mper");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Date")?MF_CHECKED:0),21,"&Date");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Comment")?MF_CHECKED:0),22,"&Comment");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Length")?MF_CHECKED:0),23,"&Length");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Fade")?MF_CHECKED:0),24,"&Fade");
	AppendMenu(hMenu2,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu2,MF_STRING|(m_bSortOrder?MF_CHECKED:0),25,"&Ascending");
	AppendMenu(hMenu2,MF_STRING|(m_bSortOrder?0:MF_CHECKED),26,"&Descending");

	AppendMenu(hMenu,MF_STRING,1,"&Play");
	AppendMenu(hMenu,MF_STRING,2,"&Remove");
	AppendMenu(hMenu,MF_STRING,3,"&Edit");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,4,"&Select All");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_POPUP,(UINT)hMenu1,"&Columns");
	AppendMenu(hMenu,MF_POPUP,(UINT)hMenu2,"Sort &by");
	AppendMenu(hMenu,MF_STRING,27,"&Resize Columns");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,28,"&Properties...");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,29,"&Hide");

	INT nFocusIndex=GetFocusedEntry();
	if(nFocusIndex<0)
		nFocusIndex=GetSelectedEntry();
	INT nSelectedIndex=GetSelectedEntry();

	EnableMenuItem(hMenu,1,MF_BYCOMMAND|(nFocusIndex>=0?MF_ENABLED:MF_GRAYED));
	EnableMenuItem(hMenu,2,MF_BYCOMMAND|(nSelectedIndex>=0?MF_ENABLED:MF_GRAYED));
	EnableMenuItem(hMenu,3,MF_BYCOMMAND|(nSelectedIndex>=0?MF_ENABLED:MF_GRAYED));
	EnableMenuItem(hMenu,4,MF_BYCOMMAND|GetDisplayedCount()?MF_ENABLED:MF_GRAYED);

	POINT pt={nXPos,nYPos};
	::ClientToScreen(m_hWnd,&pt);
	INT nSelection=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,0,m_hWnd,NULL);
	DestroyMenu(hMenu);
	DestroyMenu(hMenu1);
	DestroyMenu(hMenu2);

	BOOL bColChange=FALSE;
	UINT uSortChange=0;
	TCHAR szSortCol[50];
	strcpy(szSortCol,m_szSortColumn);
	LPCTSTR lpszNames[]={"File","Title","Game","Artist","Publisher","Dumper","Date","Comment","Length","Fade"};
	switch(nSelection)
	{
	case 1:
		ProcessEntry(nFocusIndex);
		break;
	case 2:
		app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_REMOVE);
		break;
	case 3:
		LVHITTESTINFO lvhti;
		memset(&lvhti,0,sizeof(lvhti));
		lvhti.pt.x=nXPos;lvhti.pt.y=nYPos;
		m_iItem=m_iSubItem=-1;
		if(ListView_SubItemHitTest(m_hWnd,&lvhti)!=-1)
			EditItem(lvhti.iItem,lvhti.iSubItem);
		break;
	case 4:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_EDIT_SELECT_ALL,0),(LPARAM)m_hWnd);
		break;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		bActiveCols[nSelection-5]^=1;
		bColChange=TRUE;
		break;
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		strcpy(szSortCol,lpszNames[nSelection-15]);
		uSortChange=1;
		break;
	case 25:
	case 26:
		m_bSortOrder=(nSelection==25?1:0);
		uSortChange=2;
		break;
	case 27:
		ResizeColumns();
		break;
	case 28:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_FILE_PROPERTIES,0),(LPARAM)m_hWnd);
		break;
	case 29:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_PLAYLIST,0),(LPARAM)m_hWnd);
		break;
	}
	if(bColChange)
	{
		SaveHeaderInfo();
		for(UINT i=0;i<m_uMaxColumnCount;i++)
		{
			if(bActiveCols[i]!=m_ColumnInfos[i].bActive)
			{
				if(bActiveCols[i])
				{
					for(UINT c=0;c<m_uMaxColumnCount;c++)
						if(c!=i&&m_ColumnInfos[c].nOrder>=m_ColumnInfos[i].nOrder)
							m_ColumnInfos[c].nOrder++;
				}
				else
				{
					for(UINT c=0;c<m_uMaxColumnCount;c++)
						if(c!=i&&m_ColumnInfos[c].nOrder>m_ColumnInfos[i].nOrder)
							m_ColumnInfos[c].nOrder--;
				}
				m_ColumnInfos[i].bActive=bActiveCols[i];
			}
		}
		SaveRegistrySettings();
		app.m_MainWindow.m_PlaylistFilterDlg.RefreshComboBoxStates();
		SetRedraw(FALSE);
		INT nSelInd=GetSelectedEntry();
		ListView_DeleteAllItems(m_hWnd);
		RefreshHeader();
		DisplayEntries();
		FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
		SortItems(app.m_Reg.m_szPlayListSortColumn,app.m_Reg.m_bPlayListSortOrder);
		if(app.m_Reg.m_bAutoSizeColumns)
			ResizeColumns();
		SetRedraw(TRUE);
		InvalidateRect(m_hWnd,NULL,TRUE);
		SelectEntry(nSelInd);
	}
	if(uSortChange)
	{
		if(uSortChange==1&&!stricmp(szSortCol,m_szSortColumn))
		{
			SetSortingColumn(NULL,m_bSortOrder);
			SetRedraw(FALSE);
			INT nSelInd=GetSelectedEntry();
			ListView_DeleteAllItems(m_hWnd);
			RefreshHeader();
			DisplayEntries();
			SetRedraw(TRUE);
			InvalidateRect(m_hWnd,NULL,TRUE);
			SelectEntry(nSelInd);
		}
		else
			SortItems(szSortCol,m_bSortOrder);
	}
	return TRUE;
}

void CPlayList::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(fDown)
	{
		switch(vk)
		{
		case VK_APPS:
			INT nIndex;
			nIndex=GetFocusedEntry();
			if(nIndex<0)
				nIndex=GetSelectedEntry();
			if(nIndex>=0)
			{
				POINT pt;
				ListView_GetItemPosition(m_hWnd,nIndex,&pt);
				DoContextMenu(5,pt.y+5);
			}
			break;
		}
	}
	CList::OnKey(hwnd,vk,fDown,cRepeat,flags);
}

BOOL CPlayList::Exists()
{
	return m_bExists;
}

BOOL CPlayList::AddFiles(BOOL bEnumFileList,BOOL bEnumOnlyCurrentDirectory)
{
	for(INT i=0;i<m_EntriesToAdd.size();i++)
	{
		if(m_EntriesToAdd[i].GetType()==LIST_ID_PL)
		{
			TCHAR szPath[MAX_PATH];
			m_EntriesToAdd[i].GetPath(szPath,sizeof(szPath));
			ClearFilesToAdd();
			BOOL bRes=Load(szPath);
			if(!bRes)
			{
				TCHAR szBuf[MAX_PATH+100];
				sprintf(szBuf,"Failed to load \"%s\".",szPath);
				DisplayError(m_hWnd,FALSE,szBuf);
			}
			return bRes;
		}
	}

	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
	TCHAR szSource[MAX_PATH];
	CFileList &fl=app.m_MainWindow.m_FileList;
	fl.GetSource(szSource,sizeof(szSource));
	fl.EnumOnly(TRUE);
	fl.UpdateListView(FALSE);
	m_EnumeratedEntriesToAdd.clear();
	m_bEnumFileList=bEnumFileList;
	m_bEnumOnlyCurrentDirectory=bEnumOnlyCurrentDirectory;
	unsigned long ulThread;
	if((ulThread=_beginthread(AddFilesThread,0,&m_AddFilesDlg))==-1)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return FALSE;
	}
	m_AddFilesDlg.DoModal(IDD_ADDFILES,this,m_hInstance);
	if(WaitForSingleObject((HANDLE)ulThread,INFINITE)==WAIT_TIMEOUT)
		TerminateThread((HANDLE)ulThread,0);
	fl.EnumOnly(FALSE);
	fl.IsMapped(TRUE);
	fl.Clear();
	fl.SetSource(szSource);
	fl.LoadDriveList();
	fl.LoadDirectoryList();
	fl.LoadFileList();
	fl.SortItems(fl.m_szSortColumn,fl.m_bSortOrder);
	fl.UpdateListView(TRUE);
	ClearFilesToAdd();
	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	return TRUE;
}

void __cdecl CPlayList::AddFilesThread(void *lpParameter)
{
	CAddFilesDialog *dlg=(CAddFilesDialog*)lpParameter;
	while(!dlg->IsActive())
		Sleep(10);

	if(CoInitialize(NULL)!=S_OK)
	{
		DisplayError(dlg->m_hWnd,FALSE,NULL);
		dlg->Destroy();
		_endthread();
		return;
	}

	CFileList &fl=app.m_MainWindow.m_FileList;
	CPlayList &pl=app.m_MainWindow.m_PlayList;

	pl.EnumFilesToAdd(0,dlg,pl.m_bEnumFileList);

	if(dlg->IsCanceled())
	{
		dlg->Destroy();
		_endthread();
		return;
	}
	else if(!pl.m_EnumeratedEntriesToAdd.size())
	{
		MessageBox(dlg->m_hWnd,"No SPC files were found to be added.",app.m_szAppName,MB_ICONWARNING);
		dlg->Destroy();
		_endthread();
		return;
	}
	SetDlgItemText(dlg->m_hWnd,IDC_STATIC_ADDFILES_TITLE,"Adding files...");
	SetDlgItemText(dlg->m_hWnd,IDC_STATIC_ADDFILES,"");
	ShowWindow(GetDlgItem(dlg->m_hWnd,IDC_PROGRESS_ADDFILES),SW_SHOW);
	SendDlgItemMessage(dlg->m_hWnd,IDC_PROGRESS_ADDFILES,PBM_SETRANGE32,0,pl.m_EnumeratedEntriesToAdd.size());
	SendDlgItemMessage(dlg->m_hWnd,IDC_PROGRESS_ADDFILES,PBM_SETSTEP,1,0);

	RECT rc;
	GetClientRect(GetDlgItem(dlg->m_hWnd,IDC_STATIC_ADDFILES),&rc);
	HDC hDC=GetDC(GetDlgItem(dlg->m_hWnd,IDC_STATIC_ADDFILES));
	SelectObject(hDC,(HFONT)SendMessage(dlg->m_hWnd,WM_GETFONT,0,0));
	SetBkMode(hDC,TRANSPARENT);

	pl.SetRedraw(FALSE);
	pl.UpdateListView(FALSE);

	INT nOldCount=pl.GetEntryCount();

	CListEntry* entry;
	while((entry=pl.GetNextEntryToAdd())&&!dlg->IsCanceled())
	{
		TCHAR szBuf[MAX_PATH];
		entry->GetDirectory(szBuf,sizeof(szBuf));
		TCHAR szArchive[MAX_PATH],szSubPath[MAX_PATH];
		if(app.m_Reg.m_bReadID666&&fl.IsPathInArchive(szBuf,szArchive,szSubPath))
		{
			CExtractor extractor;
			if(!extractor.Open(szArchive,FALSE))
			{
				TCHAR szErr[5000];
				sprintf(szErr,"Failed to open archive: \"%s\"",szArchive);
				DisplayError(dlg->m_hWnd,FALSE,szErr);
				dlg->Destroy();
				_endthread();
				return;
			}
			WIN32_FIND_DATA fd;
			while(extractor.GetCurrentFile(TRUE,&fd)&&!dlg->IsCanceled())
			{
				CListEntry* entry=pl.FindSpecificEntry(fd.cFileName);
				if(entry)
				{
					TCHAR szBuf0[MAX_PATH];
					entry->GetFullName(szBuf0,sizeof(szBuf0));
					FillRect(hDC,&rc,(HBRUSH)(COLOR_3DFACE+1));
					DrawText(hDC,szBuf0,strlen(szBuf0),&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX|DT_PATH_ELLIPSIS);
					SendDlgItemMessage(dlg->m_hWnd,IDC_PROGRESS_ADDFILES,PBM_STEPIT,0,0);
					char *szTempName=_tempnam(NULL,NULL);
					strcpy(szBuf,szTempName);
					free(szTempName);
					extractor.ExtractCurrentFile(szBuf);
					entry->IsArchived(TRUE);
					entry->IsRedirected(TRUE);
					entry->RedirectFile(szBuf);
					entry->IsAdded(pl.InsertEntry(*entry));
					pl.GetListEntryUnmapped(pl.GetEntryCount()-1)->IsRedirected(FALSE);
					DeleteFile(szBuf);
				}
				else
					extractor.SkipCurrentFile();
			}
			extractor.Close();
		}
		else
		{
			TCHAR szName[MAX_PATH];
			entry->GetFullName(szName,sizeof(szName));
			FillRect(hDC,&rc,(HBRUSH)(COLOR_3DFACE+1));
			DrawText(hDC,szName,strlen(szName),&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX|DT_PATH_ELLIPSIS);
			SendDlgItemMessage(dlg->m_hWnd,IDC_PROGRESS_ADDFILES,PBM_STEPIT,0,0);
			entry->IsArchived(fl.IsPathInArchive(szBuf,NULL,NULL));
			entry->IsRedirected(FALSE);
			entry->IsAdded(pl.InsertEntry(*entry));
		}
	}
	pl.m_EnumeratedEntriesToAdd.clear();
	ReleaseDC(GetDlgItem(dlg->m_hWnd,IDC_STATIC_ADDFILES),hDC);
	ShowWindow(dlg->m_hWnd,SW_HIDE);
	pl.UpdateListView(TRUE);
	pl.SortEntries(nOldCount,pl.GetEntryCount()-1);
	pl.DisplayEntries(nOldCount,pl.GetEntryCount()-1);
	pl.FilterItems(app.m_Reg.m_szFilterFields,app.m_Reg.m_bFilterMatchWholeWordOnly,app.m_Reg.m_bFilterMatchCase,app.m_Reg.m_bFilterLogicalRelation);
	pl.SortItems(pl.m_szSortColumn,pl.m_bSortOrder);
	if(app.m_Reg.m_bAutoSizeColumns)
		pl.ResizeColumns();
	pl.SetRedraw(TRUE);
	InvalidateRect(pl.m_hWnd,NULL,TRUE);
	INT nSelectedIndex=pl.MapIndexToInternal(pl.GetSelectedEntry());
	pl.SelectEntry(nSelectedIndex>=0?pl.MapIndexToExternal(nSelectedIndex):0);
	app.m_MainWindow.UpdateTitleBar();

	CoUninitialize();
	dlg->Destroy();
	_endthread();
}

BOOL CPlayList::EnumFilesToAdd(INT nLevel,CAddFilesDialog *pAddDlg,BOOL bEnumFileList)
{
	CFileList &fl=app.m_MainWindow.m_FileList;

	static HWND hWnd;
	static HDC hDC;
	static RECT rc;
	if(!nLevel)
	{
		SetDlgItemText(pAddDlg->m_hWnd,IDC_STATIC_ADDFILES_TITLE,"Enumerating files to add...");
		hWnd=GetDlgItem(pAddDlg->m_hWnd,IDC_STATIC_ADDFILES);
		GetClientRect(hWnd,&rc);
		hDC=GetDC(hWnd);
		SelectObject(hDC,(HFONT)SendMessage(hWnd,WM_GETFONT,0,0));
		SetBkMode(hDC,TRANSPARENT);
	}
	else
		fl.IsMapped(FALSE);

	TCHAR szSource[MAX_PATH];
	fl.GetSource(szSource,sizeof(szSource));

	if(bEnumFileList||nLevel)
	{
		FillRect(hDC,&rc,(HBRUSH)(COLOR_3DFACE+1));
		DrawText(hDC,szSource,strlen(szSource),&rc,DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX|DT_PATH_ELLIPSIS);
	}
	
	INT nEntries=(bEnumFileList||nLevel)?fl.GetEntryCount():m_EntriesToAdd.size();

	for(INT i=0;i<nEntries&&!pAddDlg->IsCanceled();i++)
	{
		if(!bEnumFileList||nLevel||(!m_bEnumOnlyCurrentDirectory?fl.GetEntryState(i)&LVIS_SELECTED:TRUE))
		{
			CListEntry *entry=(bEnumFileList||nLevel)?fl.GetListEntry(i):&m_EntriesToAdd[i];
			INT nType=entry->GetType();
			switch(nType)
			{
			case LIST_ID_DRV:
			case LIST_ID_DIR:
			case LIST_ID_ACE:
			case LIST_ID_RAR:
			case LIST_ID_ZIP:
				TCHAR szBuf[MAX_PATH];
				if(bEnumFileList||nLevel)
					entry->GetFullName(szBuf,sizeof(szBuf));
				else
				{
					entry->GetDirectory(szBuf,sizeof(szBuf));
					if(szBuf[strlen(szBuf)-1]!='\\')
						strcat(szBuf,"\\");
					TCHAR szName[MAX_PATH];
					entry->GetFullName(szName,sizeof(szName));
					strcat(szBuf,szName);
				}
				if(stricmp(szBuf,".."))
				{
					if(nType==LIST_ID_DRV)
					{
						sprintf(szBuf,"%c:\\",szBuf[strlen(szBuf)-3]);
						if(!fl.SetSource(szBuf))
						{
							DisplayError(pAddDlg->m_hWnd,FALSE,NULL);
							break;
						}
					}
					else if(!fl.SetSource(szBuf))
					{
						DisplayError(pAddDlg->m_hWnd,FALSE,NULL);
						break;
					}

					fl.Clear();
					fl.LoadDirectoryList();
					fl.LoadFileList();
					EnumFilesToAdd(nLevel+1,pAddDlg,bEnumFileList);
					fl.Clear();
					if(!nLevel)
					{
						fl.SetSource(szSource);
						fl.LoadDriveList();
					}
					else
						fl.SetSource("..");
					fl.LoadDirectoryList();
					fl.LoadFileList();
				}
				break;
			case LIST_ID_SPC:
				entry->IsAdded(FALSE);
				m_EnumeratedEntriesToAdd.push_back(*entry);
				break;
			}
		}
	}

	if(!nLevel)
	{
		ReleaseDC(hWnd,hDC);
	}
	
	return TRUE;
}

CListEntry* CPlayList::GetNextEntryToAdd()
{
	for(INT i=0;i<m_EnumeratedEntriesToAdd.size();i++)
		if(!m_EnumeratedEntriesToAdd[i].IsAdded())
			return &m_EnumeratedEntriesToAdd[i];
	return NULL;
}

CListEntry* CPlayList::FindSpecificEntry(LPCTSTR lpszPath)
{
	for(INT i=0;i<m_EnumeratedEntriesToAdd.size();i++)
		if(!m_EnumeratedEntriesToAdd[i].IsAdded())
		{
			TCHAR szPath[MAX_PATH];
			m_EnumeratedEntriesToAdd[i].GetPath(szPath,sizeof(szPath));
			if(!stricmp(szPath,lpszPath))
				return &m_EnumeratedEntriesToAdd[i];
		}
	return NULL;
}

void CPlayList::ClearFilesToAdd()
{
	m_EntriesToAdd.clear();
}

BOOL CPlayList::AddFileToAdd(LPCTSTR lpszFileName)
{
	TCHAR szFileName[MAX_PATH],szDrv[_MAX_DRIVE],szDir[_MAX_DIR],szName[_MAX_FNAME],szExt[_MAX_EXT];
	_splitpath(lpszFileName,szDrv,szDir,szName,szExt);

	if(!stricmp(szExt,".lnk"))
	{
		HRESULT hRes=GetLinkTarget(lpszFileName,szFileName,sizeof(szFileName));
		if(!SUCCEEDED(hRes))
			return FALSE;
	}
	else
		strncpy(szFileName,lpszFileName,sizeof(szFileName));

	CListEntry Entry;
	if(strlen(szFileName)>3)
	{
		BOOL bArchived=app.m_MainWindow.m_FileList.IsPathInArchive(szFileName,NULL,NULL);
		WIN32_FIND_DATA wfd;
		memset(&wfd,0,sizeof(wfd));

		if(!bArchived)
		{
			HANDLE hFindFile;
			if((hFindFile=FindFirstFile(szFileName,&wfd))==INVALID_HANDLE_VALUE)
				return FALSE;
			FindClose(hFindFile);
		}

		_splitpath(szFileName,szDrv,szDir,szName,szExt);
		TCHAR szBuf[MAX_PATH];
		sprintf(szBuf,"%s%s",szDrv,szDir);
		if(strlen(szBuf)>3&&szBuf[strlen(szBuf)-1]=='\\')
			szBuf[strlen(szBuf)-1]='\0';
		
		Entry.SetDirectory(szBuf);
		sprintf(szBuf,"%s%s",szName,szExt);
		Entry.SetName(szBuf);
		Entry.SetFullName(szBuf);

		LPCTSTR szExtensions[]={".pl",".ace",".rar",".rsn",".zip",".spc",".sp0",".sp1",".sp2",".sp3",".sp4",".sp5",".sp6",".sp7",".sp8",".sp9"};
		if(!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			for(INT i=0;i<sizeof(szExtensions)/sizeof(szExtensions[0]);i++)
			{
				if(!stricmp(szExt,szExtensions[i]))
				{
					switch(i)
					{
					case 0:Entry.SetType(LIST_ID_PL);break;
					case 1:Entry.SetType(LIST_ID_ACE);break;
					case 2:Entry.SetType(LIST_ID_RAR);break;
					case 3:Entry.SetType(LIST_ID_ZIP);break;
					default:Entry.SetType(LIST_ID_SPC);break;
					}
					m_EntriesToAdd.push_back(Entry);
					return TRUE;
				}
			}
		}
		else
		{
			Entry.SetType(LIST_ID_DIR);
			m_EntriesToAdd.push_back(Entry);
			return TRUE;
		}
	}
	else
	{
		Entry.SetDirectory(szFileName);
		Entry.SetName("");
		Entry.SetFullName("");
		Entry.SetType(LIST_ID_DRV);
		m_EntriesToAdd.push_back(Entry);
		return TRUE;
	}

	return FALSE;
}

BOOL CPlayList::LocateFile(LPCTSTR lpszFile, LPTSTR lpszFoundFile)
{
	HANDLE hFindFile;
	TCHAR szFoundFile[MAX_PATH];
	for(INT nCount=0;nCount<3;nCount++)
	{
		TCHAR szDrv[_MAX_DRIVE],szDir[_MAX_DIR];
		switch(nCount)
		{
		case 0:
			_splitpath(lpszFile,szDrv,szDir,NULL,NULL);
			sprintf(szFoundFile,"%s%s",szDrv,szDir);
			break;
		case 1:
			GetName(szFoundFile,sizeof(szFoundFile));
			_splitpath(szFoundFile,szDrv,szDir,NULL,NULL);
			sprintf(szFoundFile,"%s%s",szDrv,szDir);
			break;
		case 2:
			GetModuleFileName(app.m_hInstance,szFoundFile,sizeof(szFoundFile));
			_splitpath(szFoundFile,szDrv,szDir,NULL,NULL);
			sprintf(szFoundFile,"%s%s",szDrv,szDir);
			break;
		}
		if(szFoundFile[strlen(szFoundFile)-1]!='\\')
			strcat(szFoundFile,"\\");
		TCHAR szFile[_MAX_FNAME],szExt[_MAX_EXT];
		_splitpath(lpszFile,NULL,NULL,szFile,szExt);
		strcat(szFoundFile,szFile);
		strcat(szFoundFile,szExt);
		WIN32_FIND_DATA w32fd;
		if((hFindFile=FindFirstFile(szFoundFile,&w32fd))!=INVALID_HANDLE_VALUE)
			break;
	}
	if(hFindFile==INVALID_HANDLE_VALUE)
		return FALSE;
	FindClose(hFindFile);
	strcpy(lpszFoundFile,szFoundFile);
	return TRUE;
}

LRESULT CPlayList::OnColumnClick(LPNMLISTVIEW lpNMListView)
{
	if(m_LabelEdit.IsActive())
	{
		m_LabelEdit.WriteListItem();
		m_LabelEdit.Destroy();
	}
	return CList::OnColumnClick(lpNMListView);
}

BOOL CPlayList::IsAutoAdvance()
{
	return m_bAutoAdvance;
}

void CPlayList::IsAutoAdvance(BOOL bAutoAdvance)
{
	if(m_bAutoAdvance!=bAutoAdvance)
		m_bSaved=FALSE;
	m_bAutoAdvance=bAutoAdvance;
}
