// ExampleSplitterForSDK.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleSplitterForSDK.h"
#include "UseGUIToolkit.h"



/**********************************************************************************
 */
class CSplitter {
private:
	CSplitter( const CSplitter& );
	CSplitter& operator=( const CSplitter& );

public:
	HCURSOR			m_cursorSize;
	BOOL			m_bSplitterMoving;
	DWORD			m_dwSplitterPos;

	std::vector<CWnd*>		m_vecChildWnd;

	inline DWORD	SplitterPos( DWORD pos ){
		return(m_dwSplitterPos = pos);
	}

	inline DWORD	SplitterPos( void ) const {
		return(m_dwSplitterPos);
	}

	inline BOOL		SplitterMoving( BOOL bMoving ){
		return(m_bSplitterMoving = bMoving);
	}

	inline BOOL		IsSplitterMoving( void ) const {
		return(m_bSplitterMoving);
	}

	inline BOOL		IsSplitterMoving( UINT MouseKeyFlags ) const {
		return((MK_LBUTTON == MouseKeyFlags) && IsSplitterMoving());
	}

	BOOL	BeginSplitterMoving( void ){
		SetCursor( m_cursorSize );
		::SetCapture( (HWND)(*m_pParentWnd) );
		return SplitterMoving(TRUE);
	}

	BOOL	EndSplitterMoving( void ){
		::ReleaseCapture();
		return SplitterMoving(FALSE);
	}

protected:
	enum {
		SPLITBAR_SIZE		= 5		,
		MINIMUM_SIZE		= 100	,			
	};

	CWnd*			m_pParentWnd;

public:
	CSplitter( void ) : m_dwSplitterPos(200){
		LoadCursor();
	}

	virtual ~CSplitter( void ){
	}

	void	Bind( CWnd* pParentWnd ){
		m_pParentWnd	= pParentWnd;
	}

	void	Append( CWnd* pChildWnd ){
		m_vecChildWnd.push_back( pChildWnd );
	}

	virtual	void	LoadCursor( void ) {}
	virtual void	MoveSplitter( int x, int y )	= 0;
	virtual void	AdjustWindow( int cx, int cy )	= 0;
};

/**********************************************************************************
 */
class CVerticalSplitter		: public CSplitter {
public:
	CVerticalSplitter( void ) : CSplitter() {
		LoadCursor();
	}
	~CVerticalSplitter( void ) {}

	void	LoadCursor( void ) { 
		m_cursorSize	= ::LoadCursor(NULL, IDC_SIZEWE);
	}

	void	MoveSplitter( int x, int y ){
		SetCursor( m_cursorSize );
		
		CRect	rc;
		m_pParentWnd->GetClientRect( &rc );

		if( x > rc.Width() )
			return;

		m_dwSplitterPos	= x;
		m_pParentWnd->SendMessage( WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height())  );
	}

	void	AdjustWindow( int cx, int cy ){
		m_vecChildWnd[ 0 ]->MoveWindow( 0, 0                
										, cx, (SplitterPos() - SPLITBAR_SIZE) );

		m_vecChildWnd[ 1 ]->MoveWindow( 0, SplitterPos() + SPLITBAR_SIZE
										, cx, (cy - SplitterPos() - SPLITBAR_SIZE-1) );
	}
};

/**********************************************************************************
 */
class CHorizontalSplitter	: public CSplitter {
public:
	CHorizontalSplitter( void ) : CSplitter() {
		LoadCursor();
	}
	~CHorizontalSplitter( void ) {
	}

	void	LoadCursor( void ){
		m_cursorSize	= ::LoadCursor(NULL, IDC_SIZENS);
	}

