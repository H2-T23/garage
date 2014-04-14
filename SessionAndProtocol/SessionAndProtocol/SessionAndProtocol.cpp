// SessionAndProtocol.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <stdarg.h>

#include "SessionAndProtocol.h"
/**************************************************************************
 */
class CTrace {
public:
	static void	Trace( const char* fmt, ... )
	{
		CHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringA( buf );
		}
		va_end(args);
	}
	static void	Trace( const WCHAR* fmt, ... )
	{
		WCHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnwprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringW( buf );
		}
		va_end(args);
	}
};
#define	TRACE	CTrace::Trace
/**************************************************************************
 */
class CBuffer {
public:
	typedef std::vector<BYTE>	BUFFER;
	BUFFER			m_buf;

	CBuffer( void ) {
	}
	CBuffer( int nSize ) {
		m_buf.resize( nSize );
	}
	CBuffer( const BYTE* pBuf, int size ){
		Set( pBuf, size );
	}
	CBuffer( const CBuffer& buf ) {
		Set( buf );
	}

	void	Set( const BYTE* pBuf, int size ){
		m_buf.resize( size );
		memcpy( &m_buf.at(0), pBuf, size );
	}

	void	Set( const CBuffer& buf ){
		m_buf.resize( buf.m_buf.size() );
		memcpy( &m_buf.at(0), &buf.m_buf.at(0), buf.m_buf.size() );
	}

	int		Size( void ) const {
		return m_buf.size();
	}

	BYTE*	At( int idx ){
		return &m_buf.at( idx );
	}

	BYTE*	GetData( void ){
		return &(m_buf.at( 0 ));
	}

	CBuffer&	operator=( const CBuffer& buf ){
		if( this != &buf ){
			Set( buf );
		}
		return *this;
	}
};
/**************************************************************************
 */
class IHandler {
public:
	virtual int			OnAccept( CBuffer* pBuffer )	{ return 0; }
	virtual	int			OnRecv( CBuffer* pBuffer )		{ return 0; }
	virtual int			OnSend( CBuffer* pBuffer )		{ return 0; }
	virtual int			OnClose( CBuffer* pBuffer )		{ return 0; }
	virtual int			OnException( CBuffer* pBuffer )	{ return 0; }
};
/**************************************************************************
 */
class IProtocol	{
protected:
	void*				m_pHeader;
	void*				m_pData;

public:
	virtual int			Execute( CBuffer* pBuffer, IProtocol* pProtocol = NULL ) { return 0; }

	virtual void*		GetHeader( void ) const { return m_pHeader;	}
	virtual void*		GetData( void ) const {	return m_pData;	}
};
/**************************************************************************
 */
class CTcpProtocol		: public IProtocol {
public:
	int			Execute( CBuffer* pBuffer, IProtocol* pProtocol = NULL ){
		TRACE(_T("TCP.\n"));
		m_pHeader	= NULL;
		m_pData		= (LPVOID)pBuffer->GetData();
		return 0;
	}
};
/**************************************************************************
 */
class CHttpProtocol		: public IProtocol {
public:
	std::string		m_RequestLine;
	std::string		m_RequestHeader;
	std::string		m_RequestBody;

	int			Execute( CBuffer* pBuffer, IProtocol* pProtocol = NULL ){
		TRACE(_T("Http.\n"));

		std::string		work( (char*)pBuffer->At( 0 ) );

		int	cnt		= 0;
		int	idx		= 0;
		int sz		= -1;
		while( (sz = work.find('\n', idx)) >= 0 )
		{
			switch( cnt ){
			case 0:
			//	strncpy( &m_RequestLine.at( 0 ), &work.at( idx ), sz );
				m_RequestLine.assign( &work.at( idx ), sz );
				m_RequestLine.append( '\0' );
				break;
			case 1:
			//	strncpy( &m_RequestHeader.at( 0 ), &work.at( idx ), sz );
				m_RequestHeader.assign( &work.at( idx ), sz );
				m_RequestHeader.append( '\0' );
				break;
			case 3:
			//	strncpy( &m_RequestBody.at( 0 ), &work.at( idx ), sz );
				m_RequestBody.assign( &work.at( idx ), sz );
				m_RequestBody.append( '\0' );
				break;
			}

			idx = sz+1;
			cnt++;
		}

		TRACE(m_RequestLine.c_str());
		TRACE(m_RequestHeader.c_str());
		TRACE(m_RequestBody.c_str());
	
	//	m_pRequestLine		= (char*)pBuffer->At( 0 );
	//	m_pRequestHeader	= (char*)pBuffer->At( strlen(m_pRequestLine)+1 );
	//	m_pRequestBody		= (char*)pBuffer->At( strlen(m_pRequestLine)+1 + strlen(m_pRequestHeader)+1 );
		return 0;
	}
};
/**************************************************************************
 */
