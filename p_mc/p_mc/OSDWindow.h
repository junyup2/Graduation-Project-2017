// OSDWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COSDWindow window

class COSDWindow : public CWnd
{
// Construction
public:
	COSDWindow(HWND hWnd , UINT uiMsg ,int iOSDSize=0, BOOL bBold = FALSE, LPCTSTR pszFontFace = "Verdana", UINT iOSDTime = 5, COLORREF crBackColor = RGB(255,255,255), COLORREF crTextColor =RGB(0,0,0));
     void SetMessage( LPCTSTR pszMsg )
	 {
		 m_strMessage = pszMsg;
	 }
// Attributes
public:

// Operations
public:
	void ShowMessage(const char *pszMsg, int iBarCount=-1);
	void InitVar();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COSDWindow)
	public:
	virtual BOOL Create();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COSDWindow();

	// Generated message map functions
protected:
	int m_iMessageCount;
	CString m_strMessage;
	int m_iBarCount;
	int m_iOldBarCount;
	CBitmap m_bmpBackAlpha;
	CBitmap m_bmpBackText;
	CDC	m_memdc;
	BOOL m_bSameMsg;
	BOOL m_bPaintBarOnly;
	int m_iOSDSize ;
	BOOL m_bOSDBold ;
	CString m_strOSDFontName ;
	UINT m_iOSDTime ;
	COLORREF m_crOSDBackColor ;
	COLORREF m_crOSDTextColor ;
	HWND        m_hWnd ;
	UINT		   m_uiMsg;

	void CreateOSDFont(CFont *pFont, int iHeight);
	int GetOSDHeight();
	void GetOSDWindowRect(CRect *prc);

	//{{AFX_MSG(COSDWindow)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
