#pragma once
#include <process.h>

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
		explicit CSyncObject( LPCTSTR lpstrName ) : m_hObject(NULL) {
		}
		virtual ~CSyncObject( void ){
			if( m_hObject )
				::CloseHandle( m_hObject );
		}

		virtual BOOL	Lock( DWORD dwTimeout = INFINITE ){
			::WaitForSingleObject(m_hObject, dwTimeout);
			return TRUE;
		}
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
		CSemaphore( LONG lInitialCount = 1, LONG lMaxCount = 1, LPCTSTR lpstrName= NULL, LPSECURITY_ATTRIBUTES lpsaAttribute = NULL ) : CSyncObject(NULL) {
			m_hObject	= ::CreateSemaphore(lpsaAttribute, lInitialCount, lMaxCount, lpstrName);
		}
		virtual ~CSemaphore( void ){
		}

	public:
		BOOL	Unlock( void ){
			return Unlock( 1 );
		}
		BOOL	Unlock( LONG lCount, LPLONG lprevCount = NULL ){
			return ::ReleaseSemaphore(m_hObject, lCount, lprevCount);
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMutex : public CSyncObject {
	public:
		CMutex( BOOL bInitiallyOwn = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpszAttribute = NULL ) : CSyncObject(NULL) {
			m_hObject	= ::CreateMutex(lpszAttribute, bInitiallyOwn, lpszName);
		}
		virtual ~CMutex( void ){
		}

	public:
		BOOL	Unlock( void ){
			return ::ReleaseMutex( m_hObject );
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CEvent : public CSyncObject {
	public:
		CEvent( BOOL bInitiallyOwn = FALSE, BOOL bManualReset = FALSE, LPCTSTR lpszName = NULL, LPSECURITY_ATTRIBUTES lpszAttribute = NULL ) : CSyncObject(NULL) {
			m_hObject	= ::CreateEvent(lpszAttribute, bManualReset, bInitiallyOwn, lpszName);
		}
		virtual ~CEvent( void ){
		}

	public:
		BOOL	SetEvent( void ){
			return ::SetEvent( m_hObject );
		}
		BOOL	PulseEvent( void ){
			return ::PulseEvent( m_hObject );
		}
		BOOL	ResetEvent( void ){
			return ::ResetEvent( m_hObject );
		}
		BOOL	Unlock( void ){
			return SetEvent();
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CCriticalSection : public CSyncObject {
	private:
		CRITICAL_SECTION			m_sect;

		BOOL		Init( void ){
			__try
			{
				::InitializeCriticalSection( &m_sect );
			}
			__except(STATUS_NO_MEMORY == GetExceptionCode())
			{
				return FALSE;
			}
			return TRUE;
		}

	public:
		CCriticalSection( void ) : CSyncObject(NULL) {
			Init();				
		}
		virtual ~CCriticalSection( void ){
			::DeleteCriticalSection( &m_sect );
		}

	public:
		BOOL	Lock( void ){
			__try
			{
				::EnterCriticalSection( &m_sect );
			}
			__except(STATUS_NO_MEMORY == GetExceptionCode())
			{
				return FALSE;
			}
			return TRUE;
		}
		BOOL	Lock( DWORD dwTimeout ){
			return Lock();
		}
		BOOL	Unlock( void ){
			::LeaveCriticalSection( &m_sect );
			return TRUE;
		}

	public:
		operator const CRITICAL_SECTION*() const { return (const CRITICAL_SECTION*)&m_sect; }		
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
		explicit CSingleLock( CSyncObject* pObject, BOOL bIntialLock = FALSE )
			: m_pObject(pObject), m_hObject(NULL), m_bAcquired(FALSE) {
			if( bIntialLock )
				Lock();
		}
		~CSingleLock( void ){
			Unlock();
		}

		BOOL	Lock( DWORD dwTimeout = INFINITE ){
			if( m_pObject && !m_bAcquired )
				return(m_bAcquired = m_pObject->Lock( dwTimeout ));
			return(m_bAcquired);
		}
		BOOL	Unlock( void ){
			if(m_bAcquired)
				return(m_bAcquired = !m_pObject->Unlock());
			return(m_bAcquired);
		}
		BOOL	Unlock( LONG lCount, LPLONG lPrevCount = NULL ){
			if(m_bAcquired)
				return(m_bAcquired = !m_pObject->Unlock(lCount, lPrevCount));
			return(m_bAcquired);
		}
		BOOL	IsLocked( void ){
			return(m_bAcquired);
		}
	};

	/**********************************************************************************
	 *
	 *
	 *
	 */
	class CMultiLock {
	protected:
		HANDLE					m_hPreallocated[ 8 ];
		BOOL					m_bPreallocated[ 8 ];

		CSyncObject* const *	m_ppObjectArray;
		HANDLE*					m_pHandleArray;
		HANDLE*					m_bLockedArray;
		DWORD					m_dwCount;

	public:
		CMultiLock( CSyncObject* ppObjects[], DWORD dwCount, BOOL bInitialLock = FALSE ){
		}
		~CMultiLock( void ){
		}

		BOOL	Lock( DWORD dwTimeout = INFINITE, BOOL bWaitForAll = TRUE, DWORD dwWakeMask = 0 ){
		}
		BOOL	Unlock( void ){
		}
		BOOL	Unlock( LONG lCount, LPLONG lPrevCount = NULL ){
		}
		BOOL	IsLocked( DWORD dwItem ){
		}
	};

	namespace SDK 
	{
		/**********************************************************************************
		 *
		 *
		 *
		 */
		class NonCopyable {
		private:
			NonCopyable(const NonCopyable&);
			NonCopyable&	operator=(const NonCopyable&);
		protected:
			NonCopyable(){}
			~NonCopyable(){}
		};
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

namespace MT {
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
	class CThread : public IRunnable {
	private:
		static unsigned __stdcall		EntryPoint( void* lpParam ){
			CThread* pThread	= static_cast<CThread*>( lpParam );
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
		CThread( void ) : m_pRunnable(NULL) {
		}

		CThread( IRunnable* runnable ) : m_pRunnable(runnable) {
		}

		virtual ~CThread( void ){
			if(m_pRunnable){
				delete m_pRunnable;
			}
			m_pRunnable	= NULL;
		}

		void	Start( unsigned init = 0 ){
			HANDLE	hThread = (HANDLE)::_beginthreadex(NULL, 0, &CThread::EntryPoint, (void*)(this), init, &m_Id);
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

		void	Wait( DWORD dwMilliseconds = INFINITE ){
			WaitForSingleObject(m_hThread, dwMilliseconds);
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class ThreadContext {
	public:
		HANDLE		m_hThread;
		UINT		m_uThreadID;

		ThreadContext( void ) : m_hThread(NULL), m_uThreadID(0) {}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class IThread : public ThreadContext {
	private:
		static unsigned __stdcall	EntryPoint( void* lpParam ){
			IThread* pThread = (IThread*)lpParam;
			if( pThread ){
				return pThread->run( lpParam );
			}
			return 0;
		}

	protected:
		virtual unsigned		run( LPVOID lpParam )		= 0;

//	public:
//		HANDLE			m_hThread;
//		UINT			m_uThreadID;

	public:
		IThread( void ) : ThreadContext() /*, m_hThread(NULL), m_uThreadID(0)*/ {
		}

		HANDLE			Start( void ){
			m_hThread	= (HANDLE)::_beginthreadex( NULL, 0, &IThread::EntryPoint, this, 0, &m_uThreadID );
			return m_hThread;
		}

		virtual void	Close( void ){
			Terminate();
		}

		void			Terminate( void ){
			if( m_hThread )
				::CloseHandle( m_hThread );
			m_hThread	= NULL;
		}

		virtual BOOL			Suspend( void ){
			return FALSE;
		}

		virtual BOOL			Resume( void ){
			return FALSE;
		}

		virtual DWORD			Join( DWORD dwTimeout = INFINITE ){
			return ::WaitForSingleObject(m_hThread, dwTimeout);
		}

		virtual DWORD			Wait( DWORD dwTimeout = INFINITE ){
			return ::WaitForSingleObject(m_hThread, dwTimeout);
		}
	};
	/**********************************************************************************
	 *
	 *
	 *
	 */
	class IUIThread : public IThread {
	public:
		void	Quit( void ){
			::PostThreadMessage((DWORD)m_uThreadID, WM_QUIT, 0, 0);
		}
	};
}