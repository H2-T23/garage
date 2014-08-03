 // SDKOLEDragAndDrop.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SDKOLEDragAndDrop.h"
#include <OleIdl.h>


/********************************************************************************
 *
 *
 */
typedef struct {
	HENHMETAFILE		hEnhMetaFile;
	POINT				pt;
} DROPDATA, *LPDROPDATA;
/********************************************************************************
 *
 *
 */
static DROPDATA		g_dropData[ 5 ]		= {0};
static int			g_nDropDataCount	= 0;
static const int	g_nDropDataMaxCount	= sizeof(g_dropData) /  sizeof(DROPDATA);
/********************************************************************************
 *
 *
 */
typedef struct {
	RECT				rc;
	BOOL				bRectangle;
} DRAGDATA, *LPDRAGDATA;
/********************************************************************************
 *
 *
 */
static DRAGDATA		g_dragData[ 2 ]		= {0};
static const int	g_nDragDataMaxCount	= 2;
/********************************************************************************
 *
 *
 */
class CDropTarget : public IDropTarget {
public:
	STDMETHODIMP			QueryInterface( REFIID riid, void **ppvObject ){
		*ppvObject	= NULL;

		if( IsEqualIID(riid, IID_IUnknown)
		||	IsEqualIID(riid, IID_IDropTarget)	)
		{
			*ppvObject	= static_cast<IDropTarget*>( this );
		}
		else
		{
			return E_NOINTERFACE;
		}

		AddRef();

		return S_OK;
	}
	STDMETHODIMP_(ULONG)	AddRef(){
		return ::InterlockedIncrement(&m_cRef);
	}
	STDMETHODIMP_(ULONG)	Release(){
		if( ::InterlockedDecrement(&m_cRef) == 0 ){
			delete this;
			return 0;
		}
		return(m_cRef);
	}
	
	STDMETHODIMP			DragEnter( IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect ){
		*pdwEffect		= DROPEFFECT_COPY;

		FORMATETC		fmtEtc	= {0};
		fmtEtc.cfFormat		= CF_ENHMETAFILE;
		fmtEtc.ptd			= NULL;
		fmtEtc.dwAspect		= DVASPECT_CONTENT;
		fmtEtc.lindex		= -1;
		fmtEtc.tymed		= TYMED_ENHMF;

		HRESULT			hResult	= pDataObj->QueryGetData( &fmtEtc );
		if( hResult != S_OK )
		{
			m_bSupportFormat	= FALSE;
			*pdwEffect			= DROPEFFECT_NONE;
		}
		else
		{
			m_bSupportFormat	= TRUE;
		}

		return(S_OK);
	}
	STDMETHODIMP			DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect ){
		*pdwEffect	= DROPEFFECT_COPY;
		
		if( !m_bSupportFormat ){
			*pdwEffect	= DROPEFFECT_NONE;
		}

		return(S_OK);
	}
	STDMETHODIMP			DragLeave(){
		return(S_OK);
	}
	STDMETHODIMP Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect ){
		*pdwEffect	= DROPEFFECT_COPY;

		FORMATETC	fmtEtc	= {0};
		fmtEtc.cfFormat		= CF_ENHMETAFILE;
		fmtEtc.ptd			= NULL;
		fmtEtc.dwAspect		= DVASPECT_CONTENT;
		fmtEtc.lindex		= -1;
		fmtEtc.tymed		= TYMED_ENHMF;

		STGMEDIUM		medium;

		HRESULT	hResult	= pDataObj->GetData( &fmtEtc, &medium );
		if( FAILED(hResult) )
		{
			*pdwEffect	= DROPEFFECT_NONE;
			return(E_FAIL);
		}

		if( g_nDropDataCount < g_nDropDataMaxCount )
		{
			g_dropData[ g_nDropDataCount ].hEnhMetaFile	= medium.hEnhMetaFile;
			g_dropData[ g_nDropDataCount ].pt.x			= pt.x;
			g_dropData[ g_nDropDataCount ].pt.y			= pt.y;

			ScreenToClient( m_hwnd, &g_dropData[ g_nDropDataCount ].pt );
			g_nDropDataCount++;
			InvalidateRect( m_hwnd, NULL, FALSE );
		}

		return(S_OK);
	}

public:
	CDropTarget( HWND hwnd ) : m_cRef(1), m_hwnd(NULL) {
	}
	~CDropTarget(){
	}

private:
	LONG		m_cRef;
	HWND		m_hwnd;
	BOOL		m_bSupportFormat;
};
/********************************************************************************
 *
 *
 */
class CDropSource : public IDropSource {
public:
	STDMETHODIMP			QueryInterface( REFIID riid, void** ppvObject ){
		*ppvObject	= NULL;

		if( IsEqualIID(riid, IID_IUnknown)
		||	IsEqualIID(riid, IID_IDropSource)	)
		{
			*ppvObject	= static_cast<IDropSource*>( this );

		}
		else
		{
			return(E_NOINTERFACE);
		}

		AddRef();
		return(S_OK);
	}
	STDMETHODIMP_(ULONG)	AddRef(){
		return ::InterlockedIncrement( &m_cRef );
	}
	STDMETHODIMP_(ULONG)	Release(){
		if( ::InterlockedDecrement( &m_cRef ) == 0 ){
			delete this;
			return 0;
		}
		return(m_cRef);
	}

