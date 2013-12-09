// XmlRpcServer.cpp 
#include <windows.h>
#include <tchar.h>

#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <map>


/*****************************************************************************
 * CLASS@TString
 *
 */
typedef std::basic_string<TCHAR>	TString;

/*****************************************************************************
 * CLASS@CBuffer
 *
 */
class CBuffer {
private:
	BYTE	m_Data[ 64 * (1024 * 1024) ];
};



/*****************************************************************************
 * CLASS@CTcpServer
 *
 */
class CTcpProtocol {
public:
	int			PORT;

	CTcpProtocol( int	Port ) : PORT(Port) {}

	virtual bool	Run( void ){
		return false;
	}

public:
	virtual void		OnRecv( CBuffer* pBuf ){
	}

	virtual void		OnSend( CBuffer* pBuf ){
	}

	virtual void		OnError( CBuffer* pBuf ){
	}
};

/*****************************************************************************
 * CLASS@CHttpProtocol
 *
 */
class CHttpProtocol : public CTcpProtocol {
public:
	CHttpProtocol( int Port ) : CTcpProtocol(Port) {	}

protected:
	class CHttpRequest {
	public:
		class CReqestLine {
		public:
			char*		strMethod;
			char*		strURI
			char*		strVersion;

			bool	Parse( const char* strReq ){
				sscanf( strReqestLine, "%s %s %s", strMethod, strURI, strVersion );
			}
		} RequestLine ;

		class IHttpReqestHeader {
		public:
			virtual bool	Parse( const char* strReq ){
				return(false);
			}
		};

		class CGeneralHeader	: public IHttpReqestHeader {
		public:
		} GeneralHeader ;

		class CRequestHeader	: public IHttpReqestHeader {
		public:
		} RequestHeader ;

		class CEntityHeader		: public IHttpReqestHeader {
		public:
		} EntityHeader ;

		class CMessageBody {
		public:
			bool	Parse( const char* strReq ){
				return(false);
			}
		} MessageBody ;
		

		std::list<IHttpReqestHeader*>		availableHeader;

		bool	Parse( const char* strReq ){
			if( !RequestLine( strReq ) )
				return false;
		
			availableHeader.clear();

			if( GeneralHeader.Parse( strReq ) )
				availableHeader.push_back( &GeneralHeader );					

			if( RequestHeader.Parse( strReq ) )
				availableHeader.push_back( &RequestHeader );

			if( EntityHeader.Parse( strReq ) )
				availableHeader.push_back( &EntityHeader );

			return true;
		}
	} HttpRequest;

public:
	virtual void		OnRecv( CBuffer* pBuf ){
		if( strcmp(_T("GET"), Request.RequestLine.strMethod) == 0 ){
		}
	}

	virtual void		OnGet( CBuffer* pBuf )	{	}
	virtual void		OnPut( CBuffer* pBuf )	{	}
	virtual void		OnPost( CBuffer* pBuf )	{	}
	virtual void		OnDelete( CBuffer* pBuf )	{	}
};

/*****************************************************************************
 * CLASS@IXmlRpcMethod
 *
 */
class IXmlRpcMethod {
public:
	virtual TString		MethodName( void ) const	= 0;
	virtual bool		Execute( TString strXML )	= 0;
};

/*****************************************************************************
 * CLASS@CXmlRpcProtocol
 *
 */
class CXmlRpcProtocol : public CHttpProtocol {
public:
	typedef std::map<TString,IXmlRpcMethod*>	MapMethod;

private:
	MapMethod		mapMethod;

public:
	CXmlRpcProtocol( int Port ) : CHttpProtocol(Port) {
	}

	virtual ~CXmlRpcProtocol( void ){
		RemoveAllMethod();
	}

public:
	void		Register( IXmlRpcMethod* pMethod ){
		mapMethod.insert( MapMethod::value_type( pMethod->MethodName(), pMethod ) );
	}

	void		Unregister( IXmlRpcMethod* pMethod ){
	}

	IXmlRpcMethod*		FindMethod( LPTSTR lpszMethod ){
		MapMethod::iterator it = mapMethod.find( lpszMethod );
		if( it != mapMethod.end() ){
			return it->second;
		}
		return NULL;
	}

	void		RemoveAllMethod( void ){
		MapMethod::iterator it = mapMethod.begin();
		while( it != mapMethod.end() ){
			std::cout << "delete " << it->first << std::endl;
			delete it->second;
			it++;
		}
		mapMethod.clear();
	}

public:
	bool		Run( void ){
		MapMethod::iterator it = mapMethod.begin();
		while( it != mapMethod.end() ){
			std::cout << it->first << std::endl;
			IXmlRpcMethod*	pMethod = (IXmlRpcMethod*)it->second;
			if( pMethod ){
				pMethod->Execute( _T("Execute") );
			}
			it++;
		}
		return true;
	}
};

/*****************************************************************************
 * CLASS@CSetValue
 *
 */
class CSetValue : public IXmlRpcMethod {
public:
	TString		MethodName( void ) const {
		return _T("SetValue");
	}

	bool		Execute( TString strXML ){
		std::cout << MethodName() << " : " << strXML << std::endl;
		return true;
	}
};

/*****************************************************************************
 * CLASS@CGetValue
 *
 */
class CGetValue : public IXmlRpcMethod {
public:
	TString		MethodName( void ) const {
		return _T("GetValue");
	}

	bool		Execute( TString strXML ){
		std::cout << MethodName() << " : " << strXML << std::endl;
		return true;
	}
};

/*****************************************************************************
 * CLASS@CUpdateCommand
 *
 */
class CUpdateCommand : public IXmlRpcMethod {
public:
	TString		MethodName( void ) const {
		return _T("UpdateCommand");
	}

	bool		Execute( TString strXML ){
		std::cout << MethodName() << " : " << strXML << std::endl;
		return true;
	}
};

/*****************************************************************************
 * CLASS@main
 *
 */
int		main( void )
{
	CXmlRpcProtocol	protocol( 5432 );
	protocol.Register( new CSetValue );
	protocol.Register( new CGetValue );
	protocol.Register( new CUpdateCommand );

	protocol.Run();
}
