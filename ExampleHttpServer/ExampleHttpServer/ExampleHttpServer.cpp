// ExampleHttpServer.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleHttpServer.h"

#include "UseToolkit.h"

#include "Inet.h"

/**********************************************************************************
 *
 *
 *
 */
class CRequest {
public:
	SOCKET			sock;
	LPSOCKADDR		lpSockAddr;
	int				nAddrSize;
	int				nMethod;
	BOOL			fFullResponse;
	DWORD			dwConnectTime;
	DWORD			dwRecv;
	DWORD			dwSend;

	/**********************************************************************************
	 *
	 */
	CRequest( void )
	 : sock(INVALID_SOCKET)
	 , lpSockAddr(NULL)
	 , nAddrSize(0)
	 , nMethod(0)
	 , fFullResponse(FALSE)
	 , dwConnectTime(0)
	 , dwRecv(0)
	 , dwSend(0)
	{}

	/**********************************************************************************
	 *
	 */
	CRequest( SOCKET sock_, LPSOCKADDR lpSockAddr_, int nAddrSize_ )
	 : sock(INVALID_SOCKET)
	 , lpSockAddr(NULL)
	 , nAddrSize(0)
	 , nMethod(0)
	 , fFullResponse(FALSE)
	 , dwConnectTime(0)
	 , dwRecv(0)
	 , dwSend(0)
	{
		Initialize( sock_, lpSockAddr_, nAddrSize_ );
	}