	STDMETHODIMP			QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState ){
		if( fEscapePressed ){
			return(DRAGDROP_S_CANCEL);
		}

		if( (grfKeyState & MK_LBUTTON) == 0 ){
			return(DRAGDROP_S_DROP);
		}
		
		return(S_OK);
	}

	STDMETHODIMP			GiveFeedback( DWORD dwEffect ){
		return(DRAGDROP_S_USEDEFAULTCURSORS);
	}

public:
	CDropSource() : m_cRef(1) {
	}
	~CDropSource(){
	}

private:
	LONG			m_cRef;
};
/********************************************************************************
 *
 *
 */
class CDataObject : public IDataObject, public IEnumFORMATETC {
public:
	STDMETHODIMP			QueryInterface( REFIID riid, void **ppvObject ){
		*ppvObject = NULL;

		if(	IsEqualIID(riid, IID_IUnknown) 
		||	IsEqualIID(riid, IID_IDataObject)	)
		{
			*ppvObject = static_cast<IDataObject *>(this);
		}
		else
		if(	IsEqualIID(riid, IID_IEnumFORMATETC)	)
		{
			*ppvObject = static_cast<IEnumFORMATETC *>(this);
		}
		else
		{
			return(E_NOINTERFACE);
		}

		AddRef();
		return(S_OK);
	}

	STDMETHODIMP_(ULONG)	AddRef(){
		return ::InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG)	Release(){
		if( ::InterlockedDecrement(&m_cRef) == 0 ){
			delete this;
			return 0;
		}
		return(m_cRef);
	}
	STDMETHODIMP			GetData( FORMATETC* pformatetcIn, STGMEDIUM* pmedium )
	{
		if( pformatetcIn->cfFormat == CF_ENHMETAFILE ){
			pmedium->tymed			= TYMED_ENHMF;
			pmedium->hEnhMetaFile	= m_hEnhMetaFile;
			pmedium->pUnkForRelease	= NULL;
		}else{
			return(E_FAIL);
		}
		return(S_OK);
	}
	STDMETHODIMP			GetDataHere( FORMATETC* pformatetc, STGMEDIUM* pmedium ){
		return(E_NOTIMPL);
	}

	STDMETHODIMP			QueryGetData( FORMATETC* pformatetc ){
		if( pformatetc->cfFormat == CF_ENHMETAFILE ){
			return(S_OK);
		}
		return(S_FALSE);
	}
	STDMETHODIMP			GetCanonicalFormatEtc( FORMATETC* pformatectIn, FORMATETC* pformatetcOut ){
		return(E_NOTIMPL);
	}
	STDMETHODIMP			SetData( FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease ){
		if( pformatetc->cfFormat == CF_ENHMETAFILE )
		{
			if( m_hEnhMetaFile != NULL)
			{
				DeleteEnhMetaFile( m_hEnhMetaFile );
				m_hEnhMetaFile	= NULL;
			}
		
			m_hEnhMetaFile	= CopyEnhMetaFile( pmedium->hEnhMetaFile, NULL );

			if( fRelease ){
				DeleteEnhMetaFile( pmedium->hEnhMetaFile );
			}
		}
		else
		{
			return(E_FAIL);
		}
		return(S_OK);
	}
	STDMETHODIMP			EnumFormatEtc( DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc ){
		if( dwDirection == DATADIR_GET ){
			return QueryInterface( IID_PPV_ARGS( ppenumFormatEtc ));
		}else{
			return(E_NOTIMPL);
		}
	}
	STDMETHODIMP			DAdvise( FORMATETC* pformatetc, DWORD advf, IAdviseSink* pAdvSink, DWORD* pdwConnection ){
		return(E_NOTIMPL);
	}
	STDMETHODIMP			DUnadvise( DWORD dwConnection ){
		return(E_NOTIMPL);
	}
	STDMETHODIMP			EnumDAdvise( IEnumSTATDATA** ppenumAdvise ){
		return(E_NOTIMPL);
	}
	STDMETHODIMP			Next( ULONG celt, FORMATETC* rgelt, ULONG* pceltFetched ){
		FORMATETC	formatetc[] = {
			{CF_ENHMETAFILE, NULL, DVASPECT_CONTENT, -1, TYMED_ENHMF}
		};

		if( m_uEnumCount >= 1 )
			return S_FALSE;
	
		*rgelt	= formatetc[ m_uEnumCount ];

		if( pceltFetched != NULL )
			*pceltFetched	= 1;
	
		m_uEnumCount++;
		return(S_OK);
	}
	STDMETHODIMP			Skip( ULONG celt ){
		return(E_NOTIMPL);
	}

	STDMETHODIMP			Reset(VOID){
		m_uEnumCount = 0;
		return(S_OK);
	}
	STDMETHODIMP			Clone( IEnumFORMATETC** ppenum ){
		return(E_NOTIMPL);
	}


public:
	CDataObject() : m_cRef(1), m_uEnumCount(0), m_hEnhMetaFile(NULL) {
	}

