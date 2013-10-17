// OpenGLApp.cpp 

#include "OpenGLApp.h"
#include "MainFrame.h"


BOOL	COpenGLApp::InitInstance( void )
{
	INITCOMMONCONTROLSEX	InitCtrls;
	InitCtrls.dwSize	= sizeof( InitCtrls );
	InitCtrls.dwICC		= ICC_WIN95_CLASSES;
	InitCommonControlsEx( &InitCtrls );

	CWinApp::InitInstance();

	if( !AfxOleInit() ){
		return(FALSE);
	}
	AfxEnableControlContainer();

	EnableTaskbarInteraction( FALSE );

	CMainFrame* pFrame = new CMainFrame;
	if( !pFrame )
		return FALSE;

	pFrame->Create(NULL,NULL);

	m_pMainWnd = pFrame;

	m_pMainWnd->ShowWindow( SW_SHOW );
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int		COpenGLApp::ExitInstance( void )
{
	AfxOleTerm( FALSE );
	return CWinApp::ExitInstance();
}
