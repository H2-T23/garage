// Buffer.h 
#pragma once

class CBuffer {
private:
	BYTE*			m_pData;
	UINT			m_nDataNum;

public:
	CBuffer( void ) : m_pData(NULL), m_nDataNum(0) {
	}

	CBuffer( int num ) : m_pData(NULL), m_nDataNum(0) {
		Allocate( num );
	}

public:
	bool		IsValid( void ) const {
		return(m_pData!=NULL);
	}

	void			Deallocate( void ){
		if( IsValid() ){
			free( m_pData );
		}
		m_pData		= NULL;
		m_nDataNum	= 0;
	}

	int				Allocate( int num ){
		Deallocate();
		m_pData		= (BYTE*)malloc( num );		
		m_nDataNum	= num;
	}

	UINT			GetSize( void ) const {
		return(m_nDataNum);
	}

	const BYTE*		GetPtr( void ) const {
		return(m_pData);
	}

public:
	BYTE&			operator[]( int idx ) const {
		return(m_pData[ idx ]);
	}
};
