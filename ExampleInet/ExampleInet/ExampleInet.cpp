// ExampleInet.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleInet.h"

#include "UseToolkit.h"
#include "dbg.h"
#include "TString.h"
#include "Inet.h"
#include "Thread.h"

//#include "SimpleInet.h"
//#include "ToolHelpEx.h"
#include "EchoIOCP.h"

#if 0
#include "SimpleIOCP.h"
#endif
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

//	SimpleInet::EntryPoint();
//	TOOLHELP::UseToolhelp();
	
	//CModuleEntry	xa(a);
	//CModuleEntry	ya;
	//CModuleEntry	za	= a;
	//ya = a;

	//za = ya;

	ECHO_IOCP::CIOCPControler	IocpCtrl;
	IocpCtrl.init();
	IocpCtrl.run();

	return 0;
}
