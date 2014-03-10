
// LogVierwer.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "LogVierwer.h"
#include "MainFrm.h"
#include "AboutDlg.h"

/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CLogVierwerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CLogVierwerApp::OnAppAbout)
END_MESSAGE_MAP()
/**********************************************************************************
 *
 *
 *
 */
CLogVierwerApp::CLogVierwerApp()
{
}
/**********************************************************************************
 *
 *
 *
 */
CLogVierwerApp theApp;
/**********************************************************************************
 *
 *
 *
 */
BOOL	CLogVierwerApp::InitInstance()
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

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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

	if( !pFrame->Create(NULL,_T("MainFrame")) )
		return FALSE;

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}
/**********************************************************************************
 *
 *
 *
 */
void	CLogVierwerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
/**********************************************************************************
 *
 *
 *
 */



