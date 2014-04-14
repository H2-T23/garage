// ThreadLog.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>
#include <process.h>
#include <windows.h>

class ISyncObject {
public:
	virtual void	Lock() = 0;
	virtual void	Unlock() = 0;
};


class CSyncObject : public ISyncObject {
private:
	CSyncObject(void);
	CSyncObject(CSyncObject const&);
	CSyncObject& operator=(CSyncObject const&);

	ISyncObject*	m_pSync;

public:
	CSyncObject( ISyncObject* pSync ) : m_pSync(pSync) {}
	CSyncObject( ISyncObject& rSync ) : m_pSync(&rSync) {}

	void	Lock(){
		m_pSync->Lock();
	}
	void	Unlock(){
		m_pSync->Unlock();
	}
};

class CCriticalSection : public ISyncObject
{
public:
	CRITICAL_SECTION			m_cs ;
	bool						m_locked ;

	CCriticalSection()
		: m_locked(false)
	{
		::InitializeCriticalSection( &m_cs ) ;
	}

	~CCriticalSection()
	{
		if( m_locked )
			Unlock() ;
		::DeleteCriticalSection( &m_cs ) ;
	}

	void	Lock()
	{
		::EnterCriticalSection( &m_cs ) ;
		m_locked = true ;
	}

	void	Unlock()
	{
		::LeaveCriticalSection( &m_cs ) ;
		m_locked = false ;
	}
} ;

#define	LOG_MAX	1000

class CLog
{
protected:
	CLog() {}

private:
	CLog( const CLog& ) ;
	CLog& operator=( const CLog& ) ;

public:
	int				SeqNo ;
	std::string		strSubject ;
	std::string		strMessage ;

	CLog( char* pSubject, char* pMessage )
		: strSubject(pSubject)
		, strMessage(pMessage)
	{}

	~CLog() {}

private:	
	static CCriticalSection			m_cs;
	static CLog*					m_pLogArray[] ;
	static int						m_nCurrIndex ;
	static int						m_nPrevIndex ;

public:
	static void		Initialize( void ){
		memset(m_pLogArray,0,sizeof(CLog*)*LOG_MAX);
		m_nCurrIndex = m_nPrevIndex	= 0;
	}

	static void		Uninitialize( void ){
		for( int i=0; i<LOG_MAX; i++ ){
			if( m_pLogArray[ i ] ){
				delete m_pLogArray[ i ];
			}
		}
		Initialize();
	}

	static void		Append( CLog* pLog )
	{
		CSyncObject	sync( m_cs );
		sync.Lock() ;

		pLog->SeqNo	= m_nCurrIndex ;
		if( m_pLogArray[ m_nCurrIndex ] )
			delete m_pLogArray[ m_nCurrIndex ];
		m_pLogArray[ m_nCurrIndex ]	= pLog ;
		m_nCurrIndex++;
		if( m_nCurrIndex >= LOG_MAX )
		{
			m_nCurrIndex	= 0 ;
		}

		sync.Unlock() ;	
	}

	static void		Snapshot( std::vector<CLog*>& Logs )
	{
		int		nCurrIndex	= m_nCurrIndex ;
		int		nPrevIndex	= m_nPrevIndex ;

		int	nSnapNums = 0 ;
		if( nPrevIndex == nCurrIndex )
		{
			return ;
		}
		else
		if( nPrevIndex == 0 )
		{
			nSnapNums	= nCurrIndex ;
		}
		else
		if( nPrevIndex < nCurrIndex )
		{
			nSnapNums	= nCurrIndex - nPrevIndex ;
		}
		else
		{
			nSnapNums	= (LOG_MAX - nPrevIndex) + nCurrIndex ;
		}

		int	nIdx = nPrevIndex ;
		for( int i=0; i<nSnapNums; i++ )
		{
			Logs.push_back( m_pLogArray[ nIdx ] ) ;
			m_pLogArray[ nIdx ] = NULL ;

			nIdx++ ;
			if( nIdx >= LOG_MAX )
				nIdx = 0 ;
		}

		m_nPrevIndex	= nCurrIndex ;
	}
} ;

CCriticalSection		CLog::m_cs;
CLog*					CLog::m_pLogArray[ LOG_MAX ] ;
int						CLog::m_nCurrIndex	= 0 ;
int						CLog::m_nPrevIndex	= 0 ;

unsigned __stdcall	PushThread( void* )
{
	while( 1 )
	{
		printf( "PushThread[%d]\n", GetCurrentThreadId() ) ;
		CLog::Append( new CLog("Subject:PushThread", "Message:Push") ) ;
		Sleep( 100 ) ;
	}
	return 0 ;
}


unsigned __stdcall	SnapshotThread( void* )
{
	static int	nPrevSeqNo = -1 ;
	while( 1 )
	{
		Sleep( 5000 ) ;
		printf( "SnapshotThread[%d]\n", GetCurrentThreadId() ) ;

		std::vector<CLog*>	vec ;
		CLog::Snapshot( vec ) ;

		int	nCnt = vec.size() ;
		for( int i=0; i<nCnt; i++ )
		{
			CLog*	pLog = vec[ i ] ;

		#if 1
			if( (pLog->SeqNo - nPrevSeqNo) != 1 )
			{
				::DebugBreak() ;
			}
			if( LOG_MAX <= (pLog->SeqNo + 1) )
				nPrevSeqNo	= -1 ;
			else
				nPrevSeqNo	= pLog->SeqNo ;
		#endif

			printf( "%010d:%s:%s\n",	pLog->SeqNo					, 
										pLog->strSubject.c_str()	, 
										pLog->strMessage.c_str()	) ;
			delete pLog ;
		}
	}
	return 0 ;
}


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE		hThreads[ 5 ] ;
	unsigned	dwThreadsId[ 5 ] ;

	CLog::Initialize();

	int	idx = 0 ;
	for( ; idx<4; idx++ )
	{
		hThreads[ idx ]	= (HANDLE)
			_beginthreadex( NULL, 0, PushThread, NULL, 0, &dwThreadsId[ idx ] ) ;
	}

	hThreads[ idx ]	= (HANDLE)
		_beginthreadex( NULL, 0, SnapshotThread, NULL, 0, &dwThreadsId[ idx ] ) ;

	WaitForMultipleObjects( 5, hThreads, TRUE, -1 ) ;
	return 0;
}