	~CDataObject(){
		if( m_hEnhMetaFile ){
			::DeleteEnhMetaFile( m_hEnhMetaFile );
		}
	}

private:
	LONG			m_cRef;
	ULONG			m_uEnumCount;
	HENHMETAFILE	m_hEnhMetaFile;
};
/********************************************************************************
 *
 *
 */
BOOL SetDragData( IDataObject *pDataObject, LPDRAGDATA lpDragData )
{
	HRESULT      hr;
	HDC          hdc;
	RECT         rc;
	HENHMETAFILE hEnhMetaFile;
	FORMATETC    formatetc;
	STGMEDIUM    medium;
	
	hdc = CreateEnhMetaFile(NULL, NULL, NULL, NULL);

	rc = lpDragData->rc;
	if (lpDragData->bRectangle )
		Rectangle(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
	else
		Ellipse(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);

	hEnhMetaFile = CloseEnhMetaFile(hdc);

	formatetc.cfFormat = CF_ENHMETAFILE;
	formatetc.ptd      = NULL;
	formatetc.dwAspect = DVASPECT_CONTENT;
	formatetc.lindex   = -1;
	formatetc.tymed    = TYMED_ENHMF;

	medium.tymed = TYMED_ENHMF;
	medium.hEnhMetaFile = hEnhMetaFile;
	medium.pUnkForRelease = NULL;

	hr = pDataObject->SetData(&formatetc, &medium, TRUE);

	return hr == S_OK;
}
/********************************************************************************
 *
 *
 */
#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/********************************************************************************
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SDKOLEDRAGANDDROP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SDKOLEDRAGANDDROP));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SDKOLEDRAGANDDROP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SDKOLEDRAGANDDROP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	static IDropTarget*		pDropTarget	= NULL;
	static IDropSource*		pDropSource	= NULL;
	static IDataObject*		pDataObject	= NULL;


	switch( message )
	{
	case WM_CREATE:
		{
			::OleInitialize( NULL );

			pDropTarget	= new CDropTarget( hWnd );
			::RegisterDragDrop( hWnd, pDropTarget );


			pDropSource	= new CDropSource();
			pDataObject	= new CDataObject();

			g_dragData[ 0 ].bRectangle	= TRUE;
			SetRect( &g_dragData[ 0 ].rc, 30, 30, 150, 120 );

			g_dragData[ 1 ].bRectangle	= TRUE;
			SetRect( &g_dragData[ 1 ].rc, 200, 30, 320, 120 );
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId){
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		{
			RECT				rc;

			hdc = BeginPaint(hWnd, &ps);

			for( int i=0; i<g_nDragDataMaxCount; i++ )
			{
				rc	= g_dragData[ i ].rc;
				if( g_dragData[ i ].bRectangle )
				{
					::Rectangle( hdc, rc.left, rc.top, rc.right, rc.bottom );
				}
				else
				{
					::Ellipse( hdc, rc.left, rc.top, rc.right, rc.bottom );
				}
			}
		
			const int			HIMETRIC_INCH	= 2540;

			POINT				pt;
			SIZE				sz;
			ENHMETAHEADER		hdr;

			for( int i=0; i<g_nDropDataCount; i++ )
			{
				GetEnhMetaFileHeader( g_dropData[ i ].hEnhMetaFile, sizeof(ENHMETAHEADER), &hdr );
				sz.cx	= hdr.rclFrame.right	* ::GetDeviceCaps(hdc, LOGPIXELSX) / HIMETRIC_INCH;
				sz.cy	= hdr.rclFrame.bottom	* ::GetDeviceCaps(hdc, LOGPIXELSY) / HIMETRIC_INCH;
			
				pt		= g_dropData[ i ].pt;

				SetRect( &rc, pt.x, pt.y, (pt.x + sz.cx), (pt.y + sz.cy) );

				PlayEnhMetaFile( hdc, g_dropData[ i ].hEnhMetaFile, &rc );
			}

			EndPaint(hWnd, &ps);
		}
		break;

	case WM_LBUTTONDOWN:
		{
			POINT	pt;
			pt.x	= LOWORD(lParam);
			pt.y	= HIWORD(lParam);

			DWORD	dwEffect	= 0;

			for( int i=0; i<g_nDragDataMaxCount; i++ )
			{
				if( PtInRect(&g_dragData[ i ].rc, pt) )
				{
					SetDragData( pDataObject, &g_dragData[ i ] );
					::DoDragDrop( pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect );
					break;
				}
			}
		}
		break;

	case WM_DESTROY:
		{
			for( int i=0; i<g_nDropDataCount; i++ ){
				DeleteEnhMetaFile( g_dropData[ i ].hEnhMetaFile );
			}

			::RevokeDragDrop( hWnd );

			if( pDropTarget ){
				pDropTarget->Release();
			}

			if( pDropSource ){
				pDropSource->Release();
			}

			if( pDataObject ){
				pDataObject->Release();
			}

			::OleUninitialize();
			
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
