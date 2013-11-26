#pragma once
#include "Cube.h"

#define	CUBE_VECT_NUM	3

class CWorldCoordinates
{
	C3DGeometry*	m_p3D ;
	CVector3D		m_ViewVertex ;
	Polar			m_ViewPolar ;

#if 0
    CCube*   m_pCube[ CUBE_VECT_NUM ][ CUBE_VECT_NUM ][ CUBE_VECT_NUM ];
#else
	CCube*   m_pCube ;
#endif

    CBitmap m_WindowBitmap;
    CDC     m_WindowDC;

public:
	CWorldCoordinates()
	{
		m_p3D	= new C3DGeometry ;

#if 0
		for( int i=0; i<CUBE_VECT_NUM; i++ )
			for( int j=0; j<CUBE_VECT_NUM; j++ )
				for( int k=0; k<CUBE_VECT_NUM; k++ )
					m_pCube[ i ][ j ][ k ] = new CCube( i, j, k, m_p3D );
#else
		m_pCube	= new CCube( 2, 2, 2, m_p3D ) ;
#endif

		InitWorld() ;
		TransferScreen() ;
	}

	~CWorldCoordinates()
	{
#if 0
		for( int i=0; i<CUBE_VECT_NUM; i++ )
			for( int j=0; j<CUBE_VECT_NUM; j++ )
				for( int k=0; k<CUBE_VECT_NUM; k++ )
					delete m_pCube[ i ][ j ][ k ] ;
#else
		delete m_pCube ;
#endif

		delete m_p3D ;
	}

	void	InitWorld()
	{
		m_ViewPolar.r = 600;
		m_ViewPolar.p = 0;
		m_ViewPolar.q = 0;
		m_p3D->PolarToVertex( m_ViewPolar, m_ViewVertex );
	}

	void	TransferScreen()
	{
#if 0
		for( int i=0; i<CUBE_VECT_NUM; i++ )
			for( int j=0; j<CUBE_VECT_NUM; j++ )
				for( int k=0; k<CUBE_VECT_NUM; k++ )
					m_pCube[ i ][ j ][ k ]->TransferScreen( m_ViewPolar );
#else
		m_pCube->TransferScreen( m_ViewPolar ) ;
#endif
	}

	void	DrawRubic( CDC* pDC, CRect& rect )
	{
		int	WINDOW_W	= rect.Width() ;
		int	WINDOW_H	= rect.Height() ;

		m_WindowDC.CreateCompatibleDC( pDC );
		m_WindowBitmap.CreateCompatibleBitmap( pDC, WINDOW_W, WINDOW_H );
		m_WindowDC.SelectObject( &m_WindowBitmap );
		m_WindowDC.PatBlt(0, 0, WINDOW_W, WINDOW_H, PATCOPY);

#if 0
		for( int i=0; i<CUBE_VECT_NUM; i++ )
			for( int j=0; j<CUBE_VECT_NUM; j++ )
				for( int k=0; k<CUBE_VECT_NUM; k++ )
					m_pCube[ i ][ j ][ k ]->DrawCube( &m_WindowDC );
#else
		m_pCube->DrawCube( &m_WindowDC ) ;
#endif

		pDC->BitBlt( 0, 0, WINDOW_W, WINDOW_H, &m_WindowDC, 0, 0, SRCCOPY );

		m_WindowDC.DeleteDC();
		m_WindowBitmap.DeleteObject();
	}
};
