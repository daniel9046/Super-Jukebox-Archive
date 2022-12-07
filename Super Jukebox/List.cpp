// List.cpp: implementation of the CList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "List.h"
#include "Resource.h"
#include "Main.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CList::CList()
{
	m_uColumnCount=0;
	m_bUpdateListView=TRUE;
	m_Entries.clear();
	m_hImageList=NULL;
	m_hDCBack=NULL;
	m_hBmpBack=NULL;
	m_hGridPen=NULL;
	m_OldClientSize.cx=m_OldClientSize.cy=-1;
	m_bMapped=FALSE;
	m_bDontPaint=FALSE;
	m_szOldSortColumn[0]='\0';
	m_hImlDownUpArrows=NULL;
}

CList::~CList()
{

}

BOOL CList::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(IsWindow(m_hWnd))
		return FALSE;

	m_bHasImageList=FALSE;
	m_Entries.clear();
	if(!RegisterClass(hInstance))
		return FALSE;
	RECT rc={0,0,100,100};
	if(!CWnd::Create("MyListView",NULL,WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SHAREIMAGELISTS,WS_EX_CLIENTEDGE,rc,pParentWnd,uID,hInstance))
		return FALSE;
	ListView_SetExtendedListViewStyleEx(m_hWnd,LVS_EX_FULLROWSELECT,app.m_Reg.m_bFullRowSelect?LVS_EX_FULLROWSELECT:0);
	ListView_SetExtendedListViewStyleEx(m_hWnd,LVS_EX_GRIDLINES,app.m_Reg.m_bShowGridLines?LVS_EX_GRIDLINES:0);
	ListView_SetExtendedListViewStyleEx(m_hWnd,LVS_EX_HEADERDRAGDROP,LVS_EX_HEADERDRAGDROP);
	m_hWndHeader=ListView_GetHeader(m_hWnd);
	m_hImlDownUpArrows=ImageList_LoadBitmap(m_hInstance,MAKEINTRESOURCE(IDB_DOWNUPARROWS),13,0,RGB(255,0,255));
	if(!m_hImlDownUpArrows)
		return FALSE;
	Header_SetImageList(m_hWndHeader,m_hImlDownUpArrows);
	return (m_bActive=TRUE);
}

BOOL CList::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	m_hDC=GetDC(m_hWnd);
	return CWnd::OnCreate(hwnd,lpCreateStruct);
}

BOOL CList::DisplayEntries(INT nFirst,INT nLast)
{
	if(nFirst==-1)nFirst=0;
	if(nLast==-1)nLast=m_Entries.size()-1;

	for(INT i=nFirst;i<=nLast;i++)
	{
		LVITEM item;
		item.mask=LVIF_IMAGE|LVIF_PARAM;
		item.iItem=ListView_GetItemCount(m_hWnd);
		item.iSubItem=0;
		item.iImage=m_Entries[i].GetIcon();
		item.lParam=i;
		if(!m_bRedraw)
			m_bDontPaint=TRUE;
		if(ListView_InsertItem(m_hWnd,&item)==-1)
			return FALSE;

		for(UINT c=0;c<m_uColumnCount;c++)
		{
			item.mask=LVIF_TEXT;
			item.iSubItem=c;
			TCHAR szColumnName[100],szName[MAX_PATH];;
			GetColumnNameFromIndex(c,szColumnName,sizeof(szColumnName));
			m_Entries[i].GetProperty(szColumnName,szName,sizeof(szName));
			item.pszText=szName;
			if(!ListView_SetItem(m_hWnd,&item))
				return FALSE;
		}
		m_bDontPaint=FALSE;
	}
	return TRUE;
}

UINT CList::GetEntryCount()
{
	return m_Entries.size();
}

UINT CList::GetDisplayedCount()
{
	return ListView_GetItemCount(m_hWnd);
}

BOOL CList::InsertColumn(LPCTSTR lpszLabel,INT nWidth)
{
	LVCOLUMN col;

	col.mask=LVCF_TEXT|LVCF_WIDTH;
	col.cx=nWidth;
	if(col.cx==-1)
		col.cx=ListView_GetStringWidth(m_hWnd,lpszLabel)+12;
	col.pszText=LPTSTR(lpszLabel);
	if(ListView_InsertColumn(m_hWnd,m_uColumnCount,&col)==-1)
		return FALSE;
	m_uColumnCount++;

	return TRUE;
}

void CList::ConvertTimeDown(INT nSeconds, LPTSTR lpszBuffer)
{
	INT nDay,nHour,nMinute,nSecond;
	nMinute=nSeconds/60;
	nSecond=nSeconds%60;
	nHour=nMinute/60;
	nMinute=nMinute%60;
	nDay=nHour/24;
	nHour=nHour%24;
	
	if(nDay>0)
		sprintf(lpszBuffer,"%d:%02d:%02d:%02d",nDay,nHour,nMinute,nSecond);
	else if(nHour>0)
		sprintf(lpszBuffer,"%d:%02d:%02d",nHour,nMinute,nSecond);
	else if(nMinute>0)
		sprintf(lpszBuffer,"%d:%02d",nMinute,nSecond);
	else if(nSecond>0)
		sprintf(lpszBuffer,"%d",nSecond);
	else
		sprintf(lpszBuffer,"0");
}

