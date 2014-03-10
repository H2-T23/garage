#pragma once
#include "NonCopyable.h"

/**********************************************************************************
 *
 *
 *
 */
class CLogItem {
	friend class CLogList;

private:
	enum {
		LENGTH_TIME		= 32	,
		LENGTH_SUBJECT	= 256	,
		SIZE_TIME		= (sizeof(TCHAR) * LENGTH_TIME)		,
		SIZE_SUBJECT	= (sizeof(TCHAR) * LENGTH_SUBJECT)	,
		ERROR_BODY		= -1	,
	};

	DWORD		m_ID;
	TCHAR		m_Time[ LENGTH_TIME ];
	TCHAR		m_Subject[ LENGTH_SUBJECT ];
	int			m_BodySize;
	LPVOID		m_lpBody;
	BOOL		m_bRemove;

private:
	static DWORD	UpdateID( void ){
		static DWORD	id = 0;
	//	return(++id);
		return(id++);
	}

	static VOID		CurrentTime( CString& strTime ){
		SYSTEMTIME	systm;
		GetLocalTime(&systm);
		strTime.Format( _T("%02d:%02d:%02d:%02d"), systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds );
	}

	CLogItem&	Copy( const CLogItem& item ){
		if( this != &item )
		{
			ID( item.m_ID );
			Time( item.m_Time );
			Subject( item.m_Subject );
			Body( item.m_lpBody, item.m_BodySize );
		}
		return(*this);
	}

protected:
	CLogItem( const CLogItem& item ){
		Copy( item );
	}

	CLogItem( const DWORD id, LPCTSTR lpszTime, LPCTSTR lpszSubject, int nBodySize = 0, LPVOID lpBody = NULL ){
		ID( id );
		Time( lpszTime );
		Subject( lpszSubject );
		Body( lpBody, nBodySize );
	}

	CLogItem( LPCTSTR lpszSubject, int nBodySize = 0, LPVOID lpBody = NULL ){
		ID( UpdateID() );
		CString	strTime;
		CurrentTime( strTime );
		Time( strTime );
		Subject( lpszSubject );
		Body( lpBody, nBodySize );
	}

	CLogItem&	operator=( const CLogItem& item ){
		return Copy( item );
	}

public:
	BOOL		StrCpy( LPTSTR lpszDst, LPCTSTR lpszSrc, int nMaxLen ){
		int nLenSrc	= _tcsclen(lpszSrc) + 1;
		if( nLenSrc <= 1 ){
			lpszDst[ 0 ]	= _T('\0');
			return TRUE;
		}else{
			int	nLen	= 0;
			if( nLenSrc <= nMaxLen ){
				nLen	= nLenSrc;
			}else{
				nLen	= nMaxLen;
			}

			memcpy( (LPVOID)lpszDst, (LPVOID)lpszSrc, nLen*sizeof(TCHAR) );
			lpszDst[ nLen ]	= _T('\0');
			return TRUE;
		}
		return FALSE;
	}

	DWORD		ID( void ) const {
		return m_ID;
	}
	inline void		ID( DWORD id ){
		m_ID = id;
	}

	LPCTSTR		Time( void ) const {
		return m_Time;
	}
	inline void		Time( LPCTSTR lpszTime ){
		StrCpy( m_Time, lpszTime, LENGTH_TIME );
	}

	LPCTSTR		Subject( void ) const {
		return m_Subject;
	}
	inline void		Subject( LPCTSTR lpszSubject ){
		StrCpy( m_Subject, lpszSubject, LENGTH_SUBJECT );
	}

	BOOL		IsValidBody( void ) const {
		return(m_BodySize <= 0 ? FALSE : TRUE);
	}

	inline void		Body( LPVOID lpBody, int nBodySize ){
		m_BodySize	= nBodySize;
		if( m_BodySize > 0 ){
			m_lpBody	= (LPVOID)malloc( m_BodySize );
			if( m_lpBody ){
				memset( m_lpBody, 0x00, m_BodySize );
				memcpy( m_lpBody, lpBody, m_BodySize );
			}else{
				m_lpBody	= NULL;
				m_BodySize	= ERROR_BODY;
			}
		}else{
			m_lpBody	= NULL;
		}
	}

