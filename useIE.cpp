#include "useIE.h"
#include <stdarg.h>

HRESULT	GetIDOfName( IDispatch* pDisp, OLECHAR* pwasName, DISPID* pdispID )
{
	HREUSLT	hResult	= pDisp->GetIDsOfNames( IID_NULL, &pwszName, 1, LOCALE_USER_DEFAULT, pdispID );
	return(hResult);
}

HREUSLT	GetProperty( IDispatch* pDisp, OLECHAR* pwszName, VARIANT* pvResult )
{
	DISPID	dispID;	
	HRESULT	hResult = GetIDOfName( pDisp, pwszName, &dispID );
	if( FAILED(hResult) ){
		return(hResult);
	}
	DISPPARAMS	dispParams = { NULL, NULL, 0, 0 };
	hResult	= pDisp->Invoke( dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, pvResult, NULL, NULL );
	return(hResult);
}

HRESULT	PutProperty( IDispatch* pDisp, OLECHAR* pwszName, const VARIANT* pvValue )
{
	HRESLUT	hResult;
	DISPID	dispID;
	DISPID	dispIDNamedArgs[ 1 ] = { DISPID_PROPERTYPUT };
	
	hResult	= GetIDOfName( pDisp, pwszName, &dispID );
	if( FAILED(hResult) ){
		return(hResult);
	}
	
	VARIANTARG	vArgs[ 1 ];
	VariantInit( &vArgs[ 0 ] );
	VariantCopy( &vArgs[ 0 ], const_cast<VARIANT*>( pvValue );
	
	DISPPARAMS	dispParams;
	dispParams.rgvarg				= vArgs;
	dispParams.rgdispidNamedArgs	= dispIDNamedArgs;
	dispParams.cArgs				= 1;
	dispParams.cNamedArgs			= 1;
	
	hResult	= pDisp->Invoke( dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYPUT, &dispParams, NULL, NULL, NULL );
	VariantClear( &vArgs[ 0 ] );
	
	return(hResult);
}

HREUSLT	Invoke( IDispatch* pDisp, OLECHAR* pwszMethod, VARIANT* pvResult, int nArgs, ... )
{
	HREUSLT	hResult;
	DISPID	dispID;
	hResult	= GetIDOfName( pDisp, pwszMethod, &dispID );
	if( FAILED(hResult) ){
		return(hResult);
	}
	
	VARIANTARG*	pvArgs	= NULL;
	if( nArgs > 0 ){
		pvArgs	= new VARIANTARG[ nArgs ];
	
		va_list	ap;
		va_start(ap, nArgs);
		for( int i=0; i<nArgs; i++ ){
			VariantInit( &pvArgs[ i ] );
			VariantCopy( &pvArgs[ i ], &va_arg(ap, VARIANT) );
		}
		va_end(ap);
	}
	
	DISPPARAMS	dispParams;
	dispParams.rgvarg				= pvArgs;
	dispParams.rgdispidNamedArgs	= NULL;
	dispParams.cArgs				= nArgs;
	dispParams.cNamedArgs			= 0;
	
	hResult	= pDisp->Invoke( dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dispParams, pvResult, NULL, NULL );
	
	for( int i=0; i<nArgs; i++ ){
		VariantClear( &pvArgs[ i ] );
	}
	
	if( pvArgs != NULL ){
		delete[] pvArgs;
		pvArgs = NULL;
	}
	
	return(hResult);
}
	
void	useIE( void )
{
	HRESULT	hResult;
	
	hResult	= CoInitialize( NULL );
	if( FAILED(hResult) ){
		std::cout << "CoInitialze Failed" << std::endl;
		exit( 1 );
	}
	
	CLSID	clsID;
	hResult	= CLSIDFromProgID( L"InternetExplorer.Application", &clsID );
	if( FAILED(hResult) ){
		std::cout << "CLSIDFromProgID Failed" << std::endl;
		exit( 1 );
	}
	
	IDispatch* pDispIE	= NULL;
	hResult	= CoCreateInstance( clsid, NULL, CLSCTX_ALL, IID_IDispatch, (void**)&pDispIE );
	if( FAILED(hResult) ){
		std::cout << "CoCreateInstance Failed" << std::endl;
		exit( 1 );
	}
	
	VARIANT	vTrue;
	VariantInit( &vTrue );
	vTrue.boolVal	= VARIANT_TRUE;
	vTrue.vt		= VT_BOOL;
	
	hResult	= PutProperty( pDispIE, L"Visible", &vTrue );
	VariantClear( &vTrue );
	if( FAILED(hResult) ){
		std::cout << "objIE.Visible == True Failed" << std::endl;
		exit( 1 );
	}
	
	VARIANT	vResult, vStr;
	BSTR	bstrURL	= SysAllocString( L"http://www.yahoo.co.jp" );
	VARIANTInit( &vStr );
	vStr.bstrVal	= bstrURL;
	vStr.vt			= VT_BSTR;
	
	VariantInit( &vResult );
	hResult	= Invoke( pDispIE, L"Navigate", &vResult, 1, vStr );
	if( FAILED(hResult) ){
		std::cout << "objIE.Navigate Failed" << std::endl;
		eixt( 1 );
	}
	
	SysFreeString( bstrURL );
	VariantClear( &vResult );
	VariantClear( &vStr );
	
	do{
		hResult	= GetProperty( pDispIE, L"Busy", &vResult );
		if( FAILED(hResult) ){
			std::cout << "objIE.Busy Failed" << std::endl;
			exit( 1 );
		}
		assert( vResult.vt == VT_BOOL );
	}while( vResult.boolVal == VARIANT_TRUE );
	
	VariantClear( &vResult );
	
	IDispatch* pDispDoc;
	hResult	= GetProperty( pDispIE, L"Document", &vResult );
	if( FALIED(hResult) ){
		std::cout << "objIE.Document" << std::endl;
		exit( 1 );
	}
	assert( vResult.vt == VT_DISPATCH );
	pDispDoc	= vResult.pdispVal;
	pDispDoc->AddRef();
	
	VariantClear( &vResult );
	
	LPCTSTR	lpTitle;
	
	USES_CONVERSION;
	hResult	= GetProperty( pDispDoc, L"Title", &vResult );
	if( FAILED(hResult) ){
		std::cout << "objIE.Document.Title" << std::endl;
		eixt( 1 );
	}
	assert( vResult.vt == VT_BSTR );
	
	lpTitle	= OLE2CT( vResult.bstrVal );
	MessageBox( NULL, lpTitle, lptitle, MB_OK );
	
	pDispIE->Release();
	pDispDoc->Release();
	
	assert( vResult.vt == VT_DISPATCH );
	pDispDoc	= vResult.pdispVal;
	pDispDoc->AddRef();
	VariantClear( &vResult );
	
	
	CoUninitialize();
}
