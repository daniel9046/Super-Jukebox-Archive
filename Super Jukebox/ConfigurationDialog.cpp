// ConfigurationDialog.cpp: implementation of the CConfigurationDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConfigurationDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConfigurationDialog::CConfigurationDialog()
{
	m_nInitialSelection=-1;
	m_hDCBanner=NULL;
	m_hBmpBanner=NULL;
	m_bPass=TRUE;
}

CConfigurationDialog::~CConfigurationDialog()
{

}

BOOL CConfigurationDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	m_ActiveDlg=NULL;

	m_hWndTree=GetDlgItem(m_hWnd,IDC_TREE_CONFIGURATION);
	GetClientRect(m_hWnd,&m_rcChildDialog);
	RECT rc;
	GetWindowRect(m_hWndTree,&rc);
	ScreenToClient(m_hWnd,&rc);
	m_rcChildDialog.left=rc.right+rc.top;
	m_rcChildDialog.right-=rc.top;
	m_rcChildDialog.bottom-=rc.top;
	SetRect(&m_rcBanner,m_rcChildDialog.left,rc.top,m_rcChildDialog.right,rc.top+25);
	m_rcChildDialog.top=m_rcBanner.bottom+5;

	LPARAM lCount=0;

	vector<TVITEM>TreeItems;
	TVINSERTSTRUCT tvis;
	memset(&tvis,0,sizeof(tvis));
	tvis.hParent=TVI_ROOT;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Program";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[0].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Player";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[0].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="View";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[2].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Skin";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[2].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Visualization";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[2].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Active Wallpaper";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	tvis.hParent=TreeItems[0].hItem;
	tvis.hInsertAfter=TVI_LAST;
	tvis.item.mask=TVIF_TEXT|TVIF_PARAM;
	tvis.item.pszText="Miscellaneous";
	tvis.item.lParam=lCount++;
	tvis.item.hItem=TreeView_InsertItem(m_hWndTree,&tvis);
	TreeItems.push_back(tvis.item);

	TreeView_Expand(m_hWndTree,TreeItems[0].hItem,TVE_EXPAND);
	TreeView_Expand(m_hWndTree,TreeItems[2].hItem,TVE_EXPAND);

	m_ProgramDlg.InitMembers();
	m_PlayerDlg.InitMembers();
	m_ViewDlg.InitMembers();
	m_SkinDlg.InitMembers();
	m_VisualizationDlg.InitMembers();
	m_ActiveWallpaperDlg.InitMembers();
	m_MiscDlg.InitMembers();

	if(m_nInitialSelection==-1)
	{
		for(INT i=0;i<TreeItems.size();i++)
			if(TreeItems[i].lParam==app.m_Reg.m_dwSelectedConfigurationTreeItem)
			{
				m_ActiveItem=TreeItems[i];
				TreeView_SelectItem(m_hWndTree,m_ActiveItem.hItem);
				break;
			}
	}
	else
	{
		m_ActiveItem=TreeItems[m_nInitialSelection];
		TreeView_SelectItem(m_hWndTree,m_ActiveItem.hItem);
	}

	return CWnd::OnInitDialog(hwnd,hwndFocus,lParam);
}

LRESULT CConfigurationDialog::OnNotify(HWND hwnd, INT nIdCtrl, NMHDR *lpNMHdr)
{
	switch(lpNMHdr->code)
	{
	case TVN_SELCHANGED:
		switch(lpNMHdr->idFrom)
		{
		case IDC_TREE_CONFIGURATION:
			LPNMTREEVIEW lpNMTreeView=(LPNMTREEVIEW)lpNMHdr;
			if(m_bPass&&m_ActiveDlg&&!m_ActiveDlg->Validate())
			{				
				m_bPass=FALSE;
				HWND hWndOldFocus=GetFocus();
				TreeView_SelectItem(m_hWndTree,lpNMTreeView->itemOld.hItem);
				SetFocus(hWndOldFocus);
				m_bPass=TRUE;
			}
			else if(m_bPass)
			{
				m_ActiveItem=lpNMTreeView->itemNew;
				CWnd *Dlgs[]={&m_ProgramDlg,&m_PlayerDlg,&m_ViewDlg,&m_SkinDlg,&m_VisualizationDlg,&m_ActiveWallpaperDlg,&m_MiscDlg};
				LoadChildDialog(Dlgs[m_ActiveItem.lParam]);
				app.m_Reg.m_dwSelectedConfigurationTreeItem=m_ActiveItem.lParam;
			}
			break;
		}
		break;
	}
	return 0;
}

