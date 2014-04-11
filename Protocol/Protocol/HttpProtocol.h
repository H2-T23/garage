#pragma once

class CBuffer;
class IEventHandler;
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
