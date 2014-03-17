#pragma once

#include <mswsock.h>
#include "Inet.h"

namespace ECHO_IOCP {
	enum {
		SERVER_ADDRESS	= INADDR_LOOPBACK,
		SERVICE_PORT	= 4000,
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CIOCP {
	protected:
		HANDLE		m_hIOCP;

	public:
		CIOCP( void ) : m_hIOCP(NULL) {
		}
		~CIOCP( void ){
		}

		BOOL	Create( void ){
			m_hIOCP	= ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
			if( m_hIOCP == NULL ){
				return FALSE;
			}
			return TRUE;
		}

		void	Close( void ){
			if( m_hIOCP )
				::CloseHandle( m_hIOCP );
			m_hIOCP	= NULL;
		}

		void	PostQueuedCompletionStatus( void ){
			::PostQueuedCompletionStatus( m_hIOCP, 0, (DWORD)NULL, NULL );
		}

		BOOL	GetQueuedCompletionStatus( LPDWORD lpNumberOfBytesTransferred, PULONG_PTR lpCompletionKey, LPOVERLAPPED *lpOverlapped, DWORD dwMilliseconds ){
			return ::GetQueuedCompletionStatus(m_hIOCP, lpNumberOfBytesTransferred, lpCompletionKey, lpOverlapped, dwMilliseconds );
		}

		BOOL	IoCompletionPort( HANDLE hHandle, ULONG_PTR lpBuf ){
			if( ::CreateIoCompletionPort( hHandle, m_hIOCP, lpBuf, 0 ) == m_hIOCP ){
				return TRUE;
			}
			return FALSE;
		}
	}; 
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CIocpState {
	public:
		enum OPERATION {
			OP_NONE			= 0	,
			OP_ACCEPT			,
			OP_READ				,
			OP_WRITE			,

			MAX_BUF			= 1024	,
		};

		INET::CSocket	sock;
		OPERATION		operation;
		DWORD			length;
		char			buf[ MAX_BUF ];

		CIocpState( void ) : operation(OP_NONE), length(0) {}
		CIocpState( INET::CSocket& s ) : sock(s), operation(OP_NONE), length(0) {
		}

		virtual ~CIocpState( void ){
			sock.Close();
		}

		BOOL	Recv( WSAOVERLAPPED* pOverlapped )
		{
			DWORD	dwFlags	= 0;
			WSABUF	wsabuf	= { MAX_BUF, buf };

			memset(pOverlapped, 0, sizeof(WSAOVERLAPPED));
			operation	= OP_READ;

			if( sock.WSARecv(&wsabuf, 1, NULL, &dwFlags, pOverlapped, NULL) == SOCKET_ERROR )
			{
				if( ::WSAGetLastError() == WSA_IO_PENDING )
				{
					return TRUE;
				}
			}
			return FALSE;
		}

		BOOL	Send( WSAOVERLAPPED* pOverlapped )
		{
			WSABUF	wsabuf	= { length, buf };

			memset(pOverlapped, 0, sizeof(WSAOVERLAPPED));
			operation	= OP_WRITE;

			if( sock.WSASend(&wsabuf, 1, NULL, 0, pOverlapped, NULL) == SOCKET_ERROR )
			{
				if( WSAGetLastError() == WSA_IO_PENDING )
				{
					return TRUE;
				}
			}

			return FALSE;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	GUID					GuidAcceptEx	= WSAID_ACCEPTEX;
	class CListener {
	protected:
		CListener( void );

	public:
		INET::CSocket			m_sock;
		CIocpState				m_state;
		WSAOVERLAPPED			m_overlapped;

		CIOCP*					m_pIOCP;

		LPFN_ACCEPTEX			pfAcceptEx;


		void		LoadAcceptExFunction( void ){
			DWORD	dwBytes;
			WSAIoctl( m_sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &pfAcceptEx, sizeof(pfAcceptEx), &dwBytes, NULL, NULL );
		}

	public:
		CListener( CIOCP* pIOCP ) : m_pIOCP(pIOCP), pfAcceptEx(NULL) {
			m_sock	= Create();
			
			m_state.sock		= 0;
			m_state.operation	= CIocpState::OP_ACCEPT;

			m_pIOCP->IoCompletionPort( (HANDLE)m_sock, (ULONG_PTR)&m_state );
		}

		INET::CSocket		Create( void ){
			INET::CSocket	sock(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			return sock;
		}

		BOOL		Listen( void ){
			if( m_sock.Listen(100) == SOCKET_ERROR )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL		Bind( void ){
			struct sockaddr_in	sin;
			sin.sin_family			= AF_INET;
			sin.sin_addr.s_addr		= htonl( SERVER_ADDRESS );
			sin.sin_port			= htons( SERVICE_PORT );
		   
			if( m_sock.Bind((SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR )
			{
				return FALSE;
			}
			return TRUE;
		}

		BOOL		Accept( void ){
			INET::CSocket	acceptor	= Create();

			DWORD	dwExpected	= sizeof(struct sockaddr_in) + 16;
			
			m_state.sock	= acceptor;
			memset( &m_overlapped, 0, sizeof(WSAOVERLAPPED) );

			if( !pfAcceptEx( m_sock, acceptor, m_state.buf, 0, dwExpected, dwExpected, NULL, &m_overlapped ) )
			{
				if( WSAGetLastError() == ERROR_IO_PENDING )
				{
					return TRUE;
				}
			}
			return FALSE;
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CIOCPControler {
	public:
		CIOCP*			m_pIOCP;
		CListener*		m_pListener;


		CIOCPControler( void ) : m_pIOCP(NULL), m_pListener(NULL) {
			INET::WinInet::Startup();
		}

		~CIOCPControler( void ) {
			if( m_pIOCP )
				delete m_pIOCP;
			if( m_pListener )
				delete m_pListener;

			INET::WinInet::Cleanup();
		}

		void	init( void )
		{
			m_pIOCP	= new CIOCP();
			m_pIOCP->Create();

			m_pListener = new CListener( m_pIOCP );
			m_pListener->Bind();
			m_pListener->Listen();
			m_pListener->LoadAcceptExFunction();
			m_pListener->Accept();
		}

		void	run( void )
		{
			DWORD			length;

			WSAOVERLAPPED*	pOverlapped		= NULL;
			CIocpState*		pState			= NULL;

			while( 1 )
			{
				BOOL	resultOK	= m_pIOCP->GetQueuedCompletionStatus(&length, (PULONG_PTR)&pState, &pOverlapped, INFINITE);
				switch( pState->operation ){
				case CIocpState::OP_ACCEPT:
					if( resultOK )
					{
						pState->sock.SetOption( SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&m_pListener->m_sock.m_hHandle, sizeof(m_pListener->m_sock.m_hHandle) );

						CIocpState*	pNewState = new CIocpState( pState->sock );

						m_pIOCP->IoCompletionPort( (HANDLE)pNewState->sock, (ULONG_PTR)pNewState );

						pNewState->Recv( new WSAOVERLAPPED );

						m_pListener->Accept();
					}
					else
					{
						pState->sock.Close();
						m_pListener->Accept();
					}
					break;

				case CIocpState::OP_READ:
					if( resultOK )
					{
						if( length > 0 )
						{
							DBG::TRACE(_T("* read operation completed, %d bytes read."), length);
							pState->length	= length;
							pState->Send( pOverlapped );
						}
						else
						{
							DBG::TRACE(_T("* connection closed by client."));
							delete pState;
							delete pOverlapped;
						}
					}
					else
					{
						DBG::TRACE(_T("* error %d in recv, assuming connection was reset by client."), GetLastError());
						delete pState;
						delete pOverlapped;
					}
					break;

				case CIocpState::OP_WRITE:
					if( resultOK )
					{
						if( length > 0 )
						{
							DBG::TRACE(_T("* write operation completed."));
							pState->Recv( pOverlapped );
						}
						else
						{
							DBG::TRACE(_T("* connection closed by client."));
							delete pState;
							delete pOverlapped;
						}
					}
					else
					{
						DBG::TRACE(_T("* error %d on send, assuming connection was reset."), GetLastError());
						delete pState;
						delete pOverlapped;
					}
					break;

				default:
					delete pState;
					delete pOverlapped;
					break;
				}
			}
		}
	};
}//

