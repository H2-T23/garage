#pragma once
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

#include <SDKDDKVer.h>

#define	WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>

#include <windowsx.h>

#include <stdio.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

#include "Types.h"
#include "DC.h"

/****************************************************************************************
 *
 *
 *
 */
class CWnd {
public:
	HWND		m_hWnd;

public:
	CWnd( void ) : m_hWnd(NULL) {}
	CWnd( CWnd const& wnd ) : m_hWnd(wnd.m_hWnd) {}

public:
	CWnd&	operator=(CWnd const& wnd){
		m_hWnd = wnd.m_hWnd;
		return(*this);
	}

	operator HWND(){return(m_hWnd);}

protected:
    virtual DWORD				ClassStyle( void ) const		{return CS_DBLCLKS;}
	virtual DWORD				ClassExStyle( void ) const		{return 0;}
    virtual DWORD				ClassWndExStyle( void ) const	{return 0;}
    virtual DWORD				WindowStyle( void ) const		{return 0;}
    virtual DWORD				WindowExStyle( void ) const		{return 0;}

public:
	BOOL		GetClientRect( LPRECT lpRect ) const							{ return ::GetClientRect(m_hWnd,lpRect);				}
	BOOL        GetWindowRect( LPRECT lpRect ) const							{ return ::GetWindowRect(m_hWnd, lpRect);				}
	HWND        GetParent( void ) const											{ return ::GetParent(m_hWnd);							}
	BOOL		ShowWindow( int nCmdShow ) const								{ return ::ShowWindow(m_hWnd, nCmdShow);				}
	BOOL		UpdateWindow( void ) const										{ return ::UpdateWindow(m_hWnd);						}
	BOOL		PostMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) const	{ return ::PostMessage(m_hWnd, uMsg, wParam, lParam);	}
	LRESULT		SendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam ) const	{ return ::SendMessage(m_hWnd, uMsg, wParam, lParam);	}

	BOOL		ScreenToClient( LPPOINT lpPoint ) const							{ return ::ScreenToClient(m_hWnd, lpPoint);				}