UINT CList::ConvertTimeUp(LPCTSTR lpszBuffer)
{
	UINT nDay=0,nHour=0,nMinute=0,nSecond=0,nIndex=0,nEntries=0;
	TCHAR szBuf[500];

	for(INT i=0;i<strlen(lpszBuffer)+1;i++)
		if(lpszBuffer[i]==':'||lpszBuffer[i]=='\0')nEntries++;
	for(i=0;i<strlen(lpszBuffer)+1;i++)
	{
		szBuf[nIndex]=lpszBuffer[i];
		if(lpszBuffer[i]==':'||lpszBuffer[i]=='\0')
		{
			szBuf[nIndex]='\0';
			switch(nEntries)
			{
			case 1:nSecond=atoi(szBuf);break;
			case 2:nMinute=atoi(szBuf);break;
			case 3:nHour=atoi(szBuf);break;
			case 4:nDay=atoi(szBuf);break;
			}
			if(nEntries==1)
				break;
			nIndex=0;nEntries--;
		}
		else nIndex++;
	}

	return nDay*86400+nHour*3600+nMinute*60+nSecond;
}

CListEntry* CList::GetListEntry(INT nIndex)
{
	nIndex=MapIndexToInternal(nIndex);
	if(nIndex>=0)
		return &m_Entries[nIndex];
	return NULL;
}

CListEntry* CList::GetListEntryUnmapped(INT nIndex)
{
	if(nIndex>=0)
		return &m_Entries[nIndex];
	return NULL;
}

INT CList::GetSelectedEntry()
{
	UINT uItemCount=ListView_GetItemCount(m_hWnd);
	for(UINT i=0;i<uItemCount;i++)
	{
		if(ListView_GetItemState(m_hWnd,i,LVIS_SELECTED)==LVIS_SELECTED)
			return i;
	}
	return -1;
}

INT CList::GetFocusedEntry()
{
	UINT uItemCount=ListView_GetItemCount(m_hWnd);
	for(UINT i=0;i<uItemCount;i++)
	{
		if(ListView_GetItemState(m_hWnd,i,LVIS_FOCUSED)==LVIS_FOCUSED)
			return i;
	}
	return -1;
}

