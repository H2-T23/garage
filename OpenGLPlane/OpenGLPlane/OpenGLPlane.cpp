// OpenGLPlane.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "OpenGLPlane.h"

#include "UseToolkit.h"
#include "OpenGL.h"
#include "ThreeDMath.h"

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
public:
	enum {
		ID_FILE				= 0		,
		ID_HELP						,

		ID_FILE_EXIT		= 100	,
		ID_HELP_ABOUT				,
	};
protected:
	CCubeModel			m_cube;

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
		glEnable( GL_LIGHTING );
		glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

		static GLfloat	ModelAmbient[]		= {0.4, 0.4, 0.4, 1.0};
		glLightModelfv( GL_LIGHT_MODEL_AMBIENT, ModelAmbient );

		static GLfloat	Light0_Ambient[]	= {0.5, 0.5, 0.5, 1.0};
		glLightfv( GL_LIGHT0, GL_AMBIENT, Light0_Ambient );

		static GLfloat	Light0_Diffuse[]	= {1.0, 1.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_DIFFUSE, Light0_Diffuse );

		static GLfloat	Light0_Specular[]	= {1.0, 1.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_SPECULAR, Light0_Specular );

		static GLfloat	Light0_Position[]	= {0.0, 0.0, 1.0, 1.0};
		glLightfv( GL_LIGHT0, GL_POSITION, Light0_Position );

		// GL_LIGHT0(ON).
		glEnable( GL_LIGHT0 );
	}

	void	DrawPlane( GLdouble w, GLdouble d, GLdouble h ){
		GLdouble	norm[]	= {1.0, 0.0, 0.0};

		glPushMatrix();
		glBegin( GL_LINE_LOOP );
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

	void	DrawCubeFrame( void ){

		glColor3f( 1.0f, 1.0f, 1.0f );
		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 2 ] );
			glVertex3fv( m_cube[ 3 ] );
		glEnd();

		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 4 ] );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 6 ] );
			glVertex3fv( m_cube[ 7 ] );
		glEnd();

		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 4 ] );
			glVertex3fv( m_cube[ 7 ] );
			glVertex3fv( m_cube[ 2 ] );
		glEnd();

		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 3 ] );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 3 ] );
			glVertex3fv( m_cube[ 2 ] );
			glVertex3fv( m_cube[ 7 ] );
			glVertex3fv( m_cube[ 6 ] );
		glEnd();

		glBegin( GL_LINE_LOOP );
			glVertex3fv( m_cube[ 1 ] );
			glVertex3fv( m_cube[ 0 ] );
			glVertex3fv( m_cube[ 5 ] );
			glVertex3fv( m_cube[ 4 ] );
		glEnd();
	}

