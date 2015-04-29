
// ColorControlExampleDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "AboutDlg.h"
#include "ColorControlExample.h"
#include "ColorControlExampleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CColorControlExampleDlg ダイアログ



CColorControlExampleDlg::CColorControlExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CColorControlExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CColorControlExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CColorControlExampleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CColorControlExampleDlg メッセージ ハンドラー

BOOL CColorControlExampleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	DWORD	dwStyle	= (WS_CHILD | WS_VISIBLE | WS_BORDER);
	CRect	rect(0, 0, 400, 30);

	if( m_wndHdr.Create( dwStyle, rect, this, 1001 ) ){
		m_wndHdr.SetFont( GetFont() );

		TCHAR buf[256] = {0};

		HDITEM hItem = {0};
		hItem.mask = (HDI_FORMAT | HDI_TEXT | HDI_WIDTH);
		hItem.fmt			= HDF_CENTER | HDF_STRING | HDF_OWNERDRAW;
		hItem.cchTextMax	= 256;
		hItem.pszText		= buf;
		hItem.cxy			= 100;

		_tcscpy_s( buf, _T("Col1") );
		m_wndHdr.InsertItem( 0, &hItem );

		_tcscpy_s( buf, _T("Col2") );
		m_wndHdr.InsertItem( 1, &hItem );

		_tcscpy_s( buf, _T("Col3") );
		m_wndHdr.InsertItem( 2, &hItem );

		m_wndHdr.SetBkColor( 0, RGB(0xFF,0,0) );
		m_wndHdr.SetBkColor( 1, RGB(0,0xFF,0) );
		m_wndHdr.SetBkColor( 2, RGB(0,0,0xFF) );
	}

	rect.MoveToY( 100 );
	rect.bottom += 100;

	if( m_wndList.Create(dwStyle | LVS_REPORT, rect, this, 1002) ){
		m_wndList.SetFont( GetFont() );

		m_wndList.InsertColumn( 0, _T("Col1"), 0, 100 );
		m_wndList.InsertColumn( 1, _T("Col2"), 0, 100 );
		m_wndList.InsertColumn( 2, _T("Col3"), 0, 100 );

		m_wndList.InsertItem( 0, _T("Row1") );
		m_wndList.InsertItem( 1, _T("Row2") );
		m_wndList.InsertItem( 2, _T("Row3") );

		m_wndList.SetBkColor( 0, RGB(0xFF,0,0) );
		m_wndList.SetBkColor( 1, RGB(0,0xFF,0) );
		m_wndList.SetBkColor( 2, RGB(0,0,0xFF) );

		m_wndList.SetBkColor( 1, 2, RGB(0xCC,0xCC,0xCC) );
	}

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CColorControlExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CColorControlExampleDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CColorControlExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

