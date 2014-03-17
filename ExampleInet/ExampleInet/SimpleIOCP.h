#pragma once


namespace SimpleIOCP{

	class CAcceptThread : public MT::IThread {
	protected:
		unsigned	run( LPVOID ){
		}
	};

	class CIOCP {
	};

#define OP_READ     0
#define OP_WRITE    1
 
#define WORKER_THREADS_PER_PROCESSOR 2

#define MAX_BUFFER_LEN 256
#define WAIT_TIMEOUT_INTERVAL 100

	HANDLE				g_hShutdownEvent	= NULL;
	 
	int					g_nThreads = 0;
	 
	HANDLE*				g_phWorkerThreads	= NULL;
	HANDLE				g_hAcceptThread		= NULL;
	 
	WSAEVENT			g_hAcceptEvent;
	 
	/**********************************************************************************
	 *
	 *
	 *
	 */
	HANDLE				g_hIOCompletionPort	= NULL;

	CRITICAL_SECTION	g_csClientList;
	class CClientContext;
	std::vector<CClientContext*>	g_ClientContext;
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	AddToClientList( CClientContext* pClientContext )
	{
		EnterCriticalSection( &g_csClientList );

		g_ClientContext.push_back( pClientContext );

		LeaveCriticalSection( &g_csClientList );
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	RemoveFromClientListAndFreeMemory( CClientContext* pClientContext )
	{
		EnterCriticalSection( &g_csClientList );

		std::vector<CClientContext*>::iterator	IterClientContext	= g_ClientContext.begin();
		for( ; IterClientContext != g_ClientContext.end(); IterClientContext++ )
		{
			if( pClientContext == *IterClientContext )
			{
				g_ClientContext.erase( IterClientContext );
				delete pClientContext;
			break;
			}
		}

		LeaveCriticalSection( &g_csClientList );
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	CleanClientList()
	{
		EnterCriticalSection( &g_csClientList );

		std::vector<CClientContext*>::iterator	IterClientContext	= g_ClientContext.begin();
		for( ; IterClientContext != g_ClientContext.end( ); IterClientContext++ )
		{
			delete	*IterClientContext;
		}
		g_ClientContext.clear();

		LeaveCriticalSection( &g_csClientList );
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CClientContext {
	private:
		OVERLAPPED        *m_pol;
		WSABUF            *m_pwbuf;

		int               m_nTotalBytes;
		int               m_nSentBytes;

		SOCKET            m_Socket;  //accepted socket
		int               m_nOpCode; //will be used by the worker thread to decide what operation to perform
		char              m_szBuffer[MAX_BUFFER_LEN];

	public:
		void SetOpCode(int n)	{	m_nOpCode = n;	}
		int GetOpCode()	{	return m_nOpCode;	}

		void SetTotalBytes(int n)	{	m_nTotalBytes = n;	}
		int GetTotalBytes()	{	return m_nTotalBytes;	}

		void SetSentBytes(int n)	{	m_nSentBytes = n;	}
		void IncrSentBytes(int n)	{	m_nSentBytes += n;	}
		int GetSentBytes()	{	return m_nSentBytes;	}

		void SetSocket(SOCKET s)	{	m_Socket = s;	}
		SOCKET GetSocket()	{	return m_Socket;	}

		void SetBuffer(char *szBuffer)	{	strcpy(m_szBuffer, szBuffer);	}
		void GetBuffer(char *szBuffer)	{	strcpy(szBuffer, m_szBuffer);	}
		void ZeroBuffer()	{	ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);	}

		void SetWSABUFLength(int nLength)	{	m_pwbuf->len = nLength;	}
		int GetWSABUFLength()	{	return m_pwbuf->len;	}
		WSABUF* GetWSABUFPtr()	{	return m_pwbuf;	}

		OVERLAPPED* GetOVERLAPPEDPtr() {	return m_pol;	}

		void ResetWSABUF()	{
			ZeroBuffer();
			m_pwbuf->buf = m_szBuffer;
			m_pwbuf->len = MAX_BUFFER_LEN;
		}

		CClientContext()
		{
			m_pol = new OVERLAPPED;
			m_pwbuf = new WSABUF;

			ZeroMemory(m_pol, sizeof(OVERLAPPED));

			m_Socket =  SOCKET_ERROR;

			ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);

			m_pwbuf->buf = m_szBuffer;
			m_pwbuf->len = MAX_BUFFER_LEN;

			m_nOpCode = 0;
			m_nTotalBytes = 0;
			m_nSentBytes = 0;
		}

		~CClientContext()
		{
			while( !HasOverlappedIoCompleted(m_pol) ){
				Sleep(0);
			}
			closesocket(m_Socket);

			delete m_pol;
			delete m_pwbuf;
		}
	};
 	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMTTrace {
	protected:
		CRITICAL_SECTION	m_cs;

	public:
		CMTTrace( void ){
			::InitializeCriticalSection( &m_cs );
		}
		~CMTTrace( void ){
			::DeleteCriticalSection( &m_cs );
		}

		void	Trace( char *szFormat, ... )
		{
			::EnterCriticalSection( &m_cs );

			va_list	args;
			va_start(args, szFormat);
			vprintf(szFormat, args);
			va_end(args);

			::LeaveCriticalSection( &m_cs );
		}
	};
 	/**********************************************************************************
	 *
	 *
	 *
	 */
	static CMTTrace		MTTrace;
	#ifdef TRACE
		#undef TRACE
	#endif
	#define	TRACE		MTTrace.Trace
 	/**********************************************************************************
	 *
	 *
	 *
	 */
	bool	Initialize( void )
	{
		g_nThreads	= WORKER_THREADS_PER_PROCESSOR * GetNoOfProcessors();

		printf("\nNumber of processors on host: %d", GetNoOfProcessors());
		printf("\nThe following number of worker threads will be created: %d", g_nThreads);

		g_phWorkerThreads	= new HANDLE[ g_nThreads ];

		InitializeCriticalSection( &g_csClientList );

		g_hShutdownEvent	= CreateEvent( NULL, TRUE, FALSE, NULL );

		WSADATA	wsaData;

		int nResult		= WSAStartup( MAKEWORD(2,2), &wsaData );
		if( NO_ERROR != nResult  )
		{
			printf("\nError occurred while executing WSAStartup().");
			return false; //error
		}
		else
		{
			printf("\nWSAStartup() successful.");
		}

		if( false == InitializeIOCP() )
		{
			printf("\nError occurred while initializing IOCP");
			return false;
		}
		else
		{
			printf("\nIOCP initialization successful.");
		}

		return true;
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	void	DeInitialize( void )
	{
		DeleteCriticalSection( &g_csClientList );

		CloseHandle( g_hIOCompletionPort );
		CloseHandle( g_hShutdownEvent );

		delete[] g_phWorkerThreads;

		WSACleanup();
	}
	/**********************************************************************************
	 *
	 *
	 *
	 */
	int		EntryPoint( int nPortNo )
	{
		if( false == Initialize() )
		{
			return 1;
		}

		SOCKET	ListenSocket	= WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
		if( INVALID_SOCKET == ListenSocket ) 
		{
			printf("\nError occurred while opening socket: %d.", WSAGetLastError());
			goto error;
		}
		else
		{
			printf("\nWSASocket() successful.");
		}
	
		
		struct sockaddr_in ServerAddress;

		ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));

		ServerAddress.sin_family		= AF_INET;
		ServerAddress.sin_addr.s_addr	= INADDR_ANY;
		ServerAddress.sin_port			= htons(nPortNo);

		if( SOCKET_ERROR == bind(ListenSocket, (struct sockaddr *)&ServerAddress, sizeof(ServerAddress)) ) 
		{
			closesocket( ListenSocket );
			printf("\nError occurred while binding.");
			goto error;
		}
		else
		{
			printf("\nbind() successful.");
		}

		if( SOCKET_ERROR == listen(ListenSocket,SOMAXCONN) )
		{
			closesocket( ListenSocket );
			printf("\nError occurred while listening.");
			goto error;
		}
		else
		{
			printf("\nlisten() successful.");
		}

		g_hAcceptEvent	= WSACreateEvent();

		if( WSA_INVALID_EVENT == g_hAcceptEvent )
		{
			printf("\nError occurred while WSACreateEvent().");
			goto error;
		}

		if( SOCKET_ERROR == WSAEventSelect(ListenSocket, g_hAcceptEvent, FD_ACCEPT) )
		{
			printf("\nError occurred while WSAEventSelect().");
			WSACloseEvent( g_hAcceptEvent );
			goto error;
		}

		printf("\nTo exit this server, hit a key at any time on this console...");

		DWORD nThreadID;
		g_hAcceptThread		= CreateThread(0, 0, AcceptThread, (void *)ListenSocket, 0, &nThreadID);

		while( !_kbhit() )
		{
			Sleep( 0 );
		}

		TRACE("\nServer is shutting down...");

		CleanUp();

		closesocket( ListenSocket );

		DeInitialize();

		return 0;
	error:
		closesocket( ListenSocket );
		DeInitialize();
		return 1;
	}

