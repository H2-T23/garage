// ExampleIOCP.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleIOCP.h"

#include "IOCP.h"


#if 0
struct RECEIVE_CONTEXT {
	OVERLAPPED			ov;
	HANDLE				hQueue;
	MQMSGPROPS*			pmsgprops;
};

HRESULT	EnableAsyncReceive( RECEIVE_CONTEXT* prc )
{
	// Validate the input parameter.
	if( prc == NULL )
	{
		return MQ_ERROR_INVALID_PARAMETER;
	}

	// Initiate a receive operation.
	return MQReceiveMessage(
	             prc->hQueue,         // Queue handle
	             INFINITE,            // Time to wait for a message
	             MQ_ACTION_RECEIVE,
	             prc->pmsgprops,      // Pointer to an MQMSGPROPS structure
	             &prc->ov,            // OVERLAPPED structure 
	             NULL,                // No callback function
	             NULL,                // No cursor
	             MQ_NO_TRANSACTION    // Not in a transaction
	             );
}

DWORD WINAPI CompletionPortThread( LPVOID lParam )
{
	HANDLE			hPort		= (HANDLE)lParam;
	DWORD			dwNoOfBytes	= 0;
	ULONG_PTR		ulKey		= 0;
	OVERLAPPED*		pov			= NULL;
	HRESULT			hr			= MQ_OK;

	for( ; ; )
	{
		// Wait for a completion notification.
		pov = NULL;

		BOOL fSuccess = GetQueuedCompletionStatus(
		                hPort,         // Completion port handle
		                &dwNoOfBytes,  // Bytes transferred
		                &ulKey,
		                &pov,          // OVERLAPPED structure
		                INFINITE       // Notification time-out interval
		                );

		//
		// Add code to handle specific errors when fSuccess is FALSE 
		// and provide a way to exit the loop.
		//
		if( pov == NULL )
		{
			continue;	// An unrecoverable error occurred in the completion port. Wait for the next notification.
		}


		// Get the base address of the RECEIVE_CONTEXT structure 
		// containing the OVERLAPPED structure received.
		RECEIVE_CONTEXT*	prc	= CONTAINING_RECORD( pov, RECEIVE_CONTEXT, ov );


		// Get the final status (last error) for the message received.
		hr	= MQGetOverlappedResult( &prc->ov );
		if( FAILED(hr) )
		{
			return hr;
		}

		// Retrieve the application-specific data in the message.
		long lAppSpecific = prc->pmsgprops->aPropVar[0].lVal;
		wprintf( L"Message received. Thread ID: %x. Application-specific data: %ld\n", GetCurrentThreadId(), lAppSpecific );

		// Initiate the next message receive operation.
		hr = EnableAsyncReceive(prc);
		if( FAILED(hr) )
		{
			return hr;
		}
	}
	return hr;
}

