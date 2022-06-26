// OSDWindow.cpp : implementation file
//

#include "stdafx.h"
#include "OSD.H"
#include "OSDWindow.h"
#include "MMHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_1SECTIMER		(100)

typedef BOOL (WINAPI *PFN_ALPHABLEND)(
	HDC hdcDest,                 // handle to destination DC
	int nXOriginDest,            // x-coord of upper-left corner
	int nYOriginDest,            // y-coord of upper-left corner
	int nWidthDest,              // destination width
	int nHeightDest,             // destination height
	HDC hdcSrc,                  // handle to source DC
	int nXOriginSrc,             // x-coord of upper-left corner
	int nYOriginSrc,             // y-coord of upper-left corner
	int nWidthSrc,               // source width
	int nHeightSrc,              // source height
	BLENDFUNCTION blendFunction  // alpha-blending function
);

//static FILE *fo = NULL;

/////////////////////////////////////////////////////////////////////////////
// COSDWindow

COSDWindow::COSDWindow(HWND hWnd , UINT uMsg ,int iOSDSize, BOOL bBold, LPCTSTR pszFontFace, UINT iOSDTime, COLORREF crBackColor, COLORREF crTextColor )
{
	m_iMessageCount = -1;

	InitVar();

	m_iOSDSize = iOSDSize ;
	m_bOSDBold = bBold ;
	m_strOSDFontName = pszFontFace ;
	m_iOSDTime = iOSDTime ;
	m_crOSDBackColor = crBackColor ;
	m_crOSDTextColor = crTextColor ;

	m_hWnd = hWnd ;
	m_uiMsg = uMsg ;
//	fo = fopen("c:\\temp.txt", "wt");
}

COSDWindow::~COSDWindow()
{
//	fclose(fo);
}


BEGIN_MESSAGE_MAP(COSDWindow, CWnd)
	//{{AFX_MSG_MAP(COSDWindow)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COSDWindow message handlers

BOOL COSDWindow::Create() 
{
	// TODO: Add your specialized code here and/or call the base class
	return CWnd::CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, 
		AfxRegisterWndClass(0), "", 
		WS_POPUP, 
		0, 0, 0, 0, 
//		::GetDesktopWindow(), NULL, NULL);
		NULL, NULL, NULL);
}

int COSDWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
//	SetTimer(IDC_1SECTIMER, 1000, NULL);
	return 0;
}

void COSDWindow::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
//	KillTimer(IDC_1SECTIMER);

	if (m_bmpBackAlpha.GetSafeHandle())
		m_bmpBackAlpha.DeleteObject();
	if (m_bmpBackText.GetSafeHandle())
		m_bmpBackText.DeleteObject();
}

void COSDWindow::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent==IDC_1SECTIMER)
	{
		KillTimer(IDC_1SECTIMER);
		ShowWindow(SW_HIDE);

		if( IsWindow(m_hWnd))
			::PostMessage( m_hWnd , m_uiMsg , 0 , 0 );
/*
		if (m_iMessageCount >= 0)
		{
			m_iMessageCount++;
			if (m_iMessageCount == m_iOSDTime)
			{
				m_iMessageCount = -1;
				ShowWindow(SW_HIDE);
			}
		}
*/
	}
	
	CWnd::OnTimer(nIDEvent);
}

int COSDWindow::GetOSDHeight()
{
	CRect rc;
	int iHeight;

	GetMonitorRect(::GetDesktopWindow(), &rc, TRUE);

	switch(m_iOSDSize)
	{
	case 0:
		iHeight = rc.Height() / 12;
		break;
	case 1:
		iHeight = rc.Height() / 10;
		break;
	case 3:
		iHeight = rc.Height() / 6;
		break;
	case 4:
		iHeight = rc.Height() / 4;
		break;
	default:
		iHeight = rc.Height() / 8;
		break;
	}

	return iHeight;
}

void COSDWindow::CreateOSDFont(CFont *pFont, int iHeight)
{
	LOGFONT lf;
	CRect rc;

	ZeroMemory(&lf, sizeof(LOGFONT));

	lf.lfHeight = iHeight;

	if (m_bOSDBold)
		lf.lfWeight = FW_BOLD;
	else
		lf.lfWeight = FW_NORMAL;

	lf.lfCharSet = DEFAULT_CHARSET;

	strcpy(lf.lfFaceName, m_strOSDFontName);
	pFont->CreateFontIndirect(&lf);
}

