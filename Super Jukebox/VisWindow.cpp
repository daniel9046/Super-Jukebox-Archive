// VisWindow.cpp: implementation of the CVisWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VisWindow.h"
#include "Resource.h"
#include "Main.h"
#include "ConfigurationDialog.h"
#include "Application.h"

extern CApplication app;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVisWindow::CVisWindow()
{
	m_uThread=-1;

	LPCTSTR lpszColNames[]={"Channel","Active","Voice","Key On","Echo","Pitch Mod.","Noise","Surround","Env. Type","Envelope","Pitch","Volume","Output"};
	for(INT i=0;i<sizeof(m_Columns)/sizeof(m_Columns[0]);i++)
		strcpy(m_Columns[i].szName,lpszColNames[i]);

	struct{LPCTSTR lpszName;EnvM mode;COLORREF cr;}
	EnvTypes[]=
	{
		"LinDec",Dec,RGB(0,170,200),
		"ExpDec",Exp,RGB(255,128,0),
		"LinInc",Inc,RGB(0,0,255),
		"BentInc",Bent,RGB(255,255,0),
		"Release",Release,RGB(255,0,255),
		"Sustain",Sustain,RGB(0,255,255),
		"Attack",Attack,RGB(255,0,0),
		"Decay",Decay,RGB(64,128,128),
		"Direct",Direct,RGB(255,187,119),
	};
	for(i=0;i<sizeof(m_EnvTypes)/sizeof(m_EnvTypes[0]);i++)
	{
		strcpy(m_EnvTypes[i].szName,EnvTypes[i].lpszName);
		m_EnvTypes[i].mode=EnvTypes[i].mode;
		m_EnvTypes[i].cr=EnvTypes[i].cr;
	}
}

CVisWindow::~CVisWindow()
{

}

BOOL CVisWindow::RegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;
	wcx.cbSize=sizeof(wcx);
	if(!GetClassInfoEx(hInstance,"VisWindow",&wcx))
	{
		wcx.style=CS_OWNDC;
		wcx.lpfnWndProc=CWnd::WindowProc;
		wcx.cbClsExtra=0;
		wcx.cbWndExtra=0;
		wcx.hInstance=hInstance;
		wcx.hIcon=NULL;
		wcx.hIconSm=NULL;
		wcx.hCursor=(HCURSOR)LoadCursor(NULL,IDC_ARROW);
		wcx.hbrBackground=NULL;
		wcx.lpszMenuName=NULL;
		wcx.lpszClassName="VisWindow";
		if(!RegisterClassEx(&wcx))
			DisplayError(app.m_MainWindow.m_hWnd,TRUE,NULL);
	}
	return TRUE;
}

BOOL CVisWindow::Create(HINSTANCE hInstance, CWnd *pParentWnd, UINT uID)
{
	if(!RegisterClass(hInstance))
		return FALSE;
	RECT rc={0};
	if(!CWnd::Create("VisWindow",NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,WS_EX_CLIENTEDGE,rc,pParentWnd,uID,hInstance))
		return FALSE;
	return TRUE;
}

BOOL CVisWindow::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	m_hDC=GetDC(m_hWnd);
	m_uThread=-1;

	app.m_Player.GetVisParams(m_OldUVP);
	m_dwOldActiveChannels=app.m_Reg.m_dwActiveChannels;

	for(INT i=0;i<sizeof(m_hDCGroup)/sizeof(m_hDCGroup[0]);i++)
		m_hBmpGroup[i]=(HBITMAP)(m_hDCGroup[i]=NULL);
	for(i=0;i<sizeof(m_Colors)/sizeof(m_Colors[0]);i++)
		m_Pens[i]=(HPEN)(m_Brushes[i]=NULL);
	m_BlueBrush=m_RedBrush=m_BackBrush=(HBRUSH)(m_hFont=NULL);

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

	m_hOldFont=(HFONT)SelectObject(m_hDC,m_hFont);
	SetBkMode(m_hDC,TRANSPARENT);

	for(i=0;i<8;i++)
	{
		if(!CreateWindow("BUTTON",NULL,WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,0,0,13,13,m_hWnd,(HMENU)i,m_hInstance,NULL))
			return FALSE;
		if(app.m_Reg.m_dwActiveChannels&(1<<i))
			CheckDlgButton(m_hWnd,i,BST_CHECKED);
	}
	if(!Refresh())
		return FALSE;

	return TRUE;
}

