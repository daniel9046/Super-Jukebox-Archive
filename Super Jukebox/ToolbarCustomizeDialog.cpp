// ToolbarCustomizeDialog.cpp: implementation of the CToolbarCustomizeDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ToolbarCustomizeDialog.h"
#include "Resource.h"
#include "AfxRes.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolbarCustomizeDialog::CToolbarCustomizeDialog()
{
	m_hImlColorIcons=NULL;

	LPCTSTR lpszButtonNames[]={"Separator","New","Open","Save","Save As","Import Tags","Export Tags","WAV Dump","Add","Remove","Add All","Remove All","Play","Pause","Stop","Previous","Next","Edit Playlist","Find","Edit Filter","Filter View","Shuffle","Resize Columns","AWP","File List","Play List","CPL","Visualizer","Layout","Vis Mode","Refresh","Configure","Info","Help"};
	INT nButtonIDs[]={0,ID_FILE_NEW,ID_FILE_OPEN,ID_FILE_SAVE,ID_FILE_SAVE_AS,ID_FILE_IMPORTID666TAGS,ID_FILE_EXPORTID666TAGS,IDC_BUTTON_WAVDUMP,IDC_BUTTON_ADD,IDC_BUTTON_REMOVE,IDC_BUTTON_ADDALL,IDC_BUTTON_REMOVEALL,IDC_BUTTON_PLAY,IDC_BUTTON_PAUSE,IDC_BUTTON_STOP,IDC_BUTTON_PREVIOUS,IDC_BUTTON_NEXT,ID_EDIT_EDITITEM,ID_EDIT_FIND,ID_VIEW_FILTER,IDC_BUTTON_FILTERVIEW,ID_VIEW_SHUFFLE,ID_VIEW_RESIZECOLUMNS,IDC_BUTTON_AW,ID_WINDOWS_FILELIST,ID_WINDOWS_PLAYLIST,ID_WINDOWS_CONTROLPANEL,ID_WINDOWS_VISUALIZER,IDC_BUTTON_LAYOUT,IDC_BUTTON_VISMODE,ID_VIEW_REFRESH,ID_OPTIONS_CONFIGURE,ID_HELP_ABOUTSUPERJUKEBOX,ID_HELP_CONTENTS};
	INT iBitmaps[]={-1,7,8,13,31,19,23,22,0,12,14,17,10,9,16,1,4,26,3,24,2,18,20,21,27,30,25,32,28,29,11,15,6,5};
	BYTE bStyles[]={TBSTYLE_SEP,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_CHECK,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON,TBSTYLE_BUTTON};
	for(INT i=0;i<NUMBER_OF_BUTTONS;i++)
	{
		strncpy(m_Buttons[i].szName,lpszButtonNames[i],sizeof(m_Buttons[i].szName));
		m_Buttons[i].idCommand=nButtonIDs[i];
		m_Buttons[i].iBitmap=iBitmaps[i];
		m_Buttons[i].iString=i;
		m_Buttons[i].bStyle=bStyles[i];
		m_Buttons[i].bPosition=FALSE;
	}
}

CToolbarCustomizeDialog::~CToolbarCustomizeDialog()
{

}

