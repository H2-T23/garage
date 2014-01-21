// ExampleSplitterForSDK.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleSplitterForSDK.h"
#include "UseGUIToolkit.h"

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

	CMainForm	MainForm;
	if( MainForm.Create(_T("ExampleSplitterForSDK")) ){
		MainForm.ShowWindow( nCmdShow );
		MainForm.UpdateWindow();
		return MainForm.MessageLoop();
	}
	return 0;
}
