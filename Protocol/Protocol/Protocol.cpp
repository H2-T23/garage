s// Protocol.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "Protocol.h"


#include <vector>
#include <map>
#include <string>

/**********************************************************************************
 */
//class CBuffer : public std::string {};
typedef std::string		CBuffer;


/**********************************************************************************
 */
class IProtocol {
public:
	virtual int		Preform( CBuffer* pBuffer, IProtocol* pPrevProtocol ){
		return 0;
	}
};
/**********************************************************************************
 */
class CTcpProtocol		: public IProtocol {
public:
	virtual int		Preform( CBuffer* pBuffer, IProtocol* pPrevProtocol ){
		return 0;
	}
};
/**********************************************************************************
 */
class CHttpProtocol		: public IProtocol {
public:
	enum {

		REQUESTC_LINE	= 0	,
		HEADER				,
		BODY				,

	};

	std::string		m_Req, m_Header, m_Body;
	int state;
	int	response;

	CHttpProtocol( void ) : state(REQUESTC_LINE) {
	}

	virtual int		Get( std::string& uri ){
		return 0;
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

	virtual int		Preform( CBuffer* pBuffer, IProtocol* pPrevProtocol ){
		
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
				case REQUESTC_LINE:
					m_Req.append( &pBuffer->at( idx ), len );
					if( siz > 0 ){
						m_Req.append( "\n\0" );
						state++;
					}
					break;

				case HEADER:
					m_Header.append( &pBuffer->at( idx ), len );
					if( siz > 0 ){
						m_Header.append( "\n\0" );
						state++;
					}
					break;

				case BODY:
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

		if( state <= BODY ){
			return 0;
		}

		OutputDebugStringA( m_Req.c_str() );
		OutputDebugStringA( m_Header.c_str() );
		OutputDebugStringA( m_Body.c_str() );

		char cmd[32],uri[256],ver[64];

		sscanf( m_Req.c_str(), "%s %s %s", cmd, uri, ver );
		if( strcmp(cmd,"GET") == 0 ){
			Get( std::string(uri) );
		}else if( strcmp(cmd,"POST") == 0 ){
			Post( std::string(uri) );
		}else if( strcmp(cmd,"HEAD") == 0 ){
			Head( std::string(uri) );
		}else if( strcmp(cmd,"PUT") == 0 ){
			Put( std::string(uri) );
		}
		
		return 1;
	}
};
/**********************************************************************************
 */
class CXmlRpcProtocol	: public IProtocol {
public:
	virtual int		Preform( CBuffer* pBuffer, IProtocol* pPrevProtocol ){
		return 0;
	}
};
/**********************************************************************************
 */
class CProtocolManager {
protected:
	typedef std::vector<IProtocol*>		LIST;
	typedef LIST::iterator				ITERATOR;
	
	LIST			m_list;

public:
	void		Append( IProtocol* pProtocol ){
		m_list.push_back( pProtocol );
	}


	void		OnRecv( CBuffer* pBuffer ){
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
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
/**********************************************************************************
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	CProtocolManager	mag;
	mag.Append( new CTcpProtocol() );
	mag.Append( new CHttpProtocol() );
	mag.Append( new CXmlRpcProtocol() );

	std::string		str1("GET /index.html HTTP1.0\n");
	std::string		str2("Content-Length");
	std::string		str2_2(": 100\n\n");
	std::string		str3("xml-value:20\n");

	mag.OnRecv( &str1 );
	mag.OnRecv( &str2 );
	mag.OnRecv( &str2_2 );
	mag.OnRecv( &str3 );
}