BOOL CToolbarCustomizeDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_hWndLeftList=GetDlgItem(m_hWnd,IDC_LIST_TC_LEFT);
	m_hWndRightList=GetDlgItem(m_hWnd,IDC_LIST_TC_RIGHT);

	ListView_SetExtendedListViewStyleEx(m_hWndLeftList,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyleEx(m_hWndRightList,LVS_EX_FULLROWSELECT,LVS_EX_FULLROWSELECT);

	m_hImlColorIcons=ImageList_Create(24,24,ILC_COLOR8|ILC_MASK,18,0);
	HBITMAP hBmpColorIcons=LoadBitmap(app.m_hInstance,MAKEINTRESOURCE(IDB_COLOR_ICONS));
	ImageList_AddMasked(m_hImlColorIcons,hBmpColorIcons,CLR_DEFAULT);
	DeleteObject(hBmpColorIcons);
	ListView_SetImageList(m_hWndLeftList,m_hImlColorIcons,LVSIL_SMALL);
	ListView_SetImageList(m_hWndRightList,m_hImlColorIcons,LVSIL_SMALL);

	LVCOLUMN lvc;
	lvc.mask=LVCF_WIDTH;
	RECT rc;
	GetClientRect(m_hWndLeftList,&rc);
	lvc.cx=rc.right-rc.left-GetSystemMetrics(SM_CXVSCROLL);
	ListView_InsertColumn(m_hWndLeftList,0,&lvc);
	GetClientRect(m_hWndRightList,&rc);
	lvc.cx=rc.right-rc.left-GetSystemMetrics(SM_CXVSCROLL);
	ListView_InsertColumn(m_hWndRightList,0,&lvc);

	LVITEM lvi;
	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvi.iSubItem=0;

	for(INT i=0;i<NUMBER_OF_BUTTONS;i++)
		m_Buttons[i].bPosition=FALSE;

	for(i=0;i<app.m_Reg.m_ToolBarButtons.size();i++)
	{
		INT nButton=app.m_Reg.m_ToolBarButtons[i];
		lvi.iItem=i;
		lvi.pszText=m_Buttons[nButton].szName;
		lvi.iImage=m_Buttons[nButton].iBitmap;
		lvi.lParam=nButton;
		ListView_InsertItem(m_hWndRightList,&lvi);
		m_Buttons[nButton].bPosition=TRUE;
	}
	lvi.iItem=i;
	lvi.pszText="End";
	lvi.iImage=-1;
	lvi.lParam=-1;
	ListView_InsertItem(m_hWndRightList,&lvi);

	for(lvi.iItem=i=0;i<NUMBER_OF_BUTTONS;i++)
		if(!i||!m_Buttons[i].bPosition)
		{		
			lvi.pszText=m_Buttons[i].szName;
			lvi.iImage=m_Buttons[i].iBitmap;
			lvi.lParam=i;
			ListView_InsertItem(m_hWndLeftList,&lvi);
			lvi.iItem++;
		}

	SetSelectedItem(m_hWndLeftList,0);
	SetSelectedItem(m_hWndRightList,ListView_GetItemCount(m_hWndRightList)-1);

	m_bChanged=FALSE;
	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CToolbarCustomizeDialog::OnDestroy(HWND hwnd)
{
	if(m_hImlColorIcons)ImageList_Destroy(m_hImlColorIcons);
	return 0;
}

LRESULT CToolbarCustomizeDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_BUTTON_TC_ADD:
			OnAdd();
			break;
		case IDC_BUTTON_TC_REMOVE:
			OnRemove();
			break;
		case IDC_BUTTON_TC_RESET:
			OnReset();
			break;
		case IDC_BUTTON_TC_MOVEUP:
			OnMoveUp();
			break;
		case IDC_BUTTON_TC_MOVEDOWN:
			OnMoveDown();
			break;
		case IDCANCEL:
			OnClose();
			break;
		}
		break;
	}
	return 0;
}

LRESULT CToolbarCustomizeDialog::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	BOOL bEnable;

	switch(lpNMHdr->code)
	{
	case LVN_ITEMCHANGED:
		switch(lpNMHdr->idFrom)
		{
		case IDC_LIST_TC_LEFT:
			bEnable=ListView_GetSelectedCount(m_hWndLeftList);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TC_ADD),bEnable);
			break;
		case IDC_LIST_TC_RIGHT:
			INT nIndex=GetSelectedItem(m_hWndRightList);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TC_REMOVE),nIndex>=0&&nIndex<ListView_GetItemCount(m_hWndRightList)-1);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TC_MOVEUP),nIndex>0&&nIndex<ListView_GetItemCount(m_hWndRightList)-1);
			EnableWindow(GetDlgItem(m_hWnd,IDC_BUTTON_TC_MOVEDOWN),nIndex>=0&&nIndex<ListView_GetItemCount(m_hWndRightList)-2);
			break;
		}
		break;
	}
	return 0;
}

