// XmlValue.cpp 
//#include <tchar.h>


#include <string>
#include <vector>


//#include "TArray.h"

//typedef basic_string<TCHAR>	TString;

/** enum: EXmlValueType */
enum EXmlValueType {
	TYPE_EMPTY
	, TYPE_INT
	, TYPE_BOOLEAN
	, TYPE_DOUBLE
	, TYPE_DATETIME
	, TYPE_BASE64
	, TYPE_ARRAY
	, TYPE_STRUCT
	, TYPE_PARAMETER
};
	
/** class: IXmlValue */
class IXmlValue {
public:
	virtual const char*	ToXml( void ) const	= 0;
};

/** class: XmlValue */	
class XmlValue : public IXmlValue {
protected:
	EXmlValueType			m_nType;
	char*					m_strTAG;
	std::basic_string<char>		m_strValue;
	std::basic_string<char>		m_strXml;
	
public:
	XmlValue( void )
		: m_nType(TYPE_EMPTY), m_strTAG("EMPTY") {
	}
	
	XmlValue( EXmlValueType type, char* pTag ) 
		: m_nType(type), m_strTAG(pTag)
	{
	}
	
public:
	void	SetValue( int value ){
		m_strValue.Format( "%d", value );
	}
	
	void	SetValue( bool value ){
		m_strValue.Format( "%s", value == false ? "false" : "true" );
	}
	
	void	SetValue( double value ){
		m_strValue.Format( "%f", value );
	}
	
	void	SetValue( char* value ){
		m_strValue	= value;
	}
	
	#if 0
	void	SetValue( DataTime* value ){
		m_strValue	= value->ToString();
	}
	
	void	SetValue( Base64* value ){
		m_strValue	= value->ToString();
	}
	#endif
	
public:
	virtual const char*	ToXml( void ) const {
		m_strXml.Format( "<%s>%s</%s>", m_strTAG, m_strValue, m_strTAG );
		return m_strXml.c_str();
	}
};
	
/** class: XmlValueNode */
class XmlValueNode : public XmlValue {
protected:
	std::string					m_strChildTAG;
	std::vector<IXmlValue*>		m_vecChildren;
	
public:
	XmlValueNode(void) : 
		: m_nType(TYPE_EMPTY), m_strTAG("EMPTY"), m_strChildTAG("EMPTY") {
	}
	
public:
	void			Append( IXmlValue* pChild ){
		m_vecChildren.push_back( pChild );
	}
	
	bool			hasChildren( void ) const {
		return(m_vecChildren.size()==0?false:true);
	}
	
	
	void			RemoveAll( void ){
		
	}
	
public:
	virtual const char*	ToXml( void ) const {
		std::string	str;
		
		std::vector<IXmlValue*>::iterator it = m_vecChildren.begin();
		while( it != m_vecChildren.end() )
		{
			str.Format( "\t<%s>%s</%s>\n", m_strChildTAG, (*it)->ToXml(), m_strChildTAG );
			it++;
		}
		
		m_strXml.Format( "<%s>%s</%s>", m_strTAG, str, m_strTAG );
		return m_strXml.c_str();
	}
};
	
	
	
#define	DEFINED_XMLVALUE(classname,type,tag)					\
	class classname : public XmlValue {							\
	public:														\
		classname(void) : m_nType(type), m_strTAG(tag) {}		\
	}
	
#define	DEFINED_XMLVALUE_NODE(classname,type,tag,childtag)		\
	class classname : public XmlValueNode {						\
	public:														\
		classname(void) : m_nType(type), m_strTAG(tag), m_strChildTAG(childtag) {}	\
	};


DEFINED_XMLVALUE(XmlValueInt,TYPE_INT,"int");
DEFINED_XMLVALUE(XmlValueBoolean,TYPE_BOOLEAN,"boolean");
DEFINED_XMLVALUE(XmlValueDoiuble,TYPE_DOUBLE,"double");

DEFINED_XMLVALUE_NODE(XmlValueArray,TYPE_ARRAY,"array","data");
DEFINED_XMLVALUE_NODE(XmlValueStruct,TYPE_STRUCT,"struct","member");
DEFINED_XMLVALUE_NODE(XmlValueParameter,TYPE_PARAMETER,"params","param");

	

/**
	<struct>
		<member><int>3</int></member>
		<member><int>4</int></member>
		<member><int>5</int></member>
	</struct>

	<array>
		<data><double>1.23</double></data>
		<data><double>2.34</double></data>
	</array>
**/


void	test( void )
{
	XmlValue		params;
	params.Append( XmlValue::INT(100) );
	params.Append( XmlValue::DOUBLE(100.0123) );
	params.Append( XmlValue::STRING("mevius") );
	
	
	
	
	
	TString	strXml( params.ToXml() );
}

void main( void )
{
}
