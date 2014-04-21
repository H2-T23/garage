#pragma once

#include "TString.h"
#include "Thread.h"

class DBG 
{
public:
	class COutput {
	public:
		virtual void	Trace( LPCWSTR lpString ){
			::OutputDebugStringW( lpString );
		}
		virtual void	Trace( LPCSTR lpString ){
			::OutputDebugStringA( lpString );
		}
	};

	class CMsg : public COutput {
	public:
		virtual void	Trace( LPCWSTR lpString ){
			MessageBoxW(NULL, lpString, L"OK", MB_OK);
		}
		virtual void	Trace( LPCSTR lpString ){
			MessageBoxA(NULL, lpString, "OK", MB_OK);
		}
	};

	class CLog : public COutput {
	public:
		virtual void	Trace( LPCWSTR lpString ){
		}
		virtual void	Trace( LPCSTR lpString ){
		}
	};

private:
	COutput			m_DefaultOut;
	COutput*		m_pOut;

	DBG( void ) : m_pOut(&m_DefaultOut) {}
	~DBG( void ){
		Destroy( m_pOut );
	}

	inline void		SetDefault( void ){
		m_pOut = &m_DefaultOut;
	}

	inline bool		IsDefault( const COutput* pOutput ) const {
		return(&m_DefaultOut == pOutput);
	}

	inline bool		IsDefault( void ) const {
		return IsDefault(m_pOut);
	}

	inline void		Destroy( COutput* pOutput ){
		if( !IsDefault(pOutput) )
			delete pOutput;
	}

	void	Attach( COutput* pOutput ){
		if( pOutput == m_pOut )
			return;

		COutput* pPrev = m_pOut;
		m_pOut = pOutput;

		Destroy( pPrev );
	}

	void	Detach( void ){
		if( IsDefault() )
			return;

		Destroy( m_pOut );
		SetDefault();
	}

	void		Trace( LPCWSTR lpString ){
		m_pOut->Trace( lpString );
	}

	void		Trace( LPCSTR lpString ){
		m_pOut->Trace( lpString );
	}

	static DBG&		Instance( void ){
		static DBG	dbg;
		return dbg;
	}

public:
	static MT::CCriticalSection	cs;

#ifndef TRACE
	static void		TRACE( LPCWSTR fmt, ... ){
		MT::CSingleLock	lock(&cs);
		lock.Lock();
		const int BufMax = 256;
		WCHAR	szBuf[ BufMax ];

		va_list	args;
		va_start(args, fmt);
		int sz = _vsnwprintf(szBuf, _countof(szBuf), fmt, args);
		va_end(args);

		if( sz > 0 ){
			TString	str(szBuf);
			str.Append( L"\n" );
			Instance().Trace( str );
		}

		lock.Unlock();
	}

	static void		TRACE( LPCSTR fmt, ... ){
		MT::CSingleLock	lock(&cs);
		lock.Lock();
		const int	BufMax = 256;
		CHAR	szBuf[ BufMax ];

		va_list	args;
		va_start(args, fmt);
		int	sz	= _vsnprintf(szBuf, _countof(szBuf), fmt, args);
		va_end(args);

		if( sz > 0 ){
			AString		str(szBuf);
			str.Append( "\n" );
			Instance().Trace( str );
		}

		lock.Unlock();
	}
#endif

public:
	static void		Initialize( COutput* pOutput = NULL ){
		Instance().Attach( pOutput );
	}

	static void		Uninitialize( void ){
		Instance().Detach();
	}

#ifdef	_DEBUG
	void	UintTest( void )
	{
		DBG::Initialize( new CLog() );

		DBG::Uninitialize();
	}
#endif

};

MT::CCriticalSection	DBG::cs;