void COSDWindow::GetOSDWindowRect(CRect *prc)
{
	int iHeight, iMargin;

	GetMonitorRect(::GetDesktopWindow(), prc, TRUE);

	iHeight = GetOSDHeight();
	iMargin = iHeight / 10;

	prc->top = prc->bottom - iHeight;
}

void COSDWindow::ShowMessage(const char *pszMsg, int iBarCount)
{
	CRect rc;

	if (stricmp(m_strMessage, pszMsg)==0)
		m_bSameMsg = TRUE;
	else
		m_bSameMsg = FALSE;

	if (m_bSameMsg && IsWindowVisible())
		m_bPaintBarOnly = TRUE;
	else
		m_bPaintBarOnly = FALSE;

	m_iOldBarCount = m_iBarCount;
	m_strMessage = pszMsg;
	m_iBarCount = iBarCount;

	if (strlen(m_strMessage)==0)
		return;

//	fprintf(fo, "ShowMessage (Start): %s\n", m_strMessage);

	// erase previous message
	if (IsWindowVisible())
	{
//		fprintf(fo, "ShowMessage: Window is visible.\n");

		Invalidate();
//		fprintf(fo, "ShowMessage: After Invalidate.\n");
		UpdateWindow();

//		fprintf(fo, "ShowMessage: After UpdateWindow.\n");

		// after specified time, hide me
		m_iMessageCount = 0;
		SetTimer(IDC_1SECTIMER, m_iOSDTime * 1000, NULL);
	}
	else
	{
//		fprintf(fo, "ShowMessage: Window is invisible.\n");

		if (m_bmpBackAlpha.GetSafeHandle())
		{
			m_bmpBackAlpha.DeleteObject();
			m_bSameMsg = FALSE;
//			fprintf(fo, "ShowMessage: Alpha bitmap delete.\n");
		}

		GetOSDWindowRect(&rc);
		SetWindowPos(NULL, 
			rc.left, rc.top,
			rc.Width(), rc.Height(), SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER);

//		fprintf(fo, "ShowMessage: After SetWindowPos 2.\n");
		Invalidate();
//		fprintf(fo, "ShowMessage: After Invalidate 2.\n");
		UpdateWindow();

//		fprintf(fo, "ShowMessage: After UpdateWindow 2.\n");

		// after specified time, hide me
		m_iMessageCount = 0;
		SetTimer(IDC_1SECTIMER, m_iOSDTime * 1000, NULL);
	}

//	fprintf(fo, "ShowMessage (END): \n\n", m_strMessage);
}

void COSDWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CFont f, *pOldFont;
	CRect rc;
	HINSTANCE hIMG32;
	PFN_ALPHABLEND pAlphaBlend;
	int iMargin, iOffsetBar;
	CSize FontSize;
	CPoint ptBar;
	CSize szBar;
	int i;

	GetClientRect(&rc);
	iMargin = rc.Height() / 10;
	CreateOSDFont(&f, rc.Height() - (iMargin*2));

	pOldFont = dc.SelectObject(&f);
	FontSize = dc.GetTextExtent(m_strMessage);

//	fprintf(fo, "OnPaint: (Start) : %s\n", m_strMessage);

	if (m_bSameMsg==FALSE)
	{
		CBitmap *pbmpOld;

//		fprintf(fo, "OnPaint: Same message is FALSE.\n");

		if (m_memdc.GetSafeHdc()==NULL)
			m_memdc.CreateCompatibleDC(&dc);

		// AlphaBlending
		if (m_bmpBackAlpha.GetSafeHandle())
		{
			CBitmap *pbmp;

			if (m_memdc.GetSafeHdc()==NULL)
				m_memdc.CreateCompatibleDC(&dc);

			pbmp = m_memdc.SelectObject(&m_bmpBackAlpha);
			dc.BitBlt(0, 0, rc.Width(), rc.Height(), &m_memdc, 0, 0, SRCCOPY);
			m_memdc.SelectObject(pbmp);
	
//			fprintf(fo, "OnPaint: BltBlt Alpha.\n");
		}
		else
		{
			hIMG32 = LoadLibrary("MSIMG32.DLL");
			if (hIMG32 != NULL)
			{
				pAlphaBlend = (PFN_ALPHABLEND)GetProcAddress(hIMG32, "AlphaBlend");
				if (pAlphaBlend)
				{
					BLENDFUNCTION bf;
					CBitmap bm;

					bf.AlphaFormat = 0;
					bf.BlendFlags = 0;
					bf.BlendOp = AC_SRC_OVER;
					bf.SourceConstantAlpha = 50;

					bm.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
					pbmpOld = m_memdc.SelectObject(&bm);
					m_memdc.FillSolidRect(0, 0, rc.Width(), rc.Height(), m_crOSDBackColor);
					
					pAlphaBlend(dc.GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), 
						m_memdc.GetSafeHdc(), 0, 0, rc.Width(), rc.Height(), bf);

					m_memdc.SelectObject(pbmpOld);
//					fprintf(fo, "OnPaint: AlphaBlending is OK.\n");
				}
				FreeLibrary(hIMG32);
			}
		}

		// Save to memory
		if (m_bmpBackAlpha.GetSafeHandle())
			m_bmpBackAlpha.DeleteObject();
		m_bmpBackAlpha.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		pbmpOld = m_memdc.SelectObject(&m_bmpBackAlpha);
		m_memdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
		m_memdc.SelectObject(pbmpOld);

