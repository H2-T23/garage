// XmlValue.cpp 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


#include <iostream>
#include <string>
#include <vector>

using namespace std;


class stringex : public std::string {
public:
	stringex(void) : 
		std::string(){
	}
	stringex(const stringex& rthis) :
		std::string( (const std::string)rthis ){
	}
	stringex(const char* pstr) :
		std::string( pstr ){
	}

	stringex&	operator=(const stringex& rthis){
		std::string::operator=( (const std::string)rthis );
		return(*this);
	}
	stringex&	operator=(const char* pstr){
		std::string::operator=(pstr);
		return(*this);
	}

public:
	int		format(char* fmt,...){
		static char	buf[(1024*6)];
		memset((char*)buf,0x00,sizeof(buf));
		va_list	arg;
		va_start(arg,fmt);
		int n=vsprintf(buf,fmt,arg);
		va_end(arg);
		if(n>0)
			this->operator=(buf);
		return n;
	}

	int		appendformat(char* fmt,...){
		static char	buf[(1024*6)];
		memset((char*)buf,0x00,sizeof(buf));
		va_list	arg;
		va_start(arg,fmt);
		int n=vsprintf(buf,fmt,arg);
		va_end(arg);
		if(n>0)
			append(buf);
		return n;
	}
};

class CXmlValue;


class CXmlElement {
protected:
	friend class CXmlValue;

	stringex		xml;
	stringex		tag;
	stringex		val;

	CXmlElement(void){}
	CXmlElement(const char* ptag,const char* pval):
		tag(ptag), val(pval){
	}

public:
	virtual string		ToXml(int indent=0){
		printf("indent:%d.\n",indent);
		string	strIndent = getIndent(indent);
		xml.format( "%s<%s>%s</%s>", strIndent.c_str(), tag.c_str(), val.c_str(), tag.c_str() );
		return xml;
	}

	string		getIndent(int num){
		string	str;
		while(num>0){
			str.append( "    " );
			num--;
		}
		return str;
	}
};

class CXmlElementStructMember : public CXmlElement {
protected:
	friend class CXmlValue;

	const char*		m_pName;
	CXmlElement*	m_pXmlElement;

	CXmlElementStructMember(void){
	}

public:
	CXmlElementStructMember( const char* pName, CXmlElement* pElement )
		: m_pName(pName), pXmlElement(pElement){
	}

	virtual string		ToXml(int indent=0){
		
	}
};


class CXmlElementList : public CXmlElement {
protected:
	friend class CXmlValue;

	CXmlElementList(void){}
	CXmlElementList(const char* ptag)
		:CXmlElement(ptag,""){
	}

public:
	vector<CXmlElement*>		items;

	int		appendchild(CXmlElement* pElement){
		items.push_back( pElement );
		return items.size();
	}

	virtual string		ToXml(int indent=0){
		string	strIndent = getIndent(indent);
		indent++;

		xml.format( "%s<%s>\n", strIndent.c_str(), tag.c_str() );
		int num = items.size();
		for( int i=0; i<num; i++ ){
			xml.append( strIndent );
			xml.append( items[i]->ToXml(indent) );
			xml.append( "\n" );
		}
		xml.appendformat( "%s</%s>\n", strIndent.c_str(), tag.c_str() );
		return xml;
	}
};

class CXmlElementArray : public CXmlElementList {
protected:
	friend class CXmlValue;

	const char*		pTagArray;
	const char*		pTagData;
	const char*		pTagValue;

	CXmlElementArray(void)
		: pTagArray("array")
		, pTagData("data")
		, pTagValue("value"){
	}

public:
	virtual string		ToXml(int indent=0){
		string	strIndent	= getIndent(indent);
		const char*	pIndent = strIndent.c_str();
		if(pIndent==NULL)
			pIndent="";

		string strIndentSub = getIndent(indent+1);
		const char* pIndentSub = strIndentSub.c_str();

		indent++;

		xml.format( "%s<%s><%s>\n", pIndent, pTagArray, pTagData );
		for( int i=0; i<items.size(); i++ ){
			xml.appendformat( "%s<%s>", pIndentSub, pTagValue );
			xml.append( items[ i ]->ToXml() );
			xml.appendformat( "</%s>\n", pTagValue );
		}
		xml.appendformat( "%s</%s></%s>\n", pIndent, pTagData, pTagArray );
		return xml;
	}
};

class CXmlElementStruct : public CXmlElementList {
protected:
	friend class CXmlValue;

	const char*		pTagStruct;
	const char*		pTagMember;
	const char*		pTagName;
	const char*		pTagValue;

	CXmlElementStruct(void)
		: pTagStruct("struct")
		, pTagMember("member")
		, pTagName("name")
		, pTagValue("value"){
	}

public:
	virtual string		ToXml(int indent=0){
		string	strIndent	= getIndent(indent);
		const char*	pIndent = strIndent.c_str();
		if(pIndent==NULL)
			pIndent="";

		string strIndentSub = getIndent(indent+1);
		const char* pIndentSub = strIndentSub.c_str();

		indent++;

		xml.format( "%s<%s>\n", pIndent, pTagStruct );
		for( int i=0; i<items.size(); i++ ){
			xml.appendformat( "%s<%s>\n", pIndentSub, pTagMember );
			xml.append( pIndentSub );
			xml.append( items[ i ]->ToXml(indent) );
			xml.appendformat( "\n%s</%s>\n", pIndentSub, pTagMember );
		}
		xml.appendformat( "%s</%s>\n", pIndent, pTagStruct );
		return xml;
	}
};

class CXmlValue : public CXmlElementList {
public:
	CXmlValue(){}

	CXmlElement*		createValue(const bool var){
		stringex str;
		str.format("%d",var==false?0:1);
		return(new CXmlElement("boolean",str.c_str()));
	}

	CXmlElement*		createValue(const int var){
		stringex str;
		str.format("%d",var);
		return(new CXmlElement("int",str.c_str()));
	}

	CXmlElement*		createValue(const double var){
		stringex str;
		str.format("%f",var);
		return(new CXmlElement("double",str.c_str()));
	}

	CXmlElement*		createValue(const string& var){
		return(new CXmlElement("string",var.c_str()));
	}

	CXmlElementList*	createArray(void){
		return(new CXmlElementArray());
	}

	CXmlElementList*	createStruct(void){
		return(new CXmlElementStruct());
	}
};


int main(int,char**)
{
	CXmlValue	xmlvalue;

	cout << 1 << endl;
	CXmlElementList*	pList1 = xmlvalue.createStruct();
	CXmlElementList*	pList2 = xmlvalue.createArray();

	cout << 2 << endl;
	pList1->appendchild( xmlvalue.createValue(123) );

	cout << 3 << endl;
	pList1->appendchild( xmlvalue.createValue(4.5) );

	cout << 4 << endl;
	pList2->appendchild( xmlvalue.createValue(true) );
	pList2->appendchild( xmlvalue.createValue(7.89) );
	pList2->appendchild( xmlvalue.createValue(90.8) );
	pList2->appendchild( xmlvalue.createValue(string("Hello World")) );

	cout << 5 << endl;
	pList1->appendchild( pList2 );

	cout << 6 << endl;
	string	strXml = pList1->ToXml();

	cout << 7 << endl;
	cout << strXml << endl;	

	cout << 8 << endl;
	cout << pList2->ToXml() << endl;

	return 0;
}