LRESULT CVisWindow::OnDestroy(HWND hwnd)
{
	Stop();
	ReleaseDC(m_hWnd,m_hDC);
	for(INT i=0;i<sizeof(m_Colors)/sizeof(m_Colors[0]);i++)
	{
		if(m_Pens[i])DeleteObject(m_Pens[i]);
		if(m_Brushes[i])DeleteObject(m_Brushes[i]);
	}
	if(m_BlueBrush)DeleteObject(m_BlueBrush);
	if(m_RedBrush)DeleteObject(m_RedBrush);
	if(m_BackBrush)DeleteObject(m_BackBrush);
	for(i=0;i<sizeof(m_hDCGroup)/sizeof(m_hDCGroup[0]);i++)
	{
		if(m_hDCGroup[i])DeleteDC(m_hDCGroup[i]);
		if(m_hBmpGroup[i])DeleteObject(m_hBmpGroup[i]);
	}
	SelectObject(m_hDC,m_hOldFont);
	if(m_hFont)DeleteObject(m_hFont);
	return 0;
}

void CVisWindow::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	BOOL bRestart=IsRunning();
	Stop();

	m_nWidth=cx;
	m_nHeight=cy;
	m_nWaveWidth=m_nWidth-m_Columns[11].x-2;
	m_nWaveHeight=m_nHeight-(app.m_Reg.m_bShowVisLabels?15:1);
	if(m_nWaveWidth>0)
	{
		if(m_hDCGroup[7])DeleteDC(m_hDCGroup[7]);
		if(m_hBmpGroup[7])DeleteObject(m_hBmpGroup[7]);
		m_hDCGroup[7]=CreateCompatibleDC(m_hDC);
		m_hBmpGroup[7]=CreateCompatibleBitmap(m_hDC,m_nWaveWidth,m_nWaveHeight);
		SelectObject(m_hDCGroup[7],m_hBmpGroup[7]);
		if(m_hDCGroup[8])DeleteDC(m_hDCGroup[8]);
		if(m_hBmpGroup[8])DeleteObject(m_hBmpGroup[8]);
		m_hDCGroup[8]=CreateCompatibleDC(m_hDC);
		m_hBmpGroup[8]=CreateCompatibleBitmap(m_hDC,m_nWaveWidth,m_nWaveHeight);
		SelectObject(m_hDCGroup[8],m_hBmpGroup[8]);

		RECT rc;
		SetRect(&rc,0,0,m_nWaveWidth,m_nWaveHeight/2);
		FillRect(m_hDCGroup[7],&rc,m_BlueBrush);
		SetRect(&rc,0,m_nWaveHeight/2,m_nWaveWidth,m_nWaveHeight);
		FillRect(m_hDCGroup[7],&rc,m_RedBrush);
	}
	if(bRestart)
		Start();
}

void CVisWindow::OnPaint(HWND hwnd)
{
	BOOL bRestart=IsRunning();
	Stop();

	PAINTSTRUCT ps;
	BeginPaint(m_hWnd,&ps);
	if(app.m_Reg.m_bShowVisLabels)
	{
		SetTextColor(ps.hdc,m_Colors[1]);
		for(INT i=0;i<sizeof(m_Columns)/sizeof(m_Columns[0]);i++)
			TextOut(ps.hdc,m_Columns[i].x,m_Columns[i].y,m_Columns[i].szName,strlen(m_Columns[i].szName));
	}
	EndPaint(m_hWnd,&ps);
	Redraw();
	if(bRestart)
		Start();
}

void CVisWindow::DrawChannelGauge(HDC pDC, INT nXPos, INT nYPos, INT nWidth, INT nHeight,COLORREF clr0,COLORREF clr1)
{
	RECT rt;
	rt.left=nXPos;
	rt.top=nYPos;
	rt.right=nXPos+nWidth;
	rt.bottom=nYPos+nHeight;

	for(INT x=rt.left;x<=rt.right;x++)
	{
		double pos=(double)(x-rt.left)/nWidth;
		INT r=(GetRValue(clr1)-GetRValue(clr0))*pos+GetRValue(clr0);
		if(r>255)r=255;
		INT g=(GetGValue(clr1)-GetGValue(clr0))*pos+GetGValue(clr0);
		if(g>255)g=255;
		INT b=(GetBValue(clr1)-GetBValue(clr0))*pos+GetBValue(clr0);
		if(b>255)b=255;
		Line(pDC,RGB(r,g,b),x,rt.top,x,rt.bottom);
	}
}

