#include "TabControl.h"

/**********************************************************************************
 *
 *
 *
 */
IMPLEMENT_DYNAMIC(CTabControl, CTabCtrl)
/**********************************************************************************
 *
 *
 *
 */
CTabControl::CTabControl()
{
}
/**********************************************************************************
 *
 *
 *
 */
CTabControl::~CTabControl()
{
}
/**********************************************************************************
 *
 *
 *
 */
CWnd*	CTabControl::TabSelChange( int nCurSel )
{
	if( GetCurSel() != nCurSel ){
		SetCurSel( nCurSel );
	}

	TABITEMS::iterator it = m_TabItems.begin();
	for( int i=0; it!=m_TabItems.end(); it++, i++ ){
		(*it)->ShowWindow( i == nCurSel ? SW_SHOW : SW_HIDE );
	}
	return m_TabItems.at( nCurSel );
}
/**********************************************************************************
 *
 *
 *
 */
void	CTabControl::TabResize( int cx, int cy )
{
	CRect	rc(0, 0, cx, cy);
	MoveWindow( 0, 0, cx, cy, TRUE );
	GetClientRect( &rc );
	AdjustRect( FALSE, &rc );

	rc.InflateRect( -5, -5, -5, -5 );

	TABITEMS::iterator	it = m_TabItems.begin();
	for( int i=0; it!=m_TabItems.end(); it++, i++ ){
		(*it)->MoveWindow( &rc, TRUE );
	}
}
/**********************************************************************************
 *
 *
 *
 */
BEGIN_MESSAGE_MAP(CTabControl, CTabCtrl)
END_MESSAGE_MAP()
/**********************************************************************************
 *
 *
 *
 */


