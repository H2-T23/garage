// TArray.h 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

template<typename T>
class TArray {
protected:
	T*				m_pData;
	int				m_nDataNum;
	
	inline void	clear( void ){
		m_pData = NULL;
		m_nDataNum = 0;
	}
	
public:
	TArray( void )
	: m_pData(NULL), m_nDataNum(0) {
	}
	
	TArray( int num )
	: m_pData(NULL), m_nDataNum(0) {
		allocate(num);
	}
	
public:
	int			allocate( int num ){
		if( m_pData )
			return(m_nDataNum);
		
		m_pData = (T*)malloc( sizeof(T) * num );
		if( m_pData ){
			memset( m_pData, 0x0, sizeof(T) * num );
			m_nDataNum = num;
		}
		return(m_nDataNum);
	}
	
	int			reallocate( int num ){
	}
	
	void		deallocate( void ){
		if( m_pData ){
			free( m_pData );
		}
		clear();
	}
};