INT CVisWindow::Render()
{
	app.m_Player.GetVisParams(m_UVP);
	if(!m_bDrawAll&&m_UVP.uCount==m_OldUVP.uCount)
		return 1;

	INT nFrameSize=(app.m_Reg.m_dwChannels-1)+(app.m_Reg.m_dwSampleSize>>4);
	INT nNumSmp=m_UVP.uPCMBufferLength>>nFrameSize;

	RECT rc;
	INT nYPos=app.m_Reg.m_bShowVisLabels?14:0;
	for(INT i=0;i<8;i++)
	{
		if(m_bDrawAll||(app.m_Reg.m_dwActiveChannels&(1<<i))!=(m_dwOldActiveChannels&(1<<i)))
		{
			SetRect(&rc,m_Columns[0].x,i*14+nYPos,m_Columns[1].x-1,i*14+nYPos+13);
			FillRect(m_hDC,&rc,m_BackBrush);
			SetTextColor(m_hDC,m_Colors[app.m_Reg.m_dwActiveChannels&(1<<i)?3:2]);
			TCHAR szBuf[2];
			itoa(i,szBuf,10);
			TextOut(m_hDC,m_Columns[0].x,i*14+nYPos,szBuf,strlen(szBuf));
		}

		BYTE bWave,bOldWave;
		switch(i)
		{
		case 0:bWave=m_UVP.DSP.Chn1.Wave;bOldWave=m_OldUVP.DSP.Chn1.Wave;break;
		case 1:bWave=m_UVP.DSP.Chn2.Wave;bOldWave=m_OldUVP.DSP.Chn2.Wave;break;
		case 2:bWave=m_UVP.DSP.Chn3.Wave;bOldWave=m_OldUVP.DSP.Chn3.Wave;break;
		case 3:bWave=m_UVP.DSP.Chn4.Wave;bOldWave=m_OldUVP.DSP.Chn4.Wave;break;
		case 4:bWave=m_UVP.DSP.Chn5.Wave;bOldWave=m_OldUVP.DSP.Chn5.Wave;break;
		case 5:bWave=m_UVP.DSP.Chn6.Wave;bOldWave=m_OldUVP.DSP.Chn6.Wave;break;
		case 6:bWave=m_UVP.DSP.Chn7.Wave;bOldWave=m_OldUVP.DSP.Chn7.Wave;break;
		case 7:bWave=m_UVP.DSP.Chn8.Wave;bOldWave=m_OldUVP.DSP.Chn8.Wave;break;
		}
		if(m_bDrawAll||bWave!=bOldWave)
		{
			SetRect(&rc,m_Columns[2].x,i*14+nYPos,m_Columns[3].x-1,i*14+nYPos+13);
			FillRect(m_hDC,&rc,m_BackBrush);
			if(app.m_Reg.m_dwActiveChannels&(1<<i))
			{
				TCHAR szBuf[20];
				sprintf(szBuf,"%u",bWave);
				SetTextColor(m_hDC,m_Colors[m_UVP.uCount?5:4]);
				TextOut(m_hDC,m_Columns[2].x,i*14+nYPos,szBuf,strlen(szBuf));
			}
		}

		if(m_bDrawAll||(m_UVP.DSP.KOn&(1<<i))!=(m_OldUVP.DSP.KOn&(1<<i)))
		{
			SetRect(&rc,m_Columns[3].x,i*14+nYPos,m_Columns[3].x+13,i*14+nYPos+13);
			INT br=app.m_Reg.m_dwActiveChannels&(1<<i)&&m_UVP.DSP.KOn&(1<<i)?7:6;
			FillRect(m_hDC,&rc,m_Brushes[br]);
		}

		if(m_bDrawAll||(m_UVP.DSP.EOn&(1<<i))!=(m_OldUVP.DSP.EOn&(1<<i)))
		{
			SetRect(&rc,m_Columns[4].x,i*14+nYPos,m_Columns[4].x+13,i*14+nYPos+13);
			INT br=app.m_Reg.m_dwActiveChannels&(1<<i)&&m_UVP.DSP.EOn&(1<<i)?9:8;
			FillRect(m_hDC,&rc,m_Brushes[br]);
		}

		if(m_bDrawAll||(m_UVP.DSP.PMod&(1<<i))!=(m_OldUVP.DSP.PMod&(1<<i)))
		{
			SetRect(&rc,m_Columns[5].x,i*14+nYPos,m_Columns[5].x+13,i*14+nYPos+13);
			INT br=app.m_Reg.m_dwActiveChannels&(1<<i)&&m_UVP.DSP.PMod&(1<<i)?11:10;
			FillRect(m_hDC,&rc,m_Brushes[br]);
		}

		if(m_bDrawAll||(m_UVP.DSP.NOn&(1<<i))!=(m_OldUVP.DSP.NOn&(1<<i)))
		{
			SetRect(&rc,m_Columns[6].x,i*14+nYPos,m_Columns[6].x+13,i*14+nYPos+13);
			INT br=app.m_Reg.m_dwActiveChannels&(1<<i)&&m_UVP.DSP.NOn&(1<<i)?13:12;
			FillRect(m_hDC,&rc,m_Brushes[br]);
		}

		BYTE bESrnd=(m_UVP.DSP.EVolL^m_UVP.DSP.EVolR)&0x80,bOldESrnd=(m_OldUVP.DSP.EVolL^m_OldUVP.DSP.EVolR)&0x80;
		BYTE bSrnd,bOldSrnd;
		switch(i)
		{
		case 0:bSrnd=(m_UVP.DSP.Chn1.VolL^m_UVP.DSP.Chn1.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn1.VolL^m_OldUVP.DSP.Chn1.VolR);break;
		case 1:bSrnd=(m_UVP.DSP.Chn2.VolL^m_UVP.DSP.Chn2.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn2.VolL^m_OldUVP.DSP.Chn2.VolR);break;
		case 2:bSrnd=(m_UVP.DSP.Chn3.VolL^m_UVP.DSP.Chn3.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn3.VolL^m_OldUVP.DSP.Chn3.VolR);break;
		case 3:bSrnd=(m_UVP.DSP.Chn4.VolL^m_UVP.DSP.Chn4.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn4.VolL^m_OldUVP.DSP.Chn4.VolR);break;
		case 4:bSrnd=(m_UVP.DSP.Chn5.VolL^m_UVP.DSP.Chn5.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn5.VolL^m_OldUVP.DSP.Chn5.VolR);break;
		case 5:bSrnd=(m_UVP.DSP.Chn6.VolL^m_UVP.DSP.Chn6.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn6.VolL^m_OldUVP.DSP.Chn6.VolR);break;
		case 6:bSrnd=(m_UVP.DSP.Chn7.VolL^m_UVP.DSP.Chn7.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn7.VolL^m_OldUVP.DSP.Chn7.VolR);break;
		case 7:bSrnd=(m_UVP.DSP.Chn8.VolL^m_UVP.DSP.Chn8.VolR);
			bOldSrnd=(m_OldUVP.DSP.Chn8.VolL^m_OldUVP.DSP.Chn8.VolR);break;
		}
		bSrnd=(bSrnd^(m_UVP.DSP.MVolL^m_UVP.DSP.MVolR))&0x80;
		bOldSrnd=(bOldSrnd^(m_OldUVP.DSP.MVolL^m_OldUVP.DSP.MVolR))&0x80;
		bSrnd=max(bSrnd,bESrnd);
		bOldSrnd=max(bOldSrnd,bOldESrnd);
		if(m_bDrawAll||bSrnd!=bOldSrnd)
		{
			SetRect(&rc,m_Columns[7].x,i*14+nYPos,m_Columns[7].x+13,i*14+nYPos+13);
			INT br=app.m_Reg.m_dwActiveChannels&(1<<i)&&bSrnd?15:14;
			FillRect(m_hDC,&rc,m_Brushes[br]);
		}

		if(m_bDrawAll||m_UVP.Mix[i].EMode!=m_OldUVP.Mix[i].EMode||m_UVP.Mix[i].MFlg!=m_OldUVP.Mix[i].MFlg)
		{
			SetRect(&rc,m_Columns[8].x,i*14+nYPos,m_Columns[9].x-1,i*14+nYPos+13);
			FillRect(m_hDC,&rc,m_BackBrush);
			if(app.m_Reg.m_dwActiveChannels&(1<<i)&&!(m_UVP.Mix[i].MFlg&8))
			{
				for(INT e=0;e<sizeof(m_EnvTypes)/sizeof(m_EnvTypes[0]);e++)
				{
					if(m_UVP.uCount&&m_UVP.Mix[i].EMode==m_EnvTypes[e].mode)
					{
						SetTextColor(m_hDC,m_EnvTypes[e].cr);
						TextOut(m_hDC,m_Columns[8].x,i*14+nYPos,m_EnvTypes[e].szName,strlen(m_EnvTypes[e].szName));
						break;
					}
					else if(!m_UVP.uCount)
					{
						SetTextColor(m_hDC,m_Colors[4]);
						TextOut(m_hDC,m_Columns[8].x,i*14+nYPos,"Off",strlen("Off"));
						break;
					}
				}
			}
		}

		if(m_bDrawAll||m_UVP.Mix[i].EVal!=m_OldUVP.Mix[i].EVal||m_UVP.Mix[i].MFlg!=m_OldUVP.Mix[i].MFlg)
		{
			INT w=m_UVP.Mix[i].EVal*50/8191;
			BitBlt(m_hDCGroup[6],0,0,50,13,m_hDCGroup[0],0,0,SRCCOPY);
			if(app.m_Reg.m_dwActiveChannels&(1<<i)&&!(m_UVP.Mix[i].MFlg&8))
				BitBlt(m_hDCGroup[6],0,0,w,13,m_hDCGroup[1],0,0,SRCCOPY);
			BitBlt(m_hDC,m_Columns[9].x,i*14+nYPos,50,13,m_hDCGroup[6],0,0,SRCCOPY);
		}

		if(m_bDrawAll||m_UVP.Mix[i].PRate!=m_OldUVP.Mix[i].PRate||m_UVP.Mix[i].MFlg!=m_OldUVP.Mix[i].MFlg)
		{
			INT w=m_UVP.Mix[i].PRate*50/130078;
			BitBlt(m_hDCGroup[6],0,0,50,13,m_hDCGroup[2],0,0,SRCCOPY);
			if(app.m_Reg.m_dwActiveChannels&(1<<i)&&!(m_UVP.Mix[i].MFlg&8))
				BitBlt(m_hDCGroup[6],0,0,w,13,m_hDCGroup[3],0,0,SRCCOPY);
			BitBlt(m_hDC,m_Columns[10].x,i*14+nYPos,50,13,m_hDCGroup[6],0,0,SRCCOPY);
		}

		if(m_bDrawAll||m_UVP.nDecibelList[i*2]!=m_OldUVP.nDecibelList[i*2]||m_UVP.nDecibelList[i*2+1]!=m_OldUVP.nDecibelList[i*2+1]||m_UVP.Mix[i].MFlg!=m_OldUVP.Mix[i].MFlg)
		{
			INT w=max(((m_UVP.nDecibelList[i*2]>>16)-48)*50/48,((m_UVP.nDecibelList[i*2+1]>>16)-48)*50/48);
			if(w<0)w=0;
			BitBlt(m_hDCGroup[6],0,0,50,13,m_hDCGroup[4],0,0,SRCCOPY);
			if(app.m_Reg.m_dwActiveChannels&(1<<i)&&!(m_UVP.Mix[i].MFlg&8))
				BitBlt(m_hDCGroup[6],0,0,w,13,m_hDCGroup[5],0,0,SRCCOPY);
			BitBlt(m_hDC,m_Columns[11].x,i*14+nYPos,50,13,m_hDCGroup[6],0,0,SRCCOPY);
		}
	}

	if(m_nWaveWidth>0)
	{
		BitBlt(m_hDCGroup[8],0,0,m_nWaveWidth,m_nWaveHeight,m_hDCGroup[7],0,0,SRCCOPY);
		if(m_UVP.lpPCMBuffer!=NULL&&nNumSmp>0)
		{
			INT x,nHalf=m_nWaveHeight/2;
			HPEN hOldPen;
			switch(app.m_Reg.m_dwVisMode)
			{
			case 0:
				hOldPen=(HPEN)SelectObject(m_hDCGroup[8],m_Pens[23]);
				WORD wSmp;
				wSmp=GetSample(m_UVP.lpPCMBuffer,0,0);
				MoveToEx(m_hDCGroup[8],0,wSmp*nHalf/65535,NULL);
				for(x=0;x<m_nWaveWidth;x++)
				{
					wSmp=GetSample(m_UVP.lpPCMBuffer,x*nNumSmp/m_nWaveWidth,0);
					LineTo(m_hDCGroup[8],x,wSmp*nHalf/65535-1);
				}
				SelectObject(m_hDCGroup[8],m_Pens[25]);
				wSmp=GetSample(m_UVP.lpPCMBuffer,0,1);
				MoveToEx(m_hDCGroup[8],0,wSmp*nHalf/65535+nHalf,NULL);
				for(x=0;x<m_nWaveWidth;x++)
				{
					wSmp=GetSample(m_UVP.lpPCMBuffer,x*nNumSmp/m_nWaveWidth,1);
					LineTo(m_hDCGroup[8],x,wSmp*nHalf/65535+nHalf);
				}
				SelectObject(m_hDCGroup[8],hOldPen);
				break;

			case 1:
				float RealIn[1024],RealOut[1024],ImagOut[1024];

				for(x=0;x<1024;x++)
					RealIn[x]=(signed short)(GetSample(m_UVP.lpPCMBuffer,x*nNumSmp/1024,0)^0x8000);
				fftFloat(1024,0,RealIn,NULL,RealOut,ImagOut);
				hOldPen=(HPEN)SelectObject(m_hDCGroup[8],m_Pens[23]);
				for(x=0;x<m_nWaveWidth;x++)
				{
					INT idx=x*(app.m_Reg.m_dwSpectrumSize*512/100)/m_nWaveWidth;
					INT f=sqrt(RealOut[idx]*RealOut[idx]+ImagOut[idx]*ImagOut[idx]);
					if(f>1048576)f=1048576;
					MoveToEx(m_hDCGroup[8],x,nHalf-1,NULL);
					LineTo(m_hDCGroup[8],x,nHalf-(f*nHalf/1048576)-1);
				}

				for(x=0;x<1024;x++)
					RealIn[x]=(signed short)(GetSample(m_UVP.lpPCMBuffer,x*nNumSmp/1024,1)^0x8000);
				fftFloat(1024,0,RealIn,NULL,RealOut,ImagOut);
				SelectObject(m_hDCGroup[8],m_Pens[25]);
				for(x=0;x<m_nWaveWidth;x++)
				{
					INT idx=x*(app.m_Reg.m_dwSpectrumSize*512/100)/m_nWaveWidth;
					INT f=sqrt(RealOut[idx]*RealOut[idx]+ImagOut[idx]*ImagOut[idx]);
					if(f>1048576)f=1048576;
					MoveToEx(m_hDCGroup[8],x,m_nWaveHeight-1,NULL);
					LineTo(m_hDCGroup[8],x,m_nWaveHeight-(f*nHalf/1048576)-1);
				}
				SelectObject(m_hDCGroup[8],hOldPen);
				break;
			}
		}
		BitBlt(m_hDC,m_Columns[12].x,nYPos,m_nWaveWidth,m_nWaveHeight,m_hDCGroup[8],0,0,SRCCOPY);
	}
	memcpy(&m_OldUVP,&m_UVP,sizeof(m_OldUVP));
	m_dwOldActiveChannels=app.m_Reg.m_dwActiveChannels;
	m_bDrawAll=FALSE;

	return 0;
}

