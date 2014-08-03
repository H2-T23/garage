// SDKShellOLEDragAndDrop.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "SDKShellOLEDragAndDrop.h"
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <Shellapi.h>

#pragma comment (lib, "shlwapi.lib")
#pragma comment (lib, "comctl32.lib")

TCHAR	g_szBaseDirectory[256];

#define WM_SHELLCHANGE	WM_APP
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
class CDropTarget : public IDropTarget{
public:
	STDMETHODIMP			QueryInterface( REFIID riid, void** ppvObject ){
		*ppvObject = NULL;

		if(	IsEqualIID(riid, IID_IUnknown)
		||	IsEqualIID(riid, IID_IDropTarget)	)
			*ppvObject	= static_cast<IDropTarget *>(this);
		else
			return E_NOINTERFACE;

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
		return m_cRef;
	}

public:
	STDMETHODIMP	DragEnter( IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect ){
		*pdwEffect	= GetEffectFromKeyState( grfKeyState );
	
		if( m_pDropTargetHelper != NULL )
			m_pDropTargetHelper->DragEnter( m_hwnd, pDataObj, (LPPOINT)&pt, *pdwEffect );

		FORMATETC	fmtEtc	= {0};
		fmtEtc.cfFormat	= (CLIPFORMAT)RegisterClipboardFormat( CFSTR_SHELLIDLIST );
		fmtEtc.ptd		= NULL;
		fmtEtc.dwAspect	= DVASPECT_CONTENT;
		fmtEtc.lindex	= -1;
		fmtEtc.tymed	= TYMED_HGLOBAL;

		HRESULT   hr = pDataObj->QueryGetData( &fmtEtc );
		if( FAILED(hr) ){
			m_bSupportFormat	= FALSE;
			*pdwEffect			= DROPEFFECT_NONE;
		}
		else{
			m_bSupportFormat	= TRUE;
		}
		m_bRButton	= grfKeyState & MK_RBUTTON;

		return S_OK; 
	}
	STDMETHODIMP	DragOver( DWORD grfKeyState, POINTL pt, DWORD* pdwEffect ){
		*pdwEffect	= GetEffectFromKeyState( grfKeyState );
	
		if( m_pDropTargetHelper != NULL )
			m_pDropTargetHelper->DragOver( (LPPOINT)&pt, *pdwEffect );

		if( !m_bSupportFormat )
			*pdwEffect	= DROPEFFECT_NONE;

		return S_OK;
	}
	STDMETHODIMP	DragLeave(){
		if( m_pDropTargetHelper ){
			m_pDropTargetHelper->DragLeave();
		}
		return S_OK;
	}
	STDMETHODIMP	Drop( IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect ){

		*pdwEffect = GetEffectFromKeyState( grfKeyState );

		if (m_pDropTargetHelper != NULL)
			m_pDropTargetHelper->Drop( pDataObj, (LPPOINT)&pt, *pdwEffect );


		FORMATETC        formatetc;
		formatetc.cfFormat	= (CLIPFORMAT)::RegisterClipboardFormat( CFSTR_SHELLIDLIST );
		formatetc.ptd		= NULL;
		formatetc.dwAspect	= DVASPECT_CONTENT;
		formatetc.lindex	= -1;
		formatetc.tymed		= TYMED_HGLOBAL;


		STGMEDIUM        medium;
		HRESULT	hr = pDataObj->GetData( &formatetc, &medium );
		if( FAILED(hr) )
		{
			*pdwEffect	= DROPEFFECT_NONE;
			return E_FAIL;
		}

		if( m_bRButton )
		{
			HMENU	hmenuPopup	= ::CreatePopupMenu();
		
			DWORD	dwEffect	= *pdwEffect;
			if( dwEffect & DROPEFFECT_MOVE )
				InitializeMenuItem( hmenuPopup, TEXT("ここに移動"), DROPEFFECT_MOVE );
			
			if( dwEffect & DROPEFFECT_COPY )
				InitializeMenuItem( hmenuPopup, TEXT("ここにコピー"), DROPEFFECT_COPY );
			
			if( dwEffect & DROPEFFECT_LINK )
				InitializeMenuItem( hmenuPopup, TEXT("ここにリンク"), DROPEFFECT_LINK );

			InitializeMenuItem( hmenuPopup, NULL, 100 );
			InitializeMenuItem( hmenuPopup, TEXT("キャンセル"), 0 );

			int	nId = ::TrackPopupMenu( hmenuPopup, TPM_RETURNCMD, pt.x, pt.y, 0, m_hwnd, NULL );
			if( nId == 0 )
			{
				::DestroyMenu( hmenuPopup );
				*pdwEffect	= DROPEFFECT_NONE;
				return E_FAIL;
			}

			*pdwEffect = nId;
			::DestroyMenu( hmenuPopup );
		}


		LPIDA	lpIda = (LPIDA)::GlobalLock( medium.hGlobal );
	
		LPTSTR				lpszFileName;
		TCHAR				szSrcFilePath[ 256 ];
		TCHAR				szDestFilePath[ 256 ];
		PIDLIST_ABSOLUTE	pidl;	
		for( UINT i=0; i<lpIda->cidl; i++ )
		{
			pidl	= (PIDLIST_ABSOLUTE)(((LPBYTE)lpIda) + lpIda->aoffset[ 1 + i ] );
			::SHGetPathFromIDList( pidl, szSrcFilePath );

			lpszFileName	= ::PathFindFileName( szSrcFilePath );
			lstrcpy( szDestFilePath, g_szBaseDirectory );
			::PathAppend( szDestFilePath, lpszFileName );

			if(	*pdwEffect == DROPEFFECT_MOVE )
				::MoveFile( szSrcFilePath, szDestFilePath );
			else
			if( *pdwEffect == DROPEFFECT_COPY )
				::CopyFile( szSrcFilePath, szDestFilePath, TRUE );
			else
			if(	*pdwEffect == DROPEFFECT_LINK )
			{
				IShellLink*		pShellLink;
				IPersistFile*	pPersistFile;
				WCHAR			szLinkPath[256];
				LPWSTR			lpsz;
			
				CoCreateInstance(	CLSID_ShellLink
								, NULL
								, CLSCTX_INPROC_SERVER
								, IID_PPV_ARGS(&pShellLink)
								);
				pShellLink->SetPath( szSrcFilePath );
				pShellLink->QueryInterface( IID_PPV_ARGS(&pPersistFile) );
	#ifdef UNICODE
				lpsz	= szDestFilePath;
	#else
				WCHAR        szUnicode[256];
				MultiByteToWideChar( CP_ACP, 0, szDestFilePath, -1, szUnicode, MAX_PATH );
				lpsz = szUnicode;
	#endif
				wsprintfW( szLinkPath, L"%s.lnk", lpsz );
				pPersistFile->Save( szLinkPath, TRUE );
				pPersistFile->Release();
				pShellLink->Release();
			}
			else
				;
		}
	
		::GlobalUnlock( medium.hGlobal );
		::ReleaseStgMedium( &medium );
	
		return S_OK;
	}

public:
	CDropTarget( HWND hwnd ) : m_cRef(1), m_hwnd(hwnd) {
		::CoCreateInstance(	CLSID_DragDropHelper
						,	NULL
						,	CLSCTX_INPROC_SERVER
						,	IID_PPV_ARGS( &m_pDropTargetHelper )
						);
	}
	~CDropTarget(){
		if( m_pDropTargetHelper != NULL ){
			m_pDropTargetHelper->Release();
		}
	}

public:
	DWORD		GetEffectFromKeyState( DWORD grfKeyState ){
		DWORD dwEffect;

		if( grfKeyState & MK_CONTROL ){
			if( grfKeyState & MK_SHIFT )
				dwEffect	= DROPEFFECT_LINK;
			else
				dwEffect	= DROPEFFECT_COPY;
		}
		else
			dwEffect	= DROPEFFECT_MOVE;

		return dwEffect;
	}
	void		InitializeMenuItem( HMENU hmenu, LPTSTR lpszItemName, int nId ){
		MENUITEMINFO mii;
	
		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask  = MIIM_ID | MIIM_TYPE;
		mii.wID    = nId;

		if (lpszItemName != NULL) {
			mii.fType      = MFT_STRING;
			mii.dwTypeData = lpszItemName;
		}
		else
			mii.fType = MFT_SEPARATOR;

		InsertMenuItem( hmenu, nId, FALSE, &mii );
	}
private:
	LONG              m_cRef;
	HWND              m_hwnd;
	BOOL              m_bSupportFormat;
	BOOL              m_bRButton;
	IDropTargetHelper *m_pDropTargetHelper;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
class CDropSource : public IDropSource {
public:
	STDMETHODIMP			QueryInterface( REFIID riid, void** ppvObject ){
		*ppvObject = NULL;

		if(	IsEqualIID(riid, IID_IUnknown)
		||	IsEqualIID(riid, IID_IDropSource)	)
		{
			*ppvObject	= static_cast<IDropSource*>(this);
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
		return m_cRef;
	}
	STDMETHODIMP			QueryContinueDrag( BOOL fEscapePressed, DWORD grfKeyState ){
		if( fEscapePressed ){
			return DRAGDROP_S_CANCEL;
		}

		if( (grfKeyState & MK_LBUTTON) == 0 ){
			return DRAGDROP_S_DROP;
		}
		
		return S_OK;
	}
	STDMETHODIMP			GiveFeedback( DWORD dwEffect ){
		return DRAGDROP_S_USEDEFAULTCURSORS;
	}

public:
	CDropSource() : m_cRef(1) {
	}
	~CDropSource(){
	}

private:
	LONG		m_cRef;
};
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
BOOL SetDragImage(HWND hwnd, PIDLIST_ABSOLUTE *ppidlAbsolute, IDragSourceHelper *pDragSourceHelper, IDataObject *pDataObject)
{
	HRESULT     hr;
	HDC         hdc, hdcMem;
	HBITMAP     hbmp, hbmpPrev;
	SHFILEINFO  fileInfo;
	SHDRAGIMAGE dragImage;
	POINT       pt = {0, 0};
	SIZE        size = {32, 32};

	SHGetFileInfo((LPCTSTR)ppidlAbsolute[0], 0, &fileInfo, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_ICON | SHGFI_LARGEICON);

	hdc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdc);
	hbmp = CreateCompatibleBitmap(hdc, size.cx, size.cy);
	hbmpPrev = (HBITMAP)SelectObject(hdcMem, hbmp);
	DrawIcon(hdcMem, 0, 0, fileInfo.hIcon);
	SelectObject(hdcMem, hbmpPrev);
	DeleteDC(hdcMem);
	ReleaseDC(hwnd, hdc);
	DestroyIcon(fileInfo.hIcon);

