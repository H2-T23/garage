#pragma once

#include "Generic.h"
#include "Thread.h"

class CKeyValue : public GENERIC::TSingleton<CKeyValue> {
protected:
	typedef	std::string							String;
	typedef std::map<const String,String>		Map;
	typedef	Map::iterator						Iterator;
	typedef std::pair<const String,String>		Pair;

	Map						m_map;
	Iterator				m_it;

	MT::CCriticalSection	m_cs;

public:
	template<typename TYPE>
	void			Set( const String& strKey, TYPE value ){
		MT::CSingleLock		lock(&m_cs);

		std::stringstream	ss;
		ss << value << "\0";
	//	m_map.insert( Pair(strKey, ss.str()) );
		m_map[ strKey ]	= ss.str();
	}

	const char*		Get( const String& strKey ) {
		MT::CSingleLock		lock(&m_cs);
		return Find( strKey );
	}

	const char*		Find( const String& strKey ){
		MT::CSingleLock		lock(&m_cs);

		Iterator it = m_map.find( strKey );
		if( it != m_map.end() ){
			return it->second.c_str();
		}
		return NULL;
	}

	void			Clear( void ){
		MT::CSingleLock		lock(&m_cs);
		m_map.clear();
	}

	String			ToString( const String& strKey ){
		MT::CSingleLock		lock(&m_cs);

		String	str;
		const char* pValue = Find( strKey );
		if( pValue )
			str = "[" + strKey + "] " + String(pValue) + "\n";
		return str;
	}

	const char*		Begin( void ){
		MT::CSingleLock		lock(&m_cs);

		m_it	= m_map.begin();
		if( hasNext() )
			return(m_it->first.c_str());
		return NULL;
	}

	bool			hasNext( void ){
		MT::CSingleLock		lock(&m_cs);
		return(m_it!=m_map.end());
	}

	const char*		Next( void ){
		MT::CSingleLock		lock(&m_cs);
		m_it++;
		if( hasNext() ){
			return m_it->first.c_str();;
		}
		return NULL;
	}

public:
	static void	Test( void )
	{
		CKeyValue	KeyValue;

		const char*	pKey	= NULL;

		for( pKey=KeyValue.Begin(); KeyValue.hasNext(); pKey=KeyValue.Next() ){
			TRACE(KeyValue.ToString( pKey ).c_str());
		}

		KeyValue.Set( "system.mode", 1.23 );
		KeyValue.Set( "scan.mode", "BW" );
		KeyValue.Set( "num_vector", 512 );
		KeyValue.Set( "debug", true );

		for( pKey=KeyValue.Begin(); KeyValue.hasNext(); pKey=KeyValue.Next() ){
			TRACE(KeyValue.ToString( pKey ).c_str());
		}

		KeyValue.Clear();

		for( pKey=KeyValue.Begin(); KeyValue.hasNext(); pKey=KeyValue.Next() ){
			TRACE(KeyValue.ToString( pKey ).c_str());
		}
	}
};