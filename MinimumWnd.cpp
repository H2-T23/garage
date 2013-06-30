// MinimumWnd.cpp 
// 
// cl /D_AFXDLL /D_WINDOWS /D_DEBUG /MDd MinimumWnd.cpp /link /SUBSYSTEM:WINDOWS
// 

#ifndef	VC_EXTRALEAN
#define	VC_EXTRALEAN
#endif

#include <afx.h>

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxdtctl.h>
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif

#include <atlbase.h>


class CMinimumWndApp : public CWinApp {
public:
	BOOL	InitInstance(){
		InitCommonControls();
		
		CWinApp::InitInstance();
		
		CFrameWnd* pWnd = new CFrameWnd;
		if( pWnd->Create(NULL,_T("")) ){
			m_pMainWnd = pWnd;
			m_pMainWnd->ShowWindow( m_nCmdShow );
			m_pMainWnd->UpdateWindow();
			return TRUE;
		}
		return FALSE;
	}
};

CMinimumWndApp theApp;