	dragImage.sizeDragImage = size;
	dragImage.ptOffset = pt;
	dragImage.hbmpDragImage = hbmp;
	dragImage.crColorKey = RGB(0, 0, 0);
	hr = pDragSourceHelper->InitializeFromBitmap(&dragImage, pDataObject);

	return hr == S_OK;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
void	InsertListViewItem( HWND hwndListView )
{
	int             i = 0;
	TCHAR           szDirectoryName[256];
	HANDLE          hFindFile;
	WIN32_FIND_DATA findData;
	SHFILEINFO      fileInfo;
	LVITEM          item;
	
	GetCurrentDirectory(sizeof(szDirectoryName) / sizeof(TCHAR), szDirectoryName);
	lstrcat(szDirectoryName, TEXT("\\*"));

	hFindFile = FindFirstFile(szDirectoryName, &findData);

	do {
		if (lstrcmp(findData.cFileName, TEXT("..")) != 0 && lstrcmp(findData.cFileName, TEXT(".")) != 0) {
			SHGetFileInfo(findData.cFileName, 0, &fileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);

			item.mask     = LVIF_TEXT | LVIF_IMAGE;
			item.iItem    = i;
			item.iSubItem = 0;
			item.pszText  = findData.cFileName;
			item.iImage   = fileInfo.iIcon;
			ListView_InsertItem(hwndListView, &item);

			i++;
		}

	} while(FindNextFile(hFindFile, &findData));
	
	FindClose(hFindFile);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//
//
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
	LoadString(hInstance, IDC_SDKSHELLOLEDRAGANDDROP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SDKSHELLOLEDRAGANDDROP));

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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SDKSHELLOLEDRAGANDDROP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SDKSHELLOLEDRAGANDDROP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	static HWND              hwndListView = NULL;
	static IDropTarget       *pDropTarget = NULL;
	static IDropSource       *pDropSource = NULL;
	static IDragSourceHelper *pDragSourceHelper = NULL;


