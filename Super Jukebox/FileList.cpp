// FileList.cpp: implementation of the CFileList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileList.h"
#include "Resource.h"
#include "AfxRes.h"
#include "Extractor.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileList::CFileList()
{
	m_bArchived=FALSE;
	m_szArchiveFileName[0]='\0';
	m_szSource[0]='\0';
	strcpy(m_szSubSource,"\\");
	m_bEnumOnly=FALSE;
	m_uMaxColumnCount=4;
	m_bMapped=TRUE;
}

CFileList::~CFileList()
{

}

BOOL CFileList::IsArchived()
{
	return m_bArchived;
}

LPCTSTR CFileList::GetArchiveFileName()
{
	return m_szArchiveFileName;
}

BOOL CFileList::ProcessEntry(INT iIndex)
{
	iIndex=MapIndexToInternal(iIndex);
	switch(m_Entries[iIndex].GetType())
	{
	case LIST_ID_DRV:
		TCHAR szBuf[MAX_PATH];
		m_Entries[iIndex].GetFullName(szBuf,sizeof(szBuf));
		if(!SetSource(szBuf))
		{
			DisplayError(m_pParentWnd->m_hWnd,FALSE,NULL);
			return FALSE;
		}
		app.m_MainWindow.m_StatusBar.SetText(3,"Loading file list...");
		SetRedraw(FALSE);
		Clear();
		LoadDriveList();
		LoadDirectoryList();
		LoadFileList();
		SortItems(m_szSortColumn,m_bSortOrder);
		if(app.m_Reg.m_bAutoSizeColumns)
			ResizeColumns();
		SetRedraw(TRUE);
		InvalidateRect(m_hWnd,NULL,TRUE);
		UINT i;
		for(i=0;i<m_Entries.size();i++)
		{
			m_Entries[i].GetFullName(szBuf,sizeof(szBuf));
			if(!stricmp(szBuf,".."))
			{
				SelectEntry(i);
				break;
			}
		}
		app.m_MainWindow.m_StatusBar.SetText(3,NULL);
		break;
	case LIST_ID_DIR:
	case LIST_ID_ACE:
	case LIST_ID_RAR:
	case LIST_ID_ZIP:
		if(GetKeyState(VK_SHIFT)&0x8000)
			app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_ADD);
		else
		{
			m_Entries[iIndex].GetFullName(szBuf,MAX_PATH);
			TCHAR szHighlightName[MAX_PATH];
			strcpy(szHighlightName,"..");
			if(!stricmp(szBuf,".."))
			{
				TCHAR szBuf[MAX_PATH];
				m_Entries[iIndex].GetDirectory(szBuf,MAX_PATH-1);
				INT pos=0;
				while((pos=GetNextPathPart(szBuf,pos,szHighlightName))!=-1);
				if(strlen(szHighlightName)==2&&szHighlightName[1]==':')
					strcat(szHighlightName,"\\");
			}
			app.m_MainWindow.m_StatusBar.SetText(3,"Loading file list...");
			SetSource(szBuf);
			SetRedraw(FALSE);
			Clear();
			LoadDriveList();
			LoadDirectoryList();
			LoadFileList();
			SortItems(m_szSortColumn,m_bSortOrder);
			if(app.m_Reg.m_bAutoSizeColumns)
				ResizeColumns();
			SetRedraw(TRUE);
			InvalidateRect(m_hWnd,NULL,TRUE);
			for(i=0;i<GetDisplayedCount();i++)
			{
				GetListEntry(i)->GetFullName(szBuf,sizeof(szBuf));
				if(!stricmp(szBuf,szHighlightName))
				{
					SelectEntry(i);
					break;
				}
			}
			app.m_MainWindow.m_StatusBar.SetText(3,NULL);
		}
		break;
	case LIST_ID_PL:
		if(!app.m_MainWindow.m_PlayList.IsSaved())
		{
			TCHAR szBuf[500],szPlayListName[MAX_PATH];
			app.m_MainWindow.m_PlayList.GetName(szPlayListName,MAX_PATH);
			sprintf(szBuf,"Save changes to \"%s\"?",szPlayListName);
			switch(MessageBox(m_hWnd,szBuf,app.m_szAppName,MB_ICONQUESTION|MB_YESNOCANCEL))
			{
			case IDYES:if(app.m_MainWindow.m_Menu.OnFileSave())return TRUE;break;
			case IDCANCEL:return TRUE;
			}
		}
		TCHAR szPath[MAX_PATH];
		m_Entries[iIndex].GetPath(szPath,sizeof(szPath));
		if(!app.m_MainWindow.m_PlayList.Load(szPath))
		{
			TCHAR szBuf[MAX_PATH+100];
			sprintf(szBuf,"Failed to load \"%s\".",szPath);
			DisplayError(m_hWnd,FALSE,szBuf);
		}
		break;
	case LIST_ID_SPC:
		if(GetKeyState(VK_SHIFT)&0x8000)
			app.m_MainWindow.m_ToolBar.SetSongList(this);
		app.m_MainWindow.m_ToolBar.OnClicked(GetKeyState(VK_SHIFT)&0x8000?IDC_BUTTON_PLAY:IDC_BUTTON_ADD);
		break;
	}
	return TRUE;
}