public:
	void	Init( void ){
		TRACE(_T("Init.\n"));
		InitMenu();
	}
	
	void	Display( void ){
		TRACE(_T("Display.\n"));
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawPlane(2.0, 2.0, 0.0);
	//	DrawCube();
		DrawCubeFrame();
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
/**********************************************************************************
 *
 *
 *
 */
class Pointer {
public:
	typedef void*	LPVOID;
	typedef enum {	TYPE_UNDEF=0,TYPE_INT=1,TYPE_LONG,TYPE_SHORT,TYPE_FLOAT,TYPE_BYTE,TYPE_DOUBLE,TYPE_BOOLEAN,TYPE_ASTRING,TYPE_WSTRING,TYPE_WORD,TYPE_DWORD }	TYPE;
public:
	TYPE		Type;
	LPVOID		Ptr;	
public:
	Pointer(void)				: Type(TYPE_UNDEF)		, Ptr(NULL) {}
	Pointer(int* p)				: Type(TYPE_INT)		, Ptr(p) 	{}
	Pointer(int& r)				: Type(TYPE_INT)		, Ptr(&r)	{}
	Pointer(long* p)			: Type(TYPE_LONG)		, Ptr(p) 	{}
	Pointer(long& r)			: Type(TYPE_LONG)		, Ptr(&r)	{}
	Pointer(short* p)			: Type(TYPE_SHORT)		, Ptr(p) 	{}
	Pointer(short& r)			: Type(TYPE_SHORT)		, Ptr(&r)	{}
	Pointer(float* p)			: Type(TYPE_FLOAT)		, Ptr(p)	{}
	Pointer(float& r)			: Type(TYPE_FLOAT)		, Ptr(&r)	{}
	Pointer(BYTE* p)			: Type(TYPE_BYTE)		, Ptr(p)	{}
	Pointer(BYTE& r)			: Type(TYPE_BYTE)		, Ptr(&r)	{}
	Pointer(double* p)			: Type(TYPE_DOUBLE)		, Ptr(p)	{}
	Pointer(double& r)			: Type(TYPE_DOUBLE)		, Ptr(&r)	{}
	Pointer(bool* p)			: Type(TYPE_BOOLEAN)	, Ptr(p)	{}
	Pointer(bool& r)			: Type(TYPE_BOOLEAN)	, Ptr(&r)	{}
	Pointer(CHAR* p)			: Type(TYPE_ASTRING)	, Ptr(p)	{}
	Pointer(CHAR& r)			: Type(TYPE_ASTRING)	, Ptr(&r)	{}
	Pointer(WCHAR* p)			: Type(TYPE_WSTRING)	, Ptr(p)	{}
	Pointer(WCHAR& r)			: Type(TYPE_WSTRING)	, Ptr(&r)	{}
	Pointer(WORD* p)			: Type(TYPE_WORD)		, Ptr(p)	{}
	Pointer(WORD& r)			: Type(TYPE_WORD)		, Ptr(&r)	{}
	Pointer(DWORD* p)			: Type(TYPE_DWORD)		, Ptr(p)	{}
	Pointer(DWORD& r)			: Type(TYPE_DWORD)		, Ptr(&r)	{}
public:
	Pointer&	operator=(const Pointer& obj){ return Set(obj); }
	Pointer&	operator=(int* p)	{ return Set(TYPE_INT, p);		}
	Pointer&	operator=(int& r)	{ return Set(TYPE_INT, &r);		}
	Pointer&	operator=(long* p)	{ return Set(TYPE_LONG, p);		}
	Pointer&	operator=(long& r)	{ return Set(TYPE_LONG, &r);	}
	Pointer&	operator=(short* p)	{ return Set(TYPE_SHORT, p);	}
	Pointer&	operator=(short& r)	{ return Set(TYPE_SHORT, &r);	}
	Pointer&	operator=(float* p)	{ return Set(TYPE_FLOAT, p);	}
	Pointer&	operator=(float& r)	{ return Set(TYPE_FLOAT, &r);	}
	Pointer&	operator=(BYTE* p)	{ return Set(TYPE_BYTE, p);		}
	Pointer&	operator=(BYTE& r)	{ return Set(TYPE_BYTE, &r);	}
	Pointer&	operator=(double* p){ return Set(TYPE_DOUBLE, p);	}
	Pointer&	operator=(double& r){ return Set(TYPE_DOUBLE, &r);	}
	Pointer&	operator=(bool* p)	{ return Set(TYPE_BOOLEAN, p);	}
	Pointer&	operator=(bool& r)	{ return Set(TYPE_BOOLEAN, &r);	}
	Pointer&	operator=(CHAR* p)	{ return Set(TYPE_ASTRING, p);	}
	Pointer&	operator=(CHAR& r)	{ return Set(TYPE_ASTRING, &r);	}
	Pointer&	operator=(WCHAR* p)	{ return Set(TYPE_WSTRING, p);	}
	Pointer&	operator=(WCHAR& r)	{ return Set(TYPE_WSTRING, &r);	}

	Pointer&	operator=(WORD* p)	{ return Set(TYPE_WORD, p);	}
	Pointer&	operator=(WORD& r)	{ return Set(TYPE_WORD, &r);	}

	Pointer&	operator=(DWORD* p)	{ return Set(TYPE_DWORD, p);	}
	Pointer&	operator=(DWORD& r)	{ return Set(TYPE_DWORD, &r);	}

	int			operator=(int v)	{ return(*(int*)Ptr = v);	}
	short		operator=(short v)	{ return(*(short*)Ptr = v);	}
	long		operator=(long v)	{ return(*(long*)Ptr = v);	}
	float		operator=(float v)	{ return(*(float*)Ptr = v);	}
public:
	template<typename TYPE>
	TYPE		Get( void ) const {
		switch(Type){
		case TYPE_INT:		return(int)(*Ptr);
		case TYPE_SHORT:	return(short)(*Ptr);
		case TYPE_LONG:		return(long)(*Ptr);
		case TYPE_FLOAT:	return(float)(*Ptr);
		case TYPE_DOUBLE:	return(double)(*Ptr);
		case TYPE_BOOLEAN:	return(bool)(*Ptr);
		}
	}
protected:
	inline Pointer&		Set( TYPE type, LPVOID ptr ){
		Type	= type;
		Ptr		= ptr;
		return(*this);
	}
	inline Pointer&		Set( const Pointer& obj ){
		Type	= obj.Type;
		Ptr		= obj.Ptr;
		return(*this);
	}
public:
	static void	Test( void )
	{
		Pointer	ptr[10];

		int		a	= 123;
		float	b	= 0.5f;
		double	c	= 1.24;
		bool	d	= true;

		Pointer	A(a);
		Pointer	B(b);
		Pointer	C(c);
		Pointer	D(d);

		A = 456;

	}
};
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Pointer::Test();

	CMainForm	Form;
//	Ver2::CGLForm	Form;
	if( Form.Create(_T(APPLICATION_NAME), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
	return 0;
}