class CXmlRpcProtocol	: public IProtocol {
public:
	int			Execute( CBuffer* pBuffer, IProtocol* pProtocol = NULL ){
		TRACE(_T("XmlRpc.\n"));
		return 0;
	}
};
/**************************************************************************
 */
class CProtocol {
public:
	std::vector<IProtocol*>		list;

	int			Count( void ) const {
		return list.size();
	}

	void		Append( IProtocol* pProtocol ){
		list.push_back( pProtocol );
	}

	void		Clear( void ){
	}

	void		Execute( CBuffer* pBuffer )
	{
		IProtocol*	pPrevProtocol	= NULL;
		IProtocol*	pCurrProtocol	= NULL;

		std::vector<IProtocol*>::iterator	it = list.begin();
		for( ; it!=list.end(); it++ )
		{
			pCurrProtocol	= (IProtocol*)*it;

			if( pCurrProtocol->Execute(pBuffer, pPrevProtocol) >= 0 )
			{
				pPrevProtocol	= pCurrProtocol;
			}
		}
	}
};
/**************************************************************************
 */
class CSession : public IHandler {
public:
	UINT				m_id;

	CProtocol			protocol;

	CSession( UINT id ) : m_id(id) {
		protocol.Clear();
		protocol.Append( new CTcpProtocol );
		protocol.Append( new CHttpProtocol );
		protocol.Append( new CXmlRpcProtocol );
	}

	int			OnRecv( CBuffer* pBuffer ){
		protocol.Execute( pBuffer );
		return 0;
	}

	int			OnClose( CBuffer* pBuffer ){
		return 0;
	}
};
/**************************************************************************
 */
class CSessionManager {
private:
	CSessionManager( void ) {}
	CSessionManager( const CSessionManager& ) {}
	CSessionManager&	operator=( const CSessionManager& ) {}

public:
	static CSessionManager&		Instance( void ) {
		static CSessionManager	ins;
		return ins;
	}

protected:
	typedef std::map<UINT,CSession*>	MAP;
	typedef std::pair<UINT,CSession*>	PAIR;
	typedef MAP::iterator				ITERATOR;

	MAP				m_map;

public:
	CSession*		Create( UINT id ){
		CSession* pSession = new CSession(id);
		if( pSession )
			m_map.insert( PAIR(id, pSession) );
		return pSession;
	}

	void			Delete( UINT id ){
	}

	CSession*		Find( UINT id ){
		ITERATOR	it = m_map.find( id );
		if( it != m_map.end() ){
			return(it->second);
		}
		return NULL;
	}
};
/**************************************************************************
 */
class CDispatch {
public:
	CDispatch( void ){
		CSessionManager::Instance().Create( 123 );
	}

	void	Do( DWORD dwTimeout = -1 ){
		char	buf[] = 
"GET　/www.yoheim.net/?key1=value1&key2=value2 HTTP/1.1\n"
"Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
"Accept-Charset:Shift_JIS,utf-8;q=0.7,*;q=0.3"
"Accept-Encoding:gzip,deflate,sdch"
"Accept-Language:ja,en-US;q=0.8,en;q=0.6"
"Cache-Control:max-age=0"
"Connection:keep-alive"
"Cookie:utma=something;"
"Host:www.yoheim.net"
"User-Agent:Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_4) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.54 Safari/536.5\n\n"
"key1=value1&key2=value2\n"
;
		UINT	id	= 123;

		CSession*	pSession = CSessionManager::Instance().Find( id );
		if( pSession )
		{
			pSession->OnRecv( new CBuffer((BYTE*)buf,strlen(buf)+1) );
		}
	}
};
/**************************************************************************
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CDispatch	dispatch;
	dispatch.Do();

}