public:
	virtual BOOL	OnCreate( LPCREATESTRUCT lpCrreateStruct ){ return TRUE; }
	virtual void	OnDestroy( void )	{	}

	virtual void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify )	{	}
	virtual void	OnNotify( WPARAM wParam, LPARAM lParam )				{	}
	virtual void	OnTimer( UINT )	{	}	
	virtual void	OnPaint( HDC )	{	}
	virtual BOOL	OnEraseBkgnd( HDC hDC ){ return TRUE; }

	virtual void	OnMove( int x, int y)	{	}
	virtual void	OnSize( UINT state, int cx, int cy )	{	}
	virtual void	OnClose( void )	{	}
	// focus.s
	virtual void	OnSetFocus	( HWND hWndOldFocus )	{	}
	virtual void	OnKillFocus	( HWND hWndNewFocus )	{	}
	// keyboard message.
	virtual void	OnKeyDown	( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	}
	virtual void	OnKeyUp		( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	}
	virtual void	OnChar		( TCHAR ch, int nRepeat )							{	}
	virtual void	OnSysKeyDown( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	}
	virtual void	OnSysKeyUp	( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	}
	virtual void	OnSysChar	( TCHAR ch, int nRepeat )							{	}	
	// mouse message.
	virtual void	OnMouseMove		( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnLButtonDown	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnLButtonDblclk	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnLButtonUp		( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnRButtonDown	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnRButtonDblclk	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnRButtonUp		( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnMButtonDown	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnMButtonDblclk	( int x, int y, UINT KeyFlags )	{	}
	virtual void	OnMButtonUp		( int x, int y, UINT KeyFlags )	{	}
	// menu message.
	virtual void	OnInitMenu		( HMENU hMenu )											{	}
	virtual void	OnInitMenuPopup	( HMENU hMenu, UINT item, BOOL fSystemMenu )			{	}
	virtual void	OnMenuSelect	( HMENU hMenu, int item, HMENU hMenuPopup, UINT flags )	{	}
	virtual DWORD	OnMenuChar		( HMENU hMenu, UINT ch, UINT flags )					{	return 0;	}
	// scroll message.
	virtual void	OnHScroll		( HWND hWndCtl, UINT code, int pos )	{	}
	virtual	void	OnVScroll		( HWND hWndCtl, UINT code, int pos )	{	}

private:
	BOOL	Cls_OnCreate( HWND hWnd, LPCREATESTRUCT lpCreateStruct ){
		return OnCreate( lpCreateStruct );
	}

	void	Cls_OnDestroy( HWND hWnd ){
		OnDestroy();
		PostQuitMessage( 0 );
	}
	
	void	Cls_OnCommand( HWND hWnd, int id, HWND hWndCtrl, UINT nCodeNotify )	{	OnCommand(id, hWndCtrl, nCodeNotify);	}
	void	Cls_OnNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )				{	OnNotify(wParam, lParam);				}
	void	Cls_OnTimer( HWND hWnd, UINT id )									{	OnTimer(id);							}

	// paint message.
	void	Cls_OnPaint( HWND hWnd ){
		PAINTSTRUCT		ps;
		HDC hDC = ::BeginPaint( m_hWnd, &ps );
		OnPaint( hDC );
		::EndPaint( m_hWnd, &ps );
	}
	// 
	BOOL	Cls_OnEraseBkgnd( HWND hWnd, HDC hDC ){	return OnEraseBkgnd( hDC );	}

	void	Cls_OnMove( HWND hWnd, int x, int y)				{	OnMove(x,y);				}
	void	Cls_OnSize( HWND hWnd, UINT state, int cx, int cy )	{	OnSize(state, cx, cy);		}
	void	Cls_OnClose( HWND hWnd )							{	OnClose();					}
	// focus message.
	void	Cls_OnSetFocus	( HWND hWnd, HWND hWndOldFocus )		{	OnSetFocus(hWndOldFocus);	}
	void	Cls_OnKillFocus	( HWND hWnd, HWND hWndNewFocus )		{	OnKillFocus(hWndNewFocus);	}
	// keyboard message.
	void	Cls_OnKeyDown		( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnKeyDown(uVk, fDown, nRepeat, uFlags);		}
	void	Cls_OnKeyUp			( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnKeyUp(uVk, fDown, nRepeat, uFlags);		}
	void	Cls_OnChar			( HWND hWnd, TCHAR ch,             int nRepeat )				{	OnChar(ch, nRepeat);						}
	void	Cls_OnSysKeyDown	( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnSysKeyDown(uVk, fDown, nRepeat, uFlags);	}
	void	Cls_OnSysKeyUp		( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnSysKeyUp(uVk, fDown, nRepeat, uFlags);	}
	void	Cls_OnSysChar		( HWND hWnd, TCHAR ch,             int nRepeat )				{	OnSysChar(ch, nRepeat);						}
	// mouse message.
	void	Cls_OnMouseMove		( HWND hWnd,                    int x, int y, UINT KeyFlags )	{	OnMouseMove(x, y, KeyFlags);		}
	void	Cls_OnLButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnLButtonDown(x, y, KeyFlags);		}
	void	Cls_OnLButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnLButtonDblclk(x, y, KeyFlags);	}
	void	Cls_OnLButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlags )	{	OnLButtonUp(x, y, KeyFlags);		}
	void	Cls_OnRButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnRButtonDown(x, y, KeyFlags);		}
	void	Cls_OnRButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnRButtonDblclk(x, y, KeyFlags);	}
	void	Cls_OnRButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlags )	{	OnRButtonUp(x, y, KeyFlags);		}
	void	Cls_OnMButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnMButtonDown(x, y, KeyFlags);		}
	void	Cls_OnMButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlags )	{	OnMButtonDblclk(x, y, KeyFlags);	}
	void	Cls_OnMButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlags )	{	OnMButtonUp(x, y, KeyFlags);		}
	void	Cls_OnMouseWheel	( HWND hWnd, int xPos, int yPos, int zDelta, UINT fwKeys )		{		}
	// menu message.
	void	Cls_OnInitMenu		( HWND hWnd, HMENU hMenu )											{	OnInitMenu(hMenu);								}
	void	Cls_OnInitMenuPopup	( HWND hWnd, HMENU hMenu, UINT item, BOOL fSystemMenu )				{	OnInitMenuPopup(hMenu, item, fSystemMenu);		}
	void	Cls_OnMenuSelect	( HWND hWnd, HMENU hMenu, int item, HMENU hMenuPopup, UINT flags )	{	OnMenuSelect(hMenu, item, hMenuPopup, flags);	}
	DWORD	Cls_OnMenuChar		( HWND hwnd, UINT ch, UINT flags, HMENU hMenu )						{	return OnMenuChar(hMenu, ch, flags);			}
	// scroll message.
	void	Cls_OnHScroll		( HWND hWnd, HWND hWndCtl, UINT code, int pos )	{	OnHScroll(hWndCtl, code, pos);	}
	void	Cls_OnVScroll		( HWND hWnd, HWND hWndCtl, UINT code, int pos )	{	OnVScroll(hWndCtl, code, pos);	}
