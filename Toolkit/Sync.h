#pragma once

#include <process.h>

/**********************************************************************************
 *
 *
 *
 */
class ISyncObject {
public:
	virtual void		Lock()		= 0;
	virtual void		Unlock()	= 0;
};
/**********************************************************************************
 *
 *
 *
 */
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
/**********************************************************************************
 *
 *
 *
 */
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
};
/**********************************************************************************
 *
 *
 *
 */
class CMutex : public ISyncObject {
protected:
	HANDLE			m_hMutex;

	BOOL	Create(  ){
	//	m_hMutex	= ::CreateMutex();
		return(m_hMutex == NULL ? FALSE : TRUE);
	}

	BOOL	Open( DWORD dwDesiredAccess, BOOL bInheritHandle, LPCTSTR lpName ){
		m_hMutex	= ::OpenMutex( dwDesiredAccess, bInheritHandle, lpName );
		return(m_hMutex == NULL ? FALSE : TRUE);
	}

	BOOL	Release( void ){
		if( ::ReleaseMutex(m_hMutex) ){
			m_hMutex	= NULL;
			return TRUE;
		}
		return FALSE;
	}

public:
	CMutex( void )				: m_hMutex(NULL) {}
	CMutex( HANDLE hHandle )	: m_hMutex(hHandle) {}
	CMutex( const CMutex& obj )	: m_hMutex(obj.m_hMutex) {}

	void	Lock( void ){
	}

	void	Unlock( void ){
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CEvent : public ISyncObject {
public:
	void	Lock( void ){
	}

	void	Unlock( void ){
	}
};