BOOL CFileList::LoadDriveList()
{
	if(strlen(m_szSource))
		return TRUE;

	DWORD dwDrives=GetLogicalDrives();
	DWORD dwDrivesIndex=0;
	CListEntry Entry;
	char szRoot[4];
	SHFILEINFO sfi;

	while(dwDrives&&dwDrivesIndex<26)
	{
		if(dwDrives&(1<<dwDrivesIndex))
		{
			sprintf(szRoot,"%c:\\",dwDrivesIndex+'A');
			if(!m_bEnumOnly)
			{
				memset(&sfi,0,sizeof(sfi));
				::SHGetFileInfo(szRoot,0,&sfi,sizeof(sfi),SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME);
			}
			else
				strcpy(sfi.szDisplayName,szRoot);
			Entry.SetName(sfi.szDisplayName);
			Entry.SetFullName(szRoot);
			Entry.SetDirectory(m_szSource);
			Entry.SetIcon(sfi.iIcon);
			Entry.SetType(LIST_ID_DRV);
			LONGLONG llSize=0;
			TCHAR szType[50];
			switch(GetDriveType(szRoot))
			{
			case DRIVE_REMOVABLE:
				strcpy(szType,"Floppy Disk");		
				break;
			case DRIVE_FIXED:
				strcpy(szType,"Local Disk");
				llSize=GetDriveSize(szRoot);
				break;
			case DRIVE_REMOTE:
				strcpy(szType,"Network Connection");
				llSize=GetDriveSize(szRoot);
				break;
			case DRIVE_CDROM:
				strcpy(szType,"CD-ROM Disc");				
				break;
			case DRIVE_RAMDISK:
				strcpy(szType,"RAM Disk");
				llSize=GetDriveSize(szRoot);
				break;
			default:
				strcpy(szType,"Unknown");
			}
			Entry.SetSize(llSize);
			Entry.SetType(szType);
			Entry.IsArchived(FALSE);
			InsertEntry(Entry);
		}
		dwDrivesIndex++;
	}
	return TRUE;
}

