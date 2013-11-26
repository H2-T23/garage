// Wnd.h 
#pragma once

#include <windows.h>
#include <tchar.h>
#include <windowsx.h>


#include <iostream>
#include <vector>
#include <list>

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

/**********************************************************************************
 *
 *
 *
 */
class CWnd {
public:
	HWND	m_hWnd;

public:
	CWnd(void):m_hWnd(NULL){}
	CWnd(const HWND hWnd):m_hWnd(hWnd){}
	CWnd(CWnd const& rWnd):m_hWnd(rWnd.m_hWnd){}
	virtual ~CWnd( void ) {}
	
	HWND	operator=(const HWND hWnd){
		m_hWnd = hWnd;
		return(m_hWnd);
	}
	CWnd& 	operator=(CWnd const& rWnd){
		m_hWnd = rWnd.m_hWnd;
		return(*this);	
	}
	operator HWND(){return(m_hWnd);}
	
public:
	BOOL	GetWindowRect( LPRECT lpRect ) const {
		return ::GetWindowRect(m_hWnd, lpRect);
	}
	
	HWND	GetParent( void ) const {
		return ::GetParent(m_hWnd);
	}
	
public:
	virtual BOOL	OnCreate( LPCREATESTRUCT lpCrreateStruct ){ return FALSE; }
	virtual void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){}
	virtual void	OnDestroy( void ){}
	virtual void	OnMove( int x, int y){}
	virtual void	OnPaint( void ){}
	virtual void	OnEraseBkgnd( HDC hDC ){}
	virtual void	OnSetFocus( HWND hWndOldFocus ){}
	virtual void	OnKillFocus( HWND hWndNewFocus ){}
	
	virtual void	OnKeyDown( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
	virtual void	OnKeyUp( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
	virtual void	OnChar( TCHAR ch, int nRepeat ){}
	virtual void	OnSysKeyDown( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
	virtual void	OnSysKeyUp( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
	virtual void	OnSysChar( TCHAR ch, int nRepeat ){}
	
};

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS>
class TWnd : public CWnd {
protected:
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
	TWnd(void)
	{}
	
	
	
	BOOL	Create(	LPCTSTR	lpWindowName
				,	DWORD		dwStyle		= 0
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
class CWindow : public TWnd<CWindow> {
	friend class TWnd<CWindow>;

private:
	BOOL	Cls_OnCreate( HWND hWnd, LPCREATESTRUCT lpCreateStruct ){
		return OnCreate( lpCreateStruct );
	}
	
	void	Cls_OnCommand( HWND hWnd, int id, HWND hWndCtrl, UINT nCodeNotify ){
		OnCommand( id, hWndCtrl, nCodeNotify );
	}
	
	void	Cls_OnDestroy( HWND hWnd ){
		OnDestroy();
		PostQuitMessage( 0 );
	}

	void	Cls_OnKeyDown( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){
		OnKeyDown(uVk, fDown, nRepeat, uFlags);
	}
	void	Cls_OnKeyUp( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){
		OnKeyUp(uVk, fDown, nRepeat, uFlags);
	}
	void	Cls_OnChar( HWND hWnd, TCHAR ch, int nRepeat ){
		OnChar(ch, nRepeat);
	}
	void	Cls_OnSysKeyDown( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){
		OnSysKeyDown(uVk, fDown, nRepeat, uFlags);
	}
	void	Cls_OnSysKeyUp( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){
		OnSysKeyUp(uVk, fDown, nRepeat, uFlags);
	}
	void	Cls_OnSysChar( HWND hWnd, TCHAR ch, int nRepeat ){
		OnSysChar(ch, nRepeat);
	}

protected:
	virtual LRESULT		WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		switch(uMsg){
			HANDLE_MSG(m_hWnd,WM_CREATE		, Cls_OnCreate		);
			HANDLE_MSG(m_hWnd,WM_COMMAND	, Cls_OnCommand		);
			HANDLE_MSG(m_hWnd,WM_DESTROY	, Cls_OnDestroy		);
			HANDLE_MSG(m_hWnd,WM_KEYDOWN	, Cls_OnKeyDown		);
			HANDLE_MSG(m_hWnd,WM_KEYUP		, Cls_OnKeyUp		);
			HANDLE_MSG(m_hWnd,WM_CHAR		, Cls_OnChar		);
			HANDLE_MSG(m_hWnd,WM_SYSKEYDOWN	, Cls_OnSysKeyDown	);
			HANDLE_MSG(m_hWnd,WM_SYSKEYUP	, Cls_OnSysKeyUp	);
			HANDLE_MSG(m_hWnd,WM_SYSCHAR	, Cls_OnSysChar		);
		default:
			break;
		}
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CFrameWnd : public CWindow
{
public:
	CFrameWnd(void) {}
	virtual ~CFrameWnd(void) {}
	
	int	MainLoop( void ) const {
		ShowWindow( m_hWnd, SW_SHOW );
		UpdateWindow( m_hWnd );
		
		MSG msg;
		while( GetMessage(&msg,NULL,0,0) ){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		return (int)msg.wParam;		
	}
	
protected:
	virtual LPCTSTR		ClassName( void ) const	{
		return(_T("Frame"));
	}

	virtual DWORD		DefaultStyle( void ) const {
		return(WS_OVERLAPPEDWINDOW);
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CPanelWnd : public CWindow
{
public:
	CPanelWnd(void) {}
	virtual ~CPanelWnd(void) {}
	
protected:
	virtual LPCTSTR		ClassName( void ) const	{
		return(_T("Panel"));
	}

	virtual DWORD		DefaultStyle( void ) const {
		return(WS_CHILDWINDOW);
	}

protected:
	std::vector<CWnd*>		m_vecWnd;
	
public:
	void		Append( CWnd* pWnd ){
		m_vecWnd.push_back( pWnd );
	}
};

