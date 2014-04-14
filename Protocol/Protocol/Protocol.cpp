// Protocol.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Protocol.h"
#include "HttpProtocol.h"
#include "XmlRpcProtocol.h"

#include "ICommand.h"
#include "KeyValue.h"


/**********************************************************************************
 */
class CSocket	: public ISocket {
public:

public:
	CSocket( void ) : ISocket() {}

public:
	bool		Create( SocketOption& Option ){
		m_sock	= ::socket(Option.nAf, Option.nProtocol, Option.nType);
		return IsValid();
	}

	int			Close( void ){
		return ::closesocket(m_sock);
	}

	virtual int		Shutdown( void ){
		return ::shutdown(m_sock,2);
	}

	virtual int		Send( const CBuffer* pBuffer ){
		return ::send(m_sock, &pBuffer->at( 0 ), pBuffer->size(), 0 );
	}

	virtual int		Recv( CBuffer* pBuffer ){
		return ::recv(m_sock, &pBuffer->at( 0 ), pBuffer->size(), 0 );
	}

	virtual int		Accept( LPSOCKADDR lpAddr, int& nLen ){
		return ::accept(m_sock, lpAddr, &nLen );
	}
};
/**********************************************************************************
 */
class CSocketEx : public CSocket {
protected:
	

	SOCKET				m_sockAccept;
	
	OVERLAPPED			m_Overlapped;
	WSABUF				m_dataBuf;


	//bool		LoadAcceptEx( SOCKET sock ){
	//	GUID	GuidAcceptEx	= WSAID_ACCEPTEX;
	//	DWORD	dwBytes	= 0;
	//	if( 0 == ::WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &m_lpAcceptEx, sizeof(m_lpAcceptEx), &dwBytes, NULL, NULL) ){
	//		m_sockAccept	= ::socket(Af(), Type(), Protocol());
	//		if( m_sockAccept != INVALID_SOCKET ){
	//			return(true);
	//		}
	//	}
	//	return(false);
	//}

	class CAcceptEx {
	public:
		LPFN_ACCEPTEX		AcceptEx;

		inline bool		IsInvalid( void ) const {
			return(AcceptEx==NULL);
		}

		CAcceptEx( void ) : AcceptEx(NULL) {
		}

		CAcceptEx( SOCKET sock ) : AcceptEx(NULL) {
			Load(sock);
		}

		bool	Load( SOCKET sock ){
			GUID	GuidAcceptEx	= WSAID_ACCEPTEX;
			DWORD	dwBytes	= 0;
			::WSAIoctl( sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), &AcceptEx, sizeof(AcceptEx), &dwBytes, NULL, NULL );
			return !IsInvalid();
		}

		void	Unload( void ){
			if( !IsInvalid() )
				AcceptEx	= NULL;
		}
	};


	class CTransmitFile {
	public:
		LPFN_TRANSMITFILE			TransmitFile;

		inline bool		IsInvalid( void ) const {
			return(TransmitFile == NULL);
		}

		CTransmitFile( void ) : TransmitFile(NULL) {
		}

		CTransmitFile( SOCKET sock ) : TransmitFile(NULL) {
			Load(sock);
		}

		bool	Load( SOCKET sock ){
			GUID	GuidTransmitFile	= WSAID_TRANSMITFILE;
			DWORD	dwBytes	= 0;
			::WSAIoctl( sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidTransmitFile, sizeof(GuidTransmitFile), &TransmitFile, sizeof(TransmitFile), &dwBytes, NULL, NULL );
			return !IsInvalid();
		}

		void	Unload( void ){
			if( !IsInvalid() )
				TransmitFile	= NULL;
		}
	};

	CAcceptEx				m_AcceptEx;
	CTransmitFile			m_TransmitFile;

public:
	BOOL		TransmitFile( HANDLE hFile, DWORD dwNumberOfBytesToWrite, DWORD dwNumberOfBytesPerSend, LPOVERLAPPED lpOveralapped, LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers, DWORD dwFlags ){
		if( m_TransmitFile.IsInvalid() ){
			if( !m_TransmitFile.Load( m_sock ) ){
				return FALSE;
			}
		}
		return m_TransmitFile.TransmitFile(m_sock, hFile, dwNumberOfBytesToWrite, dwNumberOfBytesPerSend, lpOveralapped, lpTransmitBuffers, dwFlags);
	}