BOOL CFileList::LoadDirectoryList()
{
	if(!strlen(m_szSource))
		return TRUE;

	vector<CListEntry>TempList;
	TempList.clear();
	BOOL bFirst=TRUE;
	HANDLE hFindFile;
	WIN32_FIND_DATA w32fd;
	CListEntry Entry;
	SHFILEINFO sfi;
	UINT uFlags;

	while(bFirst?(hFindFile=FindFirstFile(&w32fd))!=INVALID_HANDLE_VALUE:FindNextFile(hFindFile,&w32fd))
	{
		if(w32fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY&&stricmp(w32fd.cFileName,"."))
		{
			if(!m_bEnumOnly)
			{
				memset(&sfi,0,sizeof(sfi));
				uFlags=SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME|SHGFI_TYPENAME|(!stricmp(w32fd.cFileName,"..")?SHGFI_USEFILEATTRIBUTES:0);
				SHGetFileInfo(w32fd.cFileName,FILE_ATTRIBUTE_DIRECTORY,&sfi,sizeof(sfi),uFlags);
				if(!strlen(sfi.szDisplayName))
					strcpy(sfi.szDisplayName,w32fd.cFileName);
			}
			else
				strcpy(sfi.szDisplayName,w32fd.cFileName);

			Entry.SetName(sfi.szDisplayName);
			Entry.SetFullName(w32fd.cFileName);
			Entry.SetDirectory(m_szSource);
			Entry.SetIcon(sfi.iIcon);
			Entry.SetType(LIST_ID_DIR);
			Entry.SetType(sfi.szTypeName);
			if(!stricmp(sfi.szDisplayName,".."))
			{
				w32fd.ftLastWriteTime.dwLowDateTime=0;
				w32fd.ftLastWriteTime.dwHighDateTime=0;
			}
			FILETIME ft=w32fd.ftLastWriteTime;
			if(!m_bArchived)
				FileTimeToLocalFileTime(&w32fd.ftLastWriteTime,&ft);
			Entry.SetModified(ft);
			Entry.IsArchived(m_bArchived);
			TempList.push_back(Entry);
		}
		bFirst=FALSE;
	}
	FindClose(hFindFile);
	BOOL bIn=FALSE;
	for(UINT i=0;i<TempList.size();i++)
	{
		TCHAR szBuf[MAX_PATH];
		TempList[i].GetFullName(szBuf,sizeof(szBuf));
		if(!stricmp(szBuf,".."))
		{
			bIn=TRUE;
			break;
		}
	}
	if(!bIn)
	{
		strcpy(w32fd.cFileName,"..");
		if(!m_bEnumOnly)
		{
			memset(&sfi,0,sizeof(sfi));
			SHGetFileInfo(w32fd.cFileName,FILE_ATTRIBUTE_DIRECTORY,&sfi,sizeof(sfi),SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME|SHGFI_TYPENAME|SHGFI_USEFILEATTRIBUTES);
			if(!strlen(sfi.szDisplayName))
				strcpy(sfi.szDisplayName,w32fd.cFileName);
		}
		else
			strcpy(sfi.szDisplayName,w32fd.cFileName);

		Entry.SetName(sfi.szDisplayName);
		Entry.SetFullName(w32fd.cFileName);
		Entry.SetDirectory(m_szSource);
		Entry.SetIcon(sfi.iIcon);
		Entry.SetType(LIST_ID_DIR);
		Entry.SetType(sfi.szTypeName);
		w32fd.ftLastWriteTime.dwLowDateTime=0;
		w32fd.ftLastWriteTime.dwHighDateTime=0;
		FILETIME ft=w32fd.ftLastWriteTime;
		if(!m_bArchived)
			FileTimeToLocalFileTime(&w32fd.ftLastWriteTime,&ft);
		Entry.SetModified(ft);
		Entry.IsArchived(m_bArchived);
		TempList.insert(TempList.begin(),Entry);
	}

	sort(TempList.begin(),TempList.end());
	for(i=0;i<TempList.size();i++)
		InsertEntry(TempList[i]);

	return TRUE;
}

