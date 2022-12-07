// SourceBar.cpp: implementation of the CSourceBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SourceBar.h"
#include "Resource.h"
#include "Main.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSourceBar::CSourceBar()
{
	m_hFont=NULL;
}

CSourceBar::~CSourceBar()
{

}

BOOL CSourceBar::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,"LocationBar",&wcx))
	{
		wcx.style=0;
		wcx.lpfnWndProc=CWnd::WindowProc;
		wcx.cbClsExtra=0;
		wcx.cbWndExtra=0;
		wcx.hInstance=hInstance;
		wcx.hIcon=NULL;
		wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
		wcx.hbrBackground=(HBRUSH)(COLOR_3DFACE+1);
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName="LocationBar";
		wcx.hIconSm=NULL;
		if(!RegisterClassEx(&wcx))
			return FALSE;
	}
	return TRUE;
}

BOOL CSourceBar::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(!RegisterClass(hInstance))
		return FALSE;
	RECT rc={0};
	if(!CWnd::Create("LocationBar",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,WS_EX_CONTROLPARENT,rc,pParentWnd,uID,hInstance))
		return FALSE;

	return TRUE;
}

BOOL CSourceBar::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	m_hWndStatic=CreateWindow("STATIC","&Location",WS_CHILD|WS_VISIBLE,0,0,50,0,m_hWnd,NULL,m_hInstance,NULL);
	if(!m_hWndStatic)
		return FALSE;
	
	m_hWndToolBar=CreateWindowEx(WS_EX_TOOLWINDOW,TOOLBARCLASSNAME,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|TBSTYLE_TRANSPARENT|TBSTYLE_FLAT|TBSTYLE_TOOLTIPS|CCS_NODIVIDER|CCS_NOPARENTALIGN|CCS_NORESIZE|CCS_TOP,0,0,0,0,m_hWnd,(HMENU)1,m_hInstance,NULL);
	if(!m_hWndToolBar)
		return FALSE;
	SendMessage(m_hWndToolBar,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
	DisplayToolTips(app.m_Reg.m_bDisplayToolTips);

	m_hImlGrayOneUp=ImageList_Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	HBITMAP hBmp=LoadBitmap(m_hInstance,MAKEINTRESOURCE(IDB_ONEUPGRAY));
	ImageList_AddMasked(m_hImlGrayOneUp,hBmp,CLR_DEFAULT);
	DeleteObject(hBmp);
	SendMessage(m_hWndToolBar,TB_SETIMAGELIST,0,(LPARAM)m_hImlGrayOneUp);
	m_hImlColorOneUp=ImageList_Create(16,16,ILC_COLOR8|ILC_MASK,1,0);
	hBmp=LoadBitmap(m_hInstance,MAKEINTRESOURCE(IDB_ONEUPCOLOR));
	ImageList_AddMasked(m_hImlColorOneUp,hBmp,CLR_DEFAULT);
	DeleteObject(hBmp);
	SendMessage(m_hWndToolBar,TB_SETHOTIMAGELIST,0,(LPARAM)m_hImlColorOneUp);

	TBBUTTON Button;
	Button.idCommand=1;
	Button.fsState=TBSTATE_ENABLED;
	Button.fsStyle=TBSTYLE_BUTTON;
	Button.iBitmap=0;
	Button.iString=-1;
	SendMessage(m_hWndToolBar,TB_ADDBUTTONS,1,(LPARAM)&Button);
	SendMessage(m_hWndToolBar,TB_AUTOSIZE,0,0);

	m_hWndComboBox=CreateWindow(WC_COMBOBOXEX,NULL,WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_DROPDOWN|CBS_AUTOHSCROLL,0,0,0,200,m_hWnd,(HMENU)2,m_hInstance,NULL);
	if(!m_hWndComboBox)
		return FALSE;
	
	SHFILEINFO sfi;
	memset(&sfi,0,sizeof(sfi));
	HIMAGELIST himl=(HIMAGELIST)SHGetFileInfo("",0,&sfi,sizeof(sfi),SHGFI_SMALLICON|SHGFI_SYSICONINDEX);
	if(!ImageList_GetImageCount(himl))
		return FALSE;
	SendMessage(m_hWndComboBox,CBEM_SETIMAGELIST,0,(LPARAM)himl);

	m_hWndEdit=(HWND)SendMessage(m_hWndComboBox,CBEM_GETEDITCONTROL,0,0);
	m_OldEditWndProc=(WNDPROC)SetWindowLong(m_hWndEdit,GWL_WNDPROC,(LONG)EditWindowProc);

    LOGFONT lf;
	lf.lfHeight=-11;
	lf.lfWidth=0;
	lf.lfEscapement=0;
	lf.lfOrientation=0;
	lf.lfWeight=FW_REGULAR;
	lf.lfItalic=0;
	lf.lfUnderline=0;
	lf.lfStrikeOut=0;
	lf.lfCharSet=ANSI_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=VARIABLE_PITCH|FF_SWISS;
	strcpy(lf.lfFaceName,"MS Sans Serif");
 
    m_hFont=CreateFontIndirect(&lf);
	if(!m_hFont)
		return FALSE;
	SendMessage(m_hWndStatic,WM_SETFONT,(WPARAM)m_hFont,0);
    SendMessage(m_hWndComboBox,WM_SETFONT,(WPARAM)m_hFont,0);
	RefreshComboBoxContents();

	AddReBarBand();
	app.m_MainWindow.m_ReBar.ShowBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),app.m_Reg.m_bShowLocationBar);
	m_bDropped=FALSE;
	return TRUE;
}

