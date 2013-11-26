#pragma once

#include <atlbase.h>

HRESULT	GetProperty( IDispatch* pDisp, OLECHAR* pwszName, VARIANT* pvResult );
HRESULT	PutProperty( IDispatch* pdisp, OLECHAR* pwszName, const VARIANT pvValue );
HRESULT	Invoke( IDispatch* pDisp, OLECHAR* pwszMethod, VARIANT* pvResult, int nArgs, ... );

