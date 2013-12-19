#pragma once

#include "Wnd.h"

/**********************************************************************************
 *
 *
 *
 */
class CPanel : public TWnd<CPanel> {
protected:
	virtual LPCTSTR		ClassName( void ) const		{	return(_T("Panel"));				}
	virtual DWORD		WindowStyle( void ) const	{	return(WS_CHILD | WS_VISIBLE | WS_BORDER);	}

public:
	BOOL	Create( CWnd* pParent, CRect& rect ){
		return TWnd<CPanel>::Create( NULL, 0, 0, rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight(), pParent->m_hWnd );
	}
};