LRESULT CSourceBar::OnDestroy(HWND hwnd)
{
	RemoveReBarBand();
	if(m_hFont)DeleteObject(m_hFont);
	if(m_hImlGrayOneUp)ImageList_Destroy(m_hImlGrayOneUp);
	if(m_hImlColorOneUp)ImageList_Destroy(m_hImlColorOneUp);
	return 0;
}

void CSourceBar::OnEditKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	switch(vk)
	{
	case VK_RETURN:
		if(fDown)
		{
			CFileList &fl=app.m_MainWindow.m_FileList;
			TCHAR szSource[MAX_PATH];
			GetWindowText(m_hWndEdit,szSource,sizeof(szSource));
			if(!fl.SetSource(szSource,FALSE))
			{
				DisplayError(m_hWnd,FALSE,NULL);
				break;
			}
			fl.SetRedraw(FALSE);
			fl.Clear();
			fl.LoadDriveList();
			fl.LoadDirectoryList();
			fl.LoadFileList();
			fl.SortItems(fl.m_szSortColumn,fl.m_bSortOrder);
			if(app.m_Reg.m_bAutoSizeColumns)
				fl.ResizeColumns();
			fl.SetRedraw(TRUE);
			InvalidateRect(fl.m_hWnd,NULL,TRUE);
			fl.SelectEntry(0);
			
			fl.GetSource(szSource,sizeof(szSource));

			if(strlen(szSource))
			{
				BOOL bIn=FALSE;
				for(INT i=0;i<10;i++)
				{
					if(!stricmp(app.m_Reg.m_szFolderHistoryList[i],szSource))
					{
						bIn=TRUE;
						break;
					}
				}
				if(!bIn)
				{
					for(i=0;i<10;i++)
					{
						if(!strlen(app.m_Reg.m_szFolderHistoryList[i]))
						{
							strcpy(app.m_Reg.m_szFolderHistoryList[i],szSource);
							break;
						}
					}
					if(i==10)
					{
						TCHAR szBuf[MAX_PATH];
						for(i=1;i<10;i++)
						{
							strcpy(szBuf,app.m_Reg.m_szFolderHistoryList[i]);
							strcpy(app.m_Reg.m_szFolderHistoryList[i-1],szBuf);
						}
						strcpy(app.m_Reg.m_szFolderHistoryList[i-1],szSource);
					}
				}
				SetLocationText(szSource);
			}
		}
		break;
	case VK_TAB:
		SetFocus(GetNextDlgTabItem(app.m_MainWindow.m_hWnd,m_hWndEdit,(GetKeyState(VK_SHIFT)&0x8000)?TRUE:FALSE));
		break;
	}
	CallWindowProc(m_OldEditWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CSourceBar::OnEditChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	if(LOBYTE(VkKeyScan(ch))==VK_RETURN||LOBYTE(VkKeyScan(ch))==VK_TAB) //Remove infernal beeping!!!
		return;
	CallWindowProc(m_OldEditWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CSourceBar::OnEditSetFocus(HWND hwnd, HWND hwndOldFocus)
{
	app.EnableAccels(FALSE);
	CallWindowProc(m_OldEditWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

void CSourceBar::OnEditKillFocus(HWND hwnd, HWND hwndNewFocus)
{
	if(!m_bDropped)
		app.EnableAccels(TRUE);
	CallWindowProc(m_OldEditWndProc,m_OriginalHWnd.top(),m_OriginalUMsg.top(),m_OriginalWParam.top(),m_OriginalLParam.top());
}

LRESULT CALLBACK CSourceBar::EditWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	app.m_MainWindow.m_SourceBar.m_OriginalHWnd.push(hWnd);
	app.m_MainWindow.m_SourceBar.m_OriginalUMsg.push(uMsg);
	app.m_MainWindow.m_SourceBar.m_OriginalWParam.push(wParam);
	app.m_MainWindow.m_SourceBar.m_OriginalLParam.push(lParam);

	switch(uMsg)
	{
	case WM_KEYDOWN:lResult=HANDLE_WM_KEYDOWN(hWnd,wParam,lParam,app.m_MainWindow.m_SourceBar.OnEditKey);break;
	case WM_CHAR:lResult=HANDLE_WM_CHAR(hWnd,wParam,lParam,app.m_MainWindow.m_SourceBar.OnEditChar);break;
	case WM_SETFOCUS:lResult=HANDLE_WM_SETFOCUS(hWnd,wParam,lParam,app.m_MainWindow.m_SourceBar.OnEditSetFocus);break;
	case WM_KILLFOCUS:lResult=HANDLE_WM_KILLFOCUS(hWnd,wParam,lParam,app.m_MainWindow.m_SourceBar.OnEditKillFocus);break;
	default:lResult=CallWindowProc(app.m_MainWindow.m_SourceBar.m_OldEditWndProc,hWnd,uMsg,wParam,lParam);break;
	}
	app.m_MainWindow.m_SourceBar.m_OriginalHWnd.pop();
	app.m_MainWindow.m_SourceBar.m_OriginalUMsg.pop();
	app.m_MainWindow.m_SourceBar.m_OriginalWParam.pop();
	app.m_MainWindow.m_SourceBar.m_OriginalLParam.pop();
	return lResult;
}

BOOL CSourceBar::AddReBarBand()
{
	REBARBANDINFO rbBand;
	rbBand.cbSize=sizeof(rbBand);
	rbBand.fMask=RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_ID|RBBIM_SIZE;
	rbBand.fStyle=app.m_Reg.m_dwSourceBarStyle;

	SIZE sz;
	SendMessage(m_hWndToolBar,TB_GETMAXSIZE,0,(LPARAM)&sz);
	RECT rc;
	GetWindowRect(m_hWndComboBox,&rc);
	rbBand.hwndChild=m_hWnd;
	rbBand.cxMinChild=0;
	rbBand.cyMinChild=max(sz.cy,rc.bottom-rc.top);
	rbBand.wID=m_uID;
	rbBand.cx=app.m_Reg.m_dwSourceBarBandWidth;

	return app.m_MainWindow.m_ReBar.AddBand(app.m_Reg.m_dwSourceBarBandIndex,&rbBand);
}

BOOL CSourceBar::RemoveReBarBand()
{
	return app.m_MainWindow.m_ReBar.DeleteBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID));
}

BOOL CSourceBar::SaveReBarInfo()
{
	app.m_Reg.m_dwSourceBarBandIndex=app.m_MainWindow.m_ReBar.GetBandIndex(m_uID);
	if(app.m_Reg.m_dwSourceBarBandIndex!=-1)
	{
		REBARBANDINFO rbBand;
		rbBand.cbSize=sizeof(rbBand);
		rbBand.fMask=RBBIM_SIZE|RBBIM_STYLE;
		SendMessage(app.m_MainWindow.m_ReBar.m_hWnd,RB_GETBANDINFO,app.m_Reg.m_dwSourceBarBandIndex,(LPARAM)&rbBand);
		app.m_Reg.m_dwSourceBarBandWidth=rbBand.cx;
		app.m_Reg.m_dwSourceBarStyle=rbBand.fStyle;
	}
	return TRUE;
}

LRESULT CSourceBar::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case CBN_DROPDOWN:
		m_bDropped=TRUE;
		app.EnableAccels(FALSE);
		ValidateHistoryList();
		break;
	case CBN_CLOSEUP:
		m_bDropped=FALSE;
		app.EnableAccels(TRUE);
		break;
	case CBN_SELENDOK:
		TCHAR szSource[MAX_PATH];
		INT nIndex;
		nIndex=SendMessage(m_hWndComboBox,CB_GETCURSEL,0,0);
		SendMessage(m_hWndComboBox,CB_GETLBTEXT,nIndex,(LPARAM)szSource);
		if(strlen(szSource))
		{
			CFileList &fl=app.m_MainWindow.m_FileList;
			if(!fl.SetSource(szSource))
			{
				DisplayError(m_hWnd,FALSE,NULL);
				break;
			}
			fl.SetRedraw(FALSE);
			fl.Clear();
			fl.LoadDriveList();
			fl.LoadDirectoryList();
			fl.LoadFileList();
			fl.SortItems(fl.m_szSortColumn,fl.m_bSortOrder);
			if(app.m_Reg.m_bAutoSizeColumns)
				fl.ResizeColumns();
			fl.SetRedraw(TRUE);
			InvalidateRect(fl.m_hWnd,NULL,TRUE);
			fl.SelectEntry(0);
		}
		break;
	case BN_CLICKED:
		switch(id)
		{
		case 1:
			CFileList &fl=app.m_MainWindow.m_FileList;
			TCHAR szSource[MAX_PATH],szHighlightName[MAX_PATH];
			fl.GetSource(szSource,sizeof(szSource));
			INT pos=0;
			while((pos=fl.GetNextPathPart(szSource,pos,szHighlightName))!=-1);
			if(strlen(szHighlightName)==2&&szHighlightName[1]==':')
				strcat(szHighlightName,"\\");
			if(!fl.SetSource(".."))
			{
				DisplayError(m_hWnd,FALSE,NULL);
				break;
			}
			fl.SetRedraw(FALSE);
			fl.Clear();
			fl.LoadDriveList();
			fl.LoadDirectoryList();
			fl.LoadFileList();
			fl.SortItems(fl.m_szSortColumn,fl.m_bSortOrder);
			if(app.m_Reg.m_bAutoSizeColumns)
				fl.ResizeColumns();
			fl.SetRedraw(TRUE);
			InvalidateRect(fl.m_hWnd,NULL,TRUE);
			for(UINT i=0;i<fl.GetDisplayedCount();i++)
			{
				fl.GetListEntry(i)->GetFullName(szSource,sizeof(szSource));
				if(!stricmp(szSource,szHighlightName))
				{
					fl.SelectEntry(i);
					break;
				}
			}
			break;
		}
		break;
	}
	return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
}

