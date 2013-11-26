#pragma once
#include "3DLib.h"
#include "3DGeometry.h"

#define	NUM_VERTEX	8
#define	NUM_FACE	6
#define	CUBE_SIZE	200

typedef struct _tagFace
{
	CVector3D*		pVertexArray[ 4 ] ;
	CVector3D		Center ;
} FACE ;

class CCube
{
	C3DGeometry*	m_p3D ;
	CVector3D		m_Center ;
	CVector3D		m_Vertex[ NUM_VERTEX ] ;
	FACE			m_Face[ NUM_FACE ] ;

public:
	CCube(void);
	~CCube(void);

	CCube( int x, int y, int z, C3DGeometry* p3D )
	{
		InitCube( x, y, z );
		m_p3D = p3D ;
	}

	void	InitCube( int x, int y, int z )
	{
		InitVertex( x - 1, y - 1, z - 1 );
		InitFace( x, y, z );
	}

	void	InitVertex( int x, int y, int z )
	{
		int cx, cy, cz;     //óßï˚ëÃÇÃíÜêSÇÃç¿ïW

		cx = m_Center.x = x * CUBE_SIZE;
		cy = m_Center.y = y * CUBE_SIZE;
		cz = m_Center.z = z * CUBE_SIZE;

		SetVertex( 0, cx + CUBE_SIZE/2, cy + CUBE_SIZE/2, cz + CUBE_SIZE/2 );
		SetVertex( 1, cx + CUBE_SIZE/2, cy + CUBE_SIZE/2, cz - CUBE_SIZE/2 );
		SetVertex( 2, cx - CUBE_SIZE/2, cy + CUBE_SIZE/2, cz - CUBE_SIZE/2 );
		SetVertex( 3, cx - CUBE_SIZE/2, cy + CUBE_SIZE/2, cz + CUBE_SIZE/2 );
		SetVertex( 4, cx + CUBE_SIZE/2, cy - CUBE_SIZE/2, cz + CUBE_SIZE/2 );
		SetVertex( 5, cx + CUBE_SIZE/2, cy - CUBE_SIZE/2, cz - CUBE_SIZE/2 );
		SetVertex( 6, cx - CUBE_SIZE/2, cy - CUBE_SIZE/2, cz - CUBE_SIZE/2 );
		SetVertex( 7, cx - CUBE_SIZE/2, cy - CUBE_SIZE/2, cz + CUBE_SIZE/2 );
	}

	void	InitFace( int x, int y, int z )
	{
		int qube_num = x + y * 3 + z * 9;     //óßï˚ëÃÇÃí Çµî‘çÜ
		SetFace( 0, 0, 3, 2, 1, qube_num );
		SetFace( 1, 5, 4, 0, 1, qube_num );
		SetFace( 2, 1, 2, 6, 5, qube_num );
		SetFace( 3, 2, 3, 7, 6, qube_num );
		SetFace( 4, 3, 0, 4, 7, qube_num );
		SetFace( 5, 7, 4, 5, 6, qube_num );
	}

	void	SetVertex( int n, double x, double y, double z )
	{
		m_Vertex[ n ].x = x;
		m_Vertex[ n ].y = y;
		m_Vertex[ n ].z = z;
	}

	void	SetFace( int n, int ver1, int ver2, int ver3, int ver4, int k )
	{
		m_Face[ n ].pVertexArray[ 0 ] = &m_Vertex[ ver1 ] ;
		m_Face[ n ].pVertexArray[ 1 ] = &m_Vertex[ ver2 ] ;
		m_Face[ n ].pVertexArray[ 2 ] = &m_Vertex[ ver3 ] ;
		m_Face[ n ].pVertexArray[ 3 ] = &m_Vertex[ ver4 ] ;
		m_Face[ n ].Center.x = ( m_Vertex[ ver1 ].x + m_Vertex[ ver2 ].x + m_Vertex[ ver3 ].x + m_Vertex[ ver4 ].x ) / 4 ;
		m_Face[ n ].Center.y = ( m_Vertex[ ver1 ].y + m_Vertex[ ver2 ].y + m_Vertex[ ver3 ].y + m_Vertex[ ver4 ].y ) / 4 ;
		m_Face[ n ].Center.z = ( m_Vertex[ ver1 ].z + m_Vertex[ ver2 ].z + m_Vertex[ ver3 ].z + m_Vertex[ ver4 ].z ) / 4 ;
	}

	void	TransferScreen( Polar ViewPolar )
	{
		for( int i=0; i<NUM_VERTEX; i++ )
		{
			m_p3D->TransferScreen( ViewPolar, m_Vertex[ i ] ) ;
		}
	}

	void	DrawCube( CDC* pDC )
	{
		CPoint	ptStart ;
		CPoint	ptEnd ;
		for( int i=0; i<NUM_FACE; i++ )
		{
			ptStart.x	= m_Face[ i ].pVertexArray[ 0 ]->x ;
			ptStart.y	= m_Face[ i ].pVertexArray[ 0 ]->y ;
			ptEnd.x		= m_Face[ i ].pVertexArray[ 1 ]->x ;
			ptEnd.y		= m_Face[ i ].pVertexArray[ 1 ]->y ;
			DrawLine( pDC, ptStart, ptEnd ) ;

			ptStart.x	= m_Face[ i ].pVertexArray[ 1 ]->x ;
			ptStart.y	= m_Face[ i ].pVertexArray[ 1 ]->y ;
			ptEnd.x		= m_Face[ i ].pVertexArray[ 2 ]->x ;
			ptEnd.y		= m_Face[ i ].pVertexArray[ 2 ]->y ;
			DrawLine( pDC, ptStart, ptEnd ) ;

			ptStart.x	= m_Face[ i ].pVertexArray[ 2 ]->x ;
			ptStart.y	= m_Face[ i ].pVertexArray[ 2 ]->y ;
			ptEnd.x		= m_Face[ i ].pVertexArray[ 3 ]->x ;
			ptEnd.y		= m_Face[ i ].pVertexArray[ 3 ]->y ;
			DrawLine( pDC, ptStart, ptEnd ) ;

			ptStart.x	= m_Face[ i ].pVertexArray[ 3 ]->x ;
			ptStart.y	= m_Face[ i ].pVertexArray[ 3 ]->y ;
			ptEnd.x		= m_Face[ i ].pVertexArray[ 0 ]->x ;
			ptEnd.y		= m_Face[ i ].pVertexArray[ 0 ]->y ;
			DrawLine( pDC, ptStart, ptEnd ) ;
		}
	}

	void	DrawLine( CDC* pDC, CPoint& ptStart, CPoint& ptEnd )
	{
		pDC->MoveTo( ptStart ) ;
		pDC->LineTo( ptEnd ) ;
		TRACE(_T("START[%d][%d]\n"), ptStart.x, ptStart.y ) ;
		TRACE(_T("E N D[%d][%d]\n"), ptEnd.x, ptEnd.y ) ;	
	}
};
