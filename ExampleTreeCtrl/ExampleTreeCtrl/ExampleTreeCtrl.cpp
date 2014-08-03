// ExampleTreeCtrl.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleTreeCtrl.h"

static HWND	g_hMainWnd	= NULL;
static bool	g_fDragging	= false;
static HWND	hTreeCtrl	= NULL;

void	InsertTreeItem( HWND hTree )
{
	TCHAR				szDirectoryName[ MAX_PATH ];
	SHFILEINFO			fileInfo;
	TVINSERTSTRUCT		is;

	GetCurrentDirectory( sizeof(szDirectoryName) / sizeof(TCHAR), szDirectoryName );
	lstrcat( szDirectoryName, L"\\*" );


	WIN32_FIND_DATA		findData	= {0};
	HANDLE				hFindFile	= ::FindFirstFileW( szDirectoryName, &findData );

	do {
		if( (lstrcmp(findData.cFileName, L"..") != 0)
		&&	(lstrcmp(findData.cFileName, L".") != 0)	)
		{
			SHGetFileInfo( findData.cFileName, 0, &fileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX );
			is.item.iImage			= fileInfo.iIcon;

			SHGetFileInfo( findData.cFileName, 0, &fileInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_OPENICON );
			is.item.iSelectedImage	= fileInfo.iIcon;
		
			is.hParent		= TVI_ROOT;
			is.item.mask	= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
			is.item.pszText	= findData.cFileName;
			is.item.lParam	= 0;
			TreeView_InsertItem( hTree, &is );
		}
	} while( ::FindNextFileW(hFindFile, &findData) );

}



void Main_OnBeginDrag( HWND hwndTV, LPNMTREEVIEW lpnmtv )
{ 
    HIMAGELIST himl;    // handle to image list 
    RECT rcItem;        // bounding rectangle of item 

    // Tell the tree-view control to create an image to use 
    // for dragging. 
    himl = TreeView_CreateDragImage(hwndTV, lpnmtv->itemNew.hItem); 

    // Get the bounding rectangle of the item being dragged. 
    TreeView_GetItemRect(hwndTV, lpnmtv->itemNew.hItem, &rcItem, TRUE); 

    // Start the drag operation. 
    ImageList_BeginDrag(himl, 0, 0, 0);
    ImageList_DragEnter(hwndTV, lpnmtv->ptDrag.x, lpnmtv->ptDrag.x); 

    // Hide the mouse pointer, and direct mouse input to the 
    // parent window. 
    ShowCursor(FALSE); 
    SetCapture(GetParent(hwndTV)); 
    g_fDragging = TRUE; 

    return; 
}

void	Main_OnMouseMove( HWND hwndParent, HWND hwndTV, LONG xCur, LONG yCur ) 
{ 
    HTREEITEM		htiTarget;  // Handle to target item. 
    TVHITTESTINFO	tvht;   // Hit test information. 

    if( g_fDragging ) 
    { 
       // Drag the item to the current position of the mouse pointer. 
       // First convert the dialog coordinates to control coordinates. 
       POINT point;
       point.x = xCur;
       point.y = yCur;
       ClientToScreen( hwndParent, &point );
       ScreenToClient( hwndTV, &point );
       ImageList_DragMove( point.x, point.y );

       // Turn off the dragged image so the background can be refreshed.
       ImageList_DragShowNolock( FALSE ); 
                
        // Find out if the pointer is on the item. If it is, 
        // highlight the item as a drop target. 
        tvht.pt.x	= point.x; 
        tvht.pt.y	= point.y; 
        if( (htiTarget = TreeView_HitTest(hwndTV, &tvht)) != NULL ) 
        { 
            TreeView_SelectDropTarget( hwndTV, htiTarget ); 
		} 
        ImageList_DragShowNolock( TRUE );
    } 
    return; 
}

void Main_OnLButtonUp(HWND hwndTV) 
{ 
    if( g_fDragging ) 
    { 
        // Get destination item.
        HTREEITEM htiDest = TreeView_GetDropHilight(hwndTV);
        if (htiDest != NULL)
        {
            // To do: handle the actual moving of the dragged node.
        }
        ImageList_EndDrag(); 
        TreeView_SelectDropTarget( hwndTV, NULL );
        ReleaseCapture(); 
        ShowCursor(TRUE); 
        g_fDragging = FALSE; 
    } 
    return; 
}

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
	LoadString(hInstance, IDC_EXAMPLETREECTRL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXAMPLETREECTRL));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXAMPLETREECTRL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXAMPLETREECTRL);
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

	switch (message)
	{
	case WM_CREATE:
		{
			INITCOMMONCONTROLSEX	cmmex;
			cmmex.dwSize	= sizeof(cmmex);
			cmmex.dwICC		= ICC_TREEVIEW_CLASSES;
			InitCommonControlsEx( &cmmex );

			DWORD	dwStyle	
			//	= (WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);
				= (TVS_TRACKSELECT | TVS_SHOWSELALWAYS | WS_BORDER | WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT);

			hTreeCtrl	= CreateWindowEx(0, WC_TREEVIEW, L"", dwStyle, 0, 0, 0, 0, hWnd, (HMENU)1, ((LPCREATESTRUCT)lParam)->hInstance, NULL );

			SHFILEINFO	fileInfo	= {0};
			HIMAGELIST	hImg	= (HIMAGELIST)SHGetFileInfo( (LPCTSTR)L"C:\\", 0, &fileInfo, sizeof(fileInfo), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
			TreeView_SetImageList( hTreeCtrl, hImg, TVSIL_NORMAL );

			InsertTreeItem( hTreeCtrl );

			TreeView_SetExtendedStyle( hTreeCtrl, TVS_EX_MULTISELECT, TVS_EX_MULTISELECT );
		}
		break;

	case WM_SIZE:
		{
			MoveWindow( hTreeCtrl, 100, 10, LOWORD(lParam)+100, HIWORD(lParam)-100, TRUE );
		}
		break;

	case WM_MOUSEMOVE:
		{
			Main_OnMouseMove( hWnd, hTreeCtrl, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
		}
		break;

	case WM_LBUTTONUP:
		{
			Main_OnLButtonUp( hTreeCtrl );
		}
		break;

	case WM_NOTIFY:
		{
			switch( ((LPNMHDR)lParam)->code ){
			case TVN_BEGINDRAG:
				{
					Main_OnBeginDrag(((LPNMHDR)lParam)->hwndFrom, (LPNMTREEVIEW)lParam);
				}
				break;

			case NM_CUSTOMDRAW:
				{
					LPNMTVCUSTOMDRAW	lpCustomDraw	= (LPNMTVCUSTOMDRAW)lParam;
					if( lpCustomDraw->nmcd.dwDrawStage == CDDS_PREPAINT )
					{
						return(CDRF_NOTIFYITEMDRAW);
					}
					else
					if( lpCustomDraw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
					{
						if( lpCustomDraw->nmcd.lItemlParam == 3 )
						{
							lpCustomDraw->clrText	= RGB(255,255,255);
							lpCustomDraw->clrTextBk	= RGB(255,0,0);
						}
						return(CDRF_NEWFONT);
					}
				}
				break;
			}// end of switch.

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
	case WM_DESTROY:
		PostQuitMessage(0);
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
