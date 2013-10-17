// OpenGLView.cpp 

#include "UseMFC.h"
#include "UseOpenGL.h"
#include "OpenGLView.h"

#define		ZERO	(0.0)
#define		ONE		(1.0)
#define		HALF	(ONE/2.0)


static void		DrawTraiangle( void );
static void		DrawCube1( void );
static void		DrawCube2( void );

/*******************************************************************************
 *
 *
 */
IMPLEMENT_DYNCREATE(COpenGLView,CView)
/*******************************************************************************
 *
 *
 */
BEGIN_MESSAGE_MAP(COpenGLView,CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()


/*******************************************************************************
 *
 *
 */
COpenGLView::COpenGLView( void ) : m_hGLRC(NULL) 
{
}
/*******************************************************************************
 *
 *
 */
COpenGLView::~COpenGLView( void )
{
}

/*******************************************************************************
 *
 *
 */
BOOL	COpenGLView::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow( cs );
}

/*******************************************************************************
 *
 *
 */
void	COpenGLView::OnDraw( CDC* pDC )
{
	::wglMakeCurrent( pDC->GetSafeHdc(), m_hGLRC );
	
	::glEnable( GL_DEPTH_TEST );
	::glEnable( GL_CULL_FACE );
	
	::glClearColor( ZERO, ZERO, ONE/4.0, ONE );
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	
	DrawCube1();
	
	::wglMakeCurrent( NULL, NULL );
}

/*******************************************************************************
 *
 *
 */
int		COpenGLView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CView::OnCreate(lpCreateStruct) == -1 )
		return -1;
	
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 
		0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		32,
		0, 
		0,
		PFD_MAIN_PLANE,
		0,
		0,
		0,
		0
	};
	
	CClientDC	dc(this);
	
	int nPixfmt = ::ChoosePixelFormat( dc.GetSafeHdc(), &pfd );
	if( nPixfmt == 0 ){
		return -1;
	}
	
	BOOL bRet	= ::SetPixelFormat( dc.GetSafeHdc(), nPixfmt, &pfd );
	if( bRet == FALSE ){
		return -1;
	}
	
	m_hGLRC		= ::wglCreateContext( dc.GetSafeHdc() );
	if( m_hGLRC == NULL ){
		return -1;
	}
	
	return 0;
}

/*******************************************************************************
 *
 *
 */
void	COpenGLView::OnDestroy()
{
	CView::OnDestroy();
	
	if( m_hGLRC != NULL ){
		::wglDeleteContext( m_hGLRC );
		m_hGLRC = NULL;
	}
}

/*******************************************************************************
 *
 *
 */
void	COpenGLView::OnSize( UINT nType, int cx, int cy )
{
	CView::OnSize(nType, cx, cy);
	
	CClientDC	dc(this);
	
	::wglMakeCurrent( dc.GetSafeHdc(), m_hGLRC );
	
	::glViewport( 0, 0, cx/2, cy/2 );
	
	::glMatrixMode( GL_PROJECTION );
	::glLoadIdentity();
	::gluPerspective( 45.0, (double)cx/(double)cy, 0.1, 100.0 );
	
	::glMatrixMode( GL_MODELVIEW );
	::glLoadIdentity();
	::gluLookAt( 2.5, 2.5, 2.5, ZERO, ZERO, ZERO, ZERO, ONE, ZERO );
	
	::wglMakeCurrent( NULL, NULL );
}

/*******************************************************************************
 *
 *
 */
void		DrawTriangle( void )
{
	::glColor3f( ONE, ONE, ZERO );
	::glBegin( GL_POLYGON );
	{
		::glVertex2f( -0.25, -0.5 );
		::glVertex2f(  0.25, -0.5 );
		::glVertex2f(  0.0 ,  0.5 );
	}
	::glEnd();
}

/*******************************************************************************
 *
 *
 */
void	DrawCube1( void )
{
	static GLfloat	vertices[][ 3 ] = {
		{ -HALF, -HALF,  HALF },
		{  HALF, -HALF,  HALF },
		{  HALF,  HALF,  HALF },
		{ -HALF,  HALF,  HALF },
		{  HALF, -HALF, -HALF },
		{ -HALF, -HALF, -HALF },
		{ -HALF,  HALF, -HALF },
		{  HALF,  HALF, -HALF },
	};
	
	
	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 0 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 1 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 2 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 3 ] );
	}
	::glEnd();
	
	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 4 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 5 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 6 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 7 ] );
	}
	::glEnd();

	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 1 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 4 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 7 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 2 ] );
	}
	::glEnd();
	
	// 
	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 5 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 0 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 3 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 6 ] );
	}
	::glEnd();
	
	// upper
	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 3 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 2 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 7 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 6 ] );
	}
	::glEnd();

	// lower
	::glBegin( GL_POLYGON );
	{
		::glColor3f( ONE, ZERO, ZERO );
		::glVertex3fv( vertices[ 1 ] );
		::glColor3f( ZERO, ONE, ZERO );
		::glVertex3fv( vertices[ 0 ] );
		::glColor3f( ZERO, ZERO, ONE );
		::glVertex3fv( vertices[ 5 ] );
		::glColor3f( ONE, ONE, ONE );
		::glVertex3fv( vertices[ 4 ] );
	}
	::glEnd();
	
	glGetFloatv( 0 );
	glMultMatrixf( );
}