INT CVisWindow::VUMeter(INT nVMax)
{
	INT ip32=32;

	nVMax>>=7;
	if(nVMax)
	{
		_asm
		{
			fild [nVMax]
			fild [ip32]
			fyl2x
			fistp [nVMax]
			fdecstp
		}
	}
	else
		nVMax=0;

	return nVMax;
}

WORD CVisWindow::GetSample(LPVOID lpBuf, INT nSmp, INT nChn)
{
	INT st=(app.m_Reg.m_dwChannels==2&&nChn==1);
	signed short *p16=(signed short*)lpBuf;
	unsigned char *p8=(unsigned char*)lpBuf;

	return (app.m_Reg.m_dwSampleSize==8)?(p8[nSmp*app.m_Reg.m_dwChannels+st]<<8):(p16[nSmp*app.m_Reg.m_dwChannels+st]^0x8000);
}

void CVisWindow::Start()
{
	if(m_uThread==-1&&!app.m_Player.IsStopped()&&(!app.m_Reg.m_bOutputToWAV||app.m_Reg.m_bOutputWithSound))
	{
		m_bDrawAll=TRUE;
		m_bKillThread=FALSE;
		do
		{
			app.m_Player.GetVisParams(m_UVP);
		}while(!m_UVP.uCount);
		memset(&m_OldUVP,0,sizeof(m_OldUVP));
		m_uThread=_beginthread(Thread,0,this);
		::SetThreadPriority((HANDLE)m_uThread,app.m_Reg.m_dwVisThreadPriority);
	}
}

