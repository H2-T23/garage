#pragma once

class CHttpProtocol;
/**********************************************************************************
 */
class CXmlRpcProtocol	: public CHttpProtocol {
public:
	virtual int			Post( std::string& uri ){
		TRACE("XmlRpc:%s",m_Body.c_str());

		return 1;
	}
};
