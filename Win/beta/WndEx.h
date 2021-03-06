#pragma once
#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

#include <windows.h>
#include <tchar.h>
#include <windowsx.h>

#include <string>
#include <vector>
#include <list>
#include <map>

#include "Rect.h"

/**********************************************************************************
 *
 *
 *
 */
class CWnd {
public:
	HWND				m_hWnd;

protected:
    virtual DWORD				ClassStyle( void ) const		{return 0;}
	virtual DWORD				ClassExStyle( void ) const		{return 0;}
    virtual DWORD				ClassWndExStyle( void ) const	{return 0;}
    virtual DWORD				WindowStyle( void ) const		{return 0;}
    virtual DWORD				WindowExStyle( void ) const		{return 0;}

public:
	CWnd(void):m_hWnd(NULL){}
	CWnd(const HWND hWnd):m_hWnd(hWnd){}
	CWnd(CWnd const& rWnd):m_hWnd(rWnd.m_hWnd){}
	virtual ~CWnd( void ) {}

	HWND        operator=(const HWND hWnd){
			m_hWnd = hWnd;
			return(m_hWnd);
	}

	CWnd&         operator=(CWnd const& rWnd){
			m_hWnd = rWnd.m_hWnd;
			return(*this);        
	}

	operator HWND(){return(m_hWnd);}

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
    virtual BOOL        OnCreate( LPCREATESTRUCT lpCrreateStruct ){ return TRUE; }
    virtual void        OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){}
    virtual void        OnDestroy( void ){}

    virtual void        OnMove( int x, int y){}

	virtual void        OnPaint( void ){}
    virtual void        OnEraseBkgnd( HDC hDC ){}

	virtual void        OnSetFocus( HWND hWndOldFocus ){}
    virtual void        OnKillFocus( HWND hWndNewFocus ){}
    
    virtual void        OnKeyDown( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
    virtual void        OnKeyUp( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
    virtual void        OnChar( TCHAR ch, int nRepeat ){}
    virtual void        OnSysKeyDown( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
    virtual void        OnSysKeyUp( UINT uVk, BOOL fDown, int nRepeat, UINT uFlags ){}
    virtual void        OnSysChar( TCHAR ch, int nRepeat ){}

	virtual void		OnNotify( WPARAM wParam, LPARAM lParam )	{	}

	virtual LRESULT		WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		switch(uMsg){
            HANDLE_MSG(m_hWnd,	WM_CREATE			, Cls_OnCreate				);
            HANDLE_MSG(m_hWnd,	WM_COMMAND			, Cls_OnCommand				);
            HANDLE_MSG(m_hWnd,	WM_DESTROY			, Cls_OnDestroy				);
            HANDLE_MSG(m_hWnd,	WM_KEYDOWN			, Cls_OnKeyDown				);
            HANDLE_MSG(m_hWnd,	WM_KEYUP			, Cls_OnKeyUp				);
            HANDLE_MSG(m_hWnd,	WM_CHAR				, Cls_OnChar				);
            HANDLE_MSG(m_hWnd,	WM_SYSKEYDOWN		, Cls_OnSysKeyDown			);
            HANDLE_MSG(m_hWnd,	WM_SYSKEYUP			, Cls_OnSysKeyUp			);
            HANDLE_MSG(m_hWnd,	WM_SYSCHAR			, Cls_OnSysChar				);
		default:
			break;
		}
		if( WM_NOTIFY == uMsg ){
			Cls_OnNotify(m_hWnd, wParam, lParam);
			return 0;
		}
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

private:
    BOOL        Cls_OnCreate( HWND hWnd, LPCREATESTRUCT lpCreateStruct ){                return OnCreate( lpCreateStruct );			}
    void        Cls_OnCommand( HWND hWnd, int id, HWND hWndCtrl, UINT nCodeNotify ){	OnCommand( id, hWndCtrl, nCodeNotify );		}
    void        Cls_OnDestroy( HWND hWnd ){
		OnDestroy();
		PostQuitMessage( 0 );
    }
    void        Cls_OnKeyDown	( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnKeyDown(uVk, fDown, nRepeat, uFlags);		}
    void        Cls_OnKeyUp		( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnKeyUp(uVk, fDown, nRepeat, uFlags);		}
    void        Cls_OnChar		( HWND hWnd, TCHAR ch, int nRepeat)								{	OnChar(ch, nRepeat);						}
    void        Cls_OnSysKeyDown( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnSysKeyDown(uVk, fDown, nRepeat, uFlags);	}
    void        Cls_OnSysKeyUp	( HWND hWnd, UINT uVk, BOOL fDown, int nRepeat, UINT uFlags )	{	OnSysKeyUp(uVk, fDown, nRepeat, uFlags);	}
    void        Cls_OnSysChar	( HWND hWnd, TCHAR ch, int nRepeat )							{	OnSysChar(ch, nRepeat);						}

	void		Cls_OnNotify	( HWND hWnd, WPARAM wParam, LPARAM lParam )						{	OnNotify(wParam, lParam);					}
};

/**********************************************************************************
 *
 *
 *
 */
template<class CLASS>
class TWnd : public CWnd {
protected:
	virtual LPCTSTR				ClassName( void ) const					= 0;
//	virtual LRESULT				WindowProc( UINT, WPARAM, LPARAM )		= 0;

public:
    TWnd(void)
    {}

	BOOL	Create(	LPCTSTR		lpWindowName
				,	DWORD		dwStyle				= 0
				,	DWORD		dwExStyle			= 0
				,	int			nX					= CW_USEDEFAULT
				,	int			nY					= CW_USEDEFAULT
				,	int			nWidth				= CW_USEDEFAULT
				,	int			nHeight				= CW_USEDEFAULT
				,	HWND		hParentWnd			= NULL
				,	HMENU		hMenu				= NULL        
				)
	{
        WNDCLASSEX	wc = {0};
        wc.cbSize				= sizeof(WNDCLASSEX);
        wc.style				= ClassStyle();
        wc.cbClsExtra			= ClassExStyle();
        wc.cbWndExtra			= ClassWndExStyle();
        
        wc.hIcon				= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
        wc.hIconSm				= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
        wc.hCursor				= (HCURSOR)	LoadImage( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
        wc.hbrBackground        = (HBRUSH)	GetStockObject( WHITE_BRUSH );
        
        wc.lpfnWndProc			= CLASS::__WindowProc__;
        wc.hInstance			= GetModuleHandle( NULL );
        wc.lpszClassName        = ClassName();
        
        if( RegisterClassEx(&wc) == 0 ){
		//	return(FALSE);
        }
        
        m_hWnd		= CreateWindowEx
						(	dwExStyle ? dwExStyle : WindowExStyle()
						,	ClassName()
						,	lpWindowName
						,	dwStyle ? dwStyle : WindowStyle()
						,	nX, nY, nWidth, nHeight
						,	hParentWnd
						,	hMenu
						,	GetModuleHandle(NULL)
						,	this
						);
        return(m_hWnd ? TRUE : FALSE);
    }
    
    static	LRESULT        CALLBACK        __WindowProc__( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
		CLASS*        pThis = NULL;
		if( uMsg == WM_NCCREATE )
		{
			LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lParam;
			pThis        = (CLASS*)pCreateStruct->lpCreateParams;
	        
			SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)pThis );
	        
			pThis->m_hWnd        = hWnd;
		}
		else
		{
			pThis        = (CLASS*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
		}

		if( pThis ){
			return(pThis->WindowProc(uMsg,wParam,lParam));
		}else{
			return(DefWindowProc(hWnd, uMsg, wParam, lParam));
		}
    }
};
