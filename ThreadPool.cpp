//threadpool.cpp 
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <queue>

#ifndef TRACE
#define	TRACE	printf
#endif

//------------------------------------------------------------------------
/**
 *
 *
 */
class IFCommand {
public:
	virtual int		execute() = 0;
};

//------------------------------------------------------------------------
/**
 *
 *
 */
class CHttpClient : public IFCommand {
public:
	int		execute()
	{
		return 0;
	}
};

//------------------------------------------------------------------------
/**
 *
 *
 */
class CFtpClient : public IFCommand {
public:
	int		execute()
	{
		return 0;
	}
};

//------------------------------------------------------------------------
/**
 *
 *
 */
 class CSync
 {
 private:
 	CSync(){}
 	CSync( const CSync& rSync ){}
 	
 	LPCRITICAL_SECTION	m_lpCS;
 	
 public:
 	CSync( LPCRITICAL_SECTION lpCS ) : m_lpCS(lpCS)
 	{
 		Lock();
 	}
 	~CSync()
 	{
 		Unlock();
 	}
 	
 	void	Lock()
 	{
 		EnterCriticalSection(m_lpCS);
 		TRACE("lock.\n");
 	}
 	
 	void	Unlock()
 	{
 		if( m_lpCS )
 		{
 			LeaveCriticalSection(m_lpCS);
 			TRACE("unlock.\n");
 		}
 		m_lpCS = NULL;
 	}
 };

//------------------------------------------------------------------------
/**
 *
 *
 */
class CRequest
{
public:
	int				nKind;
	std::string		strSubject;
	
public:
	CRequest()
		:	nKind(0)
	{
	}
};

//------------------------------------------------------------------------
/**
 *
 *
 */
class CChannel
{
private:
	std::queue<CRequest*>	m_queue;
	HANDLE					m_notifyevent;
	HANDLE					m_exitevent;
	CRITICAL_SECTION		m_cs;
	
public:
	CChannel()
	{
		InitializeCriticalSection(&m_cs);
		m_notifyevent	= CreateEvent(	NULL
									,	FALSE	// automatically reset
									,	FALSE	// nonsinaled 
									,	NULL	// TEXT("notify")
									);
									
		m_exitevent		= CreateEvent(	NULL
									,	TRUE	// manual reset
									,	FALSE	// nonsinaled 
									,	NULL	// TEXT("exit")
									);
	}
	
	~CChannel()
	{
		DeleteCriticalSection(&m_cs);
	}
	
	int			Append( CRequest* pReq )
	{
		CSync	sync(&m_cs);
		TRACE("append:%d\n",pReq);
		
		if( pReq == NULL )
		{
			SetEvent( m_exitevent );
			return 0;
		}
		
		m_queue.push( pReq );
		SetEvent( m_notifyevent );
		
		return m_queue.size();
	}
	
	CRequest*	GetNext()
	{
	//	WaitForSingleObject( m_notifyevent, INFINITE );
		HANDLE	handles[2];
		handles[0] = m_notifyevent;
		handles[1] = m_exitevent;
		DWORD dwRet = WaitForMultipleObjects( 2, handles, FALSE, INFINITE );
		if( dwRet == (WAIT_OBJECT_0 + (sizeof(handles)/sizeof(HANDLE)) - 1) )
		{
			TRACE("request is exit.\n");
			return NULL;
		}
		
		CSync	sync(&m_cs);
		
		int	nSize = m_queue.size();
		if( nSize > 0 || !m_queue.empty() )
		{
			TRACE("%d:GetNext() valid.\n",nSize);
			CRequest* pReq = m_queue.front();
			m_queue.pop();
			return pReq;
		}
		else
		{
			TRACE("%d:GetNext() invalid.\n",nSize);
			return NULL;
		}
	}
};

CChannel	channel;

//------------------------------------------------------------------------
/**
 *
 *
 */
unsigned __stdcall		WorkerThread( void* pParam )
{
	DWORD	dwThreadId = GetCurrentThreadId();
	TRACE("[%d]:beginning WorkerThread.\n",dwThreadId);
	while( 1 )
	{
		CRequest*	pReq = channel.GetNext();
		if( pReq == NULL )
		{
			TRACE("[%d]:Request is null pointer.\n",dwThreadId);
			break;
		}
		
		switch( pReq->nKind )
		{
		case 1:
			TRACE("[%d]:kind:one\n",dwThreadId);
			break;
			
		case 2:
			TRACE("[%d]:kind:two\n",dwThreadId);
			break;
			
		case 3:
			TRACE("[%d]:kind:three(begining wait:10000ms)\n",dwThreadId);
			Sleep(10000);
			TRACE("[%d]:kind:three(finished wait:10000ms)\n",dwThreadId);
			break;
		
		default:
			TRACE("[%d]:kind:other\n",dwThreadId);
		}
		
		if( pReq->strSubject.length() > 0 )
			TRACE("[%d]:%s\n\n",dwThreadId,pReq->strSubject.c_str());
		else
			TRACE("[%d]:subject is null.\n",dwThreadId);
		
		delete pReq;
	}
	TRACE("[%d]:destroy WorkerThread.\n",dwThreadId);
	return 1;
}

//------------------------------------------------------------------------
/**
 *
 *
 */
void	PumpRequest()
{
//	HANDLE hThread = (HANDLE)_beginthreadex( NULL, 0, WorkerThread, NULL, 0, NULL );
	HANDLE hThreads[3];
	
	hThreads[0]	= (HANDLE)_beginthreadex( NULL, 0, WorkerThread, NULL, 0, NULL );
	hThreads[1]	= (HANDLE)_beginthreadex( NULL, 0, WorkerThread, NULL, 0, NULL );
	hThreads[2]	= (HANDLE)_beginthreadex( NULL, 0, WorkerThread, NULL, 0, NULL );

	channel.Append( new CRequest );
	Sleep( 100 );
	
	char buf[256];
	do {
		printf( ">" );
		fgets( buf, sizeof(buf), stdin );
		TRACE( ":->%s\n", buf );
		if( (strncmp(buf,"q",1) == 0) || (strncmp(buf,"quit",4) == 0) )
		{
			TRACE("quit.\n");
			channel.Append( NULL );
			break;
		}

		if( strncmp(buf,"wait",4) == 0 )
		{
			CRequest*	pReq = new CRequest();
			pReq->nKind = 3;
			pReq->strSubject = buf;
			channel.Append( pReq );
		}
		else
		{
			CRequest*	pReq = new CRequest();
			pReq->nKind = 1;
			pReq->strSubject = buf;
			channel.Append( pReq );
		}		
	} while( 1 );
	
	TRACE("wait for workerthread.\n");
//	WaitForSingleObject( hThread, INFINITE );
	WaitForMultipleObjects( 3, hThreads, TRUE, INFINITE );
}

//------------------------------------------------------------------------
/**
 *
 *
 */
int main( int args, char** arg )
{
	PumpRequest();
	return 0;
}

