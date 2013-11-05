// TString.h 
#pragma once

#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>


/*****************************************************************************************
 *
 *
 *
 *
 */
template<typename T>
class basic_stringex : public std::basic_string<T> {
public:
	void	format(T* pFmt,...){
		TCHAR buf[256];

		va_list	args;
		va_start(args,pFmt);
		_vstprintf(buf, pFmt, args);

		this->append(buf);
		va_end(args);
	}
};

/*****************************************************************************************
 *
 *
 *
 *
 */
typedef basic_stringex<TCHAR>		TString;
