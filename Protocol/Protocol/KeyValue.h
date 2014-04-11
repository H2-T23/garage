#pragma once

class CKeyValue {
protected:
	typedef	std::string							String;
	typedef std::map<const String,String>		Map;
	typedef	Map::iterator						Iterator;
	typedef std::pair<const String,String>		Pair;

	Map					m_map;
	Iterator			m_it;

public:
	template<typename TYPE>
	void			Set( const String& strKey, TYPE value ){
		std::stringstream	ss;
		ss << value;
		m_map.insert( Pair(strKey, ss.str()) );
	}

	const char*		Get( const String& strKey ) {
		return Find( strKey );
	}

	const char*		Find( const String& strKey ){
		Iterator it = m_map.find( strKey );
		if( it != m_map.end() ){
			return it->second.c_str();
		}
		return NULL;
	}

	void			Clear( void ){
		m_map.clear();
	}

	String			ToString( const String& strKey ){
		String	str;
		const char* pValue = Find( strKey );
		if( pValue )
			str = "[" + strKey + "] " + String(pValue) + "\n";
		return str;
	}

	const char*		Begin( void ){
		m_it	= m_map.begin();
		if( hasNext() )
			return(m_it->first.c_str());
		return NULL;
	}

	bool			hasNext( void ){
		return(m_it!=m_map.end());
	}

	const char*		Next( void ){
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