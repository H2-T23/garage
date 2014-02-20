#pragma once

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