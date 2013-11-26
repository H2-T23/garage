// XmlValue.cpp 
#include <tchar.h>


#include <string>
#include <vector>


#include "TArray.h"

typedef basic_string<TCHAR>	TString;



class IXmlValue {
public:
	virtual TCHAR*		ToString( void ) const	= 0;
	virtual TCHAR*		ToXml( void ) const 	= 0;
};

class XmlValueType {
public:
	enum {
		TYPE_EMPTY = 0, TYPE_INT, TYPE_BOOLEAN, TYPE_DOUBLE, TYPE_STRING,
	};
	
	static TCHAR*		Format( int type ){
		switch( type ){
		case TYPE_INT:		return _T("%d");
		case TYPE_BOOLEAN:	return _T("%s");
		case TYPE_DOUBLE:	return _T("%f");
		case TYPE_STRING:	return _T("%s");
		default:
			return _T("");
		}
	}
	
	static TCHAR*		Tag( int type ){
		switch( type ){
		case TYPE_INT:		return _T("INT");
		case TYPE_BOOLEAN:	return _T("BOOLEAN");
		case TYPE_DOUBLE:	return _T("DOUBLE");
		case TYPE_STRING:	return _T("STRING");
		default:
			return _T("");
		}
	}
};

template<typename T>
class TXmlValue : public IXmlValue {
private:
	T							m_Value;
	std::vector<IXmlValue*>		m_vecChild;
	
public:
	static IXmlValue*	INT( int value ){
		new(TXmlValue<int>(value));
	}
	static IXmlValue*	DOUBLE( double value ){
		return(new TXmlValue<double>(value));
	} 
	static IXmlValue*	BOOLEAN( bool value ){ 
		return(new TXmlValue<bool>(value));
	}
	static IXmlValue*	STRING( TString value ){
		return(new TXmlValue<TString>(value));
	}
	
public:
	TXmlValue( T value = 0 ) : m_Value(value) {}

	void		Append( IXmlValue* pValue ){
		m_vecChild.push_back( pValue );
	}
	
	bool		hasChildren( void ) const {
		return(m_vecChildd.size()==0?false:true);
	}
	
	IXmlValue&	Children( int idx ) const {
		return *(m_vecChild[idx]);
	}
	
public:
	TCHAR*		ToString( void ) const {
		return(NULL);
	}
	
	TCHAR*		ToXml( void ) const {
		return(NULL);
	}
};



class XmlValueNode : public IXmlValue {
private:
	XmlValueNode( void );
	
protected:
	EXmlValueType			m_type;
	TCHAR*					m_strTAG;
	TCHAR*					m_strChildTAG;
	
public:
	XmlValueNode( EXmlValueType type, TCHAR
	
public:
	void		Append( XmlValue* pValue ){
	
	}
	
	bool		hasChildren( void ) const {
		
	}
	
public:
	TCHAR*		ToString( void ) const {
		
	}
	
	TCHAR*		ToXml( void ) const {
	}
};


enum EXmlValueType {
	TYPE_EMPTY, TYPE_INT, TYPE_BOOLEAN, TYPE_DOUBLE, TYPE_DATETIME, TYPE_BASE64, TYPE_ARRAY, TYPE_STRUCT, TYPE_PARAMETER,
}:
	
	
class IXmlValue {
public:
	virtual char*	ToXml( void ) const	= 0;
};
	
class XmlValue : public IXmlValue {
protected:
	const int					m_nType;
	const char*					m_strTAG;
	std::basic_string<char>		m_strValue;
	std::basic_string<char>		m_strXml;
	
public:
	XmlValue( void ) : m_nType(TYPE_EMPTY), m_strTAG("EMPTY") {
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
	
	void	SetValue( DataTime* value ){
		m_strValue	= value->ToString();
	}
	
	void	SetValue( Base64* value ){
		m_strValue	= value->ToString();
	}
	
	
public:
	virtual char*	ToXml( void ) const {
		m_strXml.Format( "<%s>%s</%s>", m_strTAG, m_strValue, m_strTAG );
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
