// OpenGLPlane.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "OpenGLPlane.h"

#include "UseToolkit.h"
#include "OpenGL.h"


class CMainForm : public IHandlerEx<CMainForm,IOpenGLForm> {
public:
	enum {
		ID_FILE				= 0		,
		ID_HELP						,

		ID_FILE_EXIT		= 100	,
		ID_HELP_ABOUT				,
	};
protected:
	void	InitMenu( void ){
		CPopupMenu	menuFile;
		menuFile.Create();
		menuFile.Insert(NULL,NULL);
		menuFile.Insert(_T("Exit")		, ID_FILE_EXIT);

		CPopupMenu	menuHelp;
		menuHelp.Create();
		menuHelp.Insert(_T("About")		, ID_HELP_ABOUT	);

		CMenu	menuMain;
		menuMain.Create();
		menuMain.Insert(_T("File")	, ID_FILE	, menuFile);
		menuMain.Insert(_T("Help")	, ID_HELP	, menuHelp);
	
		SetMenu( menuMain );
	}

	void	Lighting( void ){
		glEnable( GL_LIGHTING );							// 照光の有効化.
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );	// 両面照光モード. 

		// 環境光の設定.
		static GLfloat	ModelAmbient[]		= {0.4, 0.4, 0.4, 1.0};
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ModelAmbient );

		// 光源の環境光.
		static GLfloat	Light0_Ambient[]	= {0.5, 0.5, 0.5, 1.0};
		glLightfv( GL_LIGHT0, GL_AMBIENT, Light0_Ambient );
		// 光源の拡散光.
		static GLfloat	Light0_Diffuse[]	= {1.0, 1.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_DIFFUSE, Light0_Diffuse );
		// 光源の鏡面光.
		static GLfloat	Light0_Specular[]	= {1.0, 1.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_SPECULAR, Light0_Specular );

		// 光源の位置.
		static GLfloat	Light0_Position[]	= {0.0, 0.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_POSITION, Light0_Position );

		// GL_LIGHT0(ON).
		glEnable( GL_LIGHT0 );
	}

	void	DrawPlane( GLdouble w, GLdouble d, GLdouble h ){
		GLdouble	norm[]	= {1.0, 0.0, 0.0};

		glPushMatrix();
		glBegin( GL_POLYGON );
		{
			glColor3d( 1, 1, 0 );
			glNormal3dv( norm );
			glVertex3d(  w/2.0, h,  d/2.0 );
			glVertex3d(  w/2.0, h, -d/2.0 );
			glVertex3d( -w/2.0, h, -d/2.0 );
			glVertex3d( -w/2.0, h,  d/2.0 );
		}
		glEnd();
		glPopMatrix();
	}

	void	DrawCube( void ){
		static GLfloat	vertices[][3] = {
			{-0.5f, -0.5f,  0.5f},	// 1
			{ 0.5f, -0.5f,  0.5f},	// 2
			{ 0.5f,  0.5f,  0.5f},	// 3
			{-0.5f,  0.5f,  0.5f},	// 4
			{ 0.5f, -0.5f, -0.5f},	// 5
			{-0.5f, -0.5f, -0.5f},	// 6
			{-0.5f,  0.5f, -0.5f},	// 7
			{ 0.5f,  0.5f, -0.5f},	// 8
		};

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 0 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 1 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 2 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 3 ] );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 4 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 5 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 6 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 7 ] );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 1 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 4 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 7 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 2 ] );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 5 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 0 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 3 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 6 ] );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 3 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 2 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 7 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 6 ] );
		glEnd();

		glBegin( GL_POLYGON );
			glColor3f( 1.0f, 0.0f, 0.0f );
			glVertex3fv( vertices[ 1 ] );
			glColor3f( 0.0f, 1.0f, 0.0f );
			glVertex3fv( vertices[ 0 ] );
			glColor3f( 0.0f, 0.0f, 1.0f );
			glVertex3fv( vertices[ 5 ] );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glVertex3fv( vertices[ 4 ] );
		glEnd();
	}

public:
	void	Init( void ){
		TRACE(_T("Init.\n"));
		InitMenu();
	//	glEnable( GL_DEPTH_TEST );
	//	glEnable( GL_CULL_FACE );
	//
	//	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
	
	void	Display( void ){
		TRACE(_T("Display.\n"));
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawPlane(2.0, 2.0, 0.0);
		DrawCube();
	}

	void	Resize( int nWidth, int nHeight ){
		TRACE(_T("Reseize.\n"));
		glViewport( 0, 0, nWidth, nHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		GLdouble	dAspect	= (double)nWidth / (double)nHeight;
		GLdouble	dFovy	= 45.0;
		const double	PI	= 3.1459265;

		GLdouble	n		= 0.1;
		GLdouble	t		= n * tan((dFovy / 2.0) * PI/180.0);
		GLdouble	r		= t * dAspect;
		::glFrustum( -r, r, -t, t, 0.1, 100.0 );

	//	gluPerspective( 60, (GLfloat)nWidth/2/(GLfloat)nHeight, 0.5, 0.5 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
	//	gluLookAt( 0, 2.0, 5.0,   0, 0, 0,   0, 1, 0 );
		gluLookAt( 2.5, 2.5, 2.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	}
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	CMainForm	Form;
	if( Form.Create(_T(APPLICATION_NAME),0,0,100,100,600,600) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
	return 0;
}