void CSourceBar::IsVisible(BOOL bVisible)
{
	app.m_MainWindow.m_ReBar.ShowBand(app.m_MainWindow.m_ReBar.GetBandIndex(m_uID),bVisible);
}

void CSourceBar::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	SIZE sz0;
	HDC hDC=GetDC(m_hWndStatic);
	SelectObject(hDC,m_hFont);
	GetTextExtentPoint32(hDC,"Location",strlen("Location"),&sz0);
	ReleaseDC(m_hWndStatic,hDC);
	MoveWindow(m_hWndStatic,0,(cy-sz0.cy)/2,sz0.cx,sz0.cy,TRUE);
	sz0.cx+=4;
	SIZE sz1;
	SendMessage(m_hWndToolBar,TB_GETMAXSIZE,0,(LPARAM)&sz1);
	MoveWindow(m_hWndToolBar,sz0.cx,0,sz1.cx,sz1.cy,TRUE);
	sz1.cx+=4;
	RECT rc;
	GetWindowRect(m_hWndComboBox,&rc);
	MoveWindow(m_hWndComboBox,sz0.cx+sz1.cx,0,cx-(sz0.cx+sz1.cx),200,TRUE);
}

BOOL CSourceBar::SetLocationText(LPCTSTR szText)
{
	RefreshComboBoxContents();

	COMBOBOXEXITEM cbei;
	cbei.mask=CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
	TCHAR szBuf[MAX_PATH];
	cbei.pszText=szBuf;
	cbei.cchTextMax=sizeof(szBuf);
	BOOL bIn=FALSE;
	for(UINT i=0;i<SendMessage(m_hWndComboBox,CB_GETCOUNT,0,0);i++)
	{
		cbei.iItem=i;
		SendMessage(m_hWndComboBox,CBEM_GETITEM,0,(LPARAM)&cbei);
		cbei.iImage=cbei.iSelectedImage=app.m_MainWindow.m_FileList.GetFileImage(cbei.pszText,!stricmp(cbei.pszText,szText)?TRUE:FALSE);
		SendMessage(m_hWndComboBox,CBEM_SETITEM,0,(LPARAM)&cbei);
		if(!stricmp(cbei.pszText,szText))
		{
			SendMessage(m_hWndComboBox,CB_SETCURSEL,i,0);
			bIn=TRUE;
		}
	}

	if(!bIn)
	{
		cbei.mask=CBEIF_TEXT|(strlen(szText)?(CBEIF_IMAGE|CBEIF_SELECTEDIMAGE):0);
		cbei.iItem=-1;
		cbei.pszText=(LPTSTR)szText;
		cbei.iImage=cbei.iSelectedImage=app.m_MainWindow.m_FileList.GetFileImage(szText,TRUE);
		SendMessage(m_hWndComboBox,CBEM_SETITEM,0,(LPARAM)&cbei);
	}
	RedrawWindow(m_hWndComboBox,NULL,NULL,RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW);
	
	SendMessage(m_hWndToolBar,TB_ENABLEBUTTON,1,strlen(szText));
	return TRUE;
}

