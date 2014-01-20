#include <windows.h>
#include <tchar.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include <stdarg.h>

template<typename TYPE>
class basic_stringex : public std::basic_string<TYPE> {
public:
	basic_stringex( void ) : basic_string<TYPE>() {
	}

	explicit basic_stringex( LPCTSTR lpszText ){
		append( lpszText );
	}

	basic_stringex<TYPE>	Append( LPCTSTR lpszText ){
		append( lpszText );
	}

	basic_stringex<TYPE>	Format( LPCTSTR fmt, ... ){
		const int BufMax = 256;
		TCHAR	szBuf[BufMax];
		va_list	args;
		va_start(args, fmt);
		int sz = _vsntprintf(szBuf, _countof(szBuf), fmt, args);
		va_end(args);
		if( (0 < sz) && (sz < BufMax) ){
			OutputDebugString( szBuf );
			assign( szBuf );
		}
		return(*this);
	}

	basic_stringex<TYPE>	AppendFormat( LPCTSTR fmt, ... ){
		const int BufMax = 256;
		TCHAR	szBuf[BufMax];
		va_list	args;
		va_start(args, fmt);
		int sz = _vsntprintf(szBuf, _countof(szBuf), fmt, args);
		va_end(args);
		if( (0 < sz) && (sz < BufMax) ){
			OutputDebugString( szBuf );
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

	basic_stringex<TYPE>&	operator=( LPCTSTR lpszText ){
		assign( lpszText );
		return(*this);
	}

	operator LPCTSTR(){
		return this->c_str();
	}
};
typedef basic_stringex<TCHAR>			TString;


class TStringList : public std::list<TString> {
public:
	typedef std::list<TString>::iterator	POSITION;

public:
	// リストの先頭要素を返す 
	TString&	GetHead( void ) const {
		return this->front();
	}
	// リストの末尾要素を返す 
	TString&	GetTail( void ) const {
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