//		fprintf(fo, "OnPaint: Save Alpha.\n");

		dc.SetTextAlign(TA_LEFT|TA_TOP);
		dc.SetBkMode(TRANSPARENT);

		dc.SetTextColor(m_crOSDBackColor);
		dc.TextOut(iMargin+2, iMargin+2, m_strMessage);

		dc.SetTextColor(m_crOSDTextColor);
		dc.TextOut(iMargin, iMargin, m_strMessage);

//		fprintf(fo, "OnPaint: TextDraw.\n");

		// Save to memory
		if (m_bmpBackText.GetSafeHandle())
			m_bmpBackText.DeleteObject();
		m_bmpBackText.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
		pbmpOld = m_memdc.SelectObject(&m_bmpBackText);
		m_memdc.BitBlt(0, 0, rc.Width(), rc.Height(), &dc, 0, 0, SRCCOPY);
		m_memdc.SelectObject(pbmpOld);

//		fprintf(fo, "OnPaint: Save Back OK.\n");
	}

	dc.SelectObject(pOldFont);

	ptBar.x = iMargin + FontSize.cx + 40;
	ptBar.y = iMargin;
	iOffsetBar = rc.Width() - ptBar.x - iMargin;
	iOffsetBar = iOffsetBar / 32;
	szBar.cx = max(2, iOffsetBar * 2 / 5);
	szBar.cy = FontSize.cy;

	if (m_bSameMsg && m_bmpBackText.GetSafeHandle())
	{
		CBitmap *pbmp;

		if (m_memdc.GetSafeHdc()==NULL)
			m_memdc.CreateCompatibleDC(&dc);
		pbmp = m_memdc.SelectObject(&m_bmpBackText);
		if (m_bPaintBarOnly)
		{
			if (m_iBarCount < m_iOldBarCount)
			{
				ptBar.x += iOffsetBar * (m_iBarCount);
				dc.BitBlt(ptBar.x, ptBar.y, (m_iOldBarCount - m_iBarCount) * iOffsetBar, szBar.cy, &m_memdc, ptBar.x, ptBar.y, SRCCOPY);
			}
		}
		else
		{
			dc.BitBlt(0, 0, rc.Width(), rc.Height(), &m_memdc, 0, 0, SRCCOPY);
		}
		m_memdc.SelectObject(pbmp);
	}

	if (m_iBarCount > 0)
	{
		if (m_bPaintBarOnly)
		{
			if (m_iBarCount > m_iOldBarCount)
			{
				ptBar.x += iOffsetBar * (m_iOldBarCount);
				for ( i=m_iOldBarCount+1; i<=m_iBarCount; i++ )
				{
					dc.FillSolidRect(ptBar.x+1, ptBar.y+1, szBar.cx, szBar.cy, m_crOSDBackColor);
					dc.FillSolidRect(ptBar.x, ptBar.y, szBar.cx, szBar.cy, m_crOSDTextColor);
					ptBar.x += iOffsetBar;
				}
			}
		}
		else
		{
			for ( i=1; i<=m_iBarCount; i++ )
			{
				dc.FillSolidRect(ptBar.x+1, ptBar.y+1, szBar.cx, szBar.cy, m_crOSDBackColor);
				dc.FillSolidRect(ptBar.x, ptBar.y, szBar.cx, szBar.cy, m_crOSDTextColor);
				ptBar.x += iOffsetBar;
			}
		}
	}

//	fprintf(fo, "OnPaint: (END)\n\n");

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL COSDWindow::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void COSDWindow::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	if (bShow)
	{
	}
	else
	{
	}
}

void COSDWindow::InitVar()
{
	m_strMessage.Empty();
	m_iBarCount = -1;
	m_iOldBarCount = -1;
	m_bSameMsg = FALSE;
	m_bPaintBarOnly = FALSE;
}

