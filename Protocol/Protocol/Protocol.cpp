// Protocol.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Protocol.h"
#include "Command.h"
#include "KeyValue.h"

/**********************************************************************************
 */
//class CBuffer : public std::string {};
typedef std::string		CBuffer;
/**********************************************************************************
 */
class CSocket {
public:
	SOCKET			m_sock;

public:
	int			Send( CBuffer* pBuffer ){
	}

	int			Recv( CBuffer* pBuffer ){
	}
};
/**********************************************************************************
 */
class IEventHandler {
public:
	enum EnumType {
		OP_READ	= 0, OP_WRITE, OP_ACCEPT, OP_CLOSE, OP_CONNECT
	};

	virtual int		Preform( EnumType nType, CBuffer* pBuffer ){
		switch( nType ){
		case OP_READ:
			OnRead( pBuffer );
			break;

		case OP_WRITE:
			OnWrite( pBuffer );
			break;

		case OP_ACCEPT:
			OnAccept( pBuffer );
			break;

		case OP_CLOSE:
			OnClose( pBuffer );
			break;

		case OP_CONNECT:
			OnConnect( pBuffer );
			break;

		default:
			return -1;
		}
		return 1;
	}

	virtual int		OnRead( LPVOID )	{ return 0; }
	virtual int		OnWrite( LPVOID )	{ return 0; }
	virtual int		OnAccept( LPVOID )	{ return 0; }
	virtual int		OnClose( LPVOID )	{ return 0; }
	virtual int		OnConnect( LPVOID )	{ return 0; }
};
/**********************************************************************************
 */
/**********************************************************************************
 */
class CHttpProtocol		: public IEventHandler {
public:
	enum {
		REQUEST_LINE	= 0	,
		REQUEST_HEADER		,
		REQUEST_BODY		,
	};

	std::string		m_Req, m_Header, m_Body;
	int state;
	int	response;

	CHttpProtocol( void ) : state(REQUEST_LINE) {
	}

	virtual int		Get( std::string& uri ){
		TRACE("HTTP: %s",uri.c_str());

		return 1;
	}

	virtual int		Put( std::string& uri ){
		return 0;
	}

	virtual int		Post( std::string& uri ){
		return 0;
	}

	virtual int		Head( std::string& uri ){
		return 0;
	}

	virtual int		OnRead( LPVOID lpParam )
	{
		CBuffer* pBuffer = (CBuffer*)lpParam;

	//	int	cnt = 0;
		int	idx	= 0;
		int siz	= -1;

	//	while( 1 )
		do
		{
			int len	= 0;

			siz = pBuffer->find('\n', idx);
			if( siz > 0 ){
				len	= siz - idx;
			}else{
				len	= pBuffer->length() - idx;
			}

			if( len > 0 )
			{
				switch( state ){
				case REQUEST_LINE:
					m_Req.append( &pBuffer->at( idx ), len );
					if( siz > 0 ){
						m_Req.append( "\n\0" );
						state++;
					}
					break;

				case REQUEST_HEADER:
					m_Header.append( &pBuffer->at( idx ), len );
					if( siz > 0 ){
						m_Header.append( "\n\0" );
						state++;
					}
					break;

				case REQUEST_BODY:
					m_Body.append( &pBuffer->at( idx ), len );
					if( siz > 0 ){
						m_Body.append( "\n\0" );
						state++;
					}
					break;
				}

				if( siz > 0 )
				{
					idx = (siz + 1);
				}
				else
				{
			//		break;
				}
			}
			else
			{
				break;
			}
	//	}
		} while( siz > 0 );

		if( state <= REQUEST_BODY ){
			return 0;
		}

		TRACE( m_Req.c_str() );
		TRACE( m_Header.c_str() );
		TRACE( m_Body.c_str() );

		char cmd[32],uri[256],ver[64];

		sscanf( m_Req.c_str(), "%s %s %s", cmd, uri, ver );
		if( strcmp(cmd,"GET") == 0 ){
			return Get( std::string(uri) );
		}else if( strcmp(cmd,"POST") == 0 ){
			return Post( std::string(uri) );
		}else if( strcmp(cmd,"HEAD") == 0 ){
			return Head( std::string(uri) );
		}else if( strcmp(cmd,"PUT") == 0 ){
			return Put( std::string(uri) );
		}
		
		return 1;
	}
};
/**********************************************************************************
 */
