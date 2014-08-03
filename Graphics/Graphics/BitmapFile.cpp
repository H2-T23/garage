#include "StdAfx.h"
#include "BitmapFile.h"
#include "File.h"

BOOL	CBitmapFile::LoadImage( LPCTSTR lpszFilename ){
	int	nReadSize	= 0;

	BITMAPFILEHEADER	bmpHdr;
	memset( &bmpHdr, 0x00, sizeof(bmpHdr) );

	CReadOnlyFile	file( lpszFilename );
	if( !file.IsOpen() ){
		return FALSE;
	}

	nReadSize	= file.Read( &bmpHdr, sizeof(bmpHdr) );
	if( nReadSize != sizeof(bmpHdr) ){
		file.Close();
		return FALSE;
	}

	if( bmpHdr.bfType != *(WORD*)"BM" ){
		file.Close();
		return FALSE;
	}

	nReadSize	= file.Read( &m_BitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER) );
	if( nReadSize != sizeof(BITMAPINFOHEADER) ){
		file.Close();
		return FALSE;
	}

	this->m_nNumCompoent	= m_BitmapInfo.bmiHeader.biBitCount >> 3;
	this->m_nWidth			= m_BitmapInfo.bmiHeader.biWidth;
	this->m_nHeight			= m_BitmapInfo.bmiHeader.biHeight;

	int	nBound				= (4 - (3 * m_nWidth) % 4) % 4;
	int	nRawImageSize		= (3 * m_nWidth + nBound) * m_nHeight; 

	m_RawImage.clear();
	m_RawImage.resize( nRawImageSize );

	if( m_nNumCompoent == 3 ){
		file.Seek( bmpHdr.bfOffBits, FILE_BEGIN );
		nReadSize = file.Read( &m_RawImage.at(0), nRawImageSize );
		if( nReadSize != nRawImageSize ){
			Clear();
			file.Close();
			return FALSE;
		}
	}
	else{
		file.Close();
		return FALSE;
	}

	m_strFilename	= lpszFilename;

	file.Close();
	return TRUE;
}