HRESULT	AsyncReadCP( WCHAR * wszFormatName, DWORD dwSleepTime )
{
	int		cThread = 0, cReceive = 0;
	HANDLE		hPort			= NULL;
	QUEUEHANDLE	hQueue			= NULL;
	const int NUMBEROFTHREADS	= 2;
	const int NUMBEROFRECEIVES	= 4;
	HRESULT		hr				= MQ_OK;

	// Validate the input parameter and open the queue.
	if( wszFormatName == NULL )
	{
		return MQ_ERROR_INVALID_PARAMETER;
	}

	hr = MQOpenQueue( wszFormatName, MQ_RECEIVE_ACCESS, MQ_DENY_NONE, &hQueue );
	if( FAILED(hr) )
	{
		wprintf(L"MQOpenQueue failed. Error: 0x%X.\n, hr");
		return hr;
	}


	// Create a new completion port.
	hPort = CreateIoCompletionPort(
	          INVALID_HANDLE_VALUE,  // Do not associate a queue handle.
	          NULL,                  // New port
	          0,                     // No completion key
	          NUMBEROFTHREADS        // Number of concurrently executed threads
	          );
	if( hPort == NULL )
	{
		MQCloseQueue( hQueue );
		return GetLastError();
	{

	// Create worker threads for the completion port.
	for( cThread=0; cThread<NUMBEROFTHREADS; cThread++ )
	{
		DWORD dwThreadId = 0;
		HANDLE hThread = NULL;
		hThread = CreateThread(
		          NULL,                  // Default thread security descriptor
		          0,                     // Default stack size
		          CompletionPortThread,  // Start routine
		          hPort,                 // Start routine parameter
		          0,                     // Run immediately
		          &dwThreadId            // Thread ID
		          );

		if( hThread == NULL )
		{
			//
			// Add code to terminate any other threads created in this function.
			//
			MQCloseQueue( hQueue );
			CloseHandle( hPort );
			return GetLastError();
		}

		CloseHandle( hThread );
	}

	// Associate the queue with the completion port already created.
	hPort = CreateIoCompletionPort(
	                        hQueue,           // Queue handle
	                        hPort,            // Port handle
	                        0,                // No completion key
	                        NUMBEROFTHREADS
	                        );
	if( hPort == NULL )
	{
		MQCloseQueue( hQueue );
		return GetLastError();
	}

	// Initiate several overlapped receive operations.
	RECEIVE_CONTEXT*	prc[ NUMBEROFRECEIVES ];

	for( cReceive; cReceive<NUMBEROFRECEIVES; cReceive++ )
	{
		// Allocate and initialize a receive context.
		prc[ cReceive ] = new RECEIVE_CONTEXT;

		if( prc[ cReceive ] == NULL )
		{
			return MQ_ERROR_INSUFFICIENT_RESOURCES;
		}

		memset( prc[ cReceive ], 0, sizeof(RECEIVE_CONTEXT) );

		prc[ cReceive ]->hQueue		= hQueue;
		prc[ cReceive ]->pmsgprops	= new MQMSGPROPS;

		if( prc[ cReceive ]->pmsgprops == NULL )
		{
			return MQ_ERROR_INSUFFICIENT_RESOURCES;
		}

		const int NUMBEROFPROPERTIES = 1;

		prc[ cReceive ]->pmsgprops->aPropID		= new MSGPROPID[ NUMBEROFPROPERTIES ];
		if( prc[ cReceive ]->pmsgprops->aPropID == NULL )
		{
			return MQ_ERROR_INSUFFICIENT_RESOURCES;
		}

		prc[ cReceive ]->pmsgprops->aPropVar	= new MQPROPVARIANT[ NUMBEROFPROPERTIES ];
		if( prc[ cReceive ]->pmsgprops->aPropVar == NULL )
		{
			return MQ_ERROR_INSUFFICIENT_RESOURCES;
		}

		prc[ cReceive ]->pmsgprops->aStatus		= new HRESULT[ NUMBEROFPROPERTIES ];
		if( prc[ cReceive ]->pmsgprops->aStatus == NULL )
		{
			return MQ_ERROR_INSUFFICIENT_RESOURCES;
		}

		DWORD cPropId = 0;
		prc[ cReceive ]->pmsgprops->aPropID[ cPropId ]		= PROPID_M_APPSPECIFIC;
		prc[ cReceive ]->pmsgprops->aPropVar[ cPropId ].vt	= VT_UI4;
		cPropId++;

		prc[ cReceive ]->pmsgprops->cProp		= cPropId;

		hr = EnableAsyncReceive( prc );
		if( FAILED(hr) )
		{
			break;
		}
	}

	if( SUCCEEDED(hr) )
	{
		Sleep(dwSleepTime)
	}


	// Free the memory allocated for the RECEIVE_CONTEXT structures.
	for( ; cReceive >= 0; cReceive-- )
	{
		delete prc[cReceive]->pmsgprops->aStatus;
		delete prc[cReceive]->pmsgprops->aPropVar;
		delete prc[cReceive]->pmsgprops->aPropID;
		delete prc[cReceive]->pmsgprops;
		delete prc[cReceive];
	}

	// Close the queue and free the port.
	MQCloseQueue( hQueue );
	CloseHandle( hPort );

	return hr;
}
#endif

#include <map>
#include <process.h>

#include <winsock2.h>
#include <mswsock.h>

#include "Inet.h"

/**********************************************************************************
 *
 *
 *
 */
void	UseAcceptEx( void )
{
	LPFN_ACCEPTEX	lpfnAcceptEx	= NULL;

	GUID	GuidAcceptEx	= WSAID_ACCEPTEX;

	WSAOVERLAPPED	olOverlap;

	SOCKET	sockListen	= INVALID_SOCKET;
	SOCKET	sockAccept	= INVALID_SOCKET;

	char	lpOutputBuf[ 1024 ]	= { 0 };
	int		outBufLen			= 1024;
	DWORD	dwBytes;


	WSADATA	wsaData;
	if( WSAStartup(MAKEWORD(2,2), &wsaData) ){
	}

	HANDLE	hCompPort	= CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	sockListen	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( sockListen == INVALID_SOCKET )
	{
		WSACleanup();
		return;
	}

	CreateIoCompletionPort( (HANDLE)sockListen, hCompPort, 0, 0 );

	hostent*	pHostEnt	= gethostbyname("");
	char*		pIP			= inet_ntoa( *(in_addr*)*pHostEnt->h_addr_list );

	sockaddr_in	addr			= { 0 };
	addr.sin_family			= AF_INET;
	addr.sin_addr.s_addr	= inet_addr( pIP );
	addr.sin_port			= htons( 54321 );

	if( bind(sockListen, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ){
		closesocket( sockListen );
		return;
	}

	if( listen(sockListen, 100) == SOCKET_ERROR ){

	}

	WSAIoctl(sockListen, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL);

	sockAccept	= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( sockAccept == INVALID_SOCKET ){
		WSACleanup();
		return;
	}

	memset( &olOverlap, 0, sizeof(olOverlap) );

	lpfnAcceptEx(sockListen, sockAccept, lpOutputBuf, outBufLen - ((sizeof(sockaddr_in) + 16) * 2), sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &dwBytes, &olOverlap);


	CreateIoCompletionPort( (HANDLE)sockAccept, hCompPort, 0, 0 );
}
/**********************************************************************************
 *
 *
 *
 */
class IRunnable {
public:
	virtual void	run( void ) = 0;
	virtual void	exit( void ){}
};
/**********************************************************************************
 *
 *
 *
 */
class Thread : public IRunnable {
private:
	static unsigned __stdcall		EntryPoint( void* lpParam ){
		Thread*	pThread	= static_cast<Thread*>( lpParam );
		if( pThread ){
			pThread->run();
		}
		return 0;
	}

public:
	unsigned			m_Id;
	HANDLE				m_hThread;

protected:
	IRunnable*			m_pRunnable;

	virtual void	run( void ){
		if(m_pRunnable){
			m_pRunnable->run();
		}
	}

public:
	Thread( void ) : m_pRunnable(NULL) {
	}

	Thread( IRunnable* runnable ) : m_pRunnable(runnable) {
	}

	virtual ~Thread( void ){
		if(m_pRunnable){
			delete m_pRunnable;
		}
		m_pRunnable	= NULL;
	}

	void	Start( unsigned init = 0 ){
		HANDLE	hThread = (HANDLE)::_beginthreadex(NULL, 0, &Thread::EntryPoint, (void*)(this), init, &m_Id);
		if( hThread ){
			m_hThread	= hThread;
		}
	}

	void	Terminate( void ){
		if(m_pRunnable)
			m_pRunnable->exit();
	}

	void	Suspend( void ){
		SuspendThread( m_hThread );
	}

	void	Resume( void ){
		ResumeThread( m_hThread );
	}

	void	Join( DWORD dwMilliseconds = INFINITE ){
		WaitForSingleObject(m_hThread, dwMilliseconds);
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CIOCPContext {
public:
	enum OP {
		OP_READ, OP_WRITE,
	};

public:
	OVERLAPPED		ol;
	WSABUF			wsabuf;
	SOCKET			sock;
	OP				op;

public:
	BOOL		SendRequest( void ){
		op	= OP_WRITE;

		DWORD	dwBytes	= 0;
		DWORD	dwFlags	= 0;

		int	nRet	= WSASend(sock, &wsabuf, 1, &dwBytes, dwFlags, &ol, NULL);
		if( nRet == SOCKET_ERROR ){
			if( WSA_IO_PENDING != WSAGetLastError() ){
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL		RecvRequest( void ){
		op	= OP_READ;

		DWORD	dwBytes	= 0;
		DWORD	dwFlags	= 0;


		int	nRet	= WSARecv(sock, &wsabuf, 1, &dwBytes, &dwFlags, &ol, NULL);
		if( nRet == SOCKET_ERROR ){
			if( WSA_IO_PENDING != WSAGetLastError() ){
				return FALSE;
			}
		}
		return TRUE;
	}


private:
	CIOCPContext(OP o,SOCKET s) : op(o), sock(s) {}

private:
	typedef std::map<SOCKET,CIOCPContext*>	MAP;
	static MAP	m_map;

public:
	static CIOCPContext*	New(OP o, SOCKET s) {
		CIOCPContext*	pContext = new CIOCPContext(o, s);
		if( pContext )
			m_map.insert( std::pair<SOCKET,CIOCPContext*>(s,pContext) );
		return pContext;
	}

	static void				Remove( SOCKET s ){
		CIOCPContext*	pContext	= Find( s );
		if( pContext ){
			m_map.erase( s );
			delete pContext;
		}
	}

	static void				RemoveAll( void ){
		MAP::iterator it = m_map.begin();
		for( ; it!=m_map.end(); it++ ){
			delete it->second;
		}
		m_map.clear();
	}

	static CIOCPContext*	Find( SOCKET s ){
		MAP::iterator it = m_map.find( s );
		if( it != m_map.end() ){
			return it->second;
		}
		return NULL;
	}
};
/**********************************************************************************
 *
 *
 *
 */
std::map<SOCKET,CIOCPContext*>		CIOCPContext::m_map;
/**********************************************************************************
 *
 *
 *
 */
class IListenerThread : public IRunnable {
protected:
	CIOCP*			m_pIOCP;
	SOCKET			m_sockListen;
	HANDLE			m_hShutdown;

public:
	IListenerThread( CIOCP* pIOCP, SOCKET sock, HANDLE hShutdown )
	 : m_pIOCP(pIOCP), m_sockListen(sock), m_hShutdown(hShutdown) {
	}

	void	run( void ){
		WSAEVENT	eventAccept	= WSACreateEvent();
		WSAEventSelect(m_sockListen, eventAccept, FD_ACCEPT);

		WSANETWORKEVENTS		wsaEvents;

		while( WAIT_OBJECT_0 != WaitForSingleObject(m_hShutdown, 0) )
		{
			if( WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &eventAccept, FALSE, 100, FALSE) )
			{
				WSAEnumNetworkEvents(m_sockListen, eventAccept, &wsaEvents);

				if( wsaEvents.lNetworkEvents & FD_ACCEPT ){
					if( wsaEvents.iErrorCode[ FD_ACCEPT_BIT ] == 0 ){
						AcceptConnection( m_sockListen );
					}
				}
			}
		}
	}

	void	AcceptConnection( SOCKET sockListen ){
		sockaddr_in		addr = {0};
		int	len	= sizeof(addr);

		SOCKET	sock = accept(sockListen, (sockaddr*)&addr, &len);

		CIOCPContext*	pContext = CIOCPContext::New(CIOCPContext::OP_READ, sock);
		if( AssociateWithIOCP( pContext ) )
		{
			pContext->op	= CIOCPContext::OP_WRITE;

			DWORD	dwFlags	= 0;
			DWORD	dwBytes	= 0;

			int	nRet	= WSARecv(pContext->sock, &pContext->wsabuf, 1, &dwBytes, &dwFlags, &pContext->ol, NULL );
			if( nRet == SOCKET_ERROR ){
				if( WSA_IO_PENDING != WSAGetLastError() ){

				}
			}
		}
	}

	bool	AssociateWithIOCP( CIOCPContext* pContext ){
		if( !m_pIOCP->IoCompletionPort((HANDLE)pContext->sock, (ULONG_PTR)pContext) ){
			CIOCPContext::Remove( pContext->sock );
			return false;
		}
		return true;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class IConnection : public IRunnable {
protected:
	CIOCP*			m_pIOCP;
	UINT			m_idx;
	HANDLE			m_hShutdown;

public:
	IConnection( CIOCP* pIOCP, UINT idx, HANDLE hShutdown )
	 : m_pIOCP(pIOCP), m_idx(idx), m_hShutdown(hShutdown) {
	}

	void	run( void ){
		void*			lpVoid			= NULL;
		OVERLAPPED*		lpOverlapped	= NULL;

		while( WAIT_OBJECT_0 != WaitForSingleObject(m_hShutdown, 0) )
		{
			DWORD	dwBytesTransfered	= 0;

			BOOL	bRet	= m_pIOCP->GetQueuedCompletionStatus(&dwBytesTransfered, (LPDWORD)&lpVoid, &lpOverlapped, INFINITE);
			if( NULL == lpVoid )
			{
				break;
			}

			CIOCPContext*	pContext	= (CIOCPContext*)lpVoid;
			if( (bRet == FALSE) || ((bRet == TRUE) && (0 == dwBytesTransfered)) )
			{
				CIOCPContext::Remove( pContext->sock );
				continue;
			}

			switch( pContext->op ){
			case CIOCPContext::OP_READ:
				{
					if( pContext->SendRequest() == FALSE ){
						CIOCPContext::Remove( pContext->sock );
					}
				}
				break;

			case CIOCPContext::OP_WRITE:
				{
					if( pContext->RecvRequest() == FALSE ){
						CIOCPContext::Remove( pContext->sock );
					}
				}
				break;
			}
		}
	}

	void	exit( void ){
		m_pIOCP->PostQueuedCompletionStatus();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class IMainThread : public IRunnable {
protected:
	CIOCP*			m_pIOCP;

	Thread*			m_pListenerThread;
	Thread*			m_pConnectionThreads[ 4 ];
	HANDLE			m_hConnectionThreads[ 4 ];

	HANDLE			m_hShutdown;

public:
	IMainThread( CIOCP* pIOCP ) : m_pIOCP(pIOCP), m_hShutdown(CreateEvent(NULL, TRUE, FALSE, NULL)) {
	}

	void		run( void ){
		for( int i=0; i<4; i++ ){
			m_pConnectionThreads[ i ]	= new Thread( new IConnection(m_pIOCP, i, m_hShutdown) );
			m_pConnectionThreads[ i ]->Start();
		}

		SOCKET	sockListen	= WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

		sockaddr_in		service	= {0};
		service.sin_family			= AF_INET;
		service.sin_addr.s_addr		= INADDR_ANY;
		service.sin_port			= htons( 54321 );

		bind(sockListen, (LPSOCKADDR)&service, sizeof(service));
		listen(sockListen, SOMAXCONN);

		m_pListenerThread = new Thread( new IListenerThread(m_pIOCP, sockListen, m_hShutdown) );
		if( m_pListenerThread ){
			m_pListenerThread->Start();
		}
	}

	void		exit( void ){
		SetEvent( m_hShutdown );

		m_pListenerThread->Join();

		HANDLE	hThreads[ 4 ]	= {NULL};
		for( int i=0; i<4; i++ ){
			m_pConnectionThreads[ i ]->exit();

			hThreads[ i ]	= m_pConnectionThreads[ i ]->m_hThread;
		}

		WaitForMultipleObjects( 4, hThreads, TRUE, INFINITE );

		CloseHandle( m_hShutdown );
	}
};
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(	HINSTANCE	hInstance		,
						HINSTANCE	hPrevInstance	,
						LPTSTR		lpCmdLine		,
						int			nCmdShow		)	
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CIOCP	IOCP;
	IOCP.Create();

	Thread* pThread = new Thread( new IMainThread(&IOCP) );
	if( pThread ){
		pThread->Start();
		pThread->Join();
	}

	INET::WinInet::Cleanup();
}
