#pragma once

#include "NonCopyable.h"

/**********************************************************************************
 *
 *
 *
 */
class CReadWriteLock : private NonCopyable {
private:
	int						m_nReaderCount;

	CEvent*					m_pWriterEvent;
	CEvent*					m_pNoReaderEvent;

	CCriticalSection		m_csLockWriter;
	CCriticalSection		m_csReaderCount;

#if 0
	class CReaderCount {
	private:
		int					m_nCount;
		CCriticalSection	m_cs;

	public:
		CReaderCount( void ) : m_nCount(0) {
		}

		void	Increment( CEvent* pEvent ){
			CSingleLock		sLock( &m_cs, TRUE );

			m_nCount++;
			pEvent->ResetEvent();
		}

		void	Decrement( CEvent* pEvent ){
			CSingleLock		sLock( &m_cs, TRUE );

			m_nCount--;
			pEvent->SetEvent();
		}
	};
#endif

private:
	void	IncrementReaderCount( void ){
		CSingleLock	sLock( &m_csReaderCount, TRUE );

		m_nReaderCount++;
		m_pNoReaderEvent->ResetEvent();
	}

	void	DecrementReaderCount( void ){
		CSingleLock	sLock( &m_csReaderCount, TRUE );

		m_nReaderCount--;
		if( m_nReaderCount <= 0 ){
			m_pNoReaderEvent->SetEvent();
		}
	}

public:
	CReadWriteLock( void ) 
	 : m_nReaderCount(0)
	 , m_pWriterEvent(NULL)
	 , m_pNoReaderEvent(NULL)
	{
		m_pWriterEvent		= new CEvent( TRUE, TRUE, NULL, NULL );
		m_pNoReaderEvent	= new CEvent( TRUE, TRUE, NULL, NULL );
	}

	virtual ~CReadWriteLock( void ){
		delete m_pWriterEvent;
		delete m_pNoReaderEvent;
	}

	void	LockReader( void ){
		while( 1 ){
			WaitForSingleObject( m_pWriterEvent->m_hObject, INFINITE );

			IncrementReaderCount();

			if( WaitForSingleObject(m_pWriterEvent->m_hObject, 0) != WAIT_OBJECT_0 )
			{
				DecrementReaderCount();
			}
			else
			{
				break;
			}
		}		
	}

	void	UnlockReader( void ){
		DecrementReaderCount();
	}

	void	LockWriter( void ){
		CSingleLock	sLock( &m_csLockWriter, TRUE );

		WaitForSingleObject( m_pWriterEvent->m_hObject, INFINITE );

		m_pWriterEvent->ResetEvent();

		WaitForSingleObject( m_pNoReaderEvent->m_hObject, INFINITE );
	}

	void	UnlockWriter( void ){
		m_pWriterEvent->SetEvent();
	}
};