BOOL CFileList::LoadFileList()
{
	if(!strlen(m_szSource))
		return TRUE;

	vector<CListEntry>TempList;
	TempList.clear();
	LPCTSTR szExtensions[]={".pl",".ace",".rar",".rsn",".zip",".spc",".sp0",".sp1",".sp2",".sp3",".sp4",".sp5",".sp6",".sp7",".sp8",".sp9"};
	HANDLE hFindFile;
	WIN32_FIND_DATA w32fd;
	BOOL bFirst=TRUE;
	TCHAR szExt[_MAX_PATH];
	SHFILEINFO sfi;
	CListEntry Entry;

	while(bFirst?(hFindFile=FindFirstFile(&w32fd))!=INVALID_HANDLE_VALUE:FindNextFile(hFindFile,&w32fd))
	{
		if(!(w32fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			_splitpath(w32fd.cFileName,NULL,NULL,NULL,szExt);
			for(INT i=m_bArchived?3:0;i<sizeof(szExtensions)/sizeof(szExtensions[0]);i++)
			{
				if(!stricmp(szExt,szExtensions[i]))
				{
					if(!m_bEnumOnly)
					{
						memset(&sfi,0,sizeof(sfi));
						SHGetFileInfo(w32fd.cFileName,FILE_ATTRIBUTE_NORMAL,&sfi,sizeof(sfi),SHGFI_SYSICONINDEX|SHGFI_DISPLAYNAME|SHGFI_TYPENAME);
						if(!strlen(sfi.szDisplayName))
							strcpy(sfi.szDisplayName,w32fd.cFileName);
					}
					else
						strcpy(sfi.szDisplayName,w32fd.cFileName);

					Entry.SetName(sfi.szDisplayName);
					Entry.SetFullName(w32fd.cFileName);
					Entry.SetDirectory(m_szSource);
					Entry.SetIcon(sfi.iIcon);
					switch(i)
					{
					case 0:Entry.SetType(LIST_ID_PL);break;
					case 1:Entry.SetType(LIST_ID_ACE);break;
					case 2:Entry.SetType(LIST_ID_RAR);break;
					case 3:Entry.SetType(LIST_ID_ZIP);break;
					default:Entry.SetType(LIST_ID_SPC);break;
					}
					Entry.SetSize(((LONGLONG)w32fd.nFileSizeHigh<<32)+w32fd.nFileSizeLow);
					Entry.SetType(sfi.szTypeName);
					FILETIME ft=w32fd.ftLastWriteTime;
					if(!m_bArchived)
						FileTimeToLocalFileTime(&w32fd.ftLastWriteTime,&ft);
					Entry.SetModified(ft);
					Entry.IsArchived(m_bArchived);
					TempList.push_back(Entry);
					break;
				}
			}
		}
		bFirst=FALSE;
	}
	FindClose(hFindFile);

	sort(TempList.begin(),TempList.end());
	for(INT i=0;i<TempList.size();i++)
		InsertEntry(TempList[i]);

	return TRUE;
}

HANDLE CFileList::FindFirstFile(LPWIN32_FIND_DATA lpFindFileData)
{
	if(!m_bArchived)
	{
		TCHAR szBuf[MAX_PATH];
		strcpy(szBuf,m_szSource);
		if(szBuf[strlen(szBuf)-1]!='\\')
			strcat(szBuf,"\\");
		strcat(szBuf,"*.*");
		return ::FindFirstFile(szBuf,lpFindFileData);
	}
	else
	{
		CExtractor *extractor=new CExtractor();
		if(extractor->Open(m_szArchiveFileName,TRUE))
		{
			extractor->SetCurrentDirectory(m_szSubSource);
			strcpy(lpFindFileData->cFileName,"..");
			lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
			return extractor;
		}
	}
	return INVALID_HANDLE_VALUE;
}

BOOL CFileList::FindNextFile(HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData)
{
	if(hFindFile==INVALID_HANDLE_VALUE)
		return FALSE;

	if(!m_bArchived)
	{
		return ::FindNextFile(hFindFile,lpFindFileData);
	}
	else
	{
		CExtractor *extractor=(CExtractor*)hFindFile;
		if(extractor->GetCurrentFile(FALSE,lpFindFileData))
		{
			extractor->SkipCurrentFile();
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CFileList::FindClose(HANDLE hFindFile)
{
	if(hFindFile==INVALID_HANDLE_VALUE)
		return FALSE;

	if(!m_bArchived)
		return ::FindClose(hFindFile);
	else
	{
		CExtractor *extractor=(CExtractor*)hFindFile;
		extractor->Close();
		delete extractor;
		return TRUE;
	}
	return FALSE;
}

DWORD CFileList::SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags)
{
	TCHAR szBuf[MAX_PATH];
	strcpy(szBuf,m_szSource);
	if(szBuf[strlen(szBuf)-1]!='\\')
		strcat(szBuf,"\\");
	strcat(szBuf,pszPath);
	uFlags|=m_bArchived?SHGFI_USEFILEATTRIBUTES:0;
	return ::SHGetFileInfo(szBuf,dwFileAttributes,psfi,cbFileInfo,uFlags);
}

BOOL CFileList::SetSource(LPCTSTR lpszSource,BOOL bUpdateLocationBar)
{
	if(!lpszSource||!strlen(lpszSource))
	{
		m_szSource[0]='\0';
		if(!m_bEnumOnly&&bUpdateLocationBar)
			app.m_MainWindow.m_SourceBar.SetLocationText(m_szSource);
		return TRUE;
	}

	TCHAR szParsedSource[MAX_PATH];
	strncpy(szParsedSource,m_szSource,sizeof(szParsedSource));

	if(lpszSource[0]=='\\')
		szParsedSource[3]='\0';

	INT nPart=0;
	TCHAR lpPart[MAX_PATH];
	while((nPart=GetNextPathPart(lpszSource,nPart,lpPart))!=-1)
	{
		if(strlen(lpPart)==2&&lpPart[1]==':')
		{
			strncpy(szParsedSource,lpPart,2);
			strcpy(&szParsedSource[2],"\\");
		}
		else if(!stricmp(lpPart,".."))
		{
			if(strlen(szParsedSource)==3)
				szParsedSource[0]='\0';
			else
			{
				for(INT i=strlen(szParsedSource)-1;i>=0;i--)
					if(szParsedSource[i]=='\\')
					{szParsedSource[i]='\0';break;}
				if(szParsedSource[strlen(szParsedSource)-1]==':')
					strcat(szParsedSource,"\\");
			}
		}
		else if(stricmp(lpPart,"."))
		{
			if(szParsedSource[strlen(szParsedSource)-1]!='\\')
				strcat(szParsedSource,"\\");
			strcat(szParsedSource,lpPart);
		}
	}

	if(strlen(szParsedSource))
	{
		m_bArchived=IsPathInArchive(szParsedSource,m_szArchiveFileName,m_szSubSource);
		if(!m_bArchived)
		{
			TCHAR szBuf[MAX_PATH];
			GetCurrentDirectory(MAX_PATH,szBuf);
			BOOL bRes=SetCurrentDirectory(szParsedSource);
			SetCurrentDirectory(szBuf);
			if(!bRes)
			{
				m_bArchived=IsPathInArchive(m_szSource,m_szArchiveFileName,m_szSubSource);
				return bRes;
			}		
		}
	}
	strncpy(m_szSource,szParsedSource,sizeof(m_szSource));
	m_szSource[sizeof(m_szSource)-1]='\0';

	if(!m_bEnumOnly&&bUpdateLocationBar)
		app.m_MainWindow.m_SourceBar.SetLocationText(m_szSource);
	return TRUE;
}

void CFileList::GetSource(LPTSTR lpszSource, DWORD nBufferLength)
{
	strncpy(lpszSource,m_szSource,nBufferLength-1);
	lpszSource[nBufferLength-1]='\0';
}

BOOL CFileList::IsPathInArchive(LPCTSTR lpPath, LPTSTR lpArchive, LPTSTR lpSubPath)
{
	TCHAR szPart[MAX_PATH];
	TCHAR szExt[MAX_PATH];
	INT pos=0;

	while((pos=GetNextPathPart(lpPath,pos,szPart))!=-1)
	{
		_splitpath(szPart,NULL,NULL,NULL,szExt);
		if(!stricmp(szExt,".ace")||!stricmp(szExt,".rar")||!stricmp(szExt,".rsn")||!stricmp(szExt,".zip"))
		{
			if(lpArchive)
			{
				strncpy(lpArchive,lpPath,pos);
				lpArchive[pos]='\0';
			}
			if(lpSubPath)
			{
				if(pos<strlen(lpPath))
					strcpy(lpSubPath,&lpPath[pos]);
				else
					strcpy(lpSubPath,"\\");
			}
			return TRUE;
		}
	}
	return FALSE;
}

INT CFileList::GetNextPathPart(LPCTSTR lpPath, INT nStartPos, LPTSTR lpPart)
{
	INT i=0;
	while(lpPath[nStartPos]=='\\')nStartPos++;
	while(nStartPos<strlen(lpPath)&&lpPath[nStartPos]!='\\')
		lpPart[i++]=lpPath[nStartPos++];
	if(i>0)
	{
		lpPart[i]='\0';
		return nStartPos;
	}
	return -1;
}

void CFileList::GetSubSource(LPTSTR lpszSubSource,DWORD nBufferLength)
{
	strncpy(lpszSubSource,m_szSubSource,nBufferLength);
	lpszSubSource[nBufferLength-1]='\0';
}

void CFileList::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(fDown)
	{
		switch(vk)
		{
		case VK_BACK:
			INT i;
			for(i=0;i<GetDisplayedCount();i++)
			{
				TCHAR szBuf[MAX_PATH];
				GetListEntry(i)->GetName(szBuf,MAX_PATH);
				if(!stricmp(szBuf,".."))
				{
					ProcessEntry(i);
					break;
				}
			}
			break;
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

void CFileList::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	CList::OnRButtonDown(hwnd,fDoubleClick,x,y,keyFlags);

	if(!(GetKeyState(VK_RBUTTON)&0x8000))
		DoContextMenu(x,y);
}

BOOL CFileList::DoContextMenu(INT nXPos, INT nYPos)
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
	BOOL bActiveCols[]={GetColumnIndexFromName("Name")!=-1,GetColumnIndexFromName("Size")!=-1,GetColumnIndexFromName("Type")!=-1,GetColumnIndexFromName("Modified")!=-1};
	AppendMenu(hMenu1,MF_STRING|MF_GRAYED|(bActiveCols[0]?MF_CHECKED:0),4,"&Name");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[1]?MF_CHECKED:0),5,"&Size");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[2]?MF_CHECKED:0),6,"&Type");
	AppendMenu(hMenu1,MF_STRING|(bActiveCols[3]?MF_CHECKED:0),7,"&Modified");

	HMENU hMenu2=CreatePopupMenu();
	if(!hMenu2)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return FALSE;
	}
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Name")?MF_CHECKED:0),8,"&Name");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Size")?MF_CHECKED:0),9,"&Size");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Type")?MF_CHECKED:0),10,"&Type");
	AppendMenu(hMenu2,MF_STRING|(!stricmp(m_szSortColumn,"Modified")?MF_CHECKED:0),11,"&Modified");
	AppendMenu(hMenu2,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu2,MF_STRING|(m_bSortOrder?MF_CHECKED:0),12,"&Ascending");
	AppendMenu(hMenu2,MF_STRING|(m_bSortOrder?0:MF_CHECKED),13,"&Descending");

	INT nIndex=GetFocusedEntry();
	if(nIndex<0)
		nIndex=GetSelectedEntry();
	CListEntry *Entry=GetListEntry(nIndex);

	AppendMenu(hMenu,MF_STRING,1,"&Open");
	AppendMenu(hMenu,MF_STRING,2,nIndex>=0&&Entry->GetType()==LIST_ID_SPC?"&Play":"&Add");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,3,"&Select All");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_POPUP,(UINT)hMenu1,"&Columns");
	AppendMenu(hMenu,MF_POPUP,(UINT)hMenu2,"Sort &by");
	AppendMenu(hMenu,MF_STRING,14,"&Resize Columns");
	AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hMenu,MF_STRING,15,"&Hide");

	EnableMenuItem(hMenu,1,MF_BYCOMMAND|(nIndex>=0?MF_ENABLED:MF_GRAYED));
	EnableMenuItem(hMenu,2,MF_BYCOMMAND|(nIndex>=0?MF_ENABLED:MF_GRAYED));
	EnableMenuItem(hMenu,3,MF_BYCOMMAND|GetDisplayedCount()?MF_ENABLED:MF_GRAYED);

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
	LPCTSTR lpszNames[]={"Name","Size","Type","Modified"};
	switch(nSelection)
	{
	case 1:
		ProcessEntry(nIndex);
		break;
	case 2:
		app.m_MainWindow.m_ToolBar.OnClicked(Entry->GetType()==LIST_ID_SPC?IDC_BUTTON_PLAY:IDC_BUTTON_ADD);
		break;
	case 3:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_EDIT_SELECT_ALL,0),(LPARAM)m_hWnd);
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		bActiveCols[nSelection-4]^=1;
		bColChange=TRUE;
		break;
	case 8:
	case 9:
	case 10:
	case 11:		
		strcpy(szSortCol,lpszNames[nSelection-8]);
		uSortChange=1;
		break;
	case 12:
	case 13:
		m_bSortOrder=(nSelection==12?1:0);
		uSortChange=2;
		break;
	case 14:
		ResizeColumns();
		break;
	case 15:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_FILELIST,0),(LPARAM)m_hWnd);
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
		SetRedraw(FALSE);
		INT nSelInd=GetSelectedEntry();
		ListView_DeleteAllItems(m_hWnd);
		RefreshHeader();
		DisplayEntries();
		SortItems(m_szSortColumn,m_bSortOrder);
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

