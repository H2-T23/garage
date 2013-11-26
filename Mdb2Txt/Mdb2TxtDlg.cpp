
// Mdb2TxtDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Mdb2TxtApp.h"
#include "Mdb2TxtDlg.h"
#include "Mdb2Txt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg() : CDialog(CAboutDlg::IDD){
	}

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX){    // DDX/DDV サポート
		CDialog::DoDataExchange(pDX);
	}

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMdb2TxtDlg ダイアログ



CMdb2TxtDlg::CMdb2TxtDlg(CWnd* pParent /*=NULL*/)
//	: CDialog(CMdb2TxtDlg::IDD, pParent)
	: TMovableWnd<CDialog>(CMdb2TxtDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMdb2TxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMdb2TxtDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDBLCLK()
//	ON_WM_LBUTTONDOWN()
//	ON_WM_LBUTTONUP()
//	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CMdb2TxtDlg メッセージ ハンドラ

BOOL CMdb2TxtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	ModifyStyleEx( WS_EX_APPWINDOW, WS_EX_TOOLWINDOW );
	SetWindowPos( &CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	DragAcceptFiles();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CMdb2TxtDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CMdb2TxtDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		//CRect	rc(0,0,0,0);
		//GetWindowRect( &rc );		

		//CClientDC	dc(this);
		//dc.SetTextColor( RGB(0xFF,0xFF,0xFF) );
		//dc.SetBkColor( RGB(0x00,0xAA,0xFF) );
		//dc.DrawText( _T("MDBファイルをドロップ、または左ダブルクリックしてください。"), -1, &rc, (DT_LEFT | DT_VCENTER | DT_SINGLELINE) );
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CMdb2TxtDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void	CMdb2TxtDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	TCHAR	szFileName[ MAX_PATH ];
	ZeroMemory( szFileName, sizeof(szFileName) );

#if 0
	UINT uNum = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );
	TRACE( _T("%s\n"), szFileName );

	for( UINT i=0; i<uNum; i++ ){
		DragQueryFile( hDropInfo, i, szFileName, sizeof(szFileName) );
		TRACE( _T("%s\n"), szFileName );
	}
#else
	DragQueryFile( hDropInfo, 0, szFileName, sizeof(szFileName) );
	CMdb2Txt::Execute( szFileName, NULL, m_hWnd );
#endif

	CDialog::OnDropFiles(hDropInfo);
}

void	CMdb2TxtDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	CMdb2Txt::Execute( NULL, NULL, m_hWnd );
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL	CMdb2TxtDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	return CDialog::PreCreateWindow(cs);
}

#if 0
static bool		bMoving = false;
static CPoint	ptPrev(0,0);

void	CMdb2TxtDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	bMoving = true;
	ptPrev = point;
	ClientToScreen( &ptPrev );
	SetCapture();

	CDialog::OnLButtonDown(nFlags, point);
}

void	CMdb2TxtDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	::ReleaseCapture();
	bMoving = false;

	CDialog::OnLButtonUp(nFlags, point);
}

void	CMdb2TxtDlg::OnMouseMove( UINT nFlags, CPoint point )
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if( bMoving )
	{
		CRect	rc(0,0,0,0);
		GetWindowRect( &rc );

		CPoint	ptCurr( point );
		ClientToScreen( &ptCurr );

		CPoint	pt(ptCurr.x - ptPrev.x, ptCurr.y - ptPrev.y);

		TRACE( _T("[%d, %d]\n"), pt.x, pt.y );
		if( pt.x > 0 ){

		}

		if( pt.y > 0 ){

		}

	//	rc.MoveToXY( pt.x, pt.y );
		rc.top		+= pt.y;
		rc.bottom	+= pt.y;
		rc.left		+= pt.x;
		rc.right	+= pt.x;

		MoveWindow( &rc );

		ptPrev = ptCurr;
	}

	CDialog::OnMouseMove(nFlags, point);
}
#endif

HBRUSH	CMdb2TxtDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ここで DC の属性を変更してください。
	static CBrush	bruBK( RGB(0xFF,0x00,0xFF) );
	return (HBRUSH)bruBK;

	// TODO:  既定値を使用したくない場合は別のブラシを返します。
	return hbr;
}
