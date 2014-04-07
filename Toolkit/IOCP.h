#pragma once

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
