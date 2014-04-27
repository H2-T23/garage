// HelloWorld.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "HelloWorld.h"

#include "UseToolkit.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CForm	Form;
	if( Form.Create(_T("Hello World"), 0, 0, 100, 100, 300, 300) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
}
