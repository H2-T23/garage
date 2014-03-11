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
	// ���X�g�̐擪�v�f��Ԃ� 
	TString&	GetHead( void ){
		return this->front();
	}
	// ���X�g�̖����v�f��Ԃ� 
	TString&	GetTail( void ){
		return this->back();
	}
public:
	// �v�f�����X�g�̐擪�ɒǉ����� 
	void		AddHead( TString& str ){
		this->push_front( str );
	}
	// �v�f�����X�g�̖����ɒǉ����� 
	void		AddTail( TString& str ){
		this->push_back( str );
	}
	// ���X�g���炷�ׂĂ̗v�f���폜���� 
	void		RemoveAll( void ){
		this->clear();		// ���X�g����S�Ă̗v�f���폜 
	}
	// ���X�g�̐擪�ɂ���v�f���폜���� 
	void		RemoveHead( void ){
		this->pop_front();	// ���X�g�̐擪�v�f���폜  
	}
	// ���X�g�̖����ɂ���v�f���폜���� 
	void		RemoveTail( void ){
		this->pop_back();	// ���X�g�̍ŏI�v�f���폜 
	}
public:
//	// �w�肵���ʒu�̗v�f���擾���� 
//	TString&	GetAt( POSITION pos ) const	{
//		this->insert( 
//	}
	// �w�肵���ʒu�̗v�f��ݒ肷�� 
	void	SetAt( ){
	}
	// �w�肵���ʒu�̗v�f�����X�g����폜���� 
	void	RemoveAt( TString& str ){
		this->remove( str );
	}
public:
	// ���X�g�̗v�f����Ԃ� 
	int		GetSize( void ) const {
		return this->size();
	}
	// ���X�g�̗v�f����Ԃ� 
	int		GetCount( void ) const {
		return this->size();
	}
	// ���X�g���󂩂ǂ����Ԃ� 
	BOOL	IsEmpty( void ) const {
		return(empty()==false?FALSE:TRUE);
	}
};