	bool InitializeIOCP()
	{
		g_hIOCompletionPort	= CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

		if( NULL == g_hIOCompletionPort )
		{
			printf("\nError occurred while creating IOCP: %d.", WSAGetLastError());
			return false;
		}

		DWORD nThreadID;

		for (int ii = 0; ii < g_nThreads; ii++)
		{
			g_phWorkerThreads[ii]	= CreateThread(0, 0, WorkerThread, (void *)(ii+1), 0, &nThreadID );
		}

		return true;
	}

	void CleanUp()
	{
		SetEvent( g_hShutdownEvent );

		WaitForSingleObject( g_hAcceptThread, INFINITE );

		for (int i = 0; i < g_nThreads; i++)
		{
			PostQueuedCompletionStatus( g_hIOCompletionPort, 0, (DWORD)NULL, NULL );
		}

		WaitForMultipleObjects( g_nThreads, g_phWorkerThreads, TRUE, INFINITE );

		WSACloseEvent( g_hAcceptEvent );

		CleanClientList();
	}
 
namespace ACCEPT {
	bool	AssociateWithIOCP( CClientContext* pClientContext )
	{
		HANDLE hTemp	= CreateIoCompletionPort( (HANDLE)pClientContext->GetSocket(), g_hIOCompletionPort, (DWORD)pClientContext, 0 );

		if( NULL == hTemp ){
			TRACE("\nError occurred while executing CreateIoCompletionPort().");

			RemoveFromClientListAndFreeMemory( pClientContext );
			return false;
		}
		return true;
	}