void CFileList::EnumOnly(BOOL bFlag)
{
	m_bEnumOnly=bFlag;
}

INT CFileList::GetFileImage(LPCTSTR lpszPath,BOOL bOpened)
{
	SHFILEINFO sfi;
	memset(&sfi,0,sizeof(sfi));
	TCHAR szSubPath[MAX_PATH]={0};
	IsPathInArchive(lpszPath,NULL,szSubPath);
	UINT uFlags=SHGFI_SYSICONINDEX|(strlen(szSubPath)>2?SHGFI_USEFILEATTRIBUTES:0)|(bOpened?SHGFI_OPENICON:0);
	::SHGetFileInfo(lpszPath,FILE_ATTRIBUTE_DIRECTORY,&sfi,sizeof(sfi),uFlags);
	return sfi.iIcon;
}

LONGLONG CFileList::GetDriveSize(LPCTSTR lpszRoot)
{
	HINSTANCE hLib=LoadLibrary("Kernel32.dll");
	if(hLib!=NULL)
	{
		typedef BOOL (WINAPI *__GetDiskFreeSpaceEx)(LPCTSTR p1,PULARGE_INTEGER p2,PULARGE_INTEGER p3,PULARGE_INTEGER p4);
		__GetDiskFreeSpaceEx pFunc;
		if((pFunc=(__GetDiskFreeSpaceEx)GetProcAddress(hLib,"GetDiskFreeSpaceExA"))!=NULL)
		{
			ULARGE_INTEGER l1,l2,l3;
			if(pFunc(lpszRoot,&l1,&l2,&l3))
			{
				FreeLibrary(hLib);
				return l2.QuadPart;
			}
		}
		FreeLibrary(hLib);
	}
	DWORD dwSPC,dwBPS,dwNFC,dwTNC;
	if(GetDiskFreeSpace(lpszRoot,&dwSPC,&dwBPS,&dwNFC,&dwTNC))
		return (LONGLONG)dwSPC*dwBPS*dwTNC;
	return 0;
}
