#pragma once

#include <Windows.h>
#include <TString.h>

#include <vector>

class IImage {
public:
	typedef std::vector<BYTE>	IMAGE;
protected:
	int				m_nWidth;
	int				m_nHeight;
	IMAGE			m_RawImage;

public:
	int&			Width( void ){
		return m_nWidth;
	}

	int&			Height( void ){
		return m_nHeight;
	}

	int				RawImageSize( void ) const {
		return m_RawImage.size();
	}

	const BYTE*		RawImage( void ) const {
		if( m_RawImage.size() <=  0 ){
			return NULL;
		}
		return &m_RawImage.at(0);
	}

	virtual void	Clear( void ){
		m_nWidth	= 0;
		m_nHeight	= 0;
		m_RawImage.clear();
	}

public:
	virtual BOOL	LoadImage( LPCTSTR lpszFilename )	= 0;
};


class CBitmapFile : public IImage {
protected:
	BITMAPINFO		m_BitmapInfo;
	int				m_nNumCompoent;
	TString			m_strFilename;

public:
	void	Clear( void ){
		memset( &m_BitmapInfo, 0x00, sizeof(m_BitmapInfo) );
		m_nNumCompoent	= 0;

		IImage::Clear();
	}

	const BITMAPINFO*			GetBitmapInfo() const {
		return &m_BitmapInfo;
	}

	const BITMAPINFOHEADER*		GetBitmapInfoHeader() const {
		return &(m_BitmapInfo.bmiHeader);
	}

	TString&		Filename( void ){ return m_strFilename; }

	virtual BOOL	LoadImage( LPCTSTR lpszFilename );

public:
	CBitmapFile( void ){
		Clear();
	}

	virtual ~CBitmapFile( void ){
		Clear();
	}
};