BOOL CList::SelectEntry(INT nIndex)
{
	UINT uItemCount=ListView_GetItemCount(m_hWnd);
	if(nIndex>=0&&nIndex<uItemCount)
	{
		for(UINT i=0;i<uItemCount;i++)
		{
			if(ListView_GetItemState(m_hWnd,i,LVIS_SELECTED)==LVIS_SELECTED)
				ListView_SetItemState(m_hWnd,i,0,LVIS_SELECTED);
		}
		ListView_SetItemState(m_hWnd,nIndex,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		EnsureVisible(nIndex,-1);
		return TRUE;
	}
	return FALSE;
}

BOOL CList::InsertEntry(CListEntry &ListEntry,INT nIndex)
{
	m_Entries.push_back(ListEntry);
	INT nIndex0=m_Entries.size()-1;
	m_Entries[nIndex0].IsAdded(TRUE);
	m_Entries[nIndex0].IsDisplayed(FALSE);
	m_Entries[nIndex0].SetListID(m_uID);
	m_Entries[nIndex0].SetListIndex(nIndex0);

	if(m_bUpdateListView)
	{
		LVITEM item;
		item.mask=LVIF_IMAGE|LVIF_PARAM;
		if(nIndex<0)
			item.iItem=ListView_GetItemCount(m_hWnd);
		else
			item.iItem=nIndex;
		item.iSubItem=0;
		item.iImage=m_Entries[nIndex0].GetIcon();
		item.lParam=nIndex0;
		if(!m_bRedraw)
			m_bDontPaint=TRUE;
		if(ListView_InsertItem(m_hWnd,&item)==-1)
			return FALSE;

		for(UINT c=0;c<m_uColumnCount;c++)
		{
			item.mask=LVIF_TEXT;
			item.iSubItem=c;
			TCHAR szColumnName[100],szName[MAX_PATH];
			GetColumnNameFromIndex(c,szColumnName,sizeof(szColumnName));
			m_Entries[nIndex0].GetProperty(szColumnName,szName,sizeof(szName));
			item.pszText=szName;
			if(!ListView_SetItem(m_hWnd,&item))
				return FALSE;
		}
		m_bDontPaint=FALSE;
	}

	return TRUE;
}

BOOL CList::RemoveEntry(INT nIndex)
{
	if(nIndex>=0)
	{
		INT nMappedIndex=MapIndexToInternal(nIndex);
		m_Entries.erase(m_Entries.begin()+nMappedIndex);
		for(INT i=nMappedIndex;i<m_Entries.size();i++)
			m_Entries[i].SetListIndex(i);
		if(m_bUpdateListView)
		{
			ListView_DeleteItem(m_hWnd,nIndex);
			UINT uItemCount=ListView_GetItemCount(m_hWnd);
			for(UINT i=0;i<uItemCount;i++)
			{
				LVITEM lvi;
				lvi.mask=LVIF_PARAM;
				lvi.iItem=i;
				lvi.iSubItem=0;
				ListView_GetItem(m_hWnd,&lvi);
				if(lvi.lParam>nMappedIndex)
				{
					lvi.lParam--;
					ListView_SetItem(m_hWnd,&lvi);
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CList::Clear()
{
	if(app.m_Player.GetPlayingList()==this)
		app.m_MainWindow.m_ToolBar.OnClicked(IDC_BUTTON_STOP);

	m_Entries.clear();
	if(m_bUpdateListView)
		ListView_DeleteAllItems(m_hWnd);
}

BOOL CList::SelectRelativeEntry(INT nOffset)
{
	return SelectEntry(GetSelectedEntry()+nOffset);
}

BOOL CList::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,WC_LISTVIEW,&wcx))
		return FALSE;
	m_OriginalWndProc=wcx.lpfnWndProc;
	if(!GetClassInfoEx(hInstance,"MyListView",&wcx))
	{
		wcx.lpszClassName="MyListView";
		wcx.lpfnWndProc=CWnd::WindowProc;
		wcx.style|=CS_OWNDC;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

BOOL CList::ResizeColumns(INT nColumn)
{
	if(m_uColumnCount<=0)
		return FALSE;

	TCHAR szOldStatus[500];
	app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
	app.m_MainWindow.m_StatusBar.SetText(3,"Resizing list view columns...");
	app.m_MainWindow.MaintainCursor(TRUE);
	app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));

	UINT uItemCount=ListView_GetItemCount(m_hWnd);

	UINT uStart=0,uEnd=m_uColumnCount-1;
	if(nColumn>=0)
		uStart=uEnd=nColumn;
	TCHAR szBuffer[500];
	UINT *uColWidths=new UINT[m_uColumnCount];
	for(UINT uCol=uStart;uCol<=uEnd;uCol++)
	{
		uColWidths[uCol]=12;
		if(uItemCount)
		{
			for(UINT i=0;i<uItemCount;i++)
			{
				ListView_GetItemText(m_hWnd,i,uCol,szBuffer,sizeof(szBuffer));
				uColWidths[uCol]=max(uColWidths[uCol],ListView_GetStringWidth(m_hWnd,szBuffer)+12+(m_bHasImageList&&uCol==0?12:0));
			}
		}
		else
		{
			LVCOLUMN lvc;
			lvc.mask=LVCF_TEXT;
			lvc.pszText=szBuffer;
			lvc.cchTextMax=sizeof(szBuffer);
			ListView_GetColumn(m_hWnd,uCol,&lvc);
			uColWidths[uCol]=ListView_GetStringWidth(m_hWnd,szBuffer)+12;
		}
	}
	for(uCol=uStart;uCol<=uEnd;uCol++)
	{
		if(uColWidths[uCol]!=ListView_GetColumnWidth(m_hWnd,uCol))
			ListView_SetColumnWidth(m_hWnd,uCol,uColWidths[uCol]);
	}
	delete[]uColWidths;
	InvalidateRect(m_hWndHeader,NULL,TRUE);
	app.m_MainWindow.MaintainCursor(FALSE);
	app.m_MainWindow.SetCursor(NULL);
	app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	return TRUE;
}

UINT CList::GetEntryState(INT nIndex)
{
	return ListView_GetItemState(m_hWnd,nIndex,LVIS_FOCUSED|LVIS_SELECTED);
}

void CList::SetEntryState(INT nIndex,UINT uState)
{
	ListView_SetItemState(m_hWnd,nIndex,uState,LVIS_FOCUSED|LVIS_SELECTED);
}

BOOL CList::ProcessEntry(INT nIndex)
{
	return TRUE;
}

UINT CList::GetSelectedCount()
{
	UINT uCount=0;
	UINT uItemCount=ListView_GetItemCount(m_hWnd);
	for(UINT i=0;i<uItemCount;i++)
	{
		if(ListView_GetItemState(m_hWnd,i,LVIS_SELECTED)==LVIS_SELECTED)
			uCount++;
	}
	return uCount;;
}

UINT CList::GetTypeCount(INT nType,BOOL bOnlySelected)
{
	UINT uCount=0;
	UINT uItemCount=ListView_GetItemCount(m_hWnd);
	for(UINT i=0;i<uItemCount;i++)
	{
		if(m_Entries[i].GetType()==nType&&(bOnlySelected?ListView_GetItemState(m_hWnd,i,LVIS_SELECTED)==LVIS_SELECTED:TRUE))
			uCount++;
	}
	return uCount;
}

UINT CList::GetColumnCount()
{
	return m_uColumnCount;
}

BOOL CList::EnsureVisible(INT iItem, INT iSubItem)
{
	if(!m_bRedraw)
		return FALSE;

	INT nXPos,nOldXPos,nYPos,nOldYPos;
	SCROLLINFO si;
	si.cbSize=sizeof(si);
	si.fMask=SIF_POS;
	GetScrollInfo(m_hWnd,SB_HORZ,&si);
	nOldXPos=nXPos=si.nPos;
	GetScrollInfo(m_hWnd,SB_VERT,&si);
	nOldYPos=nYPos=si.nPos;

	if(iItem<0)
		iItem=0;
	else if(iItem>=ListView_GetItemCount(m_hWnd))
		iItem=ListView_GetItemCount(m_hWnd)-1;

	INT iFirstItem=ListView_GetTopIndex(m_hWnd);
	INT iLastItem=iFirstItem+ListView_GetCountPerPage(m_hWnd);

	RECT rt;
	ListView_GetItemRect(m_hWnd,0,&rt,LVIR_BOUNDS);
	nOldYPos=nYPos*=(rt.bottom-rt.top);

	if(iItem<iFirstItem)
		nYPos=iItem*(rt.bottom-rt.top);
	else if(iItem>=iLastItem)
		nYPos=(iItem-ListView_GetCountPerPage(m_hWnd)+1)*(rt.bottom-rt.top);

	if(iSubItem>=0)
	{
		if(iSubItem>=m_uColumnCount)
			iSubItem=m_uColumnCount-1;

		RECT ListViewRect,ItemRect;
		GetClientRect(m_hWnd,&ListViewRect);
		ListView_GetSubItemRect(m_hWnd,iItem,iSubItem,LVIR_LABEL,&ItemRect);

		if(ItemRect.left<ListViewRect.left)
			nXPos+=ItemRect.left;
		if(ItemRect.right>ListViewRect.right&&(ItemRect.right-ItemRect.left)<(ListViewRect.right-ListViewRect.left))
			nXPos+=ItemRect.right-ListViewRect.right;
	}
	ListView_Scroll(m_hWnd,nXPos-nOldXPos,nYPos-nOldYPos);
	RedrawWindow(m_hWnd,NULL,NULL,RDW_UPDATENOW);

	return TRUE;
}

INT CList::GetColumnIndexFromName(LPCTSTR lpszName)
{
	for(INT i=0;i<m_uColumnCount;i++)
	{
		TCHAR szBuf[200];
		HDITEM hdi;
		hdi.mask=HDI_TEXT;
		hdi.pszText=szBuf;
		hdi.cchTextMax=sizeof(szBuf);
		Header_GetItem(m_hWndHeader,i,&hdi);
		if(!stricmp(lpszName,szBuf))
			return i;
	}
	return -1;
}

BOOL CList::GetColumnNameFromIndex(INT nIndex,LPTSTR lpszName,INT nTextMax)
{
	HDITEM hdi;
	hdi.mask=HDI_TEXT;
	hdi.pszText=lpszName;
	hdi.cchTextMax=nTextMax;
	return Header_GetItem(m_hWndHeader,nIndex,&hdi);
}

void CList::UpdateListView(BOOL bUpdate)
{
	m_bUpdateListView=bUpdate;
}

BOOL CList::SetSystemImageList()
{
	SHFILEINFO sfi;
	memset(&sfi,0,sizeof(sfi));
	m_hImageList=(HIMAGELIST)SHGetFileInfo("",0,&sfi,sizeof(sfi),SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
	if(!ImageList_GetImageCount(m_hImageList))
		return FALSE;
	if(!ListView_SetImageList(m_hWnd,m_hImageList,LVSIL_SMALL)&&m_bHasImageList)
		return FALSE;
	m_bHasImageList=TRUE;
	Header_SetImageList(m_hWndHeader,m_hImlDownUpArrows);
	return TRUE;
}

LRESULT CList::OnDblClk(LPNMLISTVIEW lpNMListView)
{
	ProcessEntry(lpNMListView->iItem);
	return 0;
}

LRESULT CList::OnReturn(LPNMHDR lpNMHdr)
{
	ProcessEntry(GetSelectedEntry());
	return 0;
}

void CList::OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(fDown)
	{
		switch(vk)
		{
		case VK_TAB:
			SetFocus(GetNextDlgTabItem(app.m_MainWindow.m_hWnd,m_hWnd,(GetKeyState(VK_SHIFT)&0x8000)?TRUE:FALSE));
			return;
		}
	}
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	CWnd::OnKey(hwnd,vk,fDown,cRepeat,flags);
}

void CList::OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	CWnd::OnHScroll(hwnd,hwndCtl,code,pos);
}

void CList::OnVScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	CWnd::OnVScroll(hwnd,hwndCtl,code,pos);
}

BOOL CList::OnEnsureVisible(INT nIndex, BOOL bPartialOK)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	return CWnd::OnEnsureVisible(nIndex,bPartialOK);
}

