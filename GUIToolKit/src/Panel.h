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
	virtual DWORD		WindowStyle( void ) const { return(WS_CHILD | WS_VISIBLE | WS_BORDER); }

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