LRESULT CSourceBar::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	switch(lpNMHdr->code)
	{
	case TTN_GETDISPINFO:
		LPTOOLTIPTEXT lpttt;
		lpttt=(LPTOOLTIPTEXT)lpNMHdr;
		lpttt->hinst=NULL;
		lpttt->lpszText="Up One Level";
		break;
	}
	return 0;
}

void CSourceBar::Focus()
{
	SetFocus(m_hWndComboBox);
}

void CSourceBar::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	POINT pt={x,y};
	::ClientToScreen(m_hWnd,&pt);
	HMENU hMenu=GetSubMenu(GetSubMenu(app.m_MainWindow.m_Menu.m_hMenu,2),2);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_STANDARDBUTTONS,app.m_Reg.m_bShowToolBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_SOURCEBAR,app.m_Reg.m_bShowLocationBar?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_TEXTLABELS,app.m_Reg.m_bToolBarHasLabels?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_PICTURES,app.m_Reg.m_bToolBarHasPictures?MF_CHECKED:MF_UNCHECKED);
	CheckMenuItem(hMenu,ID_VIEW_TOOLBARS_WRAP,app.m_Reg.m_bWrapToolBar?MF_CHECKED:MF_UNCHECKED);
	TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,0,app.m_MainWindow.m_hWnd,NULL);
}