BOOL CConfigurationDialog::LoadChildDialog(CWnd *dlg)
{
	if(m_ActiveDlg)
		m_ActiveDlg->Destroy();
	if(m_ActiveDlg=dlg)
	{
		if(!m_ActiveDlg->Create(m_ActiveDlg->m_uID,this,m_hInstance))return FALSE;
		SetWindowPos(m_ActiveDlg->m_hWnd,NULL,m_rcChildDialog.left,m_rcChildDialog.top,0,0,SWP_NOZORDER|SWP_NOSIZE);
		ShowWindow(m_ActiveDlg->m_hWnd,SW_SHOW);
	}
	return TRUE;
}

LRESULT CConfigurationDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	LRESULT lResult=1;

	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDOK:
			lResult=0;
			if(m_ActiveDlg&&m_ActiveDlg->Validate())
			{
				if(!m_ProgramDlg.CarryOutActions())
					break;
				if(!m_PlayerDlg.CarryOutActions())
					break;
				if(!m_ViewDlg.CarryOutActions())
					break;
				if(!m_SkinDlg.CarryOutActions())
					break;
				if(!m_VisualizationDlg.CarryOutActions())
					break;
				if(!m_ActiveWallpaperDlg.CarryOutActions())
					break;
				if(!m_MiscDlg.CarryOutActions())
					break;
				lResult=1;
			}
			break;
		}
		break;
	}
	if(lResult)
		return CWnd::OnCommand(hwnd,id,hwndCtl,codeNotify);
	return lResult;
}

LRESULT CConfigurationDialog::OnDestroy(HWND hwnd)
{
	if(m_ActiveDlg)
		m_ActiveDlg->Destroy();
	if(m_hDCBanner)
	{
		DeleteDC(m_hDCBanner);
		DeleteObject(m_hBmpBanner);
	}
	return 0;
}

void CConfigurationDialog::SetInitialSelection(INT nInitialSelection)
{
	m_nInitialSelection=nInitialSelection;
}

void CConfigurationDialog::OnPaint(HWND hwnd)
{
	CWnd::OnPaint(hwnd);

	HDC hDC=GetDC(m_hWnd);

	if(!m_hDCBanner)
	{
		m_hDCBanner=CreateCompatibleDC(hDC);
		m_hBmpBanner=CreateCompatibleBitmap(hDC,m_rcBanner.right-m_rcBanner.left,m_rcBanner.bottom-m_rcBanner.top);
		SelectObject(m_hDCBanner,m_hBmpBanner);
		DrawBanner(m_hDCBanner,m_rcBanner.right-m_rcBanner.left,m_rcBanner.bottom-m_rcBanner.top);
	}
	
	BitBlt(hDC,m_rcBanner.left,m_rcBanner.top,m_rcBanner.right-m_rcBanner.left,m_rcBanner.bottom-m_rcBanner.top,m_hDCBanner,0,0,SRCCOPY);
	
	TCHAR szBuf[100];
	m_ActiveItem.mask=TVIF_TEXT;
	m_ActiveItem.pszText=szBuf;
	m_ActiveItem.cchTextMax=sizeof(szBuf);
	TreeView_GetItem(m_hWndTree,&m_ActiveItem);
	RECT rc={m_rcBanner.left+5,m_rcBanner.top,m_rcBanner.right-5,m_rcBanner.bottom};
	SetBkMode(hDC,TRANSPARENT);
	DrawText(hDC,szBuf,strlen(szBuf),&rc,DT_SINGLELINE|DT_LEFT|DT_VCENTER);

	LPCTSTR lpszDescriptions[]=
	{
		"Change general program options",
		"Change options for the player",
		"Change program view options",
		"Change options for skin support",
		"Change options for visualization",
		"Change options for Active Wallpaper",
		"Change miscellaneous program options"
	};
	SelectObject(hDC,GetStockObject(ANSI_VAR_FONT));
	DrawText(hDC,lpszDescriptions[m_ActiveItem.lParam],strlen(lpszDescriptions[m_ActiveItem.lParam]),&rc,DT_SINGLELINE|DT_RIGHT|DT_VCENTER);

	ReleaseDC(m_hWnd,hDC);
}

void CConfigurationDialog::DrawBanner(HDC hDC, INT nWidth, INT nHeight)
{
	for(INT x=0;x<nWidth;x++)
	{
		Line(hDC,RGB((x*50/nWidth)+140,(x*50/nWidth)+140,(x*50/nWidth)+140),x,0,x,nHeight);
	}
}

LRESULT CConfigurationDialog::OnHelp(HWND hwnd, LPHELPINFO lpHI)
{
	if(lpHI->iContextType==HELPINFO_WINDOW)
		WinHelp(m_hWnd,app.m_szHelpFile,HELP_CONTEXTPOPUP,lpHI->dwContextId);
	return CWnd::OnHelp(hwnd,lpHI);
}
