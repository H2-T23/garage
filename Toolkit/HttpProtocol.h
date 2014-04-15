#pragma once

class CBuffer;
class IEventHandler;
/**********************************************************************************
 */
class CHttpProtocol		: public IEventHandler {
public:
	enum {
		REQUEST_LINE	= 1	,
		REQUEST_HEADER		,
		REQUEST_BODY		,
	};

	std::string		m_RequestHeader;
	std::string		/*m_Req, m_Header, */m_Body;
	int state;
	int	response;

	CHttpProtocol( void ) : state(0) {
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

	int				ReadLine( const CBuffer* pBuffer, int iStart, std::string& str ){
		int idx = pBuffer->find( '\n', iStart );
		if( (idx > 0) && (idx > iStart) ){
			str.assign( &pBuffer->at( iStart ), (idx - iStart + 1) );
			return idx + 1;
		}
		return 0;
	}

	virtual int		OnRead( LPVOID lpParam )
	{
		CBuffer* pBuffer = (CBuffer*)lpParam;

		int	len	= pBuffer->length();
		int	idx	= 0;

		if( 0 == state )
		{
			idx	= pBuffer->find("\n\n", 0);
			if( idx <= 0 )
			{
				m_RequestHeader.append( &pBuffer->at( 0 ), len );

				int i = m_RequestHeader.find("\n\n", 0);
				if( i > 0 )
				{
					idx		= len;
					state	= REQUEST_BODY;
				}

				return 0;
			}
			else
			{
				m_RequestHeader.append( &pBuffer->at( 0 ), idx+2 );
				idx += 2;
				state	= REQUEST_BODY;
			}
		}

		if( state == REQUEST_BODY )
		{
			m_Body.append( *pBuffer );
		}

	//	bool	isNext	= bool(len - idx);
	//	while( isNext )
	//	{
	//		switch( state ){
	//		case REQUEST_LINE:
	//			{
	//				int	iStart = m_Req.length();
	//				int iNext = ReadLine( pBuffer, iStart, m_Req );
	//				if( iNext > 0 ){
	//					idx	= iNext;
	//					state++;
	//				}else{
	//					return 0;
	//				}
	//			}
	//			break;

	//		case REQUEST_HEADER:
	//			{
	//				int	iStart	= m_Header.length();
	//				int iNext = ReadLine( pBuffer, iStart, m_Header );
	//				if( iNext <= 0 ){
	//					return 0;
	//				}
	//			}
	//			break;

	//		case REQUEST_BODY:
	//			{
	//			}
	//			break;
	//		}

	//		isNext	= bool(len - idx);
	//	}

	//	if( state <= REQUEST_BODY ){
	//		return 0;
	//	}

	////	int	cnt = 0;
	//	int siz	= -1;

	////	while( 1 )
	//	do
	//	{
	//		int len	= 0;

	//		siz = pBuffer->find('\n', idx);
	//		if( siz > 0 ){
	//			len	= siz - idx;
	//		}else{
	//			len	= pBuffer->length() - idx;
	//		}

	//		if( len > 0 )
	//		{
	//			switch( state ){
	//			case REQUEST_LINE:
	//				m_Req.append( &pBuffer->at( idx ), len );
	//				if( siz > 0 ){
	//					m_Req.append( "\n\0" );
	//					state++;
	//				}
	//				break;

	//			case REQUEST_HEADER:
	//				m_Header.append( &pBuffer->at( idx ), len );
	//				if( siz > 0 ){
	//					char next = pBuffer->at( (siz + 1) );
	//					if( '\n' == next )
	//					{
	//						m_Header.append( "\n\0" );
	//						state++;
	//					}
	//				}
	//				break;

	//			case REQUEST_BODY:
	//				m_Body.append( &pBuffer->at( idx ), len );
	//				if( siz > 0 ){
	//					m_Body.append( "\n\0" );
	//					state++;
	//				}
	//				break;
	//			}

	//			if( siz > 0 )
	//			{
	//				idx = (siz + 1);
	//			}
	//			else
	//			{
	//		//		break;
	//			}
	//		}
	//		else
	//		{
	//			break;
	//		}
	////	}
	//	} while( siz > 0 );

	//	if( state <= REQUEST_BODY ){
	//		return 0;
	//	}

//		TRACE( m_Req.c_str() );
//		TRACE( m_Header.c_str() );
		TRACE( m_Body.c_str() );

		char cmd[32],uri[256],ver[64];

		sscanf( m_RequestHeader.c_str(), "%s %s %s", cmd, uri, ver );
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