void CVisWindow::Stop()
{
	if(m_uThread!=-1)
	{
		m_bKillThread=TRUE;
		if(WaitForSingleObject((HANDLE)m_uThread,3000)==WAIT_TIMEOUT)
		{
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,"The visualization thread won't cooperate. Press OK to terminate.");
			TerminateThread((HANDLE)m_uThread,0);
		}
		m_uThread=-1;
	}
}

void __cdecl CVisWindow::Thread(void *p)
{
	CVisWindow *vw=(CVisWindow*)p;

	while(!vw->m_bKillThread)
	{
		vw->Render();
		Sleep(10);
	}
	_endthread();
}

LRESULT CVisWindow::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	if(codeNotify==BN_CLICKED)
	{
		if(IsDlgButtonChecked(m_hWnd,id)==BST_CHECKED)app.m_Reg.m_dwActiveChannels|=(1<<id);
		else app.m_Reg.m_dwActiveChannels&=~(1<<id);
		app.m_Player.MuteChannels(~app.m_Reg.m_dwActiveChannels);
		Redraw();
	}
	return 0;
}

void CVisWindow::SetThreadPriority(INT nPriorityLevel)
{
	if(m_uThread!=-1)
		::SetThreadPriority((HANDLE)m_uThread,nPriorityLevel);
}

