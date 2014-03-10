
// MainFrm.cpp : CMainFrame ƒNƒ‰ƒX‚ÌŽÀ‘•
//

#include "stdafx.h"
#include "LogVierwer.h"

#include "MainFrm.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

// CMainFrame
/**********************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)
/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCTRL, OnTabSelChange)
END_MESSAGE_MAP()
/**********************************************************************************
 *
 *
 *
 */
CMainFrame::CMainFrame()
{
}
/**********************************************************************************
 *
 *
 *
 */
CMainFrame::~CMainFrame()
{
}
/**********************************************************************************
 *
 *
 *
 */
BOOL	CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}
/**********************************************************************************
 *
 *
 *
 */
int		CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	if( !m_wndTabCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_TABCTRL) )
		return -1;

	if( !m_wndViewTab.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), &m_wndTabCtrl, IDC_TAB_DEFAULT) )
		return -1;

	if( !m_wndPropertyTab.CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, (WS_CHILD | WS_VISIBLE | WS_BORDER), CRect(0,0,0,0), &m_wndTabCtrl, IDC_TAB_PROPERTY) )
		return -1;

	if( !m_wndListTab.CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, (WS_CHILD | WS_VISIBLE | WS_BORDER), CRect(0,0,0,0), &m_wndTabCtrl, IDC_TAB_LIST) )
		return -1;

	CFont	font;
	font.Attach( ::GetStockObject( DEFAULT_GUI_FONT ) );

	m_wndTabCtrl.SetFont( &font );
	m_wndTabCtrl.InsertTab( 0, _T("View")		, &m_wndViewTab		);
	m_wndTabCtrl.InsertTab( 1, _T("Property")	, &m_wndPropertyTab	);
	m_wndTabCtrl.InsertTab( 2, _T("Log")		, &m_wndListTab		);

	m_pActiveWnd	= m_wndTabCtrl.TabSelChange( 0 );
	return 0;
}
#ifdef _DEBUG
/**********************************************************************************
 *
 *
 *
 */
void	CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
/**********************************************************************************
 *
 *
 *
 */
void	CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG
/**********************************************************************************
 *
 *
 *
 */
void	CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	m_pActiveWnd->SetFocus();
}
/**********************************************************************************
 *
 *
 *
 */
BOOL	CMainFrame::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	if (m_pActiveWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
