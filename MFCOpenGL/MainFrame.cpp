// MainFrame.cpp 
#include "MainFrame.h"

IMPLEMENT_DYNCREATE(CMainFrame,CFrameWnd)

CMainFrame::CMainFrame( void )
{
}

CMainFrame::~CMainFrame( void )
{
}

BEGIN_MESSAGE_MAP(CMainFrame,CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BOOL	CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CFrameWnd::PreCreateWindow( cs ) ){
		return FALSE;
	}
	
	cs.dwExStyle		&= ~WS_EX_CLIENTEDGE;
	cs.lpszClass	= ::AfxRegisterWndClass( 0 );
	
	return TRUE;
}

int		CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 ){
		return -1;
	}
	
	if( !m_wndView.Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,CRect(0,0,0,0),this,AFX_IDW_PANE_FIRST,NULL) ){
		return -1;
	}
	
	return 0;
}

void	CMainFrame::OnSetFocus( CWnd* pOldWnd )
{
	m_wndView.SetFocus();
}