BOOL CList::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		return TRUE;
	return CWnd::OnEraseBkgnd(hwnd,hdc);
}

void CList::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	GetClientRect(m_hWnd,&m_ClientRect);
	m_ClientSize.cx=m_ClientRect.right-m_ClientRect.left;
	m_ClientSize.cy=m_ClientRect.bottom-m_ClientRect.top;
	m_ParentRect=m_ClientRect;
	ClientToScreen(m_hWnd,&m_ParentRect);
	ScreenToClient(app.m_MainWindow.m_hWnd,&m_ParentRect);

	if(app.m_MainWindow.GetActiveSkin().IsActive())
	{
		RECT rc0=m_ClientRect,rc1;
		GetWindowRect(m_hWndHeader,&rc1);
		ScreenToClient(m_hWnd,&rc1);
		rc0.top=rc1.bottom;
		InvalidateRect(m_hWnd,&rc0,FALSE);
	}
	CWnd::OnSize(hwnd,state,cx,cy);
}

void CList::OnPaint(HWND hwnd)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
	{
		if(GetUpdateRect(m_hWnd,NULL,FALSE))
			ValidateRect(m_hWnd,NULL);

		if(m_bDontPaint||m_ClientSize.cx<=0||m_ClientSize.cy<=0)
			return;

		BOOL bIsFocused=(GetFocus()==m_hWnd);
		BOOL bDrawGridLines=(ListView_GetExtendedListViewStyle(m_hWnd)&LVS_EX_GRIDLINES);
		BOOL bFullRowSelect=(ListView_GetExtendedListViewStyle(m_hWnd)&LVS_EX_FULLROWSELECT);

		INT nOrder[NUMBER_OF_COLUMNS];
		Header_GetOrderArray(m_hWndHeader,m_uColumnCount,nOrder);

		if(m_OldClientSize.cx!=m_ClientSize.cx||m_OldClientSize.cy!=m_ClientSize.cy)
		{
			if(m_hDCBack)DeleteDC(m_hDCBack);
			if(m_hBmpBack)DeleteObject(m_hBmpBack);
			if(m_hGridPen)DeleteObject(m_hGridPen);
			m_hDCBack=CreateCompatibleDC(m_hDC);
			m_hBmpBack=CreateCompatibleBitmap(m_hDC,m_ClientSize.cx,m_ClientSize.cy);
			SelectObject(m_hDCBack,m_hBmpBack);
			m_hGridPen=CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DFACE));
			SelectObject(m_hDCBack,m_hGridPen);
			SelectObject(m_hDCBack,(HFONT)SendMessage(m_hWnd,WM_GETFONT,0,0));
			SetBkMode(m_hDCBack,TRANSPARENT);
			SetROP2(m_hDCBack,R2_NOT);
			m_OldClientSize=m_ClientSize;
		}

		BitBlt(m_hDCBack,0,0,m_ClientSize.cx,m_ClientSize.cy,app.m_MainWindow.GetActiveSkin().GetDC(),m_ParentRect.left-app.m_MainWindow.m_ClientRect.left,m_ParentRect.top-app.m_MainWindow.m_ClientRect.top,SRCCOPY);
			
		INT nTop=ListView_GetTopIndex(m_hWnd);
		INT nLast=ListView_GetCountPerPage(m_hWnd)+nTop;
		if(nLast>=ListView_GetItemCount(m_hWnd))
			nLast=ListView_GetItemCount(m_hWnd)-1;

		RECT rc0,rc1,rc2;
		LVITEM lvi0,lvi1;
		TCHAR szItemText[1000];
		lvi0.mask=LVIF_IMAGE|LVIF_STATE;
		lvi0.stateMask=LVIS_SELECTED|LVIS_FOCUSED;
		lvi0.iSubItem=0;
		lvi1.mask=LVIF_TEXT;
		lvi1.pszText=szItemText;
		lvi1.cchTextMax=sizeof(szItemText);

		for(INT y=nTop;y<=nLast;y++)
		{
			if(m_bRedraw?TRUE:m_Entries[y].IsDisplayed())
			{
				lvi0.iItem=y;
				ListView_GetItem(m_hWnd,&lvi0);
				ListView_GetItemRect(m_hWnd,y,&rc0,LVIR_SELECTBOUNDS);

				rc0.left+=(m_bHasImageList&&(!bFullRowSelect||!nOrder[0])?17:0);
				if(lvi0.state&LVIS_SELECTED&&rc0.left<=rc0.right)
					FillRect(m_hDCBack,&rc0,(HBRUSH)((bIsFocused?COLOR_HIGHLIGHT:COLOR_3DFACE)+1));

				if(m_hImageList)
				{
					ListView_GetItemRect(m_hWnd,y,&rc1,LVIR_ICON);
					IntersectClipRect(m_hDCBack,rc1.left,rc1.top,rc1.right,rc1.bottom);
					ImageList_Draw(m_hImageList,lvi0.iImage,m_hDCBack,rc1.left,rc1.top,ILD_TRANSPARENT|((lvi0.state&LVIS_SELECTED&&bIsFocused)?ILD_SELECTED:0));
					SelectClipRgn(m_hDCBack,NULL);
				}
				
								
				for(INT x=0;x<m_uColumnCount;x++)
				{
					ListView_GetSubItemRect(m_hWnd,y,x,LVIR_LABEL,&rc1);
					if(IntersectRect(&rc2,&m_ClientRect,&rc1))
					{
						lvi1.iItem=y;
						lvi1.iSubItem=x;
						ListView_GetItem(m_hWnd,&lvi1);
						if(strlen(szItemText))
						{
							rc1.left+=x?6:2;
							SetTextColor(m_hDCBack,lvi0.state&LVIS_SELECTED?bIsFocused?!x||bFullRowSelect?GetSysColor(COLOR_HIGHLIGHTTEXT):app.m_MainWindow.GetActiveSkin().GetTextColor():!x||bFullRowSelect?GetSysColor(COLOR_WINDOWTEXT):app.m_MainWindow.GetActiveSkin().GetTextColor():app.m_MainWindow.GetActiveSkin().GetTextColor());
							DrawText(m_hDCBack,szItemText,strlen(szItemText),&rc1,DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
						}
					}
				}

				if(lvi0.state&LVIS_FOCUSED&&bIsFocused&&rc0.left<=rc0.right)
				{
					SetTextColor(m_hDCBack,0);
					DrawFocusRect(m_hDCBack,&rc0);
				}
				m_Entries[y].IsDisplayed(TRUE);
			}
		}
		if(bDrawGridLines)
		{
			for(INT x=0;x<m_uColumnCount;x++)
			{
				Header_GetItemRect(m_hWndHeader,x,&rc0);
				MoveToEx(m_hDCBack,rc0.right,rc0.bottom,NULL);
				LineTo(m_hDCBack,rc0.right,m_ClientRect.bottom);
			}
			nLast=ListView_GetCountPerPage(m_hWnd)+nTop;
			for(INT y=nTop;y<=nLast;y++)
			{
				POINT pt;
				ListView_GetItemPosition(m_hWnd,y,&pt);
				MoveToEx(m_hDCBack,0,pt.y,NULL);
				LineTo(m_hDCBack,m_ClientRect.right,pt.y);
			}
		}
		BitBlt(m_hDC,0,0,m_ClientSize.cx,m_ClientSize.cy,m_hDCBack,0,0,SRCCOPY);
		return;
	}
	CWnd::OnPaint(hwnd);
}

