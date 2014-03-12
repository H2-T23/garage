#pragma once


#include <stdarg.h>

namespace SimpleInet {
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	ListBoxAddString( CListBox& ListBox, LPCTSTR lpFmt, ... )
	{
		TCHAR	szBuf[256];

		va_list	args;
		va_start(args,lpFmt);
		int	sz	= _vsntprintf(szBuf, _countof(szBuf), lpFmt, args);
		va_end(args);
		if( sz > 0 ){
			ListBox.AddString( szBuf );
		}
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CReceiverThread : public MT::IThread {
	protected:
		INET::CSocket		m_sock;
		WSAEVENT			m_hEventExit;

		/**********************************************************************************
		 *
		 */
		unsigned	run( LPVOID lpParam ){

			if( m_sock.IsValid() == false )
				return unsigned(-1);

			m_hEventExit	= ::WSACreateEvent();

			WSAEVENT	hEventInet	= ::WSACreateEvent();
			::WSAEventSelect( m_sock.GetHandle(), hEventInet, (FD_READ | FD_CLOSE) );

			WSAEVENT	hEvents[] = {hEventInet, m_hEventExit};

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
						TCHAR	szBuff[ 256 ];
						m_sock.Recv( szBuff, _countof(szBuff) );
						DBG::TRACE( szBuff );
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
			Close();
		}

		void	Bind( SOCKET sock ){
			m_sock.Attach( sock );
		}

		void	Exit( void ){
			::WSASetEvent(m_hEventExit);
		}

		void	Close( void ){
			m_sock.Shutdown( SD_BOTH );
			m_sock.Close();

			if( m_hEventExit )
				::WSACloseEvent( m_hEventExit );
			m_hEventExit	= NULL;

			MT::IThread::Close();
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
			IDC_LISTBOX1			,
		};
	protected:
		CButton					m_btn1;
		CMultiEdit					m_edt1;
		CListBox				m_ListBox;
		

		INET::CSocket			m_sockClient;
		CReceiverThread			m_ReceiverThread;

		TCommandHandler<CClientForm>	cmd;

		/**********************************************************************************
		 *
		 */
		BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
			CRect	rc;
			GetClientRect( &rc );

			m_btn1.Create(this, 0, 10, 100, 25, IDC_BUTTON1, _T("SEND"));
			m_edt1.Create(this, 0, 40, rc.Width(), 60, IDC_EDIT1);
			m_ListBox.Create(this, 0, 100, rc.Width(), rc.Height()-100, IDC_LISTBOX1);

			cmd.Initialize(this);
			cmd.Register( IDC_BUTTON1, &CClientForm::OnSend );

			INET::CAddressInfo	addrInfo( AF_INET, SOCK_STREAM, IPPROTO_TCP );
			addrInfo.GetAddressInfo( "localhost", "6666" );

			TString	str;
			addrInfo.ToString( str );
			ListBoxAddString(m_ListBox, str);

			INET::CSocket	sock;
			if( sock.Create(addrInfo.Family(), addrInfo.SockType(), addrInfo.Protocol()) )
			{
				if( sock.Connect(addrInfo.SockAddr(), addrInfo.AddrLen()) != SOCKET_ERROR )
				{
					m_sockClient	= sock;
					ListBoxAddString(m_ListBox,_T("Success: Connect."));
				}
				else
				{
					sock.Close();
					ListBoxAddString(m_ListBox,_T("Failed: Connct."));
				}
			}
			else
			{
				ListBoxAddString(m_ListBox,_T("Failed: CreateSocket."));
			}

			m_ReceiverThread.Bind( (SOCKET)m_sockClient );
			m_ReceiverThread.Start();

			SetFontChildren();
			return TRUE;
		}

		/**********************************************************************************
		 *
		 */
		void	OnDestroy( void ){
			m_ReceiverThread.Exit();
			if( m_ReceiverThread.Join( 1000 ) != WAIT_OBJECT_0 )
				m_ReceiverThread.Terminate();
			m_ReceiverThread.Close();
		}

		/**********************************************************************************
		 *
		 */
		void	OnCommand(UINT uID, HWND hWndCtrl, UINT nCodeNotify ){
			cmd.Dispach(uID, nCodeNotify);
		}

		/**********************************************************************************
		 *
		 */
		void	OnSend( void ){
			TCHAR	lpTxt[256];
			int	nLen	= m_edt1.GetWindowText( lpTxt, 256 );
			if( nLen > 0 )
			{
				int	nSize	= (nLen + 1) * sizeof(TCHAR);
				int	nResult	= m_sockClient.Send( lpTxt, nSize );
				if( nResult == SOCKET_ERROR )
				{
					ListBoxAddString(m_ListBox,_T("Faild: Send(%s)."), lpTxt);
				}
				else
				{
					ListBoxAddString(m_ListBox,_T("Success: Send(%s)."), lpTxt);
				}
			}
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CClientThread : public MT::IUIThread {
	protected:
		CClientForm		Form;

		/**********************************************************************************
		 *
		 */
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
	public:
		enum {
			IDC_LISTBOX1	= 1001	,

			WM_SOCKET		= (WM_USER+0x111)	,
		};
	protected:
		CListBox				m_ListBox;

		INET::CListenSocket		m_sockListen;
		SOCKET					m_sockServer[ 10 ];

		TMessageHandler<CServerForm>	msg;

		/**********************************************************************************
		 *
		 */
		BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
			m_ListBox.Create(this, 0, 0, 0, 0, IDC_LISTBOX1);

			INET::CAddressInfo	AddrInfo(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE);
			if( AddrInfo.GetAddressInfo(NULL, "6666") )
			{
				TCHAR	szBuf[64];
				DWORD	dwBufLen = _countof(szBuf);

				LPADDRINFO	lpAddrInfo = AddrInfo.AddrInfo();
				while( lpAddrInfo ){
					::WSAAddressToString(lpAddrInfo->ai_addr, (DWORD)lpAddrInfo->ai_addrlen, NULL, szBuf, &dwBufLen);
					ListBoxAddString(m_ListBox, szBuf);
					lpAddrInfo	= lpAddrInfo->ai_next;
				}

				INET::CSocket		sock;

				if( sock.Create(AddrInfo.Family(), AddrInfo.SockType(), AddrInfo.Protocol()) )
				{
					if( sock.Bind(AddrInfo.SockAddr(), AddrInfo.AddrLen()) != SOCKET_ERROR )
					{
						if( sock.Listen( 1 ) != SOCKET_ERROR )
						{
							m_sockListen.Attach( sock.Detach() );
							ListBoxAddString(m_ListBox,_T("Success:	Listen."));
						}
					}
				}
			}

			m_sockListen.AsyncSelect(m_hWnd, WM_SOCKET, FD_ACCEPT);

			msg.Initialize(this);
			msg.Register(WM_SOCKET, &CServerForm::OnSocketHandler);

			SetFontChildren();
			return TRUE;
		}

		/**********************************************************************************
		 *
		 */
		void	OnDestroy( void ){
			m_sockListen.Close();
			m_ListBox.DestroyWindow();
		}

		/**********************************************************************************
		 *
		 */
		void	OnSize(UINT state, int cx, int cy){
			m_ListBox.MoveWindow(0, 0, cx, cy);
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
		LRESULT	OnSocketHandler( WPARAM wParam, LPARAM lParam ){
			switch( lParam ){
			case FD_ACCEPT:
				{
					SOCKADDR_STORAGE	Addr;
					int	nLen	= sizeof(SOCKADDR_STORAGE);
					INET::CSocket	sock;
					sock = m_sockListen.Accept( (LPSOCKADDR)&Addr, nLen );
					if( sock.IsValid() )
					{
						sock.AsyncSelect( m_hWnd, WM_SOCKET, (FD_READ | FD_CLOSE) );

						CHAR	szHostName[64];
						::getnameinfo( (LPSOCKADDR)&Addr, nLen, szHostName, sizeof(szHostName), NULL, 0, 0 );
						TString		str;
						TString::A2W(AString(szHostName), str);
						ListBoxAddString(m_ListBox, _T("Accept: %s.\n"), (LPCTSTR)str);					
					}
				}
				break;

			case FD_READ:
				{
					TString	str;
					str.Format(_T("RecvData:"));

					INET::CSocket	sock( (SOCKET)wParam );
					if( sock.IsValid() )
					{
						TCHAR	szBuff[ 256 ]; 
						int	nResult	= sock.Recv(szBuff, sizeof(szBuff));
						str.AppendFormat(_T("%s"), szBuff);

						sock.Send(szBuff, nResult);
					}
					ListBoxAddString(m_ListBox, str);
				}
				break;

			case FD_CLOSE:
				{
					TString	str;
					str.Format(_T("Disconnect."));

					INET::CSocket	sock( (SOCKET)wParam );
					if( sock.IsValid() )
					{
						sock.Shutdown( SD_BOTH );
						sock.Close();
					}
					ListBoxAddString(m_ListBox, str);
				}
				break;
			}

			return 0;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CServerThread : public MT::IUIThread {
	protected:
		CServerForm		Form;

		/**********************************************************************************
		 *
		 */
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
	void	EntryPoint( void )
	{
		INET::WinInet::Startup();


		CServerThread	ServerThread;
		ServerThread.Start();
		ServerThread.Join( 1000*10 );

		CClientThread	ClientThread;
		ClientThread.Start();

		HANDLE	hThreads[] = {ServerThread.m_hThread, ClientThread.m_hThread};

	#if 0
		::WaitForMultipleObjects( 2, hThreads, TRUE, (1000*5) );

		ServerThread.Quit();
		ClientThread.Quit();
	#endif

		::WaitForMultipleObjects( 2, hThreads, TRUE, INFINITE );

		INET::WinInet::Cleanup();
	}
}// end of SimpleInet.