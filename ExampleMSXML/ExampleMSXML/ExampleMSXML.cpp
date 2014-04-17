// ExampleMSXML.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"

#include <string>

#define	RAW		1
#define	SMART	2

#define	TYPE	SMART

#if TYPE==RAW
#pragma comment(lib,"msxml6.lib")
#include <stdio.h>
#include <windows.h>
#include <objbase.h>
#include <msxml6.h>
#else
#import <msxml6.dll>
//using namespace MSXML2;
#endif

#include "ExampleMSXML.h"

#if TYPE==SMART
class CXmlDomSmart {
public:
	MSXML2::IXMLDOMDocumentPtr		pXmlDoc;
	std::string						strXml;


	~CXmlDomSmart( void ){
		if( pXmlDoc )
			Release();
	}

	BOOL		Create( void ){
		HRESULT	hr	= pXmlDoc.CreateInstance( __uuidof(MSXML2::DOMDocument60), NULL, CLSCTX_INPROC_SERVER );
		return SUCCEEDED(hr);
	}

	void		Release( void ){
		pXmlDoc	= NULL;
		::CoFreeUnusedLibraries();
	}

	void		New( void ){
		try
		{
			MSXML2::IXMLDOMProcessingInstructionPtr	pi	= pXmlDoc->createProcessingInstruction("xml", "version='1.0'");
			pXmlDoc->appendChild( pi );

			MSXML2::IXMLDOMCommentPtr	pc				= pXmlDoc->createComment("sample xml file crated using XML DOM object.");
			pXmlDoc->appendChild( pc );

			MSXML2::IXMLDOMElementPtr	pRoot			= pXmlDoc->createElement("root");

			MSXML2::IXMLDOMAttributePtr	pa				= pXmlDoc->createAttribute("created");
			pa->value	= "using dom";
			pRoot->setAttributeNode( pa );

			MSXML2::IXMLDOMElementPtr	pNode			= pXmlDoc->createElement("node1");
			
			pRoot->appendChild( pXmlDoc->createTextNode("\n\t") );
			pNode->text	= "some character data";
			pRoot->appendChild( pNode );

			pNode	= pXmlDoc->createElement("node2");
			pRoot->appendChild( pXmlDoc->createTextNode("\n\t") );
			pNode->appendChild( pXmlDoc->createCDATASection("<some mark-up text") );
			pRoot->appendChild( pNode );

			pNode	= pXmlDoc->createElement("node3");
			pRoot->appendChild( pXmlDoc->createTextNode("\n\t") );

			MSXML2::IXMLDOMDocumentFragmentPtr	pfd		= pXmlDoc->createDocumentFragment();
			pfd->appendChild( pXmlDoc->createTextNode( "\n\t\t" ) );
			pfd->appendChild( pXmlDoc->createElement( "subNode1" ) );
			pfd->appendChild( pXmlDoc->createTextNode( "\n\t\t" ) );
			pfd->appendChild( pXmlDoc->createElement( "subNode2" ) );
			pfd->appendChild( pXmlDoc->createTextNode( "\n\t\t" ) );
			pfd->appendChild( pXmlDoc->createElement( "subNode3" ) );
			pfd->appendChild( pXmlDoc->createTextNode( "\n\t" ) );
			pNode->appendChild( pfd );
			pRoot->appendChild( pNode );

			pRoot->appendChild( pXmlDoc->createTextNode("\n") );

			pXmlDoc->appendChild( pRoot );

			Save("DynaDOMSmart.xml");
		}
		catch( _com_error errObject )
		{
		}
	}

	void		Load( const char* pFileName ){
		if( Create() )
		{
			try
			{
				pXmlDoc->async				= VARIANT_FALSE;
				pXmlDoc->validateOnParse	= VARIANT_FALSE;
				pXmlDoc->resolveExternals	= VARIANT_FALSE;

				if( pXmlDoc->load( pFileName ) == VARIANT_TRUE )
				{
				
				}
			}
			catch( _com_error errorObject )
			{

			}
		}
	}

	BOOL		LoadXml( const char* pXml ){
		HRESULT	hr = S_FALSE;
		if( pXmlDoc )
			hr = pXmlDoc->loadXML( pXml );
		return SUCCEEDED(hr);
	}

	BOOL		Save( const char* pFileName ){
		HRESULT	hr = S_FALSE;
		if( pXmlDoc )
			pXmlDoc->save( pFileName );
		return SUCCEEDED(hr);
	}
};
#else
class CXmlDomRaw {
public:
	HRESULT		VariantFromString( PCWSTR wszValue, VARIANT& var ){
		HRESULT	hr	= S_OK;

		BSTR	bstr	= SysAllocString( wszValue );
		if( !bstr )
		{
			hr	= E_OUTOFMEMORY;
			return hr;
		}

		V_VT( &var )	= VT_BSTR;
		V_BSTR( &var )	= bstr;

		return hr;
	}

	void		Load( void ){
		::CoInitialize(NULL);

		HRESULT	hr	= S_OK;
		IXMLDOMDocument*	pXmlDoc	= NULL;
		IXMLDOMParseError*	pXmlErr	= NULL;

		BSTR	bstrXML	= NULL;
		BSTR	bstrErr	= NULL;

		VARIANT_BOOL	varStatus;
		VARIANT			varFileName;
		VariantInit( &varFileName );

		hr	= ::CoCreateInstance(__uuidof(DOMDocument60), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pXmlDoc));
		if( SUCCEEDED(hr) )
		{
			pXmlDoc->put_async( VARIANT_FALSE );
			pXmlDoc->put_validateOnParse( VARIANT_FALSE );
			pXmlDoc->put_resolveExternals( VARIANT_FALSE );
		}

		hr	= VariantFromString(L"stock.xml", varFileName);
		if( SUCCEEDED(hr) )
		{
			hr	= pXmlDoc->load(varFileName, &varStatus);
			if( SUCCEEDED(hr) )
			{
				if( varStatus == VARIANT_TRUE )
				{
					pXmlDoc->get_xml( &bstrXML );
				}
				else
				{
					pXmlDoc->get_parseError( &pXmlErr );
					pXmlErr->get_reason( &bstrErr );
				}
			}
		}

		if( pXmlDoc ){
			pXmlDoc->Release();
			pXmlDoc	= NULL;
		}

		if( pXmlErr ){
			pXmlErr->Release();
			pXmlErr	= NULL;
		}

		SysFreeString( bstrXML );
		SysFreeString( bstrErr );
		VariantClear( &varFileName );
	}

	static void		Unload( void ){
		::CoUninitialize();
	}
};
#endif




int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	::CoInitialize(NULL);

	CXmlDomSmart	dom;

	if( dom.Create() )
	{
		dom.New();
		dom.Release();
	}

	::CoFreeUnusedLibraries();

	::CoUninitialize();
}