	switch (message)
	{
	case WM_CREATE:
		{
			INITCOMMONCONTROLSEX	ic;
			ic.dwSize	= sizeof(INITCOMMONCONTROLSEX);
			ic.dwICC	= ICC_LISTVIEW_CLASSES;
			::InitCommonControlsEx(&ic);

			::OleInitialize(NULL);

			pDropTarget	= new CDropTarget(hWnd);
			pDropSource	= new CDropSource();

			::CoCreateInstance(	CLSID_DragDropHelper
							,	NULL
							,	CLSCTX_INPROC_SERVER
							,	IID_PPV_ARGS(&pDragSourceHelper)
							);
			::RegisterDragDrop( hWnd, pDropTarget );
		
		
			hwndListView	= ::CreateWindowEx( 0
											, WC_LISTVIEW
											, TEXT("")
											, WS_CHILD | WS_VISIBLE | LVS_ICON
											, 0, 0, 0, 0
											, hWnd
											, (HMENU)1
											, ((LPCREATESTRUCT)lParam)->hInstance
											, NULL
											);

			SHFILEINFO	fileInfo;
			HIMAGELIST	himl	= (HIMAGELIST)::SHGetFileInfo(	(LPCTSTR)TEXT("C:\\")
											, 0
											, &fileInfo
											, sizeof(SHFILEINFO)
											, SHGFI_SYSICONINDEX | SHGFI_LARGEICON
											);

			ListView_SetImageList( hwndListView, himl, LVSIL_NORMAL );
			
			InsertListViewItem( hwndListView );
		
			PIDLIST_ABSOLUTE	pidlDesktop; 
			::SHGetSpecialFolderLocation( hWnd, CSIDL_DESKTOP, &pidlDesktop );

			SHChangeNotifyEntry		entry;
			entry.pidl			= pidlDesktop;
			entry.fRecursive	= TRUE;
			::SHChangeNotifyRegister( hWnd
									, SHCNRF_ShellLevel | SHCNRF_NewDelivery
									, SHCNE_CREATE | SHCNE_DELETE | SHCNE_MKDIR | SHCNE_RMDIR
									, WM_SHELLCHANGE
									, 1
									, &entry
									);

			GetCurrentDirectory( sizeof(g_szBaseDirectory)/sizeof(TCHAR), g_szBaseDirectory );
		}
		break;

	case WM_SHELLCHANGE:
		{
			LONG             lEvent;
			HANDLE           hLock;
			PIDLIST_ABSOLUTE *pidlAbsolute;
			TCHAR            szFilePath[256];
	
			hLock	= ::SHChangeNotification_Lock( (HANDLE)wParam
												, (DWORD)lParam
												, &pidlAbsolute
												, &lEvent
												);
			if( hLock != NULL )
			{
				::SHGetPathFromIDList( pidlAbsolute[0], szFilePath );
				::PathRemoveFileSpec( szFilePath );

				if(	lstrcmpi(g_szBaseDirectory, szFilePath) == 0 )
				{
					ListView_DeleteAllItems( hwndListView );
					
					InsertListViewItem( hwndListView );
				}
				::SHChangeNotification_Unlock( hLock );
			}
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
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
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;


	case WM_NOTIFY:
		{
			if( ((LPNMHDR)lParam)->code == LVN_BEGINDRAG )
			{
				int              i, j;
				int              nCount, nSelectedCount;
				TCHAR            szText[256];
				TCHAR            szFilePath[256];
				PITEMID_CHILD    *ppidlChild;
				PIDLIST_ABSOLUTE *ppidlAbsolute;
				IDataObject      *pDataObject;
				IShellFolder     *pShellFolder = NULL;
				DWORD            dwEffect;
			
				nSelectedCount	= ListView_GetSelectedCount( hwndListView );
				if( nSelectedCount == 0 ){
					return 0;
				}

				ppidlAbsolute = (PIDLIST_ABSOLUTE *)CoTaskMemAlloc(sizeof(PIDLIST_ABSOLUTE) * nSelectedCount);
				ppidlChild = (PITEMID_CHILD *)CoTaskMemAlloc(sizeof(PITEMID_CHILD) * nSelectedCount);

				nCount	= ListView_GetItemCount( hwndListView );
				for( i=0, j=0; i<nCount; i++ )
				{
					if(	ListView_GetItemState(hwndListView, i, LVIS_SELECTED) )
					{
						ListView_GetItemText( hwndListView, i, 0, szText, sizeof(szText) / sizeof(TCHAR) );
						wsprintf( szFilePath, TEXT("%s\\%s"), g_szBaseDirectory, szText );
						ppidlAbsolute[ j++ ]	= ILCreateFromPath( szFilePath );
					}
				}
			
				SHBindToParent( ppidlAbsolute[0], IID_PPV_ARGS(&pShellFolder), NULL );

				for( int i=0; i<nSelectedCount; i++ ){
					ppidlChild[ i ]	= ILFindLastID( ppidlAbsolute[ i ] );
				}
				pShellFolder->GetUIObjectOf( NULL, nSelectedCount, (LPCITEMIDLIST*)ppidlChild, IID_IDataObject, NULL, (void **)&pDataObject );
			
				SetDragImage( hWnd, ppidlAbsolute, pDragSourceHelper, pDataObject);
				::DoDragDrop( pDataObject, pDropSource, DROPEFFECT_MOVE | DROPEFFECT_COPY | DROPEFFECT_LINK, &dwEffect);

				for( int i=0; i<nSelectedCount; i++ ){
					CoTaskMemFree( ppidlAbsolute[i] );
				}

				CoTaskMemFree( ppidlAbsolute );
				CoTaskMemFree( ppidlChild );
			}
			else
			{
				;
			}
		}
		break;

	case WM_SIZE:
		MoveWindow( hwndListView, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE );
		break;

	case WM_DESTROY:
		if(pDropTarget )
			pDropTarget->Release();

		if( pDropSource )
			pDropSource->Release();

		if( pDragSourceHelper )
			pDragSourceHelper->Release();

		::RevokeDragDrop( hWnd );
		::OleUninitialize();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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