/*******************************************************************************
 *
 *
 */
void	DrawCube2( void )
{
	static GLfloat	vertices[][ 3 ] = {
		{ -HALF, -HALF,  HALF },
		{  HALF, -HALF,  HALF },
		{  HALF,  HALF,  HALF },
		{ -HALF,  HALF,  HALF },
		{  HALF, -HALF, -HALF },
		{ -HALF, -HALF, -HALF },
		{ -HALF,  HALF, -HALF },
		{  HALF,  HALF, -HALF },
	};
	
	static GLfloat	normals[][ 3 ] = {
		{ ZERO, ZERO, +ONE },
		{ ZERO, ZERO, -ONE },
		{ +ONE, ZERO, ZERO },
		{ -ONE, ZERO, ZERO },
		{ ZERO, +ONE, ZERO },
		{ ZERO, -ONE, ZERO },
	};
	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 0 ] );
		::glVertex3fv( vertices[ 0 ] );
		::glVertex3fv( vertices[ 1 ] );
		::glVertex3fv( vertices[ 2 ] );
		::glVertex3fv( vertices[ 3 ] );
	}
	::glEnd();
	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 1 ] );
		::glVertex3fv( vertices[ 4 ] );
		::glVertex3fv( vertices[ 5 ] );
		::glVertex3fv( vertices[ 6 ] );
		::glVertex3fv( vertices[ 7 ] );
	}
	::glEnd();
	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 2 ] );
		::glVertex3fv( vertices[ 1 ] );
		::glVertex3fv( vertices[ 4 ] );
		::glVertex3fv( vertices[ 7 ] );
		::glVertex3fv( vertices[ 2 ] );
	}
	::glEnd();

	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 3 ] );
		::glVertex3fv( vertices[ 5 ] );
		::glVertex3fv( vertices[ 0 ] );
		::glVertex3fv( vertices[ 3 ] );
		::glVertex3fv( vertices[ 6 ] );
	}
	::glEnd();
	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 4 ] );
		::glVertex3fv( vertices[ 3 ] );
		::glVertex3fv( vertices[ 2 ] );
		::glVertex3fv( vertices[ 7 ] );
		::glVertex3fv( vertices[ 6 ] );
	}
	::glEnd();
	
	::glBegin( GL_POLYGON );
	{
		::glNormal3fv( normals[ 5 ] );
		::glVertex3fv( vertices[ 1 ] );
		::glVertex3fv( vertices[ 0 ] );
		::glVertex3fv( vertices[ 5 ] );
		::glVertex3fv( vertices[ 4 ] );
	}
	::glEnd();
}



/*
「OpenGLで視点位置を決めて、カメラ座標系を設定する方法」

	最後に、OpenGLでカメラ座標系を設定する方法を解説します。

	ワールド座標系の中に視点を設定し、ワールド座標系からカメラ座標系への
	座標変換をするOpenGLの関数は、glLookAtです。

	この関数はOpenGLの管理する「モデル・ビュー変換行列にワールド座標系からカメラ座標系へ
	の変換行列


「OpenGLの３つの座標系まとめ」

	最後に、この章で解説したことのまとめとして、
	glFrustumで投資と宇平変換を設定し、３次元空間上に立方体を描画するプログラムを示します。


 */

class COpenGL {
private:
	HRC				hDC;
	HGLRC			hGLRC;
	
public:
	void		Draw( void ){
		::wglMakeCurrent( hDC, hGLRC );
		
		::glEnable( GL_DEPTH_TEST );
		::glEnable( GL_CULL_FACE );
		
		::glClearColor( ZERO, ZERO, ZERO, ONE );
		
		
	}
	
	void		Resize( int cx, int cy )
	{
		::wglMakeCurrent( hDC, hGLRC );
		
		::glViewport( ZERO, ZERO, cx, cy );
		
		::glMatrixMode( GL_PROJECTION );
		::glLoadidenitty();
		
		GLdouble	aspcet	= double(cx) / double(cy);
		GLdouble	fovy	= 180.0 / 4;
		
		double		PI		= 3.14;
		
		GLdouble	n		= ONE/10.0;
		GLdouble	t		= n * tan( (fovy / 2) * PI / 180.0);
		GLdouble	r		= t * aspect;
	
		::glFrustum( -r, r, -t, t, ONE/10.0, 100.0 );
		
		::glMatrixMode( GL_MODELVIEW );
		::glloadidentity();
		::glLookat( 2.5, 2.5, 2.5, ZERO, ZERO, ZERO, ZERO, ZERO, ONE, ZERO );
		
		::wglMakeCurrent( NULL, NULL );
	}
};
