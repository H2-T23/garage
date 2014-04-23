// MessageSystem.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "MessageSystem.h"

#include "MessageViewer.h"
#include "MessageReciver.h"
#include "MessageClientForm.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CThread*	pMessageReciverUIThread		= new CThread( new CMessageReciverUIThread() );
	pMessageReciverUIThread->Start();

	CThread*	pMessageClientUIThread		= new CThread( new CMessageClientUIThread() );
	pMessageClientUIThread->Start();

	CThread*	pMessageViewerThread		= new CThread( new CMessageViewerThread() );
	if( pMessageViewerThread )
	{
		pMessageViewerThread->Start();
		pMessageViewerThread->Wait();
		delete pMessageViewerThread;
	}

	INET::WinInet::Cleanup();
}
