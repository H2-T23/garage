
// MainFrm.cpp : CMainFrame クラスの実装
//

#include "stdafx.h"
#include "MFCDragAndDrop.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケーター
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame コンストラクション/デストラクション

CMainFrame::CMainFrame()
{
	// TODO: メンバー初期化コードをここに追加してください。
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Visual Studio 2005 スタイルのドッキング ウィンドウ動作を有効にします
	CDockingManager::SetDockingMode( DT_SMART );
	// Visual Studio 2005 スタイルのドッキング ウィンドウの自動非表示動作を有効にします
	EnableAutoHidePanes( CBRS_ALIGN_ANY );

	EnableDocking( CBRS_ALIGN_ANY );

	// フレームのクライアント領域全体を占めるビューを作成します。
	if( !m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL) )
	{
		TRACE0("ビュー ウィンドウを作成できませんでした。\n");
		return -1;
	}


	BOOL	bResult = FALSE;
	
	DWORD	dwStyle		= (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DLGFRAME | WS_THICKFRAME);
	DWORD	dwExStyle	= (0);

//	bResult	= m_wndFixedCtrl.Create( NULL, _T("FixedCtrlPane"), dwStyle, CRect(10, 10, 300, 300), this, 1001);
//	if( !bResult ){
//		return -1;
//	}

//	bResult	= m_wndFixedCtrl2.Create( NULL, _T("FixedCtrlPaneSub"), dwStyle, CRect(500,10, 300,300), this, 1002 );
//	if( !bResult ){
//		return -1;
//	}

#if 0
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("ツール バーの作成に失敗しました。\n");
		return -1;      // 作成できませんでした。
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("ステータス バーの作成に失敗しました。\n");
		return -1;      // 作成できない場合
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: ツール バーをドッキング可能にしない場合は、これらの 3 行を削除してください。
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
#endif

	bResult	= m_wndListCtrlDock.Create( _T("ListCtrlDock"), this, CSize(300,300), TRUE, 2001 );
	if( !bResult ){
		return -1;
	}

	m_wndListCtrlDock.EnableDocking( CBRS_ALIGN_ANY );
	DockPane( &m_wndListCtrlDock );



	bResult	= m_wndTreeCtrlDock.Create( _T("TreeCtrlDock"), this, CSize(300,300), TRUE, 2002 );
	if( !bResult ){
		return -1;
	}

	m_wndTreeCtrlDock.EnableDocking( CBRS_ALIGN_ANY );
	DockPane( &m_wndTreeCtrlDock );


	return 0;
}

BOOL	CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 診断

#ifdef _DEBUG
void	CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void	CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame メッセージ ハンドラー

void	CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ビュー ウィンドウにフォーカスを与えます。
	m_wndView.SetFocus();
}

BOOL	CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ビューに最初にコマンドを処理する機会を与えます。
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// それ以外の場合は、既定の処理を行います。
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

