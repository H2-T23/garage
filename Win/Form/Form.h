#pragma once

#include <windowsx.h>

#include <vector>
#include <list>


namespace WindowsForm {

	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CRect : public RECT {
	public:
		CRect( void ){
			Set(0,0,0,0);
		}
		CRect( int x, int y, int w, int h ){
			Set(x,y,w,h);
		}

		int		Width( void ) const { return(right-left); }
		int		Height( void ) const { return(bottom-top); }

		void	Set( int x, int y, int w, int h ){
			top = x;
			left= y;
			bottom	= top+h;
			right	= left+w;
		}
	};


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

	public:
		virtual BOOL	OnCreate( LPCREATESTRUCT lpCrreateStruct ){ return TRUE; }
		virtual void	OnDestroy( void )	{	}

		virtual void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify )	{	}
		virtual void	OnTimer( UINT )	{	}	
		virtual void	OnPaint( HDC )	{	}
		virtual BOOL	OnEraseBkgnd( HDC hDC ){ return TRUE; }

		virtual void	OnMove( int x, int y)	{	}
		virtual void	OnSize( UINT state, int cx, int cy )	{	}
		virtual void	OnClose( void )	{	}

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
		virtual void	OnLButtonDown	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnLButtonDblclk	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnLButtonUp		(                    int x, int y, UINT KeyFlag )	{	}
		virtual void	OnRButtonDown	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnRButtonDblclk	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnRButtonUp		(                    int x, int y, UINT KeyFlag )	{	}
		virtual void	OnMButtonDown	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnMButtonDblclk	( BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	}
		virtual void	OnMButtonUp		(                    int x, int y, UINT KeyFlag )	{	}

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

		void	Cls_OnTimer( HWND hWnd, UINT id )	{ OnTimer(id);	}

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
		void	Cls_OnMouseMove		( HWND hWnd,                    int x, int y, UINT KeyFlag )	{	OnMouseMove(x, y, KeyFlag);						}
		void	Cls_OnLButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnLButtonDown(fDoubleClick, x, y, KeyFlag);		}
		void	Cls_OnLButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnLButtonDblclk(fDoubleClick,x, y, KeyFlag);	}
		void	Cls_OnLButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlag )	{	OnLButtonUp(x, y, KeyFlag);						}
		void	Cls_OnRButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnRButtonDown(fDoubleClick, x, y, KeyFlag);		}
		void	Cls_OnRButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnRButtonDblclk(fDoubleClick, x, y, KeyFlag);	}
		void	Cls_OnRButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlag )	{	OnRButtonUp(x, y, KeyFlag);						}
		void	Cls_OnMButtonDown	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnMButtonDown(fDoubleClick, x, y, KeyFlag);		}
		void	Cls_OnMButtonDblclk	( HWND hWnd, BOOL fDoubleClick, int x, int y, UINT KeyFlag )	{	OnMButtonDblclk(fDoubleClick, x, y, KeyFlag);	}
		void	Cls_OnMButtonUp		( HWND hWnd,                    int x, int y, UINT KeyFlag )	{	OnMButtonUp(x, y, KeyFlag);						}
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
		virtual LRESULT		WinProc( UINT uMsg, WPARAM wParam, LPARAM lParam ){
			switch(uMsg){
				HANDLE_MSG(m_hWnd,	WM_CREATE			, Cls_OnCreate			);
				HANDLE_MSG(m_hWnd,	WM_DESTROY			, Cls_OnDestroy			);
				
				HANDLE_MSG(m_hWnd,	WM_COMMAND			, Cls_OnCommand			);
				HANDLE_MSG(m_hWnd,	WM_TIMER			, Cls_OnTimer			);

				HANDLE_MSG(m_hWnd,	WM_PAINT			, Cls_OnPaint			);
				HANDLE_MSG(m_hWnd,	WM_ERASEBKGND		, Cls_OnEraseBkgnd		);
				
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
				HANDLE_MSG(m_hWnd,	WM_INITMENU			, Cls_OnInitMenu		);
				HANDLE_MSG(m_hWnd,	WM_INITMENUPOPUP	, Cls_OnInitMenuPopup	);
				HANDLE_MSG(m_hWnd,	WM_MENUSELECT		, Cls_OnMenuSelect		);
				HANDLE_MSG(m_hWnd,	WM_MENUCHAR			, Cls_OnMenuChar		);

				// scroll message.
				HANDLE_MSG(m_hWnd,	WM_HSCROLL			, Cls_OnHScroll			);
				HANDLE_MSG(m_hWnd,	WM_VSCROLL			, Cls_OnVScroll			);
			default:
				break;
			}
			return ::DefWindowProcA(m_hWnd, uMsg, wParam, lParam);
		}

	public:
		virtual DWORD			ClassStyle( void ) const { return CS_HREDRAW | CS_VREDRAW; }
		virtual DWORD			ClsExtra( void ) const { return 0; }
		virtual DWORD			WndExtra( void ) const { return 0; }
		virtual DWORD			WindowStyle( void ) const { return 0; }
		virtual DWORD			WindowExStyle( void ) const { return 0; }


	public:
		BOOL			ShowWindow( int nCmdShow ){
			return ::ShowWindow(m_hWnd, nCmdShow);
		}
		BOOL			UpdateWindow( void ){
			return ::UpdateWindow(m_hWnd);
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
		wc.cbClsExtra		= ClsExtra();
		wc.cbWndExtra		= WndExtra();
		
		wc.hIcon			= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
		wc.hIconSm			= (HICON)	LoadImage( NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_SHARED );
		wc.hCursor			= (HCURSOR)	LoadImage( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED );
		wc.hbrBackground	= (HBRUSH)	GetStockObject( WHITE_BRUSH );
		
		wc.lpfnWndProc		= CLASS::WindowProc;
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

	public:static
		LRESULT CALLBACK		WindowProc(	HWND	hWnd
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
				return(pThis->WinProc(uMsg,wParam,lParam));
			}else{
				return(DefWindowProc(hWnd, uMsg, wParam, lParam));
			}
		}
	};


	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CForm : public TWnd<CForm> {
	protected:
		virtual LPCTSTR		ClassName( void ) const { return _T("Form"); }
		virtual DWORD		WindowStyle( void ) const { return(WS_OVERLAPPEDWINDOW); }

	public:
		int		MessageLoop( void ){
			MSG	msg;
			while( ::GetMessage(&msg, NULL, 0, 0) ){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			return int(msg.wParam);
		}
	};

	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CPanel : public TWnd<CPanel> {
	protected:
		virtual LPCTSTR		ClassName( void ) const { return(_T("Panel")); }
		virtual DWORD		WindowStyle( void ) const { return(WS_CHILDWINDOW); }

		std::list<CWnd*>		listWnd;

	public:
		BOOL		Create( CWnd* pParent, CRect& rect ){
			return TWnd<CPanel>::Create(NULL,0,0,rect.top,rect.left,rect.Width(),rect.Height(),(HWND)(*pParent),0);
		}

		void		Append( CWnd* pChild ){
			listWnd.push_back( pChild );
		}

		void		Remove( CWnd* pChild ){
			listWnd.remove( pChild );
		}
	};


	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CButton : public CWnd {
	protected:
		virtual DWORD	WindowStyle( void ) const { return(WS_CHILD | WS_VISIBLE); }

	public:
		BOOL	Create( LPCTSTR lpCaption, CRect& rect, CWnd* pParentWnd, UINT uID, HINSTANCE hInstance ){
			m_hWnd =  CreateWindowEx( 0, _T("button"), lpCaption, WindowStyle(), rect.top, rect.left, rect.Width(), rect.Height(), pParentWnd->m_hWnd, (HMENU)uID, hInstance, NULL );
			return(m_hWnd ? TRUE : FALSE);
		}
	};

	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CInputPanel : public CPanel {
	protected:
		virtual DWORD	WindowStyle( void ) const { return(WS_CHILD | WS_VISIBLE | WS_BORDER); }

		CButton		btnA;

		BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		
			CRect	rect(10,10,100,30);

			btnA.Create( _T("A"), rect, this, 1001, ::GetModuleHandle(NULL) ); 

			return(TRUE);
		}

		void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
			if( uID == 1001 ){
				::OutputDebugString(_T("1001.\n"));
			}
		}
	};

	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CPaintPanel : public CPanel {
	protected:
		virtual DWORD	WindowStyle( void ) const { return(WS_CHILD | WS_VISIBLE | WS_BORDER); }


		virtual void	OnPaint( HDC hDC ){
			static TCHAR	szText[] = _T("text out\0"); 
			if( hDC ){

				CRect	rc(10,10,40,50);
				HBRUSH	hBr = ::CreateSolidBrush( RGB(255,255,0) );
				HBRUSH	hOldBr = (HBRUSH)SelectObject( hDC, hBr );

				::FillRect( hDC, &rc, hBr );
				
				SelectObject( hDC, hOldBr );
				DeleteObject( hBr );

				TextOut( hDC, 10, 60,  szText, lstrlen(szText) );

				SetTextColor( hDC, RGB(255,0,0) );
				TextOut( hDC, 10, 90,  szText, lstrlen(szText) );

				SetTextColor( hDC, RGB(255,0,255) );
				TextOut( hDC, 10, 120,  szText, lstrlen(szText) );
			}
		}
	};

	/****************************************************************************************
	 *
	 *
	 *
	 */
	class CMainForm : public CForm {
	protected:
		CPaintPanel		wndPaint;
		CInputPanel		wndInput;

		BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
			CRect	rect(0,0,400,400);
			if( wndPaint.Create(this,rect) ){
				wndPaint.ShowWindow( SW_SHOW );
				wndPaint.UpdateWindow();
			}

			rect.Set(400,0,400,400);
			if( wndInput.Create(this,rect) ){
				wndInput.ShowWindow( SW_SHOW );
				wndInput.UpdateWindow();
			}

			return(TRUE);
		}
	};

	/****************************************************************************************
	 *
	 *
	 *
	 */

}
