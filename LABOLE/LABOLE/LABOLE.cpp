// LABOLE.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	::OleInitialize(NULL);
	{
		MULTI_QI	qi;

		qi.pIID	= &IID_IDataObject;
		qi.pItf	= NULL;


		HRESULT	hResult	= ::CoGetInstanceFromFile(
								NULL
							,	NULL
							,	NULL
							,	CLSCTX_LOCAL_SERVER
							,	STGM_READ
							,	L"C:\\Windows\\System32\\MSDRM\\MsoIrmProtector.xls"
							,	1
							,	&qi
							);
		if( FAILED(hResult) ){
			::OleUninitialize();	
			return 0;
		}

		IDataObject*	pDataObj	= static_cast<IDataObject*>( qi.pItf );

		FORMATETC	fmtetc	= {0};
		fmtetc.cfFormat		= (CLIPFORMAT)::RegisterClipboardFormat( L"Cvs" );
		fmtetc.ptd			= NULL;
		fmtetc.dwAspect		= DVASPECT_CONTENT;
		fmtetc.lindex		= -1;
		fmtetc.tymed		= TYMED_HGLOBAL;

		STGMEDIUM	medium	= {0};

		pDataObj->GetData( &fmtetc, &medium );

		CHAR*	pChar	= (CHAR*)::GlobalLock( medium.hGlobal );
		::MessageBoxA( NULL, pChar, "OK", MB_OK );
		::GlobalUnlock( medium.hGlobal );

		::ReleaseStgMedium( &medium );
		pDataObj->Release();
	}
	::OleUninitialize();

	return 0;
}

