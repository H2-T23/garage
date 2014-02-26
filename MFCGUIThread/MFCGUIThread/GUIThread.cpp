// GUIThread.cpp : 実装ファイル
//

#include "stdafx.h"
#include "MFCGUIThread.h"
#include "GUIThread.h"
#include "Form.h"


// CGUIThread

IMPLEMENT_DYNCREATE(CGUIThread, CWinThread)

CGUIThread::CGUIThread()
{
}

CGUIThread::~CGUIThread()
{
}

BOOL CGUIThread::InitInstance()
{
	// TODO:  スレッドごとの初期化をここで実行します。
	LPCTSTR	lpszClassName	= AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW, ::LoadCursor(NULL, IDC_ARROW), (HBRUSH)::GetStockObject(WHITE_BRUSH), ::LoadIcon(NULL, IDI_APPLICATION));

	CForm*	pForm	= new CForm;
	if( pForm == NULL ){
		return FALSE;
	}
	pForm->CreateEx(0,lpszClassName,_T("Form"),WS_OVERLAPPEDWINDOW,100,100,300,300,NULL,0);
	pForm->ShowWindow( SW_SHOW );
	pForm->UpdateWindow();

	m_pMainWnd	= pForm;

	m_bAutoDelete	= TRUE;

	return TRUE;
}

int CGUIThread::ExitInstance()
{
	// TODO:  スレッドごとの後処理をここで実行します。
	if( m_pMainWnd ){
		TRACE(_T("CForm::DestroyWindow().\n"));
		m_pMainWnd->DestroyWindow();
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGUIThread, CWinThread)
END_MESSAGE_MAP()


// CGUIThread メッセージ ハンドラ
