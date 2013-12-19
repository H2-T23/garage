#pragma once

#include "Wnd.h"

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