	void	MoveSplitter( int x, int y ){
		CRect	rc;
		m_pParentWnd->GetClientRect( &rc );

		if( (MINIMUM_SIZE >= y) || (y >= (rc.Height() - MINIMUM_SIZE)) )
			return;

		m_dwSplitterPos	= y;
		m_pParentWnd->SendMessage( WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height())  );
	}

	void	AdjustWindow( int cx, int cy ){
		if( MINIMUM_SIZE >= cy )
			return;
		
		m_vecChildWnd[ 0 ]->MoveWindow( 0, 0                
										, cx, (     SplitterPos() - SPLITBAR_SIZE) );
		m_vecChildWnd[ 1 ]->MoveWindow( 0, SplitterPos() + SPLITBAR_SIZE
										, cx, (cy - SplitterPos() - SPLITBAR_SIZE-1) );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CSplitterForm : public CForm {
protected:
	CHorizontalSplitter			m_Splitter;
	CMultiEdit		m_edtL, m_edtR;

public:
	enum {
		IDC_EDIT1		= 1001	,
		IDC_EDIT2				,
	};

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( !m_edtL.Create(this, 0, 0, 0, 0, IDC_EDIT1) ){
			return FALSE;
		}

		if( !m_edtR.Create(this, 0, 0, 0, 0, IDC_EDIT2) ){
			return FALSE;
		}

		m_Splitter.Bind( this );
		m_Splitter.Append( &m_edtL );
		m_Splitter.Append( &m_edtR );

		return TRUE;
	}

	virtual void	OnSize( UINT state, int cx, int cy ){
		m_Splitter.AdjustWindow( cx, cy );
	}

	virtual void	OnLButtonDown( int x, int y, UINT KeyFlags ){
		m_Splitter.BeginSplitterMoving();
	}

	virtual void	OnLButtonUp( int x, int y, UINT KeyFlags ){
		m_Splitter.EndSplitterMoving();
	}

	virtual void	OnMouseMove( int x, int y, UINT KeyFlags ){
		if( m_Splitter.IsSplitterMoving( KeyFlags ) )
			m_Splitter.MoveSplitter( x, y );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
public:
	enum {
		IDC_EDIT1		= 1001	,
		IDC_EDIT2				,

		SPLIT_WIDTH		= 2		,
		SPLIT_MIN		= 50	,
	};

	/**********************************************************************************
	 */
	CMainForm( void ) : m_bPush(FALSE), m_nWidth(100) {
	}

protected:
	CMultiEdit		m_edtL, m_edtR;
	BOOL			m_bPush;
	int				m_nWidth;

	/**********************************************************************************
	 */
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( m_edtL.Create(this, 0, 0, 0, 0, IDC_EDIT1) ){
			m_edtL.SendMessage( WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0 );
		}

		if( m_edtR.Create(this, 0, 0, 0, 0, IDC_EDIT2) ){
			m_edtR.SendMessage( WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), 0 );
		}

		CRect	rc;
		GetClientRect( &rc );

		m_nWidth	= rc.Width() / 2;

		return TRUE;
	}
	/**********************************************************************************
	 */
	void	OnSize( UINT state, int cx, int cy ){
		DBG::TRACE( _T("state:%d, cx:%d, cy:%d"), state, cx, cy );
		CRect	rc;
		GetClientRect( &rc );

		m_edtL.MoveWindow( 0						, 0, m_nWidth - SPLIT_WIDTH	, rc.Height() );
		m_edtR.MoveWindow( m_nWidth + SPLIT_WIDTH	, 0, rc.Width() - m_nWidth - SPLIT_WIDTH	, rc.Height() );
	//	m_edtL.Invalidate();
	//	m_edtR.Invalidate();
	}
	/**********************************************************************************
	 */
	void	OnLButtonDown( int x, int y, UINT uKeyFlags ){
		SetCapture( m_hWnd );
		SetCursor( LoadCursor(NULL, IDC_SIZEWE) );
		m_bPush	= TRUE;
	}
	/**********************************************************************************
	 */
	void	OnLButtonUp( int x, int y, UINT uKeyFlags ){
		CRect	rc;
		GetWindowRect( &rc );

		if( x < SPLIT_MIN ){
			m_nWidth	= SPLIT_MIN;
		}
		else
		if( x > (rc.Width() - SPLIT_MIN - 10) )
		{
			m_nWidth	= (rc.Width() - SPLIT_MIN - 10);
		}
		else
		{
			m_nWidth	= x;
		}
		
		m_bPush	= FALSE;
		ReleaseCapture();

		PostMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()) );
	}
	/**********************************************************************************
	 */
	void	OnMouseMove( int x, int y, UINT uKeyFlags ){
		if( m_bPush )
		{
			CRect	rc;
			GetWindowRect( &rc );

			if( x < SPLIT_MIN ){
				m_nWidth	= SPLIT_MIN;
			}
			else
			if( x > (rc.Width() - SPLIT_MIN - 10) )
			{
				m_nWidth	= (rc.Width() - SPLIT_MIN - 10);
			}
			else
			{
				m_nWidth	= x;
			}

			PostMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.Width(), rc.Height()) );
		}
		else
		if( (m_nWidth <= x) && (x <= m_nWidth) )
		{
			SetCursor( LoadCursor(NULL, IDC_SIZEWE) );
		}
		else
		{
			SetCursor( LoadCursor(NULL, IDC_ARROW) );
		}
	}
};


/**********************************************************************************
 *
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

#if 0
	CMainForm		Form;
#else
	CSplitterForm	Form;
#endif
	if( Form.Create(_T("ExampleSplitterForSDK")) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