	inline int			BodySize( void ) const {
		return m_BodySize;
	}

	LPVOID		Body( void ) const {
		return m_lpBody;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CLogList {
private:
	typedef std::map<DWORD, CLogItem*>	MAP_ITEM;
	MAP_ITEM		m_Items;

public:
	int		Append( const CLogItem& item ){
		m_Items[ item.ID() ]	= new CLogItem(item);
		return m_Items.size();
	}

	int		Append( LPCTSTR lpszSubject, int nBodySize = 0, LPVOID lpBody = NULL ){
		CLogItem*	pItem = new CLogItem(lpszSubject,nBodySize,lpBody);
		m_Items[ pItem->ID() ]	= pItem;
		return m_Items.size();
	}

	CLogItem*	Find( DWORD id ) const {
		MAP_ITEM::const_iterator it = m_Items.find( id );
		if( it != m_Items.end() ){
			return it->second;
		}
		return NULL;
	}

	void	Remove( DWORD id ){
		CLogItem*	pItem = Find( id );
		if( pItem ){
			m_Items.erase( pItem->ID() );
			delete pItem;
		}
	}

	void	RemoveAll( void ){
		MAP_ITEM::iterator it = m_Items.begin();
		for( ; it!=m_Items.end(); it++ ){
			delete (CLogItem*)(it->second);
		}
		m_Items.clear();
	}

	int		Count( void ) const {
		return m_Items.size();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CLogManager : private NonCopyable {
private:
	static UINT AFX_CDECL		LogReceiver( LPVOID lpParam );

	CLogList			m_LogList;

	CEvent*				m_pEventExit;
	CEvent*				m_pEventSuspend;
	CWinThread*			m_pLogReceiverThread;

protected:
	CLogManager( void )
		: m_pLogReceiverThread(NULL), m_pEventExit(NULL), m_pEventSuspend(NULL)
	{
		Constractor();
	}
	~CLogManager( void )
	{
		Destractor();
	}

	void	Constractor( void )
	{
		m_pEventExit	= new CEvent(TRUE, TRUE);
		m_pEventSuspend	= new CEvent(TRUE, TRUE);
	}

	void	Destractor( void )
	{
		TerminateLogReceiver();

		if( m_pEventExit )
			delete m_pEventExit;
		m_pEventExit	= NULL;

		if( m_pEventSuspend )
			delete m_pEventSuspend;
		m_pEventSuspend	= NULL;

		m_LogList.RemoveAll();
	}

public:
	CLogList*	GetLogLlist( void ){
		return &m_LogList;
	}

	BOOL		BeginLogReceiver( void ){
		if( m_pLogReceiverThread )
			return FALSE;

		m_pEventSuspend->SetEvent();
		m_pEventExit->SetEvent();

		m_pLogReceiverThread	= AfxBeginThread( LogReceiver, this );
		if( m_pLogReceiverThread )
			return TRUE;
		return FALSE;
	}

	VOID		TerminateLogReceiver( void ){
		if( m_pLogReceiverThread ){
			m_pEventSuspend->SetEvent();
			m_pEventExit->ResetEvent();
			
			if( WaitForSingleObject( m_pLogReceiverThread->m_hThread, 10000 ) == WAIT_TIMEOUT )
				::TerminateThread( m_pLogReceiverThread->m_hThread, (DWORD)-1 );

			m_pLogReceiverThread	= NULL;
		}
	}

	VOID		SuspendLogReceiver( void ){
		m_pEventSuspend->ResetEvent();
	}

	VOID		ResumeLogReceiver( void ){
		m_pEventSuspend->SetEvent();
	}

public:
	static CLogManager&		GetInstance( void ){
		static CLogManager	ins;
		return ins;
	}
};