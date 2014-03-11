// ExampleInet.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleInet.h"

#include "UseToolkit.h"
#include "dbg.h"
#include "TString.h"
#include "Inet.h"
#include "Thread.h"

/**********************************************************************************
 *
 *
 *
 */
/*
ADDRINFO
    int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
    int                 ai_family;      // PF_xxx
    int                 ai_socktype;    // SOCK_xxx
    int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
    size_t              ai_addrlen;     // Length of ai_addr
    char *              ai_canonname;   // Canonical name for nodename
    __field_bcount(ai_addrlen) struct sockaddr *   ai_addr;        // Binary address
    struct addrinfo *   ai_next;        // Next structure in linked list
 */
class CAddressInfo : public ADDRINFO {
protected:
	LPADDRINFO			m_lpAddrInfo;

	inline bool	IsAvailable( void ) const {
		return(m_lpAddrInfo!=NULL);
	}

	void	Free( void ){
		if( IsAvailable() )
			freeaddrinfo( m_lpAddrInfo );
		m_lpAddrInfo	= NULL;
	}

public:
	CAddressInfo( void ) : m_lpAddrInfo(NULL) {
		ZeroMemory(this, sizeof(ADDRINFO));
		Set(0, 0, 0, 0);
	}

	CAddressInfo( int family, int socktype, int protocol, int flags = 0 ) : m_lpAddrInfo(NULL) {
		ZeroMemory(this, sizeof(ADDRINFO));
		Set( family, socktype, protocol, flags );
	}

	~CAddressInfo( void ) {
		Free();
	}

	void		Set( int family, int socktype, int protocol, int flags ){
		ai_family		= family;
		ai_socktype		= socktype;
		ai_protocol		= protocol;
		ai_flags		= flags;
	}
	void		Flags( int flags ) { ai_flags = flags; }
	void		Family( int family ){ ai_family = family; }
	void		SockType( int socktype ){ ai_socktype = socktype; }
	void		Protocol( int protocol ){ ai_protocol = protocol; }

	int			Flags( void ){ return(m_lpAddrInfo->ai_flags); }
	int			Family( void ){ return(m_lpAddrInfo->ai_family); }
	int			SockType( void ){ return(m_lpAddrInfo->ai_socktype); }
	int			Protocol( void ){ return(m_lpAddrInfo->ai_protocol); }
	sockaddr*	SockAddr( void ){ return(m_lpAddrInfo->ai_addr); }
	int			AddrLen( void ){ return(m_lpAddrInfo->ai_addrlen); }

public:
	BOOL	GetAddressInfo( LPCSTR lpszHostname, LPCSTR lpszPort = NULL ){
		Free();
		return(getaddrinfo(lpszHostname, lpszPort, this, &m_lpAddrInfo) != 0 ? FALSE : TRUE);
	}

	BOOL	GetAddressInfo( LPCSTR lpszHostname, int nPort ){
		AString		str;
		str.Format("%d", nPort);

		LPCSTR	lpPort	= (LPCSTR)str;
		return GetAddressInfo(lpszHostname, lpPort);
	}