class CXmlRpcProtocol	: public CHttpProtocol {
public:
	virtual int			Post( std::string& uri ){
		TRACE("XmlRpc:%s",m_Body.c_str());

		return 1;
	}
};
/**********************************************************************************
class CProtocolManager {
protected:
	typedef std::vector<IProtocol*>		LIST;
	typedef LIST::iterator				ITERATOR;
	
	LIST			m_list;

public:
	void		Append( IProtocol* pProtocol ){
		m_list.push_back( pProtocol );
	}


	void		OnRead( CBuffer* pBuffer ){
		IProtocol*	pPrev	= NULL;

		ITERATOR	it	= m_list.begin();
		for( ; it!=m_list.end(); it++ )
		{
			if( (*it)->Preform( pBuffer, pPrev ) > 0 ){
				pPrev	= (*it);
			}
		}	
	}
};
 */
/**********************************************************************************
 */
class ISocket {
public:
	virtual int		Recv( CBuffer* pBuffer )	= 0;
	virtual int		Send( CBuffer* pBuffer )	= 0;
};
/**********************************************************************************
 */
class CSession : public IEventHandler {
public:
	ISocket*				m_pSock;
	IEventHandler*			m_pProtocol;

	CSession( ISocket* pSock, IEventHandler* pProtocol ) 
	 : m_pSock(pSock), m_pProtocol(pProtocol) {
	}

	virtual ~CSession( void ){
		delete m_pProtocol;
		delete m_pSock;
	}

	virtual int		OnRead( LPVOID lpParam )	{ return m_pProtocol->OnRead( lpParam ); }
	virtual int		OnWrite( LPVOID lpParam )	{ return m_pProtocol->OnWrite( lpParam ); }
	virtual int		OnAccept( LPVOID lpParam )	{ return m_pProtocol->OnAccept( lpParam ); }
	virtual int		OnClose( LPVOID lpParam )	{ return m_pProtocol->OnClose( lpParam ); }
	virtual int		OnConnect( LPVOID lpParam )	{ return m_pProtocol->OnConnect( lpParam ); }
};
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
class CXmlRpcValue {
public:
	enum TAG {
		TAG_NULL = 0, TAG_INT, TAG_BOOLEAN, TAG_DOUBLE, TAG_STRING, TAG_DATETIME, TAG_ARRAY, TAG_STRUCT, TAG_PARAMS
	};

	TAG					m_tag;
	std::string			m_str;
	std::string			m_xml;

	inline const char*		GetTag( TAG tag ) const {
		static const char*	tags[] = {
			"nil", "int", "boolean", "double", "string", "dateTime.iso8601", "array", "struct", "params"
		};
		return tags[tag];
	}

	inline const char*		GetTag( void ) const {
		return GetTag( m_tag );
	}

	inline bool				IsValueTag( TAG tag ) const {
		if( (TAG_INT <= tag) && (tag <= TAG_DATETIME) ){
			return true;
		}
		return false;
	}
	
	inline bool				IsValueTag( void ) const {
		return IsValueTag( m_tag );
	}

	inline bool				Indent( std::string& str, int indent ){
		str.clear();
		for( int i=0; i<indent; i++ ){
			str.append( "\t" );
		}
		return !str.empty();
	}

	int				GetStartTag( std::string& str, int indent = 0 ){
		str.clear();
		Indent(str, indent);
		if( IsValueTag() ){
			str.append( "<value>" );
		}
		str.append( "<" );
		str.append( GetTag() );
		str.append( ">" );
		return str.length();
	}

	int				GetEndTag( std::string& str, int indent = 0 ){
		str.clear();
		Indent(str, indent);
		str.append( "</" );
		str.append( GetTag() );
		str.append( ">" );
		if( IsValueTag() ){
			str.append( "</value>" );
		}
		str.append( "\n" );
		return str.length();
	}

public:
	template<typename TYPE>
	void		Set( TAG tag, TYPE value ){
		std::stringstream	ss;
		ss << value;
		m_str	= ss.str();
		m_tag	= tag;
	}

