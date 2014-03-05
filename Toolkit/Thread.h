#pragma once


namespace MT {

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CSyncObject {
	public:
		HANDLE		m_hObject;

	public:
		explicit CSycnObject( LPCTSTR lpstrName );
		virtual ~CSyncObject( void );

		virtual BOOL	Lock( DWORD dwTimeout = INFINITE );
		virtual BOOL	Unlock() = 0;
		virtual BOOL	Unlock( LONG, LPLONG ){
			return TRUE;
		}

	public:
		operator HANDLE() const {
			return m_hObject;
		}
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CSemaphore : public CSyncObject {
	public:
		CSemaphore( LONG lInitialCount = 1, LONG lMaxCount = 1, LPCTSTR lpstrName= NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL );
		virtual ~CSemaphore( void );

	public:
		BOOL	Unlock( void );
		BOOL	Unlock( LONG lCount, LPLONG, lprevCount = NULL );
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMutex : public CSyncObject {
	public:
		CMutex( BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTE lpszAttribute = NULL );
		virtual ~CMutex( void );

	public:
		BOOL	Unlock( void );
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CEvent : public CSyncObject {
	public:
		CEvent( BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpszAttribute = NULL );
		virtual ~CEvent( void );

	public:
		BOOL	SetEvent( void );
		BOOL	PulseEvent( void );
		BOOL	ResetEvent( void );
		BOOL	Unlock( void );
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CCriticalSection : public CSyncObject {
	private:
		CRITICAL_SECTION			m_sect;

		BOOL		Init( void );

	public:
		CCriticalSection( void );
		virtual ~CCriticalSection( void );

	public:
		BOOL	Lock( void );
		BOOL	Lock( DWORD dwTimeout );
		BOOL	Unlock( void );

	public:
		operator CRITICAL_SECTION* () const { return &m_sect; }		
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CSingleLock {
	protected:
		CSyncObject*	m_pObject;
		HANDLE			m_hObject;
		BOOL			m_bAcquired;

	public:
		explicit CSingleLock( CSyncObject* pObject, BOOL bIntialLock = FALSE );
		~CSingleLock( void );

		BOOL	Lock( DWORD dwTimeout = INFINTE );
		BOOL	Unlock( void );
		BOOL	Unlock( LONG lCount, LPLONG lPrevCount = NULL );
		BOOL	IsLocked( void );
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMultiLock {
	protected:
		HANDLE			m_hPreallocated[ 8 ];
		BOOL			m_bPreallocated[ 8 ];

		CSycnObject*const*	m_ppObjectArray;
		HANDLE*				m_pHandleArray;
		HANDLE*				m_bLockedArray;
		DWORD				m_dwCount;

	public:
		CMultiLock( CSycnObject* ppObjects[], DWORD dwCount, BOOL bInitialLock = FALSE );
		~CMultiLock( void );

		BOOL	Lock( DWORD dwTimeout = INFINITE, BOOL bWaitForAll = TRUE, DWORD dwWakeMask = 0 );
		BOOL	Unlock( void );
		BOOL	Unlock( LONG lCount, LPLONG lPrevCount = NULL );
		BOOL	IsLocked( DWORD dwItem );
	};

	namespace SDK 
	{
		/**********************************************************************************
		 *
		 *
		 *
		 */
		class CReadWriteLock : private NonCopyable {
		private:
			int						m_nReaderCount;

			HANDLE					m_hWriterEvent;
			HANDLE					m_hNoReaderEvent;

			CRITICAL_SECTION		m_csLockWriter;
			CRITICAL_SECTION		m_csReaderCount;

		private:
			void	IncrementReaderCount( void ){
				EnterCriticalSection( &m_csReaderCount );
				{
					m_nReaderCount++;
					ResetEvent( m_hNoReaderEvent );
				}
				LeaveCriticalSection( &m_csReaderCount );
			}

			void	DecrementReaderCount( void ){
				EnterCriticalSection( &m_csReaderCount );
				{
					m_nReaderCount--;
					if( m_nReaderCount <= 0 ){
						SetEvent( m_hNoReaderEvent );
					}
				}
				LeaveCriticalSection( &m_csReaderCount );
			}

		public:
			CReadWriteLock( void ) 
			 : m_nReaderCount(0), m_hWriterEvent(NULL), m_hNoReaderEvent(NULL) {
				m_hWriterEvent		= CreateEvent( NULL, TRUE, TRUE, NULL );
				m_hNoReaderEvent	= CreateEvent( NULL, TRUE, TRUE, NULL );

				InitializeCriticalSection( &m_csLockWriter );
				InitializeCriticalSection( &m_csReaderCount );
			}

			virtual ~CReadWriteLock( void ){
				DeleteCriticalSection( &m_csLockWriter );
				DeleteCriticalSection( &m_csReaderCount );

				CloseHandle( m_hWriterEvent );
				CloseHandle( m_hNoReaderEvent );
			}

			void	LockReader( void ){
				while( 1 ){
					WaitForSingleObject( m_hWriterEvent, INFINITE );

					IncrementReaderCount();

					if( WaitForSingleObject(m_hWriterEvent, 0) != WAIT_OBJECT_0 ){
						DecrementReaderCount();
					}else{
						break;
					}
				}		
			}

			void	UnlockReader( void ){
				DecrementReaderCount();
			}

			void	LockWriter( void ){
				EnterCriticalSection( &m_csLockWriter );
				{
					WaitForSingleObject( m_hWriterEvent, INFINITE );

					ResetEvent( m_hWriterEvent );

					WaitForSingleObject( m_hNoReaderEvent, INFINITE );
				}
				LeaveCriticalSection( &m_csLockWriter );
			}

			void	UnlockWriter( void ){
				SetEvent( m_hWriterEvent );
			}
		};
	}
}

namespace TM {

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class ThreadContext {
	protected:
		HANDLE		m_hThread;
		DWORD		m_dwThreadID;

		ThreadContext( void ) : m_hThread(NULL), m_dwThreadID(0) {}
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class IThread : public ThreadContext {
	private:
		static int		EntryPoint( LPVOID lpParam ){
			IThread* pThread = (IThread*)lpParam;
			if( pThread ){
				return pThread->run( lpParam );
			}
			return 0;
		}

	protected:
		virtual int		run( LPVOID lpParam )		= 0;

	public:
		BOOL		Start( LPVOID lpParam ){
			return FALSE;
		}

		BOOL		Terminate( void ){
			return FALSE;
		}

		BOOL		Suspend( void ){
			return FALSE;
		}

		BOOL		Resume( void ){
			return FALSE;
		}
	};
}