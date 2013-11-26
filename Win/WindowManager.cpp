// WindowManager.cpp 
#include <windows.h>
#include <tchar.h>


#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")


struct StateInfo {
	UINT		id;
};


template<class CLASS>
class Window {
protected:
	HWND		m_hWnd;

public:
	Window(void) : m_hWnd(NULL) {}

	BOOL	Create(LPCTSTR lpWindowName,DWORD dwStyle,DWORD dwExStyle=0,int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,int nWidth=CW_USEDEFAULT,int nHeight=CW_USEDEFAULT,HWND hWndParent=NULL,HMENU hMenu=NULL) {
		WNDCLASS	wc = {0};
		wc.lpfnWndProc		= CLASS::WindowProc;
		wc.hInstance		= GetModuleHandle( NULL );
		wc.lpszClassName	= ClassName();

		RegisterClass(&wc);

		m_hWnd	= CreateWindowEx(dwExStyle,ClassName(),lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,GetModuleHandle(NULL),this);
		return(m_hWnd ? TRUE : FALSE);
	}

	operator HWND(){ return(m_hWnd); }

protected:
	virtual LPCTSTR		ClassName( void ) const = 0;
	virtual LRESULT		HandleMessage( UINT, WPARAM, LPARAM ) = 0;

public:
	static LRESULT CALLBACK	WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ){
		CLASS* pThis = NULL;
		
		if( uMsg == WM_NCCREATE )
		{
			LPCREATESTRUCT pCreateStruct	= (LPCREATESTRUCT)lParam;
			pThis = (CLASS*)pCreateStruct->lpCreateParams;

			SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pThis );

			pThis->m_hWnd	= hWnd;
		}
		else
		{
			pThis	= (CLASS*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		}

		if( pThis ){
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}else{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
};

class ChildWindow : public Window<ChildWindow> {
public:
	LPCTSTR		ClassName(void) const {
		return(_T("child window"));
	}

	LRESULT		HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam){
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
};

class MainWindow : public Window<MainWindow> {
public:
	ChildWindow		child;

	LPCTSTR		ClassName(void) const {
		return(_T("main window"));
	}

	LRESULT		HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		switch( uMsg ){
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		case WM_CREATE:
			{
				if( child.Create(_T("child"),WS_OVERLAPPEDWINDOW) ){
					ShowWindow( (HWND)child, SW_SHOW );
					UpdateWindow( (HWND)child );
				}
			}
			return 0;

		case WM_PAINT:
			{
				PAINTSTRUCT	ps;
				HDC hdc = BeginPaint(m_hWnd, &ps);
				FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
				EndPaint(m_hWnd, &ps);
			}
			return 0;
	
		default:
			return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		}
		return TRUE;
	}
};

int WINAPI	_tWinMain( HINSTANCE hInstance, HINSTANCE, PTSTR pCmdLine, int nCmdShow )
{
	MainWindow main;

	if( !main.Create(_T("main"), WS_OVERLAPPEDWINDOW) ){
		return 0;
	}

	ShowWindow( (HWND)main, nCmdShow );

	MSG msg;
	while( GetMessage(&msg,NULL,0,0) ){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