	CXmlRpcValue( TAG tag = TAG_NULL ) : m_tag(tag) {
	//	m_xml	= "<nil/>";
	}

	CXmlRpcValue( const int value ){
		Set(TAG_INT, value);
	}

	CXmlRpcValue( const bool value ){
		Set(TAG_BOOLEAN, value);
	}

	CXmlRpcValue( const double value ){
		Set(TAG_DOUBLE, value);
	}

	CXmlRpcValue( const char* value ){
		Set(TAG_STRING, value);
	}

	CXmlRpcValue( const std::string& value ){
		Set(TAG_STRING, value.c_str());
	}

	CXmlRpcValue( const struct tm& t ){
		char	buf[ 256 ];
		sprintf( buf, "%4d/%02d/%02d:%02d:%02d:%02d", 
			t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );
		Set(TAG_DATETIME, buf);
	}

	CXmlRpcValue( const CXmlRpcValue& value ){
	//	m_xml = const_cast<CXmlRpcValue&>(value).ToXml();
		m_tag	= value.m_tag;
		m_str	= const_cast<CXmlRpcValue&>(value).ToXml();
	}

	virtual std::string&	ToXml( int indent = 0 ) {
		std::string		strStartTag, strEndTag;

		GetStartTag(strStartTag, indent);
		GetEndTag(strEndTag, indent);
		
		m_xml.append( strStartTag );
		m_xml.append( m_str );
		m_xml.append( strEndTag );

		return m_xml;
	}
};
/**********************************************************************************
 */
class CXmlRpcArray : public CXmlRpcValue {
protected:
	typedef std::vector<CXmlRpcValue>	ARRAY;
	typedef ARRAY::iterator				ITERATOR;
	
	ARRAY			arry;

public:
	CXmlRpcArray( TAG tag = TAG_ARRAY ) : CXmlRpcValue(tag) {}

	template<typename TYPE>
	void		Append( TYPE value ){
		arry.push_back( CXmlRpcValue(value) );
	}

	virtual std::string&		ToXml( int indent = 0 ) {
		std::string		strStartTag, strEndTag;

		GetStartTag(strStartTag, indent);
		GetEndTag(strEndTag, indent);

		m_xml	= strStartTag;
		m_xml.append( "\n" );

		ITERATOR	it = arry.begin();
		for( ; it!=arry.end(); it++ ){
			m_xml += it->ToXml( indent + 1 );
		}

		m_xml.append( strEndTag );
		m_xml.append( "\n" );

		return m_xml;
	}
};
/**********************************************************************************
 */
class CXmlRpcParams : public CXmlRpcArray {
public:
	CXmlRpcParams( TAG tag = TAG_PARAMS ) : CXmlRpcArray(tag) {}
};
/**********************************************************************************
 */
class CXmlRpcRequest : public CXmlRpcValue {
public:
	std::string&		ToXml( void ) {
		return m_xml;
	}
};
/**********************************************************************************
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CKeyValue::Test();

	CTestCommandList	test;

	CXmlRpcProtocol	Protocol;

	std::string		str1("POST /index.html HTTP1.0\n");
	std::string		str2("Content-Length");
	std::string		str2_2(": 12\n\n");
	std::string		str3("xml-value:20\n");

	Protocol.OnRead( &str1 );
	Protocol.OnRead( &str2 );
	Protocol.OnRead( &str2_2 );
	Protocol.OnRead( &str3 );

	CXmlRpcValue	value(999);
	TRACE(value.ToXml().c_str());

	time_t	now = time(NULL);
	tm*	pNow	= localtime( &now );

	CXmlRpcArray	arry;
	arry.Append( "debug" );
	arry.Append( true );

	CXmlRpcParams	params;
	params.Append( 123 );
	params.Append( arry );
	params.Append( *pNow );
	TRACE(params.ToXml().c_str());
}

