// Wnd.h 
#pragma once

#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS>
class TWnd {
protected:
	HWND			m_hWnd;
	
	virtual LPCTSTR		ClassName( void ) const				= 0;
	virtual LRESULT		WindowProc( UINT, WPARAM, LPARAM )	= 0;

	virtual DWORD		ClassStyle( void ) const {
		return 0;
	}
	
	virtual DWORD		ClassExStyle( void ) const {
		return 0;
	}
	
	virtual DWORD		ClassWndExStyle( void ) const {
		return 0;
	}
	
	virtual DWORD		DefaultStyle( void ) const {
		return 0;
	}
	
	virtual DWORD		DefaultExStyle( void ) const {
		return 0;
	}
	
public:
	operator HWND(){ return(m_hWnd); }
	
public:
	TWnd(void) : m_hWnd(NULL) {}
	
	BOOL	Create(	LPCTSTR	lpWindowName
				,	DWORD		dwStyle
				,	DWORD		dwExStyle	= 0
				,	int			nX			= CW_USEDEFAULT
				,	int			nY			= CW_USEDEFAULT
				,	int			nWidth		= CW_USEDEFAULT
				,	int			nHeight		= CW_USEDEFAULT
				,	HWND		hParentWnd	= NULL
				,	HMENU		hMenu		= NULL	
	){
		WNDCLASSEX		wc = {0};
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= ClassStyle();
		wc.cbClsExtra		= ClassExStyle();
		wc.cbWndExtra		= ClassWndExStyle();
		
		wc.hIcon			= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
		wc.hIconSm			= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
		wc.hCursor			= (HCURSOR)	LoadImage( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
		wc.hbrBackground	= (HBRUSH)	GetStockObject( WHITE_BRUSH );
		
		wc.lpfnWndProc		= CLASS::__WindowProc__;
		wc.hInstance		= GetModuleHandle( NULL );
		wc.lpszClassName	= ClassName();
		
		if( RegisterClassEx(&wc) == 0 ){
			return(FALSE);
		}
		
		m_hWnd	= CreateWindowEx(	dwExStyle ? dwExStyle : DefaultExStyle()
								, 	ClassName()
								,	lpWindowName
								,	dwStyle ? dwStyle : DefaultStyle()
								,	nX, nY, nWidth, nHeight
								,	hParentWnd
								,	hMenu
								,	GetModuleHandle(NULL)
								,	this
								);
		return(m_hWnd ? TRUE : FALSE);
	}
	
	static
	LRESULT	CALLBACK	__WindowProc__( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		CLASS*	pThis = NULL;
		
		if( uMsg == WM_NCCREATE )
		{
			LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
			pThis	= (CLASS*)pCreateStruct->lpCreateParams;
			
			SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pThis );
			
			pThis->m_hWnd	= hWnd;
		}
		else
		{
			pThis	= (CLASS*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		}
		
		if( pThis ){
			return(pThis->WindowProc(uMsg,wParam,lParam));
		}else{
			return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		}
	}
};
	
/**********************************************************************************
 *
 *
 *
 */
class CWnd : public TWnd<CWnd> {
public:	
	virtual LPCTSTR		ClassName(void) const {
		return(_T("Wnd"));
	}
	
	virtual LRESULT		WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam){
		if( uMsg == WM_DESTROY ){
			PostQuitMessage( 0 );
			return 0;
		}
		return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
	}
	
	virtual int			MainLoop( void ){
		ShowWindow( m_hWnd, SW_SHOW );
		UpdateWindow( m_hWnd );
		
		MSG msg;
		while( GetMessage(&msg,NULL,0,0) ){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		return (int)msg.wParam;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CChildWnd : public TWnd<CChildWnd> {
protected:
	virtual LPCTSTR		ClassName( void ) const {
		return(_T("child window"));
	}
	virtual DWORD		DefaultStyle( void ) const {
		return(WS_CHILDWINDOW);
	}
	/*
	BOOL	Create(	LPCTSTR	lpWindowName
				,	DWORD		dwStyle
				,	DWORD		dwExStyle	= 0
				,	int			nX			= CW_USEDEFAULT
				,	int			nY			= CW_USEDEFAULT
				,	int			nWidth		= CW_USEDEFAULT
				,	int			nHeight		= CW_USEDEFAULT
				,	HWND		hParentWnd	= NULL
				,	HMENU		hMenu		= NULL	
	 */
		
public:
	virtual BOOL		Create( LPCTSTR lpszCaption, HWND hParentWnd, int nX, int nY, int nWidth, int nHeight ){
		return(TWnd<CChildWnd>::Create(lpszCaption, DefaultStyle(), 0, nX, nY, nWidth, nHeight, hParentWnd));
	}
		
	virtual LRESULT		WindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam){
		if( uMsg == WM_DESTROY ){
			PostQuitMessage( 0 );
			return 0;
		}
		else if( uMsg == WM_COMMAND )
		{
			if( HIWORD(wParam) == CBN_SELCHANGE )
			{
				MessageBox( NULL, _T("CBN_SELCHANGE"), _T("OK"), MB_OK );
			}
			else
				MessageBox( NULL, _T("É{É^ÉìÇ™âüÇ≥ÇÍÇ‹ÇµÇΩÅB"), _T("OK"), MB_OK );
		}

		return DefWindowProc(m_hWnd,uMsg,wParam,lParam);
	}
};

