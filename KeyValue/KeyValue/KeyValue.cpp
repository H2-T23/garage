// KeyValue.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "KeyValue.h"

#include "Inet.h"
#include "Thread.h"
#include "IOCP.h"

#include "ISocket.h"
#include "IEventHandler.h"

using namespace INET;
using namespace MT;

//template<class EventHandlerClass>
//class TSession : public EventHandlerClass {
//public:
//	TSession( SOCKET sock ) /*: EventHandlerClass(sock)*/ {}
//
//};

class CKeyValueProtocol : public IEventHandler {
public:
	CBuffer			m_Buffer;

	int		OnRead( LPVOID lpParam ){

		m_Buffer.Append( *((CBuffer*)lpParam) );

		std::string		str( (char*)m_Buffer.At( 0 ) );

		int	idx	= str.find( "\n", 0 );
		if( idx > 0 )
			return 1;

		return 0;
	}

	int		OnWrite( LPVOID lpParam ){

		return 0;
	}
};

class CSession : public IEventHandler {
public:
	enum {
		OP_READ, OP_WRITE
	};

	OVERLAPPED			m_ol;
	WSABUF				m_wsaBuf;
	CBuffer				m_Buffer;
	CSocket*			m_pSocket;
	IEventHandler*		m_pProtocol;
	int					m_op;

	CSession( SOCKET s )
		: m_pSocket(new CSocket(s)), m_pProtocol(new CKeyValueProtocol())
	{
		unsigned	nSize	= 3;//0xFFFF;
		m_Buffer.Resize( nSize );
	}

	void		OnEvent( void ){
		switch( m_op ){
		case OP_READ:
			if( m_pProtocol->OnRead( &m_Buffer ) >= 0 )
			{
				m_op	= OP_WRITE;
			}
			else
			{
				Recv();
			}
			break;

		case OP_WRITE:
			m_pProtocol->OnWrite( &m_Buffer );
			break;
		}
	}

	BOOL		Recv( void ){
		DWORD	dwFlags	= 0;

		m_wsaBuf.buf	= (char*)m_Buffer.At( 0 );
		m_wsaBuf.len	= m_Buffer.Size();

		memset(&m_ol, 0, sizeof(WSAOVERLAPPED));
		m_op	= OP_READ;

		if( m_pSocket->WSARecv(&m_wsaBuf, 1, NULL, &dwFlags, &m_ol, NULL) == SOCKET_ERROR )
		{
			if( ::WSAGetLastError() == WSA_IO_PENDING )
			{
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}
};

//class CIOCPState {
//public:
//	OVERLAPPED			m_ol;
//	WSABUF				m_wsaBuf;
//
//	CIOCPState( HANDLE hHandle ) : m_pSesison(new CSession((SOCKET)hHandle)) {
//
//	}
//
//	CIOCPState( SOCKET s ) : m_pSesison(new CSession(s)) {}
//
//	CSession*			m_pSesison;
//};
//
class CSessionManager {
private:
	static std::map<DWORD, LPVOID>		m_map;

public:
	static LPVOID		New( DWORD dwKey, LPVOID lpParam ){
		CSession* pSession = new CSession( *(SOCKET*)lpParam );
		if( pSession )
		{
			m_map.insert( std::pair<DWORD, LPVOID>(dwKey, (LPVOID)pSession) );
			return (LPVOID)pSession;
		}
		return NULL;
	}

	static void			Delete( DWORD dwKey ){
		std::map<DWORD, LPVOID>::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			LPVOID	lpParam = it->second;
			m_map.erase( it );

			if( lpParam )
				delete lpParam;
		}
	}

	static LPVOID		Find( DWORD dwKey ){
		std::map<DWORD, LPVOID>::iterator	it = m_map.find( dwKey );
		if( it != m_map.end() )
		{
			return it->second;
		}
		return NULL;
	}
};

std::map<DWORD, LPVOID>		CSessionManager::m_map;


class CIOCPWorker	: public IRunnable {
public:
	CIOCP*			m_pIOCP;
	CEvent*			m_pEventShutdown;