LRESULT CList::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive()&&lpNMHdr->hwndFrom==m_hWndHeader)
	{
		RECT rc0=m_ClientRect,rc1;
		GetWindowRect(m_hWndHeader,&rc1);
		ScreenToClient(m_hWnd,&rc1);
		rc0.top=rc1.bottom;
		InvalidateRect(m_hWnd,&rc0,FALSE);
	}
	return CWnd::OnNotify(hwnd,nIdCtrl,lpNMHdr);
}

LRESULT CList::OnDestroy(HWND hwnd)
{
	if(m_hDCBack)DeleteDC(m_hDCBack);
	if(m_hBmpBack)DeleteObject(m_hBmpBack);
	if(m_hGridPen)DeleteObject(m_hGridPen);
	if(m_hImlDownUpArrows)ImageList_Destroy(m_hImlDownUpArrows);
	ReleaseDC(m_hWnd,m_hDC);
	return CWnd::OnDestroy(hwnd);
}

BOOL CList::OnScroll(INT dx, INT dy)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	return CWnd::OnScroll(dx,dy);
}

BOOL CList::SearchEntry(INT nEntry, LPCTSTR lpszColumn, LPCTSTR lpszSearchString, BOOL bMatchWholeWord, BOOL bMatchCase)
{
	if((nEntry=MapIndexToInternal(nEntry))<0)
		return FALSE;

	TCHAR *lpszDest=NULL,szBuf0[1000],szBuf1[1000];
	strncpy(szBuf1,lpszSearchString,sizeof(szBuf1));
	m_Entries[nEntry].GetProperty(lpszColumn,szBuf0,sizeof(szBuf0));
	if(!bMatchCase)
	{
		strlwr(szBuf0);
		strlwr(szBuf1);
	}

	vector<_SEARCHITEM>SearchItems;
	SearchItems.clear();
	UINT uStrLen=strlen(szBuf1);

	for(UINT i=0;i<uStrLen;)
	{
		_SEARCHITEM item;
		UINT x=0;
		while(i<uStrLen)
		{
			if(szBuf1[i]!=' ')
				item.szBuf[x++]=szBuf1[i++];
			else
			{
				TCHAR szBuf[5];
				strncpy(szBuf,&szBuf1[i],4);
				szBuf[4]='\0';
				if(!strcmp(szBuf," && ")){item.bRelation=FALSE;i+=4;break;}
				else if(!strcmp(szBuf," || ")){item.bRelation=TRUE;i+=4;break;}
				else item.szBuf[x++]=szBuf1[i++];
			}
		}
		item.szBuf[x]='\0';
		SearchItems.push_back(item);
	}

	vector<BOOL>Matches;
	Matches.clear();
	for(i=0;i<SearchItems.size();i++)
	{
		INT nComparison=-1,nTextIndex=0;
		BOOL bMatch=FALSE,bNot=FALSE;
		strcpy(szBuf1,SearchItems[i].szBuf);

		if(szBuf1[0]=='>'){
			nComparison=0;nTextIndex=1;
			if(szBuf1[1]=='='){nComparison=1;nTextIndex=2;}
		}
		else if(szBuf1[0]=='<'){
			nComparison=2;nTextIndex=1;
			if(szBuf1[1]=='='){nComparison=3;nTextIndex=2;}
		}
		else if(szBuf1[0]=='='&&szBuf1[1]=='='){nComparison=4;nTextIndex=2;}
		else if(szBuf1[0]=='!')
		{
			if(szBuf1[1]=='='){nComparison=5;nTextIndex=2;}
			else {bNot=TRUE;nTextIndex=1;}
		}

		if(nComparison>=0)
		{
			INT nVal0,nVal1;
			if(!stricmp(lpszColumn,"Length")||!stricmp(lpszColumn,"Fade"))
				nVal0=ConvertTimeUp(szBuf0);
			else
				nVal0=atoi(szBuf0);
			if(strchr(szBuf1,':'))
				nVal1=ConvertTimeUp(&szBuf1[nTextIndex]);
			else
				nVal1=atoi(&szBuf1[nTextIndex]);
			switch(nComparison)
			{
			case 0:			
				if(nVal0>nVal1)bMatch=TRUE;
				break;
			case 1:
				if(nVal0>=nVal1)bMatch=TRUE;
				break;
			case 2:
				if(nVal0<nVal1)bMatch=TRUE;
				break;
			case 3:
				if(nVal0<=nVal1)bMatch=TRUE;
				break;
			case 4:
				if(nVal0==nVal1)bMatch=TRUE;
				break;
			case 5:
				if(nVal0!=nVal1)bMatch=TRUE;
			}
		}
		else
		{
			lpszDest=strstr(szBuf0,&szBuf1[nTextIndex]);
			if(lpszDest)
			{
				if(bMatchWholeWord)
				{
					BOOL bBefore,bAfter;
					if(lpszDest>szBuf0)
						bBefore=__iscsym(*(lpszDest-1));
					else bBefore=FALSE;
					bAfter=__iscsym(*(lpszDest+strlen(szBuf1)));
					if(!bBefore&&!bAfter)
						bMatch=TRUE;
				}
				else bMatch=TRUE;
			}
		}
		Matches.push_back(bMatch^bNot);
	}
	
	BOOL bMatch=Matches[0];
	for(i=0;i<SearchItems.size()-1;i++)
	{
		if(SearchItems[i].bRelation)
			bMatch=bMatch||Matches[i+1];
		else
			bMatch=bMatch&&Matches[i+1];
	}
	return bMatch;
}

