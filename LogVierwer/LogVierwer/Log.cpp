#include "Log.h"

/**********************************************************************************
 *
 *
 *
 */
UINT AFX_CDECL		CLogManager::LogReceiver( LPVOID lpParam )
{
	CLogManager*	pLogManager	= (CLogManager*)lpParam;
	if( pLogManager == NULL )
	{
		return 0;
	}

	TRACE(_T("BeginThread.\n"));
	DWORD	dwExit = 0;
	while( (dwExit = WaitForSingleObject(pLogManager->m_pEventExit->m_hObject, 0)) == WAIT_OBJECT_0 )
	{
		pLogManager->GetLogLlist()->Append( _T("Subject") );
		Sleep( 100 );

		if( WaitForSingleObject( pLogManager->m_pEventSuspend->m_hObject, 0 ) != WAIT_OBJECT_0 )
		{
			TRACE(_T("SuspendThread.\n"));
			WaitForSingleObject( pLogManager->m_pEventSuspend->m_hObject, INFINITE );
			TRACE(_T("ResumeThread.\n"));
		}
	}

	TRACE(_T("ExitThread.\n"));
	::AfxEndThread( 1 );
	return 1;
}
/**********************************************************************************
 *
 *
 *
 */
/*
1. WSAStartupでWinSockの初期化を行う。

2. socket関数でソケットの作成を行う。

3. サーバのIPアドレスを取得する。inet_addr関数でIPアドレスの取得ができなかった場合、
	WSAAsyncGetHostByName関数でサーバのIPアドレスの取得する。
	このときサーバの情報が取得できたらウィンドウメッセージとして通知されます。

4. WSAAsyncSelect関数でソケットイベントをウィンドウメッセージに通知されるようにします。

5. 作成されたソケットを使用して、サーバのIPアドレスとポート番号を指定してconnect関数でサーバに接続する。

6. 送信バッファに空きができたことを通知するウィンドウメッセージが来たら、
	ソケットに対して、send関数で接続しているサーバにデータを送信する。

7. サーバから
 */