	CIOCPWorker( CIOCP* pIOCP, CEvent* pEvent )
	 : m_pIOCP(pIOCP), m_pEventShutdown(pEvent) {
	}

	void	run( void ){
		while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_pEventShutdown->m_hObject, 0) )
		{
			LPOVERLAPPED		lpOverlapped		= NULL;
			DWORD				dwBytesTransfered	= 0;
			CSession*			pSession			= NULL;
			BOOL	bReturn	= m_pIOCP->GetQueuedCompletionStatus( &dwBytesTransfered, (LPDWORD)&pSession, &lpOverlapped, INFINITE );
			if( pSession == NULL ){
				break;
			}

			if( (bReturn == FALSE) || ((bReturn == TRUE) && (dwBytesTransfered == 0))	)
			{
				CSessionManager::Delete( (DWORD)pSession->m_pSocket->m_hHandle );
				continue;
			}

			pSession->OnEvent();
		}
	}
};

class CIOCPServer	: public IRunnable {
public:
	CEvent				m_evntShutdown;

	CListenSocket		m_sockListen;

	CIOCP				m_IOCP;

	CThread*			m_pThreads[ 4 ];

	void	run( void )
	{
		m_evntShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

		m_IOCP.Create();

		for( int i=0; i<4; i++ ){
			m_pThreads[ i ]	= new CThread( new CIOCPWorker(&m_IOCP, &m_evntShutdown) );
			m_pThreads[ i ]->Start();
		}


		if( m_sockListen.Create( 54321 ) )
		{
			while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_evntShutdown.m_hObject, 0) )
			{
				SOCKADDR_IN		addr;
				int	len	= sizeof(addr);
				CSocket	sock	= m_sockListen.Accept( (LPSOCKADDR)&addr, len );
				if( sock.IsValid() )
				{
					SOCKET	s	= sock.Detach();
					CSession*	pSession	= (CSession*)CSessionManager::New( (DWORD)s, (LPVOID)&s );
					if( pSession )
					{
						m_IOCP.IoCompletionPort( (HANDLE)s, (ULONG_PTR)pSession );

						pSession->Recv();
					}
				}
			}
		}

		for( int i=0; i<4; i++ ){
			m_pThreads[ i ]->exit();
		}
	}
};



class CKeyValueClient	: public IRunnable {
protected:
	void		run( void )
	{
		CTCPSocket		sock;
		if( sock.Create() )
		{
			struct sockaddr_in	server;
			server.sin_family			= AF_INET;
			server.sin_port				= htons( 54321 );
			server.sin_addr.s_addr		= inet_addr( "127.0.0.1" );
			if( server.sin_addr.S_un.S_addr < 0 )
			{
				struct hostent*	pHost = gethostbyname( "127.0.0.1" );
				if( pHost )
				{
					server.sin_addr.S_un.S_addr	= *(unsigned int*)pHost->h_addr_list[ 0 ];
				}
				else
				{
					return;
				}
			}

			if( sock.Connect((const sockaddr*)&server, sizeof(server)) == 0 )
			{
				std::string		str;
				str = "Set system.version 1.123\n";
				sock.Send( (LPVOID)str.c_str(), str.length() );

				Sleep( 1000 );

				sock.Shutdown( 2 );
			}
			sock.Close();
		}
	}
};


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	INET::WinInet::Startup();

	CThread*	pThreadServer	= new CThread( new CIOCPServer() );
	if( pThreadServer )
	{
		pThreadServer->Start();

		Sleep( 500 );

		CThread*	pThreadClient	= new CThread( new CKeyValueClient() );
		pThreadClient->Start();
		pThreadServer->Wait();
	}

	INET::WinInet::Cleanup();
}
