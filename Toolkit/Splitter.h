#pragma once

#include "Wnd.h"

/**********************************************************************************
 *
 *
 *
 */
class CSplitter {
private:
	CSplitter( const CSplitter& );
	CSplitter& operator=( const CSplitter& );

protected:
	enum {
		SPLITBAR_SIZE		= 2		,
		MINIMUM_SIZE		= 100	,			
	};

	HCURSOR					m_cursorSize;
	BOOL					m_bSplitterMoving;
	DWORD					m_dwSplitterPos;

	CWnd*					m_pParentWnd;
	std::vector<CWnd*>		m_vecChildWnd;

public:
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

public:
	CSplitter( void ) : m_dwSplitterPos(200){
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
 *
 *
 *
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
		CRect	rc;
		m_pParentWnd->GetClientRect( &rc );

		if( (MINIMUM_SIZE >= x) || (x >= (rc.Width() - MINIMUM_SIZE)) )
			return;

		SplitterPos( x );
		m_pParentWnd->SendMessage( WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height())  );
	}

	void	AdjustWindow( int cx, int cy ){
		if( MINIMUM_SIZE >= cx )
			return;

		if( SplitterPos() < MINIMUM_SIZE )
			SplitterPos( MINIMUM_SIZE );
		else
		if( SplitterPos() > DWORD(cx - MINIMUM_SIZE) )
			SplitterPos( cx - MINIMUM_SIZE );

		m_vecChildWnd[ 0 ]->MoveWindow( 0, 0                
										, (SplitterPos() - SPLITBAR_SIZE), cy );

		m_vecChildWnd[ 1 ]->MoveWindow( (SplitterPos() + SPLITBAR_SIZE), 0
										, (cx - SplitterPos() - SPLITBAR_SIZE-1), cy );
	}
};
/**********************************************************************************
 *
 *
 *
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

		SplitterPos( y );
		m_pParentWnd->SendMessage( WM_SIZE, 0, MAKELPARAM(rc.Width(), rc.Height()) );
	}

	void	AdjustWindow( int cx, int cy ){
		if( MINIMUM_SIZE >= cy )
			return;

		if( SplitterPos() < MINIMUM_SIZE )
			SplitterPos( MINIMUM_SIZE );
		else
		if( SplitterPos() > DWORD(cy - MINIMUM_SIZE) )
			SplitterPos( cy - MINIMUM_SIZE );
		
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
template<class BASE, class SPLITTER>
class TSplitter : public BASE {
protected:
	SPLITTER		m_Splitter;
public:
	void	OnLButtonDown( int x, int y, UINT KeyFlags ){
		m_Splitter.BeginSplitterMoving();	
	}

	void	OnLButtonUp( int x, int y, UINT KeyFlags ){
		m_Splitter.EndSplitterMoving();
	}

	void	OnMouseMove(  int x, int y, UINT KeyFlags ){
		if( m_Splitter.IsSplitterMoving(KeyFlags) )
			m_Splitter.MoveSplitter(x, y);
	}

	void	OnSize(UINT, int cx, int cy){
		m_Splitter.AdjustWindow(cx, cy);
	}
};
/**********************************************************************************
 *
 *
 *
 */
