// OpenGLTexture.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "OpenGLTexture.h"

#include "UseToolkit.h"
#include "OpenGL.h"

#include "Texture.h"

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
	CCubeModel		m_cube;
	CTexture		m_texture;
	UINT			m_nTextureName;
	float			m_fAngle;

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

	void	DrawCubeFrame( const int nDrawMode = GL_LINE_LOOP ){
		//static GLfloat	normals[][3] = {
		//	{  0,  0,  1 },
		//	{  0,  0, -1 },
		//	{  1,  0,  0 },
		//	{ -1,  0,  0 },
		//	{  0,  1,  0 },
		//	{  0, -1,  0 },
		//};

	//	glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 0 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 0 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 1 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 2 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 3 ] );
		glEnd();

		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 1 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 4 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 5 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 6 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 7 ] );
		glEnd();

		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 2 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 1 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 4 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 7 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 2 ] );
		glEnd();

		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 3 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 5 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 0 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 3 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 4 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 3 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 2 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 7 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( nDrawMode );
	//		glNormal3fv( normals[ 5 ] );
			glTexCoord2f( 0, 0 );
			glVertex3fv( m_cube[ 1 ] );
			glTexCoord2f( 1, 0 );
			glVertex3fv( m_cube[ 0 ] );
			glTexCoord2f( 1, 1 );
			glVertex3fv( m_cube[ 5 ] );
			glTexCoord2f( 0, 1 );
			glVertex3fv( m_cube[ 4 ] );
		glEnd();
	}

	void	OnTimer( UINT uID ){
		m_fAngle	+= 0.8f;
		if( m_fAngle > 360.0f ){
			m_fAngle	= 0.0f;
		}
		Invalidate();
	}

	void	OnDestroy( void ){
		KillTimer( 123 );
	}

public:
	void		Init( void ){
		m_fAngle	= 0.0f;
		m_texture.LoadImage( _T("Happy.bmp") );

		glGenTextures( 1, &m_nTextureName );
		glBindTexture( GL_TEXTURE_2D, m_nTextureName );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_texture.Width(), m_texture.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_texture.RawImage() );

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glEnable( GL_LIGHTING );
		glEnable( GL_LIGHT0 );

		glShadeModel( GL_SMOOTH );

		glClearColor( 0, 0, 0, 1 );

		SetTimer( 123, 100 );
	}

	void		Display( void ){
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		Light();
		Material();

		glEnable( GL_TEXTURE_2D );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
		glBindTexture( GL_TEXTURE_2D, m_nTextureName );

		glPushMatrix();
		glRotatef( m_fAngle, 0, 1, 0 );
		DrawCubeFrame( GL_POLYGON );
		glPopMatrix();
	}

	void		Resize( int nWidth, int nHeight ){
		glViewport( 0, 0, nWidth, nHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective( 45, (double)nWidth/(double)nHeight, 0.1, 100 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	2.5, 2.5, 2.5
				,	0, 0, 0
				,	0, 1, 0	);
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
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
