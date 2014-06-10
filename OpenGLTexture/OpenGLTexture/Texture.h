#pragma once

#include "BitmapFile.h"

class CTexture : public IImage {
protected:
	CBitmapFile		m_bmp;

	BOOL	CreateTexImage( void );
	void	DeleteTexImage( void );

public:
	void	Clear( void ){
	//	m_bmp.Clear();
		IImage::Clear();
	}

	virtual BOOL	LoadImage( LPCTSTR lpszFilename ) {
		if( !m_bmp.LoadImage( lpszFilename ) ){
			return FALSE;
		}

		return CreateTexImage();
	}

public:
	CTexture( void ){
	}

	virtual ~CTexture( void ){
	}
};

