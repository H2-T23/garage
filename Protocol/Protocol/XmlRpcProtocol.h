#pragma once

#include "ICommand.h"

class CHttpProtocol;

/**********************************************************************************
 */
class CXmlRpcValueEx {
protected:
	std::string			m_xml;

public:
	const char*			ToXml( void ){
		return m_xml.c_str();
	}
};

/**********************************************************************************
 */
class CXmlRpcRequest	: public CXmlRpcValueEx {
protected:
	double				dXmlVersion;
	std::string			strMethodName;
	std::string			strParams;

	void		Reset( void ){
		dXmlVersion		= 0;
		strMethodName.clear();
		strParams.clear();
	}

public:
	const char*	GetMethodName( void ) const {
		return strMethodName.c_str();
	}

	const char*	GetParams( void ) const {
		return strParams.c_str();
	}

	bool		IsValid( void  ) const {
		return !strMethodName.empty();
	}

public:
	CXmlRpcRequest( const std::string& strXml ){
		Reset();
		if( !Parse( strXml ) ){
			Reset();
		}
	}

	bool		Parse( const std::string& strXml ){
		std::string		xml( strXml );

		ToUpper( xml );

//		char	strTmp[64];
//		sscanf( strXml.c_str(), "<?xml version=%s?>", strVersion );
//		sscanf( xml.c_str(), "<?xml version=\"%s\"?>", strTmp );

		
		Extract( xml, "METHODNAME", strMethodName );
		Extract( xml, "PARAMS", strParams );

		TRACE("methodname: %s\n",GetMethodName());
		TRACE("params: %s\n",GetParams());

		return true;
	}

	void		ToUpper( std::string& str ){
		bool	bTransform = false;

		char*	p = &str.at( 0 );
		while( *p )
		{
			if( '<' == *p ){
				bTransform	= true;
				p++;
				continue;
			}else if( '>' == *p ){
				bTransform	= false;
				p++;
				continue;
			}

			if( bTransform ){
				*p	= toupper( *p );
			}

			p++;
		}
	}

	void		Extract( const std::string& strXml, const char* pTag, std::string& str ){
		char	tagStart[64];
		char	tagEnd[64];
		sprintf( tagStart, "<%s>\0", pTag );
		sprintf( tagEnd, "</%s>\0", pTag );

		int iStart	= strXml.find( tagStart ) + strlen( tagStart );
		int iEnd	= strXml.find( tagEnd );
		if( iEnd <= strXml.length() )
			str.assign( &strXml.at( iStart ), (iEnd - iStart) );
	}
};
/**********************************************************************************
 */
class CXmlRpcResponse 	: public CXmlRpcValueEx {
public:
	void			Fault( void ){
		m_xml	= 
			"<?xml version=\"1.0\"?>"
			"<methodResponse>"
				"<fault>"
				"<value>"
				"</value>"
				"</fault>"
			"</methodResponse>"
			"\n"
			;
	}
};
/**********************************************************************************
 */
class CXmlRpcProtocol	: public CHttpProtocol {
protected:
	CCommandList			cmdList;

	/**********************************************************************************
	 */
	class CSetDRP : public ICommand {
	public:
		CSetDRP( const char* pName ) : ICommand(pName) {}

		int			Execute( LPARAM lParam, WPARAM wParam ){
			CXmlRpcResponse*	pRes	= (CXmlRpcResponse*)wParam;
			TRACE("%s(%s)\n",GetCommandName(),(char*)lParam);
			pRes->Fault();
			return 0;
		}

	};

public:
	CXmlRpcProtocol( void ) : CHttpProtocol() {
		cmdList.Register( new CSetDRP("SetDRParameter") );
	}

	virtual int			Post( std::string& uri ){
		TRACE("XmlRpc:%s",m_Body.c_str());

		const char	pReq[] = 
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

		ISocket* pSock	= NULL;

		CXmlRpcRequest	req( m_Body );
		if( req.IsValid()  )
		{
			ICommand*	pCmd	= cmdList.Find( req.GetMethodName() );
			if( pCmd )
			{
				CXmlRpcResponse		res;
				if( pCmd->Execute( (LPARAM)req.GetParams(), (WPARAM)&res ) )
				{
					CBuffer	buf( res.ToXml() );
					pSock->Send( &buf );
				}
			}
		}

		return 1;
	}
};