INT CList::FindEntry(INT nStartEntry, LPCTSTR lpszColumn, LPCTSTR lpszSearchString, BOOL bMatchWholeWord, BOOL bMatchCase, BOOL bDirection)
{
	for(INT i=bDirection?nStartEntry+1:nStartEntry-1;bDirection?i<GetDisplayedCount():i>=0;bDirection?i++:i--)
		if(SearchEntry(i,lpszColumn,lpszSearchString,bMatchWholeWord,bMatchCase))
			return i;
	return -1;
}

void CList::IsVisible(BOOL bVisible)
{
	CWnd::IsVisible(bVisible);
	if(bVisible)
	{
		GetClientRect(m_hWnd,&m_ClientRect);
		m_ClientSize.cx=m_ClientRect.right-m_ClientRect.left;
		m_ClientSize.cy=m_ClientRect.bottom-m_ClientRect.top;
		m_ParentRect=m_ClientRect;
		ClientToScreen(m_hWnd,&m_ParentRect);
		ScreenToClient(app.m_MainWindow.m_hWnd,&m_ParentRect);
	}
}

INT CList::MapIndexToInternal(INT nListViewItem)
{
	if(!m_bMapped)
		return nListViewItem;

	if(nListViewItem>=0&&nListViewItem<ListView_GetItemCount(m_hWnd))
	{
		LVITEM lvi;
		lvi.mask=LVIF_PARAM;
		lvi.iItem=nListViewItem;
		lvi.iSubItem=0;
		ListView_GetItem(m_hWnd,&lvi);
		return lvi.lParam;
	}
	return -1;
}

