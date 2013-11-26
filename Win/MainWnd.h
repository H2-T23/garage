// MainWnd.h 
#pragma once

#include "Wnd.h"

class CMainWnd : public CWnd {
public:
	CMainWnd(void){
		if( !Create(_T("MainWnd"), WS_OVERLAPPEDWINDOW) ){
			return;
		}
	}
};