	void	AcceptConnection( SOCKET ListenSocket )
	{
		sockaddr_in ClientAddress;
		int nClientLength = sizeof(ClientAddress);

		SOCKET Socket = accept(ListenSocket, (sockaddr*)&ClientAddress, &nClientLength);

		if( INVALID_SOCKET == Socket )
		{
			TRACE("\nError occurred while accepting socket: %ld.", WSAGetLastError());
		}
		TRACE("\nClient connected from: %s", inet_ntoa(ClientAddress.sin_addr));

		CClientContext   *pClientContext  = new CClientContext;

		pClientContext->SetOpCode( OP_READ );
		pClientContext->SetSocket( Socket );

		AddToClientList( pClientContext );

		if( true == AssociateWithIOCP(pClientContext) )
		{
			pClientContext->SetOpCode( OP_WRITE );

			WSABUF*		p_wbuf	= pClientContext->GetWSABUFPtr();
			OVERLAPPED*	p_ol	= pClientContext->GetOVERLAPPEDPtr();

			DWORD dwFlags = 0;
			DWORD dwBytes = 0;

			int nBytesRecv = WSARecv( pClientContext->GetSocket(), p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );
			if( (SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()) )
			{
				TRACE("\nError in Initial Post.");
			}
		}
	}

	DWORD WINAPI AcceptThread( LPVOID lParam )
	{
		SOCKET ListenSocket	= (SOCKET)lParam;
		WSANETWORKEVENTS	WSAEvents;

		while( WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0) )
		{
			if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &g_hAcceptEvent, FALSE, WAIT_TIMEOUT_INTERVAL, FALSE))
			{
				WSAEnumNetworkEvents( ListenSocket, g_hAcceptEvent, &WSAEvents );

				if( (WSAEvents.lNetworkEvents & FD_ACCEPT) && (0 == WSAEvents.iErrorCode[ FD_ACCEPT_BIT ]) )
				{
					AcceptConnection( ListenSocket );
				}
			}
		}
		return 0;
	}
}
 
	DWORD WINAPI	WorkerThread( LPVOID lpParam )
	{    
		int nThreadNo	= (int)lpParam;

		void *lpContext = NULL;
		OVERLAPPED       *pOverlapped = NULL;
		CClientContext   *pClientContext = NULL;
		DWORD            dwBytesTransfered = 0;
		int nBytesRecv = 0;
		int nBytesSent = 0;
		DWORD             dwBytes = 0, dwFlags = 0;

		while( WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0) )
		{
			BOOL bReturn	= GetQueuedCompletionStatus(g_hIOCompletionPort	,
														&dwBytesTransfered	,
														(LPDWORD)&lpContext	,
														&pOverlapped		,
														INFINITE			);
			if( NULL == lpContext )
			{
				break;
			}

			pClientContext		= (CClientContext *)lpContext;

			if( (FALSE == bReturn) || ((TRUE == bReturn) && (0 == dwBytesTransfered)) )
			{
				RemoveFromClientListAndFreeMemory( pClientContext );
				continue;
			}

			WSABUF *		p_wbuf	= pClientContext->GetWSABUFPtr();
			OVERLAPPED *	p_ol	= pClientContext->GetOVERLAPPEDPtr();

			switch( pClientContext->GetOpCode() )
			{
			case OP_READ:
				pClientContext->IncrSentBytes( dwBytesTransfered );

				if( pClientContext->GetSentBytes() < pClientContext->GetTotalBytes() )
				{
					pClientContext->SetOpCode( OP_READ );

					p_wbuf->buf += pClientContext->GetSentBytes();
					p_wbuf->len = pClientContext->GetTotalBytes() - pClientContext->GetSentBytes();

					dwFlags = 0;

					nBytesSent	= WSASend( pClientContext->GetSocket(), p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

					if( (SOCKET_ERROR == nBytesSent) && (WSA_IO_PENDING != WSAGetLastError()) )
					{
						RemoveFromClientListAndFreeMemory( pClientContext );
					}
				}
				else
				{
					pClientContext->SetOpCode( OP_WRITE );
					pClientContext->ResetWSABUF();

					dwFlags = 0;

					nBytesRecv	= WSARecv( pClientContext->GetSocket(), p_wbuf, 1, &dwBytes, &dwFlags, p_ol, NULL );

					if( (SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()) )
					{
						TRACE("\nThread %d: Error occurred while executing WSARecv().", nThreadNo);

						RemoveFromClientListAndFreeMemory( pClientContext );
					}
				}
				break;

			case OP_WRITE:
				char	szBuffer[ MAX_BUFFER_LEN ];

				pClientContext->GetBuffer( szBuffer );

				TRACE("\nThread %d: The following message was received: %s", nThreadNo, szBuffer);

				pClientContext->SetOpCode( OP_READ );
				pClientContext->SetTotalBytes( dwBytesTransfered );
				pClientContext->SetSentBytes( 0 );

				p_wbuf->len	= dwBytesTransfered;

				dwFlags = 0;

				nBytesSent	= WSASend( pClientContext->GetSocket(), p_wbuf, 1, &dwBytes, dwFlags, p_ol, NULL );

				if( (SOCKET_ERROR == nBytesSent) && (WSA_IO_PENDING != WSAGetLastError()) )
				{
					TRACE("\nThread %d: Error occurred while executing WSASend().", nThreadNo);

					RemoveFromClientListAndFreeMemory( pClientContext );
				}

				break;

			default:
				break;
			} // switch
		} // while

		return 0;
	}
 
 

	int		GetNoOfProcessors()
	{
		static int nProcessors = 0;

		if( 0 == nProcessors ){
			SYSTEM_INFO	si;

			GetSystemInfo( &si );
			nProcessors	= si.dwNumberOfProcessors;
		}

		return nProcessors;
	}




}// end of "SimpleIOCP".
