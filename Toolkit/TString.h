#pragma once

#include <windows.h>
#include <tchar.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <stdarg.h>

/**********************************************************************************
 *
 *
 *
 */
template<typename TYPE, std::size_t BUFF_SIZE = 256>
class basic_stringex : public std::basic_string<TYPE> {
protected:
	inline int	Vsnprintf(CHAR* lpBuf, size_t cch, const CHAR* lpFmt, va_list& argptr){
		return _vsnprintf(lpBuf, cch, lpFmt, argptr);
	}

	inline int	Vsnprintf(WCHAR* lpBuf, size_t cch, const WCHAR* lpFmt, va_list& argptr){
		return _vsnwprintf(lpBuf, cch, lpFmt, argptr);
	}

public:
	basic_stringex( void ) : basic_string<TYPE>() {
	}

	explicit basic_stringex( const TYPE* lpszText ){
		append( lpszText );
	}

	bool					IsEmpty( void ) const {
		return empty();
	}

	void					Empty( void ){
		erase();
	}

	int						Length( void ) const {
		return length();
	}

	basic_stringex<TYPE>	Append( const TYPE* lpszText ){
		append( lpszText );
		return(*this);
	}

	basic_stringex<TYPE>	Format( const TYPE* fmt, ... ){
		TYPE	szBuf[ BUFF_SIZE ];
		va_list	args;
		va_start(args, fmt);
		int	sz	= Vsnprintf(szBuf, _countof(szBuf), fmt, args);
	//	int sz = _vsntprintf(szBuf, _countof(szBuf), fmt, args);
	//	int	sz = 0;
	//	const type_info& info = typeid(fmt[0]);
	//	if( info == typeid(CHAR) ){
	//		sz	= _vsnprintf(szBuf, _countof(szBuf), fmt, args);
	//	}else if( info == typeid(WCHAR) ){
	//		sz	= _vsnwprintf(szBuf, _countof(szBuf), fmt, args);
	//	}
		va_end(args);
		if( (0 < sz) && (sz < BUFF_SIZE) ){
			assign( szBuf );
		}
		return(*this);
	}

	basic_stringex<TYPE>	AppendFormat( const TYPE* fmt, ... ){
		TYPE	szBuf[ BUFF_SIZE ];
		va_list	args;
		va_start(args, fmt);
		int	sz	= Vsnprintf(szBuf, _countof(szBuf), fmt, args);
	//	int sz = _vsntprintf(szBuf, _countof(szBuf), fmt, args);
	//	int	sz = 0;
	//	const type_info& info = typeid(fmt[0]);
	//	if( info == typeid(CHAR) ){
	//		sz	= _vsnprintf(szBuf, _countof(szBuf), fmt, args);
	//	}else if( info == typeid(WCHAR) ){
	//		sz	= _vsnwprintf(szBuf, _countof(szBuf), fmt, args);
	//	}
		va_end(args);
		if( (0 < sz) && (sz < BUFF_SIZE) ){
			append( szBuf );
		}
		return(*this);
	}

	basic_stringex<TYPE>&	operator=( const basic_stringex<TYPE>& rObj ){
		if( this != &rObj ){
			assign( rObj );
		}
		return(*this);
	}

	basic_stringex<TYPE>&	operator=( const TYPE* lpszText ){
		assign( lpszText );
		return(*this);
	}

	operator const TYPE*(){
		return this->c_str();
	}
	operator TYPE*(){
		return (TYPE*)this->c_str();
	}

	static
	BOOL		A2W( const CHAR* lpSrc, const int cbMultiByte, WCHAR* lpDst, const int cchWideChar ){
		if( ::MultiByteToWideChar(CP_THREAD_ACP, MB_COMPOSITE, lpSrc, cbMultiByte, lpDst, cchWideChar) )
			return TRUE;
		return FALSE;
	}

