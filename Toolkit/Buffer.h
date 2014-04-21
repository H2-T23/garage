#pragma once

#include <vector>

class IBuffer {
public:

};

class CBuffer {
private:
	typedef unsigned				INDEX, SIZE;
	typedef unsigned char			BYTE;
	typedef unsigned char*			LPBYTE;
	typedef std::vector<BYTE>		BUFFER;

	BUFFER			m_Buffer;

public:
	CBuffer( void ) {}
	CBuffer( const CBuffer& buf ){
		Set( buf );
	}
	CBuffer( const BUFFER& buf ){
		Set( buf );
	}
	CBuffer( const LPBYTE pBuf, SIZE nLen ){
		Set( pBuf, nLen );
	}

public:
	CBuffer&	operator=( const CBuffer& buf ){
		Set( buf );
		return(*this);
	}

	CBuffer&	operator=( const BUFFER& buf ){
		Set( buf );
		return(*this);
	}

public:
	CBuffer&	operator+=( const CBuffer& buf ){
		Append( buf );
		return(*this);
	}

	CBuffer&	operator+=( const BUFFER& buf ){
		Append( buf );
		return(*this);
	}

public:
	inline LPBYTE		At( INDEX idx ) {
		return &(m_Buffer.at( idx ));
	}

	inline SIZE			Size( void ) const {
		return(m_Buffer.size());
	}

	inline bool			IsEmpty( void ) const {
		return(m_Buffer.size()==0);
	}

public:
	SIZE		Set( const LPBYTE pBuf, SIZE nLen ){
		BUFFER	wk(nLen);
		memcpy( &wk.at( 0 ), pBuf, nLen );
		m_Buffer	= wk;
		return Size();
	}

	SIZE		Set( const BUFFER& buf ){
		m_Buffer	= buf;
		return Size();
	}

	SIZE		Set( const CBuffer& buf ){
		m_Buffer	= buf.m_Buffer;
		return Size();
	}

public:
	SIZE		Append( const LPBYTE pBuf, SIZE nLen ){
		SIZE	szOld	= Size();
		Resize( szOld + nLen );
		memcpy( At( szOld ), pBuf, nLen );
		return Size();
	}

	SIZE		Append( const BUFFER&	buf ){
		SIZE	szOld	= Size();
		Resize( szOld + buf.size() );
		memcpy( At( szOld ), &buf.at( 0 ), buf.size() );
		return Size();
	}

	SIZE		Append( const CBuffer& buf ){
		SIZE	szOld	= Size();
		Resize( szOld + buf.Size() );
		memcpy( At( szOld ), const_cast<CBuffer*>(&buf)->At( 0 ), buf.Size() );
		return Size();
	}

	SIZE		Resize( SIZE sz ){
		if( sz > m_Buffer.size() ){
			m_Buffer.resize( sz );
		}
		return Size();
	}

	void		Clear( void ){
		if( Size() > 0 ){
			memset(At(0), 0, Size());
		}
		m_Buffer.clear();
	}

	int			Find( BYTE byte, INDEX idx )
	{
		for( INDEX	i=idx; i<m_Buffer.size(); i++ )
		{
			if( m_Buffer.at( i ) == byte )
			{
				return i; 
			}
		}
		return -1;
	}

	void		Erace( INDEX a, INDEX b ){
	//	m_Buffer.erase( a, b );
	}
};
