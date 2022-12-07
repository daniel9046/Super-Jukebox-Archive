// ProgramDialog.cpp: implementation of the CProgramDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgramDialog.h"
#include "Resource.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgramDialog::CProgramDialog()
{
	m_uID=IDD_PROGRAM;
}

CProgramDialog::~CProgramDialog()
{

}

void CProgramDialog::InitMembers()
{
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	for(INT i=0;i<sizeof(nThreadPriorities)/sizeof(nThreadPriorities[0]);i++)
		if(app.m_Reg.m_dwMainThreadPriority==nThreadPriorities[i])
		{
			m_nMainThreadPriority=i;
			break;
		}
	INT nProcessPriorities[]={IDLE_PRIORITY_CLASS,NORMAL_PRIORITY_CLASS,HIGH_PRIORITY_CLASS,REALTIME_PRIORITY_CLASS};
	for(i=0;i<sizeof(nProcessPriorities)/sizeof(nProcessPriorities[0]);i++)
		if(app.m_Reg.m_dwPriorityClass==nProcessPriorities[i])
		{
			m_nProcessPriority=i;
			break;
		}

	EXT Extensions[]=
	{
		"PL",0,0,
		"SPC",0,0,
		"SP0",0,0,
		"SP1",0,0,
		"SP2",0,0,
		"SP3",0,0,
		"SP4",0,0,
		"SP5",0,0,
		"SP6",0,0,
		"SP7",0,0,
		"SP8",0,0,
		"SP9",0,0
	};

	memcpy(m_Extensions,Extensions,sizeof(m_Extensions));
	strncpy(m_szAssocExt,app.m_Reg.m_szAssocExt,sizeof(m_szAssocExt));
	for(i=0;i<strlen(m_szAssocExt);i++)
	{
		UINT c=0;
		TCHAR szBuf[4];
		while(i<strlen(m_szAssocExt)&&m_szAssocExt[i]!=' ')
			szBuf[c++]=m_szAssocExt[i++];
		szBuf[c]='\0';
		for(c=0;c<sizeof(m_Extensions)/sizeof(m_Extensions[0]);c++)
			if(!stricmp(m_Extensions[c].szName,szBuf))
			{
				m_Extensions[c].bSelected=m_Extensions[c].bOrgSelected=TRUE;
				break;
			}
	}
	m_bRegFileTypes=app.m_Reg.m_bRegFileTypes;
}

BOOL CProgramDialog::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	LPCTSTR lpszThreadPriorities[]={"Idle","Lowest","Below Normal","Normal","Above Normal","Highest","Time Critical"};
	for(INT i=0;i<sizeof(lpszThreadPriorities)/sizeof(lpszThreadPriorities[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_MAIN_THREAD_PRIORITY,CB_ADDSTRING,0,(LPARAM)lpszThreadPriorities[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_MAIN_THREAD_PRIORITY,CB_SETCURSEL,m_nMainThreadPriority,0);

	LPCTSTR lpszProcessPriorities[]={"Idle","Normal","High","RealTime"};
	for(i=0;i<sizeof(lpszProcessPriorities)/sizeof(lpszProcessPriorities[0]);i++)
		SendDlgItemMessage(m_hWnd,IDC_COMBO_PROCESS_PRIORITY,CB_ADDSTRING,0,(LPARAM)lpszProcessPriorities[i]);
	SendDlgItemMessage(m_hWnd,IDC_COMBO_PROCESS_PRIORITY,CB_SETCURSEL,m_nProcessPriority,0);

	for(i=0;i<sizeof(m_Extensions)/sizeof(m_Extensions[0]);i++)
	{
		SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_ADDSTRING,0,(LPARAM)m_Extensions[i].szName);
		SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_SETSEL,m_Extensions[i].bSelected,i);
	}
	CheckDlgButton(m_hWnd,IDC_CHECK_REGTYPES,m_bRegFileTypes?BST_CHECKED:BST_UNCHECKED);
	return FALSE;
}

BOOL CProgramDialog::Validate()
{
	m_nMainThreadPriority=SendDlgItemMessage(m_hWnd,IDC_COMBO_MAIN_THREAD_PRIORITY,CB_GETCURSEL,0,0);
	m_nProcessPriority=SendDlgItemMessage(m_hWnd,IDC_COMBO_PROCESS_PRIORITY,CB_GETCURSEL,0,0);

	for(UINT i=0;i<sizeof(m_Extensions)/sizeof(m_Extensions[0]);i++)
		m_Extensions[i].bSelected=SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_GETSEL,i,0)>0;
	m_bRegFileTypes=IsDlgButtonChecked(m_hWnd,IDC_CHECK_REGTYPES)==BST_CHECKED;
	return TRUE;
}

