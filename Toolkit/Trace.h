#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/**************************************************************************
 */
class CTrace {
public:
	static void	Trace( const char* fmt, ... )
	{
		CHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringA( buf );
		}
		va_end(args);
	}

	static void	Trace( const WCHAR* fmt, ... )
	{
		WCHAR	buf[256];
		va_list	args;
		va_start(args, fmt);
		if( _vsnwprintf(buf,_countof(buf),fmt,args) > 0 ){
			OutputDebugStringW( buf );
		}
		va_end(args);
	}
};
#define	TRACE	CTrace::Trace