	int		ToString( TString& str ){
		str.Empty();
		if( !IsAvailable() )
			return 0;

		TCHAR	tmp[256];

		str.resize( 256 );
		LPTSTR lpResult		= (LPTSTR)str.data();
		int		szCapacity	= str.capacity();
		int		szMaxSize	= str.max_size();
		DWORD	dwSize		= 0;
		WSAAddressToString( m_lpAddrInfo->ai_addr, (DWORD)m_lpAddrInfo->ai_addrlen, NULL, tmp, &dwSize );
		return str.Length();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CReceiverThread : public TM::IThread {
protected:
	INET::CSocket		m_sock;
	WSAEVENT			m_hEventExit;

	unsigned	run( LPVOID lpParam ){



		const int	szBuff = 256;
		BYTE	Buff[ szBuff ];
		
		m_hEventExit	= ::WSACreateEvent();

		WSAEVENT	hEventInet	= ::WSACreateEvent();
		::WSAEventSelect( m_sock.GetHandle(), hEventInet, (FD_READ | FD_CLOSE) );

		WSAEVENT	hEvents[] = {m_hEventExit, hEventInet};

		WSANETWORKEVENTS	events;
		DWORD	dwResult	= 0;
		while(1)
		{
			dwResult = ::WSAWaitForMultipleEvents(2, hEvents, FALSE, WSA_INFINITE, FALSE);
			if( dwResult == WSA_WAIT_FAILED )
				break;

			if( (dwResult - WSA_WAIT_EVENT_0) == 0 )
			{
				::WSAEnumNetworkEvents( m_sock.GetHandle(), hEventInet, &events );
				if( events.lNetworkEvents & FD_CLOSE )
				{
					break;
				}
				else
				if( events.lNetworkEvents & FD_READ )
				{
					m_sock.Recv( Buff, szBuff);
				}
			}
			else
			if( (dwResult - WSA_WAIT_EVENT_0) == 1)
			{
				break;
			}

			::WSAResetEvent( hEventInet );
		}

		::WSACloseEvent( hEventInet );
		return 0;
	}

public:
	CReceiverThread( void ) : m_hEventExit(NULL) {
	}
	virtual ~CReceiverThread( void ) {
	}

	void	Bind( SOCKET sock ){
		m_sock.Attach( sock );
	}

	void	Exit( void ){
		::WSASetEvent(m_hEventExit);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CClientForm : public CForm {
public:
	enum {
		IDC_BUTTON1		= 1001	, 
		IDC_EDIT1				,
	};
protected:
	CButton					m_btn1;
	CEdit					m_edt1;
	INET::CListenSocket		m_sockListen;
	CReceiverThread			m_ReceiverThread;

	TCommandHandler<CClientForm>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		INET::WinInet::Startup();

		m_btn1.Create(this, 10, 10, 100, 25, IDC_BUTTON1, _T("SEND"));
		m_edt1.Create(this, 10, 30, 400, 25, IDC_EDIT1);

		cmd.Initialize(this);
		cmd.Register( IDC_BUTTON1, &CClientForm::OnSend );

		CAddressInfo	addrInfo( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		addrInfo.GetAddressInfo( "localhost", "" );

		TString	str;
		addrInfo.ToString( str );
		DBG::TRACE( (LPTSTR)str );

		INET::CSocket	sock;
		sock.Create( addrInfo.Family(), addrInfo.SockType(), addrInfo.Protocol() );

		if( sock.Connect(addrInfo.SockAddr(), addrInfo.AddrLen()) == SOCKET_ERROR )
		{
			sock.Close();
		//	return FALSE;
		}

		m_ReceiverThread.Bind( sock.Detach() );
		m_ReceiverThread.Start();

		SetFontChildren();
		return TRUE;
	}

	void	OnDestroy( void ){
		INET::WinInet::Cleanup();
	}

	void	OnCommand(UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach(uID, nCodeNotify);
	}

	void	OnSend( void ){

	}
};
/**********************************************************************************
 *
 *
 *
 */
class CClientThread : public TM::IUIThread {
protected:
	CClientForm		Form;

	unsigned	run( LPVOID lpParam ){
		if( Form.Create(_T("Client"), 0, 0, 100, 100, 600, 600) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
		return 0;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CServerForm : public CForm {
};
/**********************************************************************************
 *
 *
 *
 */
class CServerThread : public TM::IUIThread {
protected:
	CServerForm		Form;

	unsigned	run( LPVOID lpParam ){
		if( Form.Create(_T("Server"), 0, 0, 700, 100, 600, 600) ){
			Form.ShowWindow( SW_SHOW );
			Form.UpdateWindow();
			Form.MessageLoop();
		}
		return 0;
	}
};
/**********************************************************************************
 *
 *
 *
 */
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

	CServerThread	ServerThread;
	ServerThread.Start();

	CClientThread	ClientThread;
	ClientThread.Start();

	HANDLE	hThreads[] = {ServerThread.m_hThread, ClientThread.m_hThread};

	::WaitForMultipleObjects( 2, hThreads, TRUE, (1000*5) );

	ServerThread.Quit();
	ClientThread.Quit();

	::WaitForMultipleObjects( 2, hThreads, TRUE, INFINITE );
	return 0;
}
