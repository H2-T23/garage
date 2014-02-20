// CheckMail.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "CheckMail.h"

#include <stdarg.h>

#include "UseToolkit.h"
#include "Inet.h"

#define	SM_GETHOST	(WM_USER + 1)
#define	SM_ASYNC	(WM_USER + 2)

/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
public:
	enum {
		IDC_SERVER_EDIT = 1001	, IDC_SERVER_LABEL,
		IDC_USERID_EDIT			, IDC_USERID_LABEL,
		IDC_PASSWORD_EDIT		, IDC_PASSWORD_LABEL,
		IDC_CHECKMAIL_BUTTON,
		IDC_CLOSE_BUTTON,
		IDC_TEXT,
	};

	enum STATE {
		STATE_OFF			= 0	,
		STATE_CONNECTING	= 1	,
		STATE_USER				,
		STATE_PASS				,
		STATE_STAT				,
		STATE_QUIT				,
	};

	CMainForm( void ){
		int idx = 0;
		m_pChildren[ idx++ ]	= &m_labServer;
		m_pChildren[ idx++ ]	= &m_labUserID;
		m_pChildren[ idx++ ]	= &m_labPassword;
		m_pChildren[ idx++ ]	= &m_edtServer;
		m_pChildren[ idx++ ]	= &m_edtUserID;
		m_pChildren[ idx++ ]	= &m_edtPassword;
		m_pChildren[ idx++ ]	= &m_btnCheckMail;
		m_pChildren[ idx++ ]	= &m_btnClose;
		m_pChildren[ idx++ ]	= &m_edtText;
		m_nChildNum	=idx;
	}

