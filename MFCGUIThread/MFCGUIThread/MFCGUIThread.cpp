
// MFCGUIThread.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "MFCGUIThread.h"
#include "MainFrm.h"
#include "AboutDlg.h"

#include "GUIThread.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCGUIThreadApp

BEGIN_MESSAGE_MAP(CMFCGUIThreadApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMFCGUIThreadApp::OnAppAbout)
END_MESSAGE_MAP()


// CMFCGUIThreadApp コンストラクション

CMFCGUIThreadApp::CMFCGUIThreadApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}

// 唯一の CMFCGUIThreadApp オブジェクトです。

CMFCGUIThreadApp theApp;


// CMFCGUIThreadApp 初期化

BOOL CMFCGUIThreadApp::InitInstance()
{
	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// OLE ライブラリを初期化します。
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	// メイン ウィンドウを作成するとき、このコードは新しいフレーム ウィンドウ オブジェクトを作成し、
	// それをアプリケーションのメイン ウィンドウにセットします
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// フレームをリソースからロードして作成します
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);



	m_pGUIThread	= (CGUIThread*)AfxBeginThread(RUNTIME_CLASS(CGUIThread));


	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出してください。
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	return TRUE;
}

int CMFCGUIThreadApp::ExitInstance()
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	DWORD	dwRet = ::WaitForSingleObject( m_pGUIThread->m_hThread, 10 );
	if( dwRet == WAIT_TIMEOUT )
	{
		::PostThreadMessage( m_pGUIThread->m_nThreadID, WM_QUIT, 0, 0 );
		dwRet = ::WaitForSingleObject( m_pGUIThread->m_hThread, 1000 );
		if( dwRet != WAIT_OBJECT_0 )
		{
			::TerminateThread( m_pGUIThread->m_hThread, 0 );
		}
	}

	return CWinApp::ExitInstance();
}


// CMFCGUIThreadApp メッセージ ハンドラ

// ダイアログを実行するためのアプリケーション コマンド
void	CMFCGUIThreadApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCGUIThreadApp メッセージ ハンドラ