INT CList::MapIndexToExternal(INT nListViewItem)
{
	if(!m_bMapped)
		return nListViewItem;

	if(nListViewItem>=0&&nListViewItem<m_Entries.size())
	{
		LVFINDINFO lvfi;
		lvfi.flags=LVFI_PARAM;
		lvfi.lParam=nListViewItem;
		return ListView_FindItem(m_hWnd,-1,&lvfi);
	}
	return -1;
}

void CList::IsMapped(BOOL bMapped)
{
	m_bMapped=bMapped;
}

BOOL CList::IsMapped()
{
	return m_bMapped;
}

LRESULT CList::OnTimer(HWND hwnd, UINT id)
{
	if(app.m_MainWindow.GetActiveSkin().IsActive())
		InvalidateRect(m_hWnd,NULL,FALSE);
	return CWnd::OnTimer(hwnd,id);
}

BOOL CList::IsVisible()
{
	return CWnd::IsVisible();
}

BOOL CList::SortEntries(INT nFirst, INT nLast)
{
	if(nFirst<0)
		nFirst=0;
	if(nLast<0||nLast>=m_Entries.size())
		nLast=m_Entries.size()-1;
	sort(m_Entries.begin()+nFirst,m_Entries.begin()+(nLast+1));

	for(INT i=nFirst;i<=nLast;i++)
		m_Entries[i].SetListIndex(i);
	return TRUE;
}

BOOL CList::RefreshListView()
{
	ListView_DeleteAllItems(m_hWnd);
	return DisplayEntries();
}

LRESULT CList::OnColumnClick(LPNMLISTVIEW lpNMListView)
{
	TCHAR szColName[50];
	GetColumnNameFromIndex(lpNMListView->iSubItem,szColName,sizeof(szColName));
	stricmp(m_szSortColumn,szColName)?m_bSortOrder=TRUE:m_bSortOrder^=1;
	SortItems(szColName,m_bSortOrder);
	return 0;
}

void CList::SortItems(LPCTSTR lpszColumn, BOOL bOrder)
{
	if(lpszColumn&&strlen(lpszColumn))
	{
		TCHAR szOldStatus[500];
		app.m_MainWindow.m_StatusBar.GetText(3,szOldStatus,sizeof(szOldStatus));
		app.m_MainWindow.m_StatusBar.SetText(3,"Sorting playlist entries...");
		app.m_MainWindow.MaintainCursor(TRUE);
		app.m_MainWindow.SetCursor(LoadCursor(NULL,IDC_WAIT));
		strncpy(m_szSortColumn,lpszColumn,sizeof(m_szSortColumn));
		m_bSortOrder=bOrder;
		ListView_SortItems(m_hWnd,CompareFunc,(LPARAM)this);
		SetSortingColumn(m_szSortColumn,m_bSortOrder);
		INT nIndex=GetFocusedEntry();
		if(nIndex<0)
			nIndex=GetSelectedEntry();
		EnsureVisible(nIndex,-1);
		app.m_MainWindow.MaintainCursor(FALSE);
		app.m_MainWindow.SetCursor(NULL);
		app.m_MainWindow.m_StatusBar.SetText(3,szOldStatus);
	}
	else
		SetSortingColumn(lpszColumn,bOrder);
}

int CALLBACK CList::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CList *list=(CPlayList*)lParamSort;

	if(list->m_Entries[lParam1].GetType()==LIST_ID_DIR&&list->m_Entries[lParam2].GetType()!=LIST_ID_DIR)
		return 0;

	TCHAR szBuf1[1000];
	list->m_Entries[lParam1].GetProperty(list->m_szSortColumn,szBuf1,sizeof(szBuf1));
	TCHAR szBuf2[1000];
	list->m_Entries[lParam2].GetProperty(list->m_szSortColumn,szBuf2,sizeof(szBuf2));

	if(!stricmp(list->m_szSortColumn,"Length")||!stricmp(list->m_szSortColumn,"Fade"))
	{
		INT nVal1=list->ConvertTimeUp(szBuf1);
		INT nVal2=list->ConvertTimeUp(szBuf2);
		if(nVal1>nVal2)return list->m_bSortOrder?1:-1;
		else if(nVal1<nVal2)return list->m_bSortOrder?-1:1;
		return 0;
	}
	else if(!stricmp(list->m_szSortColumn,"Name"))
	{
		if(list->m_Entries[lParam1].GetType==LIST_ID_DRV)
			list->m_Entries[lParam1].GetFullName(szBuf1,sizeof(szBuf1));
		if(list->m_Entries[lParam2].GetType==LIST_ID_DRV)
			list->m_Entries[lParam2].GetFullName(szBuf2,sizeof(szBuf2));
	}
	else if(!stricmp(list->m_szSortColumn,"Size"))
	{
		LONGLONG llSize1=list->m_Entries[lParam1].GetSize();
		LONGLONG llSize2=list->m_Entries[lParam2].GetSize();
		if(llSize1>llSize2)return list->m_bSortOrder?1:-1;
		else if(llSize1<llSize2)return list->m_bSortOrder?-1:1;
		return 0;
	}
	else if(!stricmp(list->m_szSortColumn,"Modified"))
	{
		FILETIME ft1=list->m_Entries[lParam1].GetModified();
		FILETIME ft2=list->m_Entries[lParam2].GetModified();
		return CompareFileTime(&ft1,&ft2)*(list->m_bSortOrder?1:-1);
	}

	TCHAR szBuf3[1000];
	for(UINT i=0,c=0;i<strlen(szBuf1);i++)
		if(isalnum(szBuf1[i])||isspace(szBuf1[i]))
			szBuf3[c++]=szBuf1[i];
	szBuf3[c]='\0';
	TCHAR szBuf4[1000];
	for(i=0,c=0;i<strlen(szBuf2);i++)
		if(isalnum(szBuf2[i])||isspace(szBuf2[i]))
			szBuf4[c++]=szBuf2[i];
	szBuf4[c]='\0';

	return stricmp(szBuf3,szBuf4)*(list->m_bSortOrder?1:-1);
}