	/**********************************************************************************
	 *
	 */
	BOOL	Initialize( SOCKET sock_, LPSOCKADDR lpSockAddr_, int nAddrSize_ ){
		lpSockAddr	= (LPSOCKADDR)malloc( nAddrSize_ );
		if( lpSockAddr == NULL ){
			return(NULL);
		}
		nAddrSize	= nAddrSize_;
		sock		= sock_;
		memcpy( lpSockAddr, lpSockAddr_, nAddrSize_ );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CRequestManager {
protected:
	std::list<CRequest*>	m_List;

public:
	/**********************************************************************************
	 *
	 */
	CRequest*	Add( SOCKET sock, LPSOCKADDR lpSockAddr, int nAddrLen ){
		CRequest*	lpReq = new CRequest;
		if( lpReq == NULL ){
			return(NULL);
		}

		lpReq->lpSockAddr	= (LPSOCKADDR)malloc( nAddrLen );
		if( lpReq->lpSockAddr == NULL ){
			free( lpReq );
			return(NULL);
		}

		lpReq->sock			= sock;
		lpReq->nAddrSize		= nAddrLen;
		
		m_List.push_back( lpReq );
	}

	CRequest*	Get( SOCKET sock );
	CRequest*	GetFirst( void );
	CRequest*	GetNext( CRequest* lpReq );
	void		Remove( CRequest* lpReq );
	void		RemoveAll( void );
};

/**********************************************************************************
 *
 *
 *
 */
class CHttpServer {
protected:
	HWND			m_hWnd;
	UINT			m_uMsgAsy;
	UINT			m_uMsgApp;
	TString			m_strWebRoot;
	INET::CSocket	m_sockListen;

public:
	struct Argment {
		USHORT		Port;
		LPCTSTR		lpRootDir;
		HWND		hWnd;
		UINT		uMsgAsy;
		UINT		uMsgApp;
	};

	/**********************************************************************************
	 *
	 */
	BOOL	Start( Argment& arg ){
		SOCKADDR_IN		saServer;
		LPSERVENT		lpServEnt;
		DWORD			dwAddrStrLen;
		char			szBuf[ 256 ];
		char			szAddress[ 128 ];
		int				nRet;


		if( arg.lpRootDir ){
			m_strWebRoot	= arg.lpRootDir;
		}else{
			m_strWebRoot	= _T("/WebPages");
		}

		m_sockListen.Create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if( !m_sockListen.IsValid() ){
			DBG::TRACE(_T("Could not create listen socket."));
			return FALSE;
		}

		nRet	= WSAAsyncSelect( (SOCKET)m_sockListen, m_hWnd, m_uMsgAsy, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE );
		if( nRet == SOCKET_ERROR ){
			DBG::TRACE(_T("WSAAsyncSelect() error"));
			m_sockListen.Close();
			return FALSE;
		}

		if( arg.Port ){
			saServer.sin_port	= htons( arg.Port );
		}else{
		}

		saServer.sin_family			= AF_INET;
		saServer.sin_addr.s_addr	= INADDR_ANY;
		
		if( SOCKET_ERROR == m_sockListen.Bind((LPSOCKADDR)&saServer, sizeof(struct sockaddr)) ){
			DBG::TRACE(_T("bind() error"));
			m_sockListen.Close();
			return FALSE;
		}

		if( SOCKET_ERROR == m_sockListen.Listen( SOMAXCONN ) ){
			DBG::TRACE(_T("listen() error"));
			m_sockListen.Close();
			return FALSE;
		}

		return TRUE;
	}

	/**********************************************************************************
	 *
	 */
	void	Stop( void ){
	}

	/**********************************************************************************
	 *
	 */
	void	OnAsycMsg( WPARAM wParam, LPARAM lParam ){
		int	nErrorCode	= WSAGETSELECTERROR(lParam);

		switch( WSAGETSELECTEVENT(lParam) ){
		case FD_ACCEPT:
			OnAccept( (SOCKET)wParam, nErrorCode );
			break;

		case FD_READ:
			break;

		case FD_WRITE:
			break;

		case FD_CLOSE:
			break;
		}
	}

	/**********************************************************************************
	 *
	 */
	void	OnAccept( SOCKET sock, int nErrorCode ){
		SOCKADDR_IN		SockAddr;
	
		int	nLen	= sizeof(SOCKADDR_IN);
		SOCKET peerSocket	= m_sockListen.Accept( (LPSOCKADDR)&SockAddr, &nLen );
		if( peerSocket == SOCKET_ERROR ){
			int	nRet = WSAGetLastError();
			if( nRet != WSAEWOULDBLOCK ){
				DBG::TRACE(_T("accept() error"));
				return;
			}
		}

		int	nRet	= WSAAsyncSelect( peerSocket, hwnd, guMsgAsy, FD_READ | FD_WRITE | FD_CLOSE );
		if( peerSocket == SOCKET_ERROR ){

			return;
		}

		CRequest* lpReq = AddRequest( peerSocket, (LPSOCKADDR)&SockAddr, nLen );
		if( lpReq == NULL ){
			closesocket( peerSocket );
		}

		LogEvent( _T("Connection accepted on socket(%d) from: %s"), peerSocket, inet_ntoa(SockAddr.sin_addr) );
	}

	/**********************************************************************************
	 *
	 */
	void	OnRead( SOCKET sock, int nErrorCode ){
		static BYTE	buf[ 2048 ];

		memset(buf, 0, sizeof(buf);

		CRequest* lpReq = GetRequest( sock );
		if( lpReq == NULL ){
			int	nRet	= 0;
			while( (nRet = recv(sock, buf, sizeof(buf)-1, 0)) != SOCKET_ERROR );
			closesocket( sock );
			return;
		}

		int	nRet	= recv(sock, buf, sizeof(buf)-1, 0);
		if( nRet == SOCKET_ERROR ){
			if( WSAGetLastError() == WSAEWOULBLOCK ){
				return;
			}

			LogWinSockError( "recv()", WSAGetLastError() );
			CloseConnection( lpReq );
			return;
		}

		lpReq->dwRecv += nRet;

		ParseRequest( lpReq, buf );
	}

	/**********************************************************************************
	 *
	 */
	void	OnWirte( SOCKET sock, int nErrorCode ){

		CRequest* lpReq	= GetRequest( sock );
		if( lpReq == NULL ){
			BYTE buf[ 1024 ];
			int	nRet = 0;
			while( (nRet = recv(sock, buf, sizeof(buf)-1, 0)) != SOCKET_ERROR );
			closesocket( sock );
			return;
		}

		if( lpReq->hFile == HFILE_ERROR ){
			return;
		}

		SendFileContents( lpReq );
	}

	/**********************************************************************************
	 *
	 */
	void	OnClose( SOCKET sock, int nErrorCode ){
		CRequest* lpReq = GetRequest( sock );
		if( lpReq == NULL ){
			return;
		}

		CloseConnection( lpReq );
	}

	/**********************************************************************************
	 *
	 */
	void	ParseRequest( CRequest* lpReq, LPBYTE lpBuf ){
		char	szFileName[ _MAX_PATH ];
		char	szSeps[]	= "\n";
		char*	cpToken;
	}

	/**********************************************************************************
	 *
	 */
	void	CloseConnection( CRequest* lpReq ){
		HTTPSTATS	stats;

		LogEvent("Closing socket: %d", lpReq->sock);
		closesocket( lpReq->sock );

		if( lpReq->hFile != HFILE_ERROR ){
			_lclose( lpReq->hFile );
		}

		stats.dwElapsedTime	= (GetTickCount() - lpReq->dwConnectTime);
		stats.dwRecv		= lpReq->dwRecv;
		stats.dwSend		= lpReq->dwSend;
		SendMessage( ghwnd, guMsgApp, HTTP_STATS_MSG, (LPARAM)&stats );

		DelRequest( lpReq );
	}

	/**********************************************************************************
	 *
	 */
	void	SendFile( CRequest* lpReq, LPCSTR lpFileName ){
		CFile	file;
		lpReq->hFile	= file.Open( lpFileName, OF_READ | OF_SHARE_COMPAT );
		if( lpReq->hFile == HFILE_ERROR ){
			SendMessage(ghWnd, guMsgApp, HTTP_FILENOTFOUND_MSG, (LPARAM)(LPCSTR)lpFileName);
			CloseConnection( lpReq );
			return;
		}

		SendMessage(ghWnd, guMsgApp, HTTP_FILEOK_MSG, (LPARAM)(LPCSTR)(lpFileName + strlen(szWebRoot)) );

		lpReq->dwFilePtr	= 0L;
		SendFileContents( lpReq );
	}

	/**********************************************************************************
	 *
	 */
	void	SendError( CRequest* lpReq, UINT uError ){
		static char		szMsg[ 512 ];
		static char*	lpszStartMsgs[]	= {
			"200 OK","201 Created","202 Accepted","204 No Content",
		};

		if( (uError < 0) || (uError > _countof(lpszStartMsgs) ){
			return;
		}

		wsprintf( szMsg, "<body><h1>%s</h1></body>", lpszStartMsgs[ uError ] );

		int	nRet	= send( lpReq->Sock, szMsg, strlen(szMsg), 0 );
		if( nRet == SOCKET_ERROR ){
			if( WSAGetLastError() != WSAEWOULDBLOCK ){
				LogWinSockError( ghwnd, "send()@SendError()", WSAGetLastError() );
				nRet = 0;
			}
		}
		lpReq->dwSend	+= nRet;
	}

	/**********************************************************************************
	 *
	 */
	void	SendFileContents( CRequest* lpReq ){
		if( lpReq->dwFilePtr > 0 ){
			::_llseek( lpReq->hFile, lpReq->dwFilePtr, FILE_BEGIN );
		}

		UINT	uBytes	= 0;
		while( 1 ){
			uBytes	= ::_lread( lpReq->hFile, buf, sizeof(buf) );
			if( uBytes == HFILE_ERROR ){
				LogEvent( ghWnd, "", GetLastError() );

				SendError( lpReq, HTTP_STATUS_SERVERERROR );
				CloseConnection( lpReq );

				::_lclose( lpReq->hFile );

				lpReq->hFile	= HFILE_ERROR;
				return;
			}

			if( uBytes < sizeof(buf) ){
				fEof	= TRUE;
			}

			nBytesSent	= send( lpReq->sock, buf, uBytes, 0 );
			if( nBytesSent == SOCKET_ERROR ){
				if( WSAGetLastError() != WSAEWOULDBLCOK ){


					CloseConnection( lpReq );
					::_lclose( lpReq->hFile );
					lpReq->hFile	= HFILE_ERROR;
					return;
				}

				nBytesSent	= 0;
			}

			lpReq->dwFilePtr += nBytesSent;
		}
	}

	/**********************************************************************************
	 *
	 */
	void	LogEvent( HWND hWnd, LPCTSTR lpFormat, ... ){
		TCAHR	szBuf[ 256 ];

		va_list	args;
		va_start(args, lpFormat);
		_vstprintf(szBuf, lpFormat, args);
		va_end(args);

		SendMessage(ghwnd, guMsgApp, HTTP_EVENT_MSG, (LPARAM)szBuf);
	}

	/**********************************************************************************
	 *
	 */
	void	LogWinSockError( HWND hWnd, LPCTSTR lpText, int nErrorCode ){
		TCHAR	szBuf[ 256 ]	= {'\0'};

		LoadString( GetWindowInstance(hWnd), nErrorCode, szBuf, sizeof(szBuf) );
		LogEvent( hWnd, _T("%s : %s"), lpText, szBuf );
	}

	/**********************************************************************************
	 *
	 */
	int		GetLocalAddress( LPTSTR lpStr, LPDWORD lpdwStrLen ){
		struct in_addr*	lpInAddr	= NULL;

		int	nRet	= gethostname(lpStr, *lpdwStrLen);
		if( nRet == SOCKET_ERRO ){
			return(SOCKET_ERROR);
		}

		LPHOSTENT	lpHostEnt	= gethostbyname( lpStr );
		if( lpHostEnt == NULL ){
			return(SOCKET_ERROR);
		}

		lpInAddr	= ((LPIN_ADDR)lpHostEnt->h_addr);
		int	nLen	= strlen( inet_ntoa( *lpInAddr ) );
		if( (DWORD)nLen > *lpdwStrLen ){
			*lpdwStrLen	= nLen;
			WSASetLastError( WSAEINVAL );
			return(SOCKET_ERROR);
		}

		*lpdwStrLen	= nLen;
		strcpy( lpStr, inet_ntoa(*lpInAddr) );

		return 0;
	}
};

/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
public:
	enum {
		IDC_LBX_EVENT		= 1001	,
	};
protected:
	CListBox	m_lbxEvent;

	/**********************************************************************************
	 *
	 */
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( !m_lbxEvent.Create(this, 0, 300, 300, 300, IDC_LBX_EVENT) ){
			return FALSE;
		}
		m_lbxEvent.SetFont();

		return TRUE;
	}

	/**********************************************************************************
	 *
	 */
	void	OnSize( UINT uState, int cx, int cy ){
		m_lbxEvent.MoveWindow( 0, 300, cx, cy );
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

	CMainForm	Form;
	if( Form.Create(_T("HTTP"), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}