BOOL CSourceBar::RefreshComboBoxContents()
{
	COMBOBOXEXITEM cbei;
	cbei.mask=CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_TEXT;
	cbei.iItem=-1;
	SendMessage(m_hWndComboBox,CB_RESETCONTENT,0,0);
	
	for(INT i=9;i>=0;i--)
	{
		if(strlen(app.m_Reg.m_szFolderHistoryList[i]))
		{
			cbei.pszText=app.m_Reg.m_szFolderHistoryList[i];
			cbei.iImage=cbei.iSelectedImage=app.m_MainWindow.m_FileList.GetFileImage(cbei.pszText,FALSE);
			SendMessage(m_hWndComboBox,CBEM_INSERTITEM,0,(LPARAM)&cbei);
		}
	}
	return TRUE;
}

void CSourceBar::DisplayToolTips(BOOL bDisplay)
{
	HWND hWndTT=(HWND)SendMessage(m_hWndToolBar,TB_GETTOOLTIPS,0,0);
	if(hWndTT)
		SendMessage(hWndTT,TTM_ACTIVATE,(WPARAM)bDisplay,0);
}

void CSourceBar::ValidateHistoryList()
{
	BOOL bChanged=FALSE;
	for(INT i=0;i<10;)
	{
		if(strlen(app.m_Reg.m_szFolderHistoryList[i])>0)
		{
			TCHAR szArchive[MAX_PATH];
			strncpy(szArchive,app.m_Reg.m_szFolderHistoryList[i],sizeof(szArchive));
			app.m_MainWindow.m_FileList.IsPathInArchive(app.m_Reg.m_szFolderHistoryList[i],szArchive,NULL);
			WIN32_FIND_DATA fd;
			HANDLE hFindFile;
			hFindFile=FindFirstFile(szArchive,&fd);
			if(hFindFile==INVALID_HANDLE_VALUE)
			{
				app.m_Reg.m_szFolderHistoryList[i][0]='\0';
				if(i<9)
				{
					for(INT c=i+1;c<10;c++)
						if(strlen(app.m_Reg.m_szFolderHistoryList[c])>0)
						{
							strcpy(app.m_Reg.m_szFolderHistoryList[c-1],app.m_Reg.m_szFolderHistoryList[c]);
							app.m_Reg.m_szFolderHistoryList[c][0]='\0';
						}
				}
				bChanged=TRUE;
			}
			else
			{
				FindClose(hFindFile);
				i++;
			}
		}
		else
			i++;
	}

	if(bChanged)
	{
		TCHAR szBuf[MAX_PATH];
		GetWindowText(m_hWndComboBox,szBuf,sizeof(szBuf));
		RefreshComboBoxContents();
		SetLocationText(szBuf);
	}
}