void CVisWindow::Redraw()
{
	BOOL bRestart=IsRunning();
	Stop();
	m_bDrawAll=TRUE;
	Render();
	if(bRestart)
		Start();
}

void CVisWindow::OnRButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	HMENU hMenu=CreatePopupMenu();
	if(!hMenu)
	{
		DisplayError(m_hWnd,FALSE,NULL);
		return;
	}
	AppendMenu(hMenu,MF_STRING,1,"&Hide");
	AppendMenu(hMenu,MF_STRING|(app.m_Reg.m_bShowVisLabels?MF_CHECKED:0),2,"Column &Labels");
	AppendMenu(hMenu,MF_STRING,3,"&Configure...");
	POINT pt={x,y};
	::ClientToScreen(m_hWnd,&pt);
	INT nSelection=TrackPopupMenu(hMenu,TPM_LEFTALIGN|TPM_TOPALIGN|TPM_NONOTIFY|TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,0,m_hWnd,NULL);
	DestroyMenu(hMenu);

	switch(nSelection)
	{
	case 1:
		SendMessage(app.m_MainWindow.m_hWnd,WM_COMMAND,MAKELONG(ID_WINDOWS_VISUALIZER,0),(LPARAM)m_hWnd);
		break;
	case 2:
		app.m_Reg.m_bShowVisLabels^=1;
		Refresh();
		app.m_MainWindow.AdjustLayout();
		app.m_MainWindow.RePaint(NULL);
		break;
	case 3:
		CConfigurationDialog dlg;
		dlg.SetInitialSelection(4);
		if(dlg.DoModal(IDD_CONFIGURATION,&app.m_MainWindow,m_hInstance)==-1)
			DisplayError(app.m_MainWindow.m_hWnd,FALSE,NULL);
		break;
	}
}