void CList::SetSortingColumn(LPCTSTR lpszColumn, BOOL bOrder)
{
	LVCOLUMN lvColumn;
	if(strlen(m_szOldSortColumn))
	{
		lvColumn.mask=LVCF_FMT;
		lvColumn.fmt=LVCFMT_LEFT;
		ListView_SetColumn(m_hWnd,GetColumnIndexFromName(m_szOldSortColumn),&lvColumn);
	}
	if(lpszColumn&&strlen(lpszColumn))
	{
		strncpy(m_szSortColumn,lpszColumn,sizeof(m_szSortColumn));
		strncpy(m_szOldSortColumn,lpszColumn,sizeof(m_szOldSortColumn));
		m_bSortOrder=bOrder;
		if(strlen(m_szSortColumn))
		{
			lvColumn.mask=LVCF_FMT|LVCF_IMAGE;
			lvColumn.fmt=LVCFMT_LEFT|LVCFMT_IMAGE|LVCFMT_BITMAP_ON_RIGHT;
			lvColumn.iImage=m_bSortOrder;
			ListView_SetColumn(m_hWnd,GetColumnIndexFromName(m_szSortColumn),&lvColumn);
		}
	}
	else
		m_szSortColumn[0]=m_szOldSortColumn[0]='\0';
	SaveRegistrySettings();
}

BOOL CList::LoadRegistrySettings()
{
	switch(m_uID)
	{
	case IDC_LIST_FILELIST:
		strncpy(m_szSortColumn,app.m_Reg.m_szFileListSortColumn,sizeof(m_szSortColumn));
		m_bSortOrder=app.m_Reg.m_bFileListSortOrder;
		memcpy(m_ColumnInfos,app.m_Reg.m_FileListColumnInfos,sizeof(m_ColumnInfos));
		break;
	case IDC_LIST_PLAYLIST:
		strncpy(m_szSortColumn,app.m_Reg.m_szPlayListSortColumn,sizeof(m_szSortColumn));
		m_bSortOrder=app.m_Reg.m_bPlayListSortOrder;
		memcpy(m_ColumnInfos,app.m_Reg.m_PlayListColumnInfos,sizeof(m_ColumnInfos));
		break;
	}
	return TRUE;
}

BOOL CList::SaveRegistrySettings()
{
	switch(m_uID)
	{
	case IDC_LIST_FILELIST:
		strncpy(app.m_Reg.m_szFileListSortColumn,m_szSortColumn,sizeof(m_szSortColumn));
		app.m_Reg.m_bFileListSortOrder=m_bSortOrder;
		memcpy(app.m_Reg.m_FileListColumnInfos,m_ColumnInfos,sizeof(m_ColumnInfos));
		break;
	case IDC_LIST_PLAYLIST:
		strncpy(app.m_Reg.m_szPlayListSortColumn,m_szSortColumn,sizeof(m_szSortColumn));
		app.m_Reg.m_bPlayListSortOrder=m_bSortOrder;
		memcpy(app.m_Reg.m_PlayListColumnInfos,m_ColumnInfos,sizeof(m_ColumnInfos));
		break;
	}
	return TRUE;
}

BOOL CList::RefreshHeader()
{
	for(INT i=m_uColumnCount-1;i>=0;i--)
		ListView_DeleteColumn(m_hWnd,i);

	INT nOrder[NUMBER_OF_COLUMNS];
	for(m_uColumnCount=i=0;i<m_uMaxColumnCount;i++)
	{
		if(m_ColumnInfos[i].bActive)
		{
			if(m_ColumnInfos[i].nWidth<0)
				m_ColumnInfos[i].nWidth=ListView_GetStringWidth(m_hWnd,m_ColumnInfos[i].szName)+12+(m_bHasImageList?12:0);
			LVCOLUMN col;
			col.mask=LVCF_TEXT|LVCF_WIDTH;
			col.pszText=m_ColumnInfos[i].szName;
			col.cx=m_ColumnInfos[i].nWidth;
			if(ListView_InsertColumn(m_hWnd,m_uColumnCount,&col)==-1)
				return FALSE;
			nOrder[m_ColumnInfos[i].nOrder]=m_uColumnCount++;
		}
	}
	Header_SetOrderArray(m_hWndHeader,m_uColumnCount,nOrder);
	
	return TRUE;
}

void CList::SaveHeaderInfo()
{
	INT nOrder[NUMBER_OF_COLUMNS];
	Header_GetOrderArray(m_hWndHeader,m_uColumnCount,nOrder);
	for(INT i=0;i<m_uColumnCount;i++)
	{
		TCHAR szColumnName[100];
		GetColumnNameFromIndex(nOrder[i],szColumnName,sizeof(szColumnName));
		for(INT c=0;c<m_uMaxColumnCount;c++)
			if(!stricmp(szColumnName,m_ColumnInfos[c].szName))
			{				
				m_ColumnInfos[c].bActive=TRUE;
				m_ColumnInfos[c].nWidth=ListView_GetColumnWidth(m_hWnd,nOrder[i]);
				m_ColumnInfos[c].nOrder=i;
				break;
			}
	}
}

void CList::OnSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	app.m_MainWindow.m_ToolBar.SetSongList(this);
	CWnd::OnSetFocus(hwnd,hwndOldFocus);
}
