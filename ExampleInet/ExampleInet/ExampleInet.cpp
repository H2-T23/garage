// ExampleInet.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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

	ECHO_IOCP::CIOCPControler	iocpCtrl;
	iocpCtrl.Start();

	ECHO_IOCP::CEchoClient		client;
	client.Start();
	client.Wait();

	iocpCtrl.Terminate();

	return 0;
}
