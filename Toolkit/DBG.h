#pragma once

#include "TString.h"

class DBG 
{
public:
	class COutput {
	public:
		virtual void	Trace( LPCTSTR lpString ){
			OutputDebugString( lpString );
		}
	};

	class CMsg : public COutput {
	public:
		virtual void	Trace( LPCTSTR lpString ){
			MessageBox(NULL, lpString, _T("OK"), MB_OK);
		}
	};

	class CLog : public COutput {
	public:
		virtual void	Trace( LPCTSTR lpString ){
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

	void		Trace( LPCTSTR lpString ){
		m_pOut->Trace( lpString );
	}

	static DBG&		Instance( void ){
		static DBG	dbg;
		return dbg;
	}

public:
#ifndef TRACE
	static void		TRACE( LPCTSTR fmt, ... ){
		const int BufMax = 256;
		TCHAR	szBuf[BufMax];
		va_list	args;
		va_start(args, fmt);
		int sz = _vsntprintf(szBuf, _countof(szBuf), fmt, args);
		va_end(args);
		TString	str( szBuf );
		str.Append( _T("\n") );
		if( sz > 0 )
		Instance().Trace( str );
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