protected:
	CLabel		m_labServer, m_labUserID, m_labPassword;
	CEdit		m_edtServer, m_edtUserID, m_edtPassword;
	CButton		m_btnCheckMail, m_btnClose;
	CMultiEdit	m_edtText;

	AString		m_strServer, m_strUserID, m_strPassword;

	int			m_nChildNum;
	CWnd*		m_pChildren[ 9 ];

	TString		m_strText;

	TCommandHandler<CMainForm>		cmd;
	TMessageHandler<CMainForm>		msg;


	HANDLE			m_hGetHost;
	char			m_HostEnt[ MAXGETHOSTSTRUCT ];
	SOCKET			sockPop;

	char			gszTemp[ 255 ];
	DWORD			m_dwTicks;


	STATE			m_State;

	/**********************************************************************************
	 *
	 */
	void	DISPLAY( LPCTSTR lpszFmt, ... ){
		TCHAR	szBuf[ 256 ];
		va_list	args;
		va_start(args, lpszFmt);
		int sz = _vsntprintf(szBuf, _countof(szBuf), lpszFmt , args);
		va_end(args);
		if( (0 < sz) && (sz < 256) ){
			DBG::TRACE( szBuf );
			m_edtText.SendMessage( EM_REPLACESEL, 0, (LPARAM)szBuf );
		}
	}
	/**********************************************************************************
	 *
	 */
	 void	MSGBOX( LPCTSTR lpszCaption, LPCTSTR lpszFmt, ... ){
		TCHAR	szBuf[ 256 ];
		va_list	args;
		va_start(args, lpszFmt);
		int	sz	= _vsntprintf(szBuf, _countof(szBuf), lpszFmt, args);
		va_end(args);
		if( (0 < sz) && (sz < 256) ){
			DBG::TRACE( szBuf );
			MessageBox(m_hWnd, szBuf, lpszCaption, MB_OK);
		}
	 }
	/**********************************************************************************
	 *
	 */
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		INET::WinInet::Startup();

		m_labServer		.Create(this, 10, 10, 200, 20, IDC_SERVER_LABEL, _T("POP Server Name"));
		m_edtServer		.Create(this, 10, 30, 200, 20, IDC_SERVER_EDIT);

		m_labUserID		.Create(this, 10, 60, 200, 20, IDC_USERID_LABEL, _T("UserID"));
		m_edtUserID		.Create(this, 10, 80, 200, 20, IDC_USERID_EDIT);

		m_labPassword	.Create(this, 10, 110, 200, 20, IDC_PASSWORD_LABEL, _T("Password"));
		m_edtPassword	.Create(this, 10, 130, 200, 20, IDC_PASSWORD_EDIT);

		m_btnCheckMail	.Create(this, 10, 160, 100, 25, IDC_CHECKMAIL_BUTTON, _T("CheckMail"));
		m_btnClose		.Create(this, 10, 190, 100, 25, IDC_CLOSE_BUTTON, _T("Close"));

		m_edtText		.Create(this, 250, 10, 300, 550, IDC_TEXT);

		for( int i=0; i<m_nChildNum; i++ ){
			m_pChildren[ i ]->SetFont();
		}

		cmd.Initialize( this );
		cmd.Register( IDC_CHECKMAIL_BUTTON	, &CMainForm::OnClickCheckMail	);
		cmd.Register( IDC_CLOSE_BUTTON		, &CMainForm::OnClickClose		);

		msg.Initialize( this );
		msg.Register( SM_GETHOST, &CMainForm::OnGetHost );
		msg.Register( SM_ASYNC	, &CMainForm::OnAsync	);

		return TRUE;
	}
	/**********************************************************************************
	 *
	 */
	void	OnDestroy( void ){
		INET::WinInet::Cleanup();
	}
	/**********************************************************************************
	 *
	 */
	void	OnCommand( UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);	
	}
	/**********************************************************************************
	 *
	 */
	void	OnWndMsg( UINT uMsg, WPARAM wParam, LPARAM lParam ){
		msg.Dispach(uMsg, wParam, lParam);
	}
	/**********************************************************************************
	 *
	 */
	void	OnClickCheckMail( void ){
		TCHAR		szBuf[256];
		TString		str;

		m_edtServer.GetText( szBuf, 256 );
		str	= szBuf;
		m_strServer.W2A( str, m_strServer );

		m_edtUserID.GetText( szBuf, 256 );
		str	= szBuf;
		m_strUserID.W2A( str, m_strUserID );

		m_edtPassword.GetText( szBuf, 256 );
		str	= szBuf;
		m_strPassword.W2A( str, m_strPassword );


		DBG::TRACE( _T("[%s][%s][%s]"), (LPCSTR)m_strServer, (LPCSTR)m_strUserID, (LPCSTR)m_strPassword );

		DWORD	dwTicks		= ::GetTickCount();

		m_dwTicks	= ::GetTickCount();
		m_hGetHost	= ::WSAAsyncGetHostByName(m_hWnd, SM_GETHOST, m_strServer.c_str(), m_HostEnt, MAXGETHOSTSTRUCT );
		if( !m_hGetHost )
		{
			MSGBOX(_T("CheckMail"), _T("Error initiating WSAAsyncGetHostByName()"));
		}
		else
		{
			m_State	= STATE_OFF;
		}
	}
	/**********************************************************************************
	 *
	 */
	void	OnClickClose( void ){
		if( m_State ){
			CloseSocket( sockPop );
		}
	}
	/**********************************************************************************
	 *
	 */
	LRESULT	OnGetHost( WPARAM wParam, LPARAM lParam ){
		AString			str;
		SOCKADDR_IN		saServ;
		LPHOSTENT		lpHostEnt;
		LPSERVENT		lpServEnt;

		DISPLAY(_T("SM_GETHOST message received\n"));
		if( (HANDLE)wParam != m_hGetHost ){
			return 0;
		}

		DWORD	dwElapsed	= (GetTickCount() - m_dwTicks);
		DISPLAY(_T("WSAAsyncGetHostByName() took %ld milliseconds to complete\r\n"), dwElapsed);

		int	nRet	= WSAGETASYNCERROR( lParam );
		if( nRet ){
			DISPLAY(_T("WSAAsyncGetHostByName() error: %d\r\n"), nRet);
			return 0;
		}

		DISPLAY(_T("Server found OK\r\n\r\n"));

		DISPLAY(_T("----- CONNECT TO HOST -----\r\n"));
		DISPLAY(_T("Calling socket(AF_INET, SOCK_STREAM, 0);\r\n"));
		sockPop	= socket( AF_INET, SOCK_STREAM, 0 );
		if( sockPop == INVALID_SOCKET ){
			DISPLAY(_T("Could not create a socket\r\n"));
			return 0;
		}

		DISPLAY(_T("Calling WSAAsyncSelect()\r\n"));
		if( WSAAsyncSelect( sockPop, m_hWnd, SM_ASYNC, FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE ) ){
			DISPLAY(_T("WSAAsyncSelect() failed\r\n"));
			return 0;
		}

		m_dwTicks	= GetTickCount();
		lpServEnt	= getservbyname( "pop3", "tcp" );
		dwElapsed	= (GetTickCount() - m_dwTicks);
		DISPLAY(_T("getservbyname() took %ld milliseconds to complete\r\n"), dwElapsed);
		
		if( !lpServEnt )
		{
			saServ.sin_port	= htons(110);
			DISPLAY(_T("getservbyent() failed. Using port 110\r\n"));
		}
		else
		{
			saServ.sin_port	= lpServEnt->s_port;
		}

		saServ.sin_family	= AF_INET;

		lpHostEnt	= (LPHOSTENT)m_HostEnt;
		saServ.sin_addr	= *((LPIN_ADDR)*lpHostEnt->h_addr_list);

		DISPLAY(_T("Calling connect()\r\n"));

		nRet	= connect( sockPop, (LPSOCKADDR)&saServ, sizeof(SOCKADDR_IN) );
		if( nRet == SOCKET_ERROR )
		{
			if( WSAGetLastError() != WSAEWOULDBLOCK )
			{
				DISPLAY(_T("Error connecting\r\n"));
				return 0;
			}
		}

		m_State	= STATE_CONNECTING;
		DISPLAY(_T("\r\n----- PROCESS MESSAGE -----\r\n"));
		return 0;
	}
	/**********************************************************************************
	 *
	 */
	LRESULT	OnAsync( WPARAM wPrarm, LPARAM lParam ){
		char	lpBufRecv[ 256 ];
		int		nBytesRead = 0;
		switch( WSAGETSELECTEVENT(lParam) ){
		case FD_CONNECT:
			DISPLAY(_T("FD_CONNECT	notification received\n"));
			break;

		case FD_WRITE:
			DISPLAY(_T("FD_WRITE	notification received\n"));
			break;

		case FD_READ:
			DISPLAY(_T("FD_READ		notification received\n"));
			nBytesRead	= recv( sockPop, lpBufRecv, sizeof(lpBufRecv), 0 );

			if( nBytesRead == 0 ){
				MSGBOX(_T("recv() error"), _T("Connection closed unexpectedly"));
				break;
			}
			if( nBytesRead == SOCKET_ERROR ){
				MSGBOX(_T("recv() error"), _T("recv() error: %d"), nBytesRead);
				break;
			}

			lpBufRecv[ nBytesRead ] = '\0';
			ProcessData( lpBufRecv, nBytesRead );
			break;

		case FD_CLOSE:
			DISPLAY(_T("FD_CLOSE	notification received\n"));
			break;
		}

		return 0;
	}
	/**********************************************************************************
	 *
	 */
	void	ProcessData( LPSTR lpBuf, int nBytesRead ){
		static char	szResponse[ 512 ] = { 0 };
		static int	nLen = 0;

		if( (nLen + nBytesRead) > sizeof(szResponse) ){
			DISPLAY(_T("!!!! Buffer overran, data truncated\n"));
			nLen	= 0;
			szResponse[ 0 ]	= '\0';
			return;
		}

		strcat( szResponse, lpBuf );
		nLen	= strlen( szResponse );
		
		char*	pChar = NULL;
		while( 1 ){
			pChar	= strchr( szResponse, '\n' );
			if( pChar == NULL ){
				break;
			}

			*pChar	= '\0';
			ProcessLine( pChar );
			pChar++;

			if( strlen(pChar) )
				memmove( szResponse, pChar, strlen(pChar) + 1 );
			else
				szResponse[ 0 ]	= '\0';

		}
	}
	/**********************************************************************************
	 *
	 */
	void	ProcessLine( LPSTR lpStr ){
		int	nRet;
		long	lCount, lSize;

		AString		str;


		DISPLAY(_T("Resposen from server:\n%s\n"), lpStr);

		if( lpStr[ 0 ] == '-' )
		{
			DISPLAY(_T("Negative response: "));

			switch( m_State ){
			case STATE_CONNECTING:
				DISPLAY(_T("Connection denied\n"));
				break;
			case STATE_USER:
				DISPLAY(_T("Unknown UserID\n"));
				break;
			case STATE_PASS:
				DISPLAY(_T("Wrong Password\n"));
				break;
			case STATE_STAT:
				DISPLAY(_T("STAT command not supported\n"));
				break;
			case STATE_QUIT:
				DISPLAY(_T("QUIT command not supported\n"));
				break;
			}
			DISPLAY(_T("Sending QUIT\n"));
			DISPLAY(_T("QUIT\r\n"));
			str.Format( "QUIT\r\n" );
			int	nRet	= send( sockPop, str.c_str(), str.length(), 0 );

			m_State	= STATE_QUIT;
			return;
		}

		switch( m_State ){
		case STATE_CONNECTING:
			DISPLAY(_T("AppState = CONNECTING, sending USER\n"));
			str.Format( "USER %s\r\n", m_strUserID );
			nRet	= send( sockPop, str.c_str(), str.length(), 0 );
			m_State	= STATE_USER;
			break;

		case STATE_USER:
			DISPLAY(_T("AppState = USER, sending PASS\n"));
			str.Format( "PASS %s\r\n", m_strPassword );
			nRet	= send( sockPop, str.c_str(), str.length(), 0 );
			m_State	= STATE_PASS; 
			break;

		case STATE_PASS:
			DISPLAY(_T("AppState = PASS, sending STAT\n"));
			str.Format( "STATE\r\n" );
			nRet	= send( sockPop, str.c_str(), str.length(), 0 );
			m_State	= STATE_STAT;
			break;

		case STATE_STAT:
			DISPLAY(_T("AppState = STAT, reading response\n"));
			sscanf( lpStr, "%s %ld %ld", gszTemp, &lCount, &lSize );
			DISPLAY(_T("----- RESULT ------\n"));
			DISPLAY(_T("%ld message %ld bytes\n"), lCount, lSize );
			DISPLAY(_T("Sending QUIT\n"));
			str.Format( "QUIT\r\n" );
			nRet	= send( sockPop, str.c_str(), str.length(), 0 );
			m_State	= STATE_QUIT;
			break;

		case STATE_QUIT:
			DISPLAY(_T("Host QUIT OK\n"));
			DISPLAY(_T("Closing socket\n"));
			CloseSocket( sockPop );
			break;
		}
	}
	/**********************************************************************************
	 *
	 */
	void	CloseSocket( SOCKET sock ){
		char	szBuf[ 256 ];

		shutdown( sock, 1 );

		while( 1 ){
			int nRet	= recv( sock, szBuf, sizeof(szBuf), 0 );
			if( (nRet == 0) || (nRet == SOCKET_ERROR) ){
				break;
			}
		}

		shutdown( sock, 2 );
		closesocket( sock );
	}
};

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

 	// TODO: ここにコードを挿入してください。

	CMainForm	Form;
	if( Form.Create(_T("CheckMail"), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}