	static 
	BOOL		A2W( const basic_stringex<CHAR>& strSrc, basic_stringex<WCHAR>& strDst ){
		BOOL	bResult = FALSE;

		int nBufSize = ::MultiByteToWideChar(CP_THREAD_ACP, MB_COMPOSITE, strSrc.c_str(), strSrc.length(), NULL, 0 );
		if( nBufSize > 0 ){
			WCHAR*	lpszBuf = new WCHAR[ nBuSize + 1 ];

			if( ::MultiByteToWideChar(CP_THREAD_ACP, MB_COMPOSITE, strSrc.c_str(), strStr.length(), lpszBuf, nBufSize) ){
				bResult	= TRUE;
				lpszBuf[ nBufSize ]	= _T('\0');
				strDst	= lpszBuf;
			}

			delete[] lpszBuf;
		}

		return bResult;
	}

	static
	BOOL		W2A( const WCHAR* lpSrc, const int cchWideChar, CHAR* lpDst, const int cbMultiBye ){
		if( ::WideCharToMultiByte(CP_THREAD_ACP, 0, lpSrc, cchWideChar, lpDst, cbMultiByte, NULL, NULL) )
			return TRUE;
		return FALSE;
	}

	static
	BOOL		W2A( const basic_stringex<WCHAR>& strSrc, basic_stringex<CHAR>& strDst ){
		BOOL	bResult	= FALSE;

		int	nBufSize = ::WideCharToMultiByte(CP_THREAD_ACP, 0, strSrc.c_str(), strSrc.length(), NULL, 0, NULL, NULL );
		if( nBufSize > 0 )
		{
			CHAR*	lpszBuf	= new CHAR[ nBufSize + 1 ];

			if( ::WideCharToMultiByte(CP_THREAD_ACP, 0, strSrc.c_str(), strSrc.length(), lpszBuf, nBufSize, NULL, NULL) ){
				bResult	= TRUE;
				lpszBuf[ nBufSize ]	= '\0';
				strDst	= lpszBuf;
			}

			delete[] lpszBuf;
		}
		return bResult;
	}
};

/**********************************************************************************
 *
 *
 *
 */
typedef basic_stringex<CHAR>			AString;
typedef basic_stringex<WCHAR>			WString;
typedef basic_stringex<TCHAR>			TString;


/**********************************************************************************
 *
 *
 *
 */
class TStringList : public std::list<TString> {
public:
	typedef std::list<TString>::iterator	POSITION;

public:
	// リストの先頭要素を返す 
	TString&	GetHead( void ){
		return this->front();
	}
	// リストの末尾要素を返す 
	TString&	GetTail( void ){
		return this->back();
	}
public:
	// 要素をリストの先頭に追加する 
	void		AddHead( TString& str ){
		this->push_front( str );
	}
	// 要素をリストの末尾に追加する 
	void		AddTail( TString& str ){
		this->push_back( str );
	}
	// リストからすべての要素を削除する 
	void		RemoveAll( void ){
		this->clear();		// リストから全ての要素を削除 
	}
	// リストの先頭にある要素を削除する 
	void		RemoveHead( void ){
		this->pop_front();	// リストの先頭要素を削除  
	}
	// リストの末尾にある要素を削除する 
	void		RemoveTail( void ){
		this->pop_back();	// リストの最終要素を削除 
	}
public:
//	// 指定した位置の要素を取得する 
//	TString&	GetAt( POSITION pos ) const	{
//		this->insert( 
//	}
	// 指定した位置の要素を設定する 
	void	SetAt( ){
	}
	// 指定した位置の要素をリストから削除する 
	void	RemoveAt( TString& str ){
		this->remove( str );
	}
public:
	// リストの要素数を返す 
	int		GetSize( void ) const {
		return this->size();
	}
	// リストの要素数を返す 
	int		GetCount( void ) const {
		return this->size();
	}
	// リストが空かどうか返す 
	BOOL	IsEmpty( void ) const {
		return(empty()==false?FALSE:TRUE);
	}
};
