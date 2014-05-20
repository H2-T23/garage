#pragma once

#include "SessionProtocol.h"

namespace IOCP {
	using namespace MT;
	using namespace INET;
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

		BOOL	IoCompletionPort( SOCKET sock, ULONG_PTR lpBuf ){
			return CIOCP::IoCompletionPort( (HANDLE)sock, lpBuf );
		}
	}; 
	/**********************************************************************************
	 *
	 *
	 *
	 */
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
					CSessionManager::Instance().Delete( (DWORD)pSession->m_pSocket->m_hHandle );
					continue;
				}

				pSession->OnEvent( dwBytesTransfered );
			}
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	template<class PROTOCOL, UINT PORT, UINT THREAD_NUMS=4>
	class TIOCPServer	: public IRunnable {
	public:
		CIOCP				m_IOCP;
		CEvent				m_evntShutdown;
		CListenSocket		m_sockListen;

		CThread*			m_pThreads[ THREAD_NUMS ];

		void	run( void )
		{
			m_evntShutdown.m_hObject	= ::CreateEvent(NULL, TRUE, FALSE, NULL);

			m_IOCP.Create();

			for( UINT i=0; i<THREAD_NUMS; i++ ){
				m_pThreads[ i ]	= new CThread( new CIOCPWorker(&m_IOCP, &m_evntShutdown) );
				m_pThreads[ i ]->Start();
			}

			if( m_sockListen.Create( PORT ) )
			{
				while( WAIT_OBJECT_0 != ::WaitForSingleObject(m_evntShutdown.m_hObject, 0) )
				{
					SOCKADDR_IN		addr;
					int	len	= sizeof(addr);
					CSocket	sock	= m_sockListen.Accept( (LPSOCKADDR)&addr, len );
					if( sock.IsValid() )
					{
						SOCKET	s	= sock.Detach();
						CSession*	pSession	= (CSession*)CSessionManager::Instance().New<PROTOCOL>( (DWORD)s, (LPVOID)&s );
						if( pSession )
						{
							m_IOCP.IoCompletionPort( s, (ULONG_PTR)pSession );

							pSession->OnEvent( OP_CONNECT );
						}
					}
				}
			}

			for( UINT i=0; i<THREAD_NUMS; i++ ){
				m_pThreads[ i ]->exit();
			}
		}
	};
}// End of IOCP.
