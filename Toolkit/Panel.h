#pragma once

#include "Wnd.h"

/****************************************************************************************
 *
 *
 *
 */
class CPanel : public TWnd<CPanel> {
protected:
	virtual LPCTSTR		ClassName( void ) const { return(_T("Panel")); }
	virtual DWORD		WindowStyle( void ) const { return(WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS); }

	std::list<CWnd*>		listWnd;

public:
	BOOL		Create( CWnd* pParent, int x, int y, int nWidth, int nHeight, LPCTSTR lpWindowName = NULL ){
		return TWnd<CPanel>::Create( lpWindowName, 0, 0, x, y, nWidth, nHeight, (HWND)(*pParent) );
	}

	BOOL		Create( CWnd* pParent, CRect& rect, LPCTSTR lpWindowName = NULL ){
		return CPanel::Create( pParent, rect.X(), rect.Y(), rect.Width(), rect.Height(), lpWindowName );
	}

	void		Append( CWnd* pChild ){
		listWnd.push_back( pChild );
	}

	void		Remove( CWnd* pChild ){
		listWnd.remove( pChild );
	}
};
