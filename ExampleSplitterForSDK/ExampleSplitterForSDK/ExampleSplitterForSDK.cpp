// ExampleSplitterForSDK.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "ExampleSplitterForSDK.h"
#include "UseGUIToolkit.h"



/**********************************************************************************
 *
 *
 *
 */
class CSplitterForm : public CForm {
protected:
	CVerticalSplitter		m_Splitter;
	CMultiEdit				m_edtL;
	CEdit					m_edtR;

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
	if( Form.Create(_T("ExampleSplitterForSDK"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