public:
	CSocketEx( void )
		: CSocket(), m_sockAccept(INVALID_SOCKET) {
	}

public:
	bool		Create( SocketOption& Option ){
		if(CSocket::Create(Option)){
		//	if(LoadAcceptEx(m_sock))
			{
				return(true);
			}
		}
		return(false);
	}

	virtual int		Send( const CBuffer* pBuffer ){
		m_dataBuf.buf		= (char*)&pBuffer->at( 0 );
		m_dataBuf.len		= pBuffer->size();

		DWORD	dwSize	= 0;

		int	nRet = ::WSASend( m_sock, &m_dataBuf, 1, &dwSize, 0, &m_Overlapped, NULL );
		if( (nRet == SOCKET_ERROR) && (WSA_IO_PENDING == ::WSAGetLastError()) ){
			return 0;
		}
		return -1;
	}

	virtual int		Recv( CBuffer* pBuffer ){
		m_dataBuf.buf		= &pBuffer->at( 0 );
		m_dataBuf.len		= pBuffer->size();

		DWORD	dwSize	= 0;

		int	nRet = ::WSARecv( m_sock, &m_dataBuf, 1, &dwSize, 0, &m_Overlapped, NULL );
		if( (nRet == SOCKET_ERROR) && (WSA_IO_PENDING == ::WSAGetLastError()) ){
			return 0;
		}
		return -1;
	}

	virtual int		Accept( LPSOCKADDR lpAddr, int& nLen ){
	/****
		BOOL	AcceptEx(
					SOCKET			sListenSocket			// [I N] リッスンソケット 
				,	SOCKET			sAcceptSocket			// [I N] クライアントと接続したいサーバソケット 
				,	PVOID			lpOutputBuffer			// [I N] バッファ
				,	DWORD			dwReceiveDataLength		// [I N] 受信バッファサイズ 
				,	DWORD			dwLocalAddressLength	// [I N] サーバーアドレスサイズ(SOCKADDR_IN + 16バイト)
				,	DWORD			dwRemoteAddressLength	// [I N] クライアントアドレスサイズ(SOCKADDR_IN + 16バイト)
				,	LPDWORD			lpdwBytesReceived		// [OUT] 受信サイズ 
				,	LPOVERLAPPED	lpOverlapped			// [I N] 
			);

			lpOutputBuffer---->	+-----------------------+
								|						|
								| 受信バッファ			|
								|						|
								+-----------------------+
								| サーバーアドレス		|
								+-----------------------+
								| クライアントアドレス	|
								+-----------------------+
	 */
		if( m_AcceptEx.IsInvalid() ){
			if( !m_AcceptEx.Load( m_sock ) ){
				return FALSE;
			}
		}
		DWORD	dwBytes	= 0;
		return m_AcceptEx.AcceptEx(m_sock, m_sockAccept, NULL, 0, 0, 0, &dwBytes, &m_Overlapped);
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

	std::string		strBody = 
		"<?xml version=\"1.0\"?>"
		"<methodCall>"
		"<methodName>SetDRParameter</methodName>"
		"<params>"
		"<param>"
		"<value><int>123</int></value>"
		"</param>"
		"</params>"
		"</methodCall>"
		"\n"
		;

	std::string		strReq("POST /RPC2 HTTP1.0\n");
	std::string		str2("Content-Length");
	std::string		str2_2(": 12\n\n");
	std::string		str3("xml-value:20\n");

	std::string		strUserAgent("User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows XP)\n");
	std::string		strHost("Host: XmlRpc\n");
	std::string		strContentType("Content-Type: text/xml\n");
	std::string		strContentLength("Content-Length: ");
//	strContentLength.append( atoi(strBody.c_str()) );
	char	tmp[32];
	sprintf(tmp, "%d\n", strBody.length());
	std::string		strLen(tmp);

	std::string		strNN("\n");

	Protocol.OnRead( &strReq );
	Protocol.OnRead( &strUserAgent );
	Protocol.OnRead( &strHost );
	Protocol.OnRead( &strContentType );
	Protocol.OnRead( &strContentLength );
	Protocol.OnRead( &strLen );
	Protocol.OnRead( &strNN );
	Protocol.OnRead( &strBody );


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

