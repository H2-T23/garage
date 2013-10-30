// Session.cpp 
#include <windows.h>
#include <tchar.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>

/*****************************************************************************************************
 */
#include "Buffer.h"

/*****************************************************************************************************
 */
#include "SessionState.h"

/*****************************************************************************************************
 */
#define SINGLETON(classname,methodname)	private:classname(void){};classname(classname&);classname& operator=(classname&);	\
										public:static classname& methodname(void){static classname _ins;return(_ins);}	

/*****************************************************************************************************
 */
#if 1
	template<typename TYPE>
	class basic_stringEx : public std::basic_string<TYPE> {
	public:
		basic_stringEx( void ) : std::basic_string<TYPE>() {}
		basic_stringEx( TYPE* str ) : std::basic_string<TYPE>( str ) {}
		basic_stringEx( const TYPE* str ) : std::basic_string<TYPE>( str ) {}
		basic_stringEx( basic_string<TYPE>& str ) : std::basic_string<TYPE>( str ) {}
		

		void	Format( TYPE* strFmt ){
			basic_string<TYPE> Fmt(strFmt);
		}
	};

	typedef basic_stringEx<TCHAR>			TString;
#else
//	typedef std::basic_string<TCHAR>		TString;
#endif

/*****************************************************************************************************
 */
class CSession {
public:
	UINT				ID;
	TString				strID;
	CBuffer				bufRecv;
	CBuffer				bufSend;
	ESessionState		State;

private:
public:
	CSession( void ) : ID(0) {}
	CSession( UINT id ) : ID(id) {}
	CSession( UINT id, LPCTSTR lpszID ) : ID(id), strID(lpszID) {}
};

/*****************************************************************************************************
 */

/*****************************************************************************************************
 */
class CSessionManager : private std::map<UINT,CSession*> {
	typedef std::map<UINT,CSession*>::iterator			ITERATOR;

#if 1
	SINGLETON(CSessionManager,GetInstance);
#else
private:
	CSessionManager(void){}
	CSessionManager(CSessionManager&);
	CSessionManager& operator=(CSessionManager&);

public:
	static CSessionManager&	GetInstance(void) {
		static CSessionManager _ins;
		return _ins;
	}
#endif

public:
	void		Register( CSession* pSession ){
		this->operator[]( pSession->ID ) = pSession;
	}

	void		Unregister( CSession* pSession ){
		this->operator[]( pSession->ID ) = NULL;	
	}

	CSession*	Find( UINT id ) {
		iterator it = find(id);
		if( it != end() ){
			return it->second;
		}
		return NULL;

	}

	ITERATOR	Begin( void ){
		return begin();
	}

	ITERATOR	End( void ){
		return end();
	}
};

/*****************************************************************************************************
 */
void	UseSssionManager( void )
{
	TString		str;
	str.Format( _T("") );

	CSessionManager::GetInstance().Register( new CSession( 0, _T("null object") ) );
	CSessionManager::GetInstance().Register( new CSession( 1, _T("1") ) );
	CSessionManager::GetInstance().Register( new CSession( 11, _T("11") ) );
	CSessionManager::GetInstance().Register( new CSession( 11, _T("12") ) );
	CSessionManager::GetInstance().Register( new CSession( 11, _T("13") ) );

	CSession*	pSession = NULL;

	pSession	= CSessionManager::GetInstance().Find( 11 ); 
	if( pSession )
		std::cout << pSession->strID << std::endl;
	
	pSession	= CSessionManager::GetInstance().Find( 111 ); 
	if( pSession )
		std::cout << pSession->strID << std::endl;

	CSessionManager::ITERATOR it = CSessionManager::GetInstance().Begin();
	for( ; it!=CSessionManager::GetInstance().End(); it++ ){
		std::cout << it->second->strID << std::endl;
	}
}




/*****************************************************************************************************
 */
void	main( void )
{
	UseSssionManager();
}