BOOL CProgramDialog::CarryOutActions()
{
	INT nThreadPriorities[]={THREAD_PRIORITY_IDLE,THREAD_PRIORITY_LOWEST,THREAD_PRIORITY_BELOW_NORMAL,THREAD_PRIORITY_NORMAL,THREAD_PRIORITY_ABOVE_NORMAL,THREAD_PRIORITY_HIGHEST,THREAD_PRIORITY_TIME_CRITICAL};
	if(app.m_Reg.m_dwMainThreadPriority!=nThreadPriorities[m_nMainThreadPriority])
	{
		app.m_Reg.m_dwMainThreadPriority=nThreadPriorities[m_nMainThreadPriority];
		SetThreadPriority(GetCurrentThread(),app.m_Reg.m_dwMainThreadPriority);
	}
	INT nProcessPriorities[]={IDLE_PRIORITY_CLASS,NORMAL_PRIORITY_CLASS,HIGH_PRIORITY_CLASS,REALTIME_PRIORITY_CLASS};
	if(app.m_Reg.m_dwPriorityClass!=nProcessPriorities[m_nProcessPriority])
	{
		app.m_Reg.m_dwPriorityClass=nProcessPriorities[m_nProcessPriority];
		SetPriorityClass(GetCurrentProcess(),app.m_Reg.m_dwPriorityClass);
	}

	TCHAR szProgramLocation[MAX_PATH],szBuf[MAX_PATH+20];
	GetModuleFileName(m_hInstance,szProgramLocation,sizeof(szProgramLocation));

	app.m_Reg.m_szAssocExt[0]='\0';
	for(UINT i=0;i<sizeof(m_Extensions)/sizeof(m_Extensions[0]);i++)
	{
		if(m_Extensions[i].bSelected)
		{
			strcat(app.m_Reg.m_szAssocExt,m_Extensions[i].szName);
			strcat(app.m_Reg.m_szAssocExt," ");
		}
		if(m_Extensions[i].bSelected!=m_Extensions[i].bOrgSelected)
		{
			if(m_Extensions[i].bSelected)
			{
				if(!stricmp(m_Extensions[i].szName,"PL"))
				{
					sprintf(szBuf,"%s,0",szProgramLocation);
					if(!app.RegisterFileType("plfile","Super Jukebox Playlist",szBuf,"Open"))
						DisplayError(NULL,FALSE,NULL);
					sprintf(szBuf,"\"%s\" %%1",szProgramLocation);
					if(!app.AddFileTypeCommand("plfile","Open",szBuf))
						DisplayError(NULL,FALSE,NULL);
					TCHAR szWinDir[MAX_PATH];
					GetWindowsDirectory(szWinDir,sizeof(szWinDir));
					sprintf(szBuf,"\"%s\\Notepad.exe\" %%1",szWinDir);
					if(!app.AddFileTypeCommand("plfile","Edit With Notepad",szBuf))
						DisplayError(NULL,FALSE,NULL);

					if(!app.AssociateFileExtension(m_Extensions[i].szName,"plfile"))
						DisplayError(NULL,FALSE,NULL);
				}
				else
				{
					sprintf(szBuf,"%s,1",szProgramLocation);
					if(!app.RegisterFileType("spcfile","Super Nintendo Song",szBuf,"Play"))
						DisplayError(NULL,FALSE,NULL);
					sprintf(szBuf,"\"%s\" %%1",szProgramLocation);
					if(!app.AddFileTypeCommand("spcfile","Play",szBuf))
						DisplayError(NULL,FALSE,NULL);

					if(!app.AssociateFileExtension(m_Extensions[i].szName,"spcfile"))
						DisplayError(NULL,FALSE,NULL);
				}
			}
			else
			{
				if(!app.DisassociateFileExtension(m_Extensions[i].szName))
					DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
			}
		}
	}
	app.m_Reg.m_bRegFileTypes=m_bRegFileTypes;

	return TRUE;
}

LRESULT CProgramDialog::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch(codeNotify)
	{
	case BN_CLICKED:
		switch(id)
		{
		case IDC_BUTTON_ADD_SMG:
			if(!app.CreateStartMenuGroup())
				DisplayError(m_hWnd,FALSE,"Failed to add the Start Menu program group.");
			break;
		case IDC_BUTTON_ADD_DI:
			if(!app.CreateDesktopIcon())
				DisplayError(m_hWnd,FALSE,"Failed to add the Desktop icon.");
			break;
		case IDC_BUTTON_ADD_QLI:
			if(!app.CreateQuickLaunchIcon())
				DisplayError(m_hWnd,FALSE,"Failed to add the QuickLaunch icon.");
			break;
		case IDC_BUTTON_SELALL:
			UINT i;
			for(i=0;i<SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_GETCOUNT,0,0);i++)
				SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_SETSEL,TRUE,i);
			break;
		case IDC_BUTTON_SELNONE:
			for(i=0;i<SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_GETCOUNT,0,0);i++)
				SendDlgItemMessage(m_hWnd,IDC_LIST_ASSOCEXT,LB_SETSEL,FALSE,i);
			break;
		}
		break;
	}
	return 0;
}
