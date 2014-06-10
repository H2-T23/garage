// OpenGLMultiViewport.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "OpenGLMultiViewport.h"

#include "UseToolkit.h"
#include "OpenGL.h"

/**********************************************************************************
 *
 *
 *
 */
class CCubeModel {
public:
	enum {
		X = 0, Y, Z,
		PLANE_NUM	= 8,
	};
protected:
	GLfloat		m_Vertices[PLANE_NUM][3];

	inline
	bool	InRange( int idx ) const {
		return(0<=idx && idx<PLANE_NUM);
	}

	inline
	void	SetVertex( int nPlane, double x, double y, double z ){
		m_Vertices[ nPlane ][ X ]	= (GLfloat)x;
		m_Vertices[ nPlane ][ Y ]	= (GLfloat)y;
		m_Vertices[ nPlane ][ Z ]	= (GLfloat)z;
	}
	inline
	void	SetVertex( int nPlane, float x, float y, float z ){
		m_Vertices[ nPlane ][ X ]	= x;
		m_Vertices[ nPlane ][ Y ]	= y;
		m_Vertices[ nPlane ][ Z ]	= z;
	}

	void	SetDefault( void ){
		/*
		{-0.5f, -0.5f,  0.5f},	// 1
		{ 0.5f, -0.5f,  0.5f},	// 2
		{ 0.5f,  0.5f,  0.5f},	// 3
		{-0.5f,  0.5f,  0.5f},	// 4
		{ 0.5f, -0.5f, -0.5f},	// 5
		{-0.5f, -0.5f, -0.5f},	// 6
		{-0.5f,  0.5f, -0.5f},	// 7
		{ 0.5f,  0.5f, -0.5f},	// 8
		*/
		SetVertex( 0, 	-0.5, -0.5,  0.5	);	// 1
		SetVertex( 1, 	 0.5, -0.5,  0.5	);	// 2
		SetVertex( 2, 	 0.5,  0.5,  0.5	);	// 3
		SetVertex( 3, 	-0.5,  0.5,  0.5	);	// 4
		SetVertex( 4, 	 0.5, -0.5, -0.5	);	// 5
		SetVertex( 5,	-0.5, -0.5, -0.5	);	// 6
		SetVertex( 6,	-0.5,  0.5, -0.5	);	// 7
		SetVertex( 7,	 0.5,  0.5, -0.5	);	// 8
	}

public:
	CCubeModel( void ){
		SetDefault();
	}