public:
	virtual LRESULT		WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		switch(uMsg){
			HANDLE_MSG(m_hWnd,	WM_CREATE			, Cls_OnCreate			);
			HANDLE_MSG(m_hWnd,	WM_DESTROY			, Cls_OnDestroy			);
				
			HANDLE_MSG(m_hWnd,	WM_COMMAND			, Cls_OnCommand			);
			HANDLE_MSG(m_hWnd,	WM_TIMER			, Cls_OnTimer			);

			HANDLE_MSG(m_hWnd,	WM_PAINT			, Cls_OnPaint			);
		//	HANDLE_MSG(m_hWnd,	WM_ERASEBKGND		, Cls_OnEraseBkgnd		);
				
			HANDLE_MSG(m_hWnd,	WM_MOVE				, Cls_OnMove			);
			HANDLE_MSG(m_hWnd,	WM_SIZE				, Cls_OnSize			);
		//	HANDLE_MSG(m_hWnd,	WM_CLOSE			, Cls_OnClose			);

			HANDLE_MSG(m_hWnd,	WM_SETFOCUS			, Cls_OnSetFocus		);
			HANDLE_MSG(m_hWnd,	WM_KILLFOCUS		, Cls_OnKillFocus		);
			// keyboard message.
			HANDLE_MSG(m_hWnd,	WM_KEYDOWN			, Cls_OnKeyDown			);
			HANDLE_MSG(m_hWnd,	WM_KEYUP			, Cls_OnKeyUp			);
			HANDLE_MSG(m_hWnd,	WM_CHAR				, Cls_OnChar			);
			HANDLE_MSG(m_hWnd,	WM_SYSKEYDOWN		, Cls_OnSysKeyDown		);
			HANDLE_MSG(m_hWnd,	WM_SYSKEYUP			, Cls_OnSysKeyUp		);
			HANDLE_MSG(m_hWnd,	WM_SYSCHAR			, Cls_OnSysChar			);
			// mouse message.
			HANDLE_MSG(m_hWnd,	WM_MOUSEMOVE		, Cls_OnMouseMove		);
			HANDLE_MSG(m_hWnd,	WM_LBUTTONDOWN		, Cls_OnLButtonDown		);
			HANDLE_MSG(m_hWnd,	WM_LBUTTONDBLCLK	, Cls_OnLButtonDblclk	);
			HANDLE_MSG(m_hWnd,	WM_LBUTTONUP		, Cls_OnLButtonUp		);
			HANDLE_MSG(m_hWnd,	WM_RBUTTONDOWN		, Cls_OnRButtonDown		);
			HANDLE_MSG(m_hWnd,	WM_RBUTTONDBLCLK	, Cls_OnRButtonDblclk	);
			HANDLE_MSG(m_hWnd,	WM_RBUTTONUP		, Cls_OnRButtonUp		);
			HANDLE_MSG(m_hWnd,	WM_MBUTTONDOWN		, Cls_OnMButtonDown		);
			HANDLE_MSG(m_hWnd,	WM_MBUTTONDBLCLK	, Cls_OnMButtonDblclk	);
			HANDLE_MSG(m_hWnd,	WM_MBUTTONUP		, Cls_OnMButtonUp		);
			HANDLE_MSG(m_hWnd,	WM_MOUSEWHEEL		, Cls_OnMouseWheel		);
			// menu message.
		//	HANDLE_MSG(m_hWnd,	WM_INITMENU			, Cls_OnInitMenu		);
		//	HANDLE_MSG(m_hWnd,	WM_INITMENUPOPUP	, Cls_OnInitMenuPopup	);
		//	HANDLE_MSG(m_hWnd,	WM_MENUSELECT		, Cls_OnMenuSelect		);
		//	HANDLE_MSG(m_hWnd,	WM_MENUCHAR			, Cls_OnMenuChar		);
			// scroll message.
			HANDLE_MSG(m_hWnd,	WM_HSCROLL			, Cls_OnHScroll			);
			HANDLE_MSG(m_hWnd,	WM_VSCROLL			, Cls_OnVScroll			);
		default:
			break;
		}
		if( WM_NOTIFY == uMsg ){
			Cls_OnNotify(m_hWnd, wParam, lParam);
			return 0;
		}
		return ::DefWindowProcA(m_hWnd, uMsg, wParam, lParam);
	}
};


/****************************************************************************************
 *
 *
 *
 */
template<typename CLASS>
class TWnd : public CWnd {
protected:
	virtual LPCTSTR			ClassName( void ) const = 0;

public:
	BOOL	Create(	LPCTSTR		lpWindowName
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
		
		wc.lpfnWndProc		= CLASS::MainWindowProc;
		wc.hInstance		= GetModuleHandle( NULL );
		wc.lpszClassName	= ClassName();
		
		if( RegisterClassEx(&wc) == 0 ){
		//	return(FALSE);
		}

		dwExStyle	= (dwExStyle ? dwExStyle : WindowExStyle());
		dwStyle		= (dwStyle ? dwStyle : WindowStyle());
		
		m_hWnd	= ::CreateWindowEx(	dwExStyle
								, 	ClassName()
								,	lpWindowName
								,	dwStyle
								,	nX, nY, nWidth, nHeight
								,	hParentWnd
								,	hMenu
								,	GetModuleHandle(NULL)
								,	this
								);
		return(m_hWnd ? TRUE : FALSE);
	}

private:
	static
	LRESULT CALLBACK		MainWindowProc(	HWND	hWnd
										,	UINT	uMsg
										,	WPARAM	wParam
										,	LPARAM	lParam
										)
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
