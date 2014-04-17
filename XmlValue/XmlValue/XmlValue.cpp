// XmlValue.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "XmlValue.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

class CXmlElement {
protected:
	string			m_xml;

public:
	template<typename TYPE>
	void		Set( TYPE value ){
		std::stringstream	ss;
		ss << value;
		m_xml.assign( ss.str() );
	}

	const char*		ToXml( void ){
		return m_xml.c_str();
	}
};

class CXmlValue : public CXmlElement {
protected:
	virtual const char*		GetStartTag( void ) const {
		return "<value>";
	}
	virtual const char*		GetEndTag( void ) const {
		return "</value>";
	}
public:
	const char*		ToXml( void ){
		m_xml.insert( 0, GetStartTag() );
		m_xml.append( GetEndTag() );
		return m_xml.c_str();
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	CXmlElement		e1, e2, e3;
	e1.Set( 123 );
	e2.Set( 4.56f );
	e3.Set( "debug" );

	CXmlValue		v1, v2, v3;
	v1.Set( 123 );
	v2.Set( 4.56 );
	v3.Set( "mode" );

	::OutputDebugStringA( v2.ToXml() );
}