	GLfloat*	operator[]( int idx ) const {
		if( InRange(idx) )
			return (GLfloat*)(m_Vertices[ idx ]);
		return NULL;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public IHandlerEx<CMainForm,IOpenGLForm> {
protected:
	CCubeModel			m_cube;
	int					m_nWidth;
	int					m_nHeight;

	void	DrawCubeFrame( const int nDrawMode = GL_LINE_LOOP ){
		static GLfloat	normals[][3] = {
			{  0,  0,  1 },
			{  0,  0, -1 },
			{  1,  0,  0 },
			{ -1,  0,  0 },
			{  0,  1,  0 },
			{  0, -1,  0 },
		};

		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( nDrawMode );
			glNormal3fv( normals[ 0 ] );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 2 ] );
			glVertex3fv( m_cube[ 3 ] );
		glEnd();

		glBegin( nDrawMode );
			glNormal3fv( normals[ 1 ] );
			glVertex3fv( m_cube[ 4 ] );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 6 ] );
			glVertex3fv( m_cube[ 7 ] );
		glEnd();

		glBegin( nDrawMode );
			glNormal3fv( normals[ 2 ] );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 4 ] );
			glVertex3fv( m_cube[ 7 ] );
			glVertex3fv( m_cube[ 2 ] );
		glEnd();

		glBegin( nDrawMode );
			glNormal3fv( normals[ 3 ] );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 3 ] );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( nDrawMode );
			glNormal3fv( normals[ 4 ] );
			glVertex3fv( m_cube[ 3 ] );
			glVertex3fv( m_cube[ 2 ] );
			glVertex3fv( m_cube[ 7 ] );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( nDrawMode );
			glNormal3fv( normals[ 5 ] );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 4 ] );
		glEnd();
	}

	void	Material( void ){
		static GLfloat	cubeDiffuse[]	= {0.75, 0, 1};
		static GLfloat	cubeAmbient[]	= {0.25, 0.25, 0.25};
		static GLfloat	cubeSpecular[]	= {1, 1, 1};
		static GLfloat	cubeShininess[]	= {32.0};

		glMaterialfv( GL_FRONT, GL_DIFFUSE, cubeDiffuse );
		glMaterialfv( GL_FRONT, GL_AMBIENT, cubeAmbient );
		glMaterialfv( GL_FRONT, GL_SPECULAR, cubeSpecular );
		glMaterialfv( GL_FRONT, GL_SHININESS, cubeShininess );
	}

	void	Light( void ){
		static GLfloat	lightPostion[]		= {0.75, 2, 0, 1};
		static GLfloat	lightDiffuse[]		= {1, 1, 1};
		static GLfloat	lightAmbient[]		= {0.25, 0.25, 0.25};
		static GLfloat	lightSpecular[]		= {1, 1, 1};
		static GLfloat	lightDirection[]	= {0, -1, 0};

		glLightfv( GL_LIGHT0, GL_POSITION, lightPostion );
		glLightfv( GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
		glLightfv( GL_LIGHT0, GL_AMBIENT, lightAmbient );
		glLightfv( GL_LIGHT0, GL_SPECULAR, lightSpecular );
		glLightfv( GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection );
		glLightf( GL_LIGHT0, GL_SPOT_CUTOFF, 30 );
	}

public:
#if 0
	void	Init( void ){
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glShadeModel( GL_FLAT );
	}

	void	Display( void ){
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


	//	glLoadIdentity();
	//	gluLookAt(	2.5, 2.5, 5.0
	//			,	0.0, 0.0, 0.0
	//			,	0.0, 1.0, 0.0	);
	//	glScaled( 1, 1, 1 );
	//	glColor3d( 1, 1, 1 );
	//	DrawCubeFrame();

		glViewport( m_nWidth/2, m_nHeight/2, m_nWidth, m_nHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 60, m_nWidth/m_nHeight, 1.5, 20 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	//	glColor3d( 1, 0, 1 );
		gluLookAt(	2.5, 2.5, 5.0
				,	0.0, 0.0, 0.0
				,	0.0, 1.0, 0.0	);
		glScaled( 2, 2, 2 );
		DrawCubeFrame();


		glViewport( 0, 0,  m_nWidth/2, m_nHeight/2 );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 60, m_nWidth/m_nHeight, 1.5, 20 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	2.5, 2.5, 5.0
				,	0.0, 0.0, 0.0
				,	0.0, 1.0, 0.0	);
	//	glColor3d( 1, 1, 1 );
		glScaled( 2, 2, 2 );
		DrawCubeFrame();
	}

	void	Resize( int nWidth, int nHeight ){
		m_nWidth	= nWidth;
		m_nHeight	= nHeight;
		
		glViewport( 0, 0, nWidth/2, nHeight/2 );
		glViewport( nWidth/2, nHeight/2, nWidth, nHeight );
		glMatrixMode( GL_PROJECTION );
		gluPerspective( 60, nWidth/nHeight, 1.5, 20 );
		glMatrixMode( GL_MODELVIEW );
	}
#endif
	void	DrawXYZ( void ){
		glBegin( GL_LINES );

		glColor3d( 0, 1, 0 );
		glVertex2d( -100, 0 );
		glVertex2d(  100, 0 );

		glColor3d( 1, 0, 0 );
		glVertex2d( 0, -100 );
		glVertex2d( 0,  100 );

		glColor3d( 0, 0, 1 );
		glVertex3d( 0, 0, -100 );
		glVertex3d( 0, 0,  100 );

		glEnd();
	}

	void	DrawTri( void ){
		glBegin( GL_TRIANGLES );
		glVertex2d(  0.0, 1.0 );
		glVertex2d( -0.5, 0.0 );
		glVertex2d(  0.5, 0.0 );
		glEnd();
	}

	void	DrawWindow1( int x, int y, int w, int h ){
		static int r = 0;

		glViewport( x, y, w, h );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 15, (double)w/(double)h, 1, 100 );
	//	gluPerspective( 15, (double)m_nWidth/(double)m_nHeight, 1, 100 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	3, 4, 5
				,	0, 0, 0
				,	0, 1, 0	);

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawXYZ();

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );

		Material();

		glPushMatrix();
		glRotated( r, 0, 1, 0 );
	//	glColor3d( 1, 1, 0 );
	//	DrawTri();
		DrawCubeFrame( GL_POLYGON );
		glPopMatrix();

	//	glPushMatrix();
	//	glRotated( r+90, 0, 1, 0 );
	//	glColor3d( 0, 1, 0 );
	//	DrawTri();
	//	glPopMatrix();

		glPushMatrix();
		glColor3d( 1, 0, 1 );
		glLineWidth( 1.5 );
		glBegin( GL_LINES );
		glVertex3d( 0.5, 0.0, 0.5 );
		glVertex3d( 0.5, 0.5, 0.5 );
		glEnd();
		glLineWidth( 1 );
		glPopMatrix();

		Light();

		glDisable( GL_LIGHT0 );
		glDisable( GL_LIGHTING );
		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );

		r += 1;
		if( r > 360 ){
			r	= 0;
		}
	}

	void	DrawWindow2( int x, int y, int w, int h ){
		static int r = 0;

		glViewport( x, y, w, h );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 30, (double)w/(double)h, 1, 100 );
	//	gluPerspective( 30, (double)m_nWidth/(double)m_nHeight, 1, 100 );
		
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( -3, 4, 5
				,	0, 0, 0
				,	0, 1, 0	);

		DrawXYZ();

		glEnable( GL_DEPTH_TEST );

		glPushMatrix();
		glRotated( r, 0, 1, 0 );
	//	glColor3d( 1, 1, 0 );
	//	DrawTri();
		DrawCubeFrame();
		glPopMatrix();

	//	glPushMatrix();
	//	glRotated( r+90, 0, 1, 0 );
	//	glColor3d( 0, 1, 0 );
	//	DrawTri();
	//	glPopMatrix();

		glPushMatrix();
		glColor3d( 1, 0, 1 );
		glLineWidth( 1.5 );
		glBegin( GL_LINES );
		glVertex3d( 0.5, 0.0, 0.5 );
		glVertex3d( 0.5, 0.5, 0.5 );
		glEnd();
		glLineWidth( 1 );
		glPopMatrix();

		glDisable( GL_DEPTH_TEST );

		r += 1;
		if( r > 360 ){
			r	= 0;
		}
	}

	void	Init( void ){
		glShadeModel( GL_SMOOTH );

		glClearColor( 0, 0, 0, 1 );
	}

	void	Display( void ){
#if 0
		DrawWindow1( 0, 0, m_nWidth, m_nHeight/2 );
		DrawWindow2( 0, m_nHeight/2, m_nWidth, m_nHeight/2 );
#else
		DrawWindow1( 0, 0, m_nWidth/2, m_nHeight );
		DrawWindow2( m_nWidth/2, 0, m_nWidth/2, m_nHeight );
#endif
	}

	void	Resize( int nWidth, int nHeight ){
		m_nWidth	= nWidth;
		m_nHeight	= nHeight;
	}
};
/**********************************************************************************
 *
 *
 *
 */
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CMainForm	Form;
	if( Form.Create(_T(APPLICATION_NAME), 0, 0, 100, 100, 400, 400) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}

