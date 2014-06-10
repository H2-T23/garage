#include "StdAfx.h"
#include "Texture.h"

BOOL	CTexture::CreateTexImage( void ){
	DeleteTexImage();

	int	nShift	= 0;
	int	nSize	= sizeof(int) * 8;

	for( int i=0; i<nSize; i++ ){
		if( (m_bmp.Width() >> i) & 0x00000001 ){
			nShift	= i;
		}
	}

	Width()		= 0x00000001 << nShift;

	for( int i=0; i<nSize; i++ ){
		if( (m_bmp.Height() >> i) & 0x00000001 ){
			nShift	= i;
		}
	}

	Height()	= 0x00000001 << nShift;

	m_RawImage.clear();
	m_RawImage.resize( 3 * Width() * Height() );

	POINT	ptStart, ptEnd;
	ptStart.x	= (m_bmp.Width() - Width()) / 2;
	ptStart.y	= (m_bmp.Height() - Height()) / 2;

	ptEnd.x		= ptStart.x + Width(); 
	ptEnd.y		= ptStart.y + Height();

	int	nBound		= (4 - (3 * m_bmp.Width()) % 4) % 4;
	int	nLineBytes	= 3 * m_bmp.Width() + nBound;

	for( int y=ptStart.y; y<ptEnd.y; y++ ){
		for( int x=ptStart.x; x<ptEnd.x; x++ )
		{
			int	nInOfs	= 3 * (Width() * (y - ptStart.y) + (x - ptStart.x));
			int	nOutOfs	= nLineBytes * y + 3 * y;

			m_RawImage[ nInOfs + 0 ]	= m_RawImage[ nOutOfs + 2 ];
			m_RawImage[ nInOfs + 1 ]	= m_RawImage[ nOutOfs + 1 ];
			m_RawImage[ nInOfs + 2 ]	= m_RawImage[ nOutOfs + 0 ];
		}
	}

	return TRUE;
}

void	CTexture::DeleteTexImage( void ){
	Clear();
}
