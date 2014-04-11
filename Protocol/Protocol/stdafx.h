// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

#include <sstream>
#include <iostream>

#include <vector>
#include <map>
#include <string>

#include <winsock2.h>
#include <mswsock.h>

#pragma comment(lib,"Ws2_32.lib")

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
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