BOOL CToolbarCustomizeDialog::SetSelectedItem(HWND hWnd,INT nIndex)
{
	if(nIndex>=0&&nIndex<ListView_GetItemCount(hWnd))
	{
		for(INT i=0;i<ListView_GetItemCount(hWnd);i++)
		{
			if(ListView_GetItemState(hWnd,i,LVIS_SELECTED)==LVIS_SELECTED)
				ListView_SetItemState(hWnd,i,0,LVIS_SELECTED);
		}
		ListView_SetItemState(hWnd,nIndex,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
		ListView_EnsureVisible(hWnd,nIndex,FALSE);
		return TRUE;
	}
	return FALSE;
}

INT CToolbarCustomizeDialog::GetSelectedItem(HWND hWnd)
{
	for(INT i=0;i<ListView_GetItemCount(hWnd);i++)
	{
		if(ListView_GetItemState(hWnd,i,LVIS_SELECTED)==LVIS_SELECTED)
			return i;
	}
	return -1;
}

void CToolbarCustomizeDialog::OnAdd()
{
	INT nIndex=GetSelectedItem(m_hWndLeftList);
	LVITEM lvi;
	lvi.mask=LVIF_PARAM;
	lvi.iItem=nIndex;
	lvi.iSubItem=0;
	ListView_GetItem(m_hWndLeftList,&lvi);
	INT nButton=lvi.lParam;

	if(nButton)
	{
		ListView_DeleteItem(m_hWndLeftList,nIndex);
		if(nIndex>=ListView_GetItemCount(m_hWndLeftList))
			nIndex--;
		SetSelectedItem(m_hWndLeftList,nIndex);
	}

	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvi.iItem=GetSelectedItem(m_hWndRightList);
	if(lvi.iItem<0)
		SetSelectedItem(m_hWndRightList,lvi.iItem=ListView_GetItemCount(m_hWndRightList)-1);
	lvi.pszText=m_Buttons[nButton].szName;
	lvi.iImage=m_Buttons[nButton].iBitmap;
	lvi.lParam=nButton;
	ListView_InsertItem(m_hWndRightList,&lvi);
	ListView_EnsureVisible(m_hWndRightList,GetSelectedItem(m_hWndRightList),FALSE);
	m_bChanged=m_Buttons[nButton].bPosition=TRUE;

	app.m_Reg.m_ToolBarButtons.clear();
	for(INT i=0;i<ListView_GetItemCount(m_hWndRightList)-1;i++)
	{
		LVITEM lvi;
		lvi.mask=LVIF_PARAM;
		lvi.iItem=i;
		lvi.iSubItem=0;
		ListView_GetItem(m_hWndRightList,&lvi);
		app.m_Reg.m_ToolBarButtons.push_back(lvi.lParam);
	}
	app.m_MainWindow.m_ToolBar.GenerateButtons();
}

void CToolbarCustomizeDialog::OnRemove()
{
	INT nIndex=GetSelectedItem(m_hWndRightList);
	LVITEM lvi;
	lvi.mask=LVIF_PARAM;
	lvi.iItem=nIndex;
	lvi.iSubItem=0;
	ListView_GetItem(m_hWndRightList,&lvi);
	INT nButton=lvi.lParam;

	if(nButton>=0)
	{
		ListView_DeleteItem(m_hWndRightList,nIndex);
		m_bChanged=TRUE;
		if(nIndex>=ListView_GetItemCount(m_hWndRightList))
			nIndex--;
		SetSelectedItem(m_hWndRightList,nIndex);

		lvi.iItem=nButton;
		if(lvi.iItem>0)
		{
			for(INT i=0;i<ListView_GetItemCount(m_hWndLeftList);i++)
			{
				lvi.iItem=i;
				ListView_GetItem(m_hWndLeftList,&lvi);
				if(nButton<lvi.lParam)
					break;
			}
			lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
			lvi.iItem=i;
			lvi.pszText=m_Buttons[nButton].szName;
			lvi.iImage=m_Buttons[nButton].iBitmap;
			lvi.lParam=nButton;
			ListView_InsertItem(m_hWndLeftList,&lvi);
			m_Buttons[nButton].bPosition=FALSE;			
		}
		SetSelectedItem(m_hWndLeftList,lvi.iItem);

		app.m_Reg.m_ToolBarButtons.clear();
		for(INT i=0;i<ListView_GetItemCount(m_hWndRightList)-1;i++)
		{
			LVITEM lvi;
			lvi.mask=LVIF_PARAM;
			lvi.iItem=i;
			lvi.iSubItem=0;
			ListView_GetItem(m_hWndRightList,&lvi);
			app.m_Reg.m_ToolBarButtons.push_back(lvi.lParam);
		}
		app.m_MainWindow.m_ToolBar.GenerateButtons();
	}
}

void CToolbarCustomizeDialog::OnClose()
{
	DestroyWindow(m_hWnd);
}

void CToolbarCustomizeDialog::OnReset()
{
	LVITEM lvi;
	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvi.iSubItem=0;

	for(INT i=0;i<NUMBER_OF_BUTTONS;i++)
		m_Buttons[i].bPosition=FALSE;

	INT nButtons[]={8,9,10,11,0,12,13,14,0,15,16,0,19,30,31,0,33};
	SendMessage(m_hWndLeftList,WM_SETREDRAW,FALSE,0);
	SendMessage(m_hWndRightList,WM_SETREDRAW,FALSE,0);
	ListView_DeleteAllItems(m_hWndRightList);
	for(i=0;i<sizeof(nButtons)/sizeof(nButtons[0]);i++)
	{
		INT nButton=nButtons[i];
		lvi.iItem=i;
		lvi.pszText=m_Buttons[nButton].szName;
		lvi.iImage=m_Buttons[nButton].iBitmap;
		lvi.lParam=nButton;
		ListView_InsertItem(m_hWndRightList,&lvi);
		m_Buttons[nButton].bPosition=TRUE;
	}
	lvi.iItem=i;
	lvi.pszText="End";
	lvi.iImage=-1;
	lvi.lParam=-1;
	ListView_InsertItem(m_hWndRightList,&lvi);

	ListView_DeleteAllItems(m_hWndLeftList);
	for(lvi.iItem=i=0;i<NUMBER_OF_BUTTONS;i++)
		if(!i||!m_Buttons[i].bPosition)
		{		
			lvi.pszText=m_Buttons[i].szName;
			lvi.iImage=m_Buttons[i].iBitmap;
			lvi.lParam=i;
			ListView_InsertItem(m_hWndLeftList,&lvi);
			lvi.iItem++;
		}

	SendMessage(m_hWndLeftList,WM_SETREDRAW,TRUE,0);
	SendMessage(m_hWndRightList,WM_SETREDRAW,TRUE,0);
	SetSelectedItem(m_hWndLeftList,0);
	SetSelectedItem(m_hWndRightList,0);
	m_bChanged=TRUE;

	app.m_Reg.m_ToolBarButtons.clear();
	for(i=0;i<ListView_GetItemCount(m_hWndRightList)-1;i++)
	{
		LVITEM lvi;
		lvi.mask=LVIF_PARAM;
		lvi.iItem=i;
		lvi.iSubItem=0;
		ListView_GetItem(m_hWndRightList,&lvi);
		app.m_Reg.m_ToolBarButtons.push_back(lvi.lParam);
	}
	app.m_MainWindow.m_ToolBar.GenerateButtons();
}

void CToolbarCustomizeDialog::OnMoveUp()
{
	INT nIndex=GetSelectedItem(m_hWndRightList);
	LVITEM lvi;
	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvi.iItem=nIndex;
	lvi.iSubItem=0;
	TCHAR szBuf[100];
	lvi.pszText=szBuf;
	lvi.cchTextMax=sizeof(szBuf);
	ListView_GetItem(m_hWndRightList,&lvi);
	ListView_DeleteItem(m_hWndRightList,lvi.iItem);
	lvi.iItem--;
	ListView_InsertItem(m_hWndRightList,&lvi);
	SetSelectedItem(m_hWndRightList,lvi.iItem);
	m_bChanged=TRUE;

	app.m_Reg.m_ToolBarButtons.clear();
	for(INT i=0;i<ListView_GetItemCount(m_hWndRightList)-1;i++)
	{
		LVITEM lvi;
		lvi.mask=LVIF_PARAM;
		lvi.iItem=i;
		lvi.iSubItem=0;
		ListView_GetItem(m_hWndRightList,&lvi);
		app.m_Reg.m_ToolBarButtons.push_back(lvi.lParam);
	}
	app.m_MainWindow.m_ToolBar.GenerateButtons();
}

void CToolbarCustomizeDialog::OnMoveDown()
{
	INT nIndex=GetSelectedItem(m_hWndRightList);
	LVITEM lvi;
	lvi.mask=LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
	lvi.iItem=nIndex;
	lvi.iSubItem=0;
	TCHAR szBuf[100];
	lvi.pszText=szBuf;
	lvi.cchTextMax=sizeof(szBuf);
	ListView_GetItem(m_hWndRightList,&lvi);
	ListView_DeleteItem(m_hWndRightList,lvi.iItem);
	lvi.iItem++;
	ListView_InsertItem(m_hWndRightList,&lvi);
	SetSelectedItem(m_hWndRightList,lvi.iItem);
	m_bChanged=TRUE;

	app.m_Reg.m_ToolBarButtons.clear();
	for(INT i=0;i<ListView_GetItemCount(m_hWndRightList)-1;i++)
	{
		LVITEM lvi;
		lvi.mask=LVIF_PARAM;
		lvi.iItem=i;
		lvi.iSubItem=0;
		ListView_GetItem(m_hWndRightList,&lvi);
		app.m_Reg.m_ToolBarButtons.push_back(lvi.lParam);
	}
	app.m_MainWindow.m_ToolBar.GenerateButtons();
}

LRESULT CToolbarCustomizeDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