BOOL CVisWindow::SuspendThread()
{
	if(m_uThread!=-1)
	{
		::SuspendThread((HANDLE)m_uThread);
		return TRUE;
	}
	return FALSE;
}

BOOL CVisWindow::ResumeThread()
{
	if(m_uThread!=-1)
	{
		::ResumeThread((HANDLE)m_uThread);
		return TRUE;
	}
	return FALSE;
}

BOOL CVisWindow::IsRunning()
{
	return (m_uThread!=-1);
}

BOOL CVisWindow::Refresh()
{
	BOOL bRestart=IsRunning();
	Stop();
	memcpy(&m_Colors,&app.m_Reg.m_crVisColors[app.m_Reg.m_dwVisColorScheme],sizeof(m_Colors));

	SIZE sz;
	for(UINT i=0;i<sizeof(m_Columns)/sizeof(m_Columns[0]);i++)
	{
		if(!i)
			m_Columns[i].x=0;
		else
		{			
			GetTextExtentPoint32(m_hDC,m_Columns[i-1].szName,strlen(m_Columns[i-1].szName),&sz);
			m_Columns[i].x=m_Columns[i-1].x+sz.cx+5;
		}
		m_Columns[i].y=0;
	}
	if(!app.m_Reg.m_bShowVisLabels)
	{
		GetTextExtentPoint32(m_hDC,"0",strlen("0"),&sz);
		m_Columns[1].x=m_Columns[0].x+sz.cx+5;
		m_Columns[2].x=m_Columns[1].x+18;
		GetTextExtentPoint32(m_hDC,"000",strlen("000"),&sz);
		m_Columns[3].x=m_Columns[2].x+sz.cx+5;
		m_Columns[4].x=m_Columns[3].x+18;
		m_Columns[5].x=m_Columns[4].x+18;
		m_Columns[6].x=m_Columns[5].x+18;
		m_Columns[7].x=m_Columns[6].x+18;
		m_Columns[8].x=m_Columns[7].x+18;
		GetTextExtentPoint32(m_hDC,"Release",strlen("Release"),&sz);
		m_Columns[9].x=m_Columns[8].x+sz.cx+5;
	}
	m_Columns[10].x=m_Columns[9].x+55;
	m_Columns[11].x=m_Columns[10].x+55;
	m_Columns[12].x=m_Columns[11].x+55;

	for(i=0;i<8;i++)
		SetWindowPos(GetDlgItem(m_hWnd,i),NULL,m_Columns[1].x,i*14+(app.m_Reg.m_bShowVisLabels?14:0),0,0,SWP_NOZORDER|SWP_NOSIZE);

	for(i=0;i<sizeof(m_Colors)/sizeof(m_Colors[0]);i++)
	{
		if(m_Pens[i])DeleteObject(m_Pens[i]);
		m_Pens[i]=CreatePen(PS_SOLID,1,m_Colors[i]);
		if(m_Brushes[i])DeleteObject(m_Brushes[i]);
		m_Brushes[i]=CreateSolidBrush(m_Colors[i]);
	}

	for(i=0;i<3;i++)
	{
		if(m_hDCGroup[i*2])DeleteDC(m_hDCGroup[i*2]);
		m_hDCGroup[i*2]=CreateCompatibleDC(m_hDC);
		if(m_hBmpGroup[i*2])DeleteObject(m_hBmpGroup[i*2]);
		m_hBmpGroup[i*2]=CreateCompatibleBitmap(m_hDC,50,13);
		SelectObject(m_hDCGroup[i*2],m_hBmpGroup[i*2]);
		INT r0=GetRValue(m_Colors[(i+8)*2])*0.7;
		INT g0=GetGValue(m_Colors[(i+8)*2])*0.7;
		INT b0=GetBValue(m_Colors[(i+8)*2])*0.7;
		INT r1=GetRValue(m_Colors[(i+8)*2+1])*0.7;
		INT g1=GetGValue(m_Colors[(i+8)*2+1])*0.7;
		INT b1=GetBValue(m_Colors[(i+8)*2+1])*0.7;
		DrawChannelGauge(m_hDCGroup[i*2],0,0,50,13,RGB(r0,g0,b0),RGB(r1,g1,b1));

		if(m_hDCGroup[i*2+1])DeleteDC(m_hDCGroup[i*2+1]);
		m_hDCGroup[i*2+1]=CreateCompatibleDC(m_hDC);
		if(m_hBmpGroup[i*2+1])DeleteObject(m_hBmpGroup[i*2+1]);
		m_hBmpGroup[i*2+1]=CreateCompatibleBitmap(m_hDC,50,13);
		SelectObject(m_hDCGroup[i*2+1],m_hBmpGroup[i*2+1]);
		r0=GetRValue(m_Colors[(i+8)*2])*1.3;
		if(r0>255)r0=255;
		g0=GetGValue(m_Colors[(i+8)*2])*1.3;
		if(g0>255)g0=255;
		b0=GetBValue(m_Colors[(i+8)*2])*1.3;
		if(b0>255)b0=255;
		r1=GetRValue(m_Colors[(i+8)*2+1])*1.3;
		if(r1>255)r1=255;
		g1=GetGValue(m_Colors[(i+8)*2+1])*1.3;
		if(g1>255)g1=255;
		b1=GetBValue(m_Colors[(i+8)*2+1])*1.3;
		if(b1>255)b1=255;
		DrawChannelGauge(m_hDCGroup[i*2+1],0,0,50,13,RGB(r0,g0,b0),RGB(r1,g1,b1));
	}
	if(m_hDCGroup[6])DeleteDC(m_hDCGroup[6]);
	m_hDCGroup[6]=CreateCompatibleDC(m_hDC);
	if(m_hBmpGroup[6])DeleteObject(m_hBmpGroup[6]);
	m_hBmpGroup[6]=CreateCompatibleBitmap(m_hDC,50,13);
	SelectObject(m_hDCGroup[6],m_hBmpGroup[6]);

	HDC hDC=CreateCompatibleDC(m_hDC);
	HBITMAP hBmp=CreateCompatibleBitmap(m_hDC,8,8);
	SelectObject(hDC,hBmp);

	RECT rc={0,0,8,8};
	FillRect(hDC,&rc,m_Brushes[22]);
	COLORREF clr=RGB(GetRValue(m_Colors[22])*0.5,GetGValue(m_Colors[22])*0.5,GetBValue(m_Colors[22])*0.5);
	for(INT y=0;y<8;y++)
		for(INT x=y%2;x<8;x+=2)
			SetPixel(hDC,x,y,clr);
	if(m_BlueBrush)DeleteObject(m_BlueBrush);
	m_BlueBrush=CreatePatternBrush(hBmp);
	FillRect(hDC,&rc,m_Brushes[24]);
	clr=RGB(GetRValue(m_Colors[24])*0.5,GetGValue(m_Colors[24])*0.5,GetBValue(m_Colors[24])*0.5);
	for(y=0;y<8;y++)
		for(INT x=y%2;x<8;x+=2)
			SetPixel(hDC,x,y,clr);
	if(m_RedBrush)DeleteObject(m_RedBrush);
	m_RedBrush=CreatePatternBrush(hBmp);

	DeleteDC(hDC);
	DeleteObject(hBmp);

	if(m_hDCGroup[7])
	{
		SetRect(&rc,0,0,m_nWaveWidth,m_nWaveHeight/2);
		FillRect(m_hDCGroup[7],&rc,m_BlueBrush);
		SetRect(&rc,0,m_nWaveHeight/2,m_nWaveWidth,m_nWaveHeight);
		FillRect(m_hDCGroup[7],&rc,m_RedBrush);
	}

	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,(LONG)NULL);
	if(m_BackBrush)DeleteObject(m_BackBrush);
	m_BackBrush=CreateSolidBrush(m_Colors[0]);
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,(LONG)m_BackBrush);

	RePaint(NULL);

	if(bRestart)
		Start();

	return TRUE;
}

void CVisWindow::MuteChannels(int id)
{
	switch(id)
	{
	case ID_MUTE0:app.m_Reg.m_dwActiveChannels^=1;break;
	case ID_MUTE1:app.m_Reg.m_dwActiveChannels^=2;break;
	case ID_MUTE2:app.m_Reg.m_dwActiveChannels^=4;break;
	case ID_MUTE3:app.m_Reg.m_dwActiveChannels^=8;break;
	case ID_MUTE4:app.m_Reg.m_dwActiveChannels^=16;break;
	case ID_MUTE5:app.m_Reg.m_dwActiveChannels^=32;break;
	case ID_MUTE6:app.m_Reg.m_dwActiveChannels^=64;break;
	case ID_MUTE7:app.m_Reg.m_dwActiveChannels^=128;break;
	case ID_MUTEALL:app.m_Reg.m_dwActiveChannels^=255;break;
	}
	app.m_Player.MuteChannels(~app.m_Reg.m_dwActiveChannels);
	if(IsActive())
	{	
		for(UINT i=0;i<8;i++)
			CheckDlgButton(m_hWnd,i,app.m_Reg.m_dwActiveChannels&(1<<i));
		Redraw();
	}
}
