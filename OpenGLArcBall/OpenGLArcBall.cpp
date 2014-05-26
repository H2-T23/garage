// OpenGLArcBall.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "OpenGLArcBall.h"

#include "UseToolkit.h"
#include "OpenGL.h"
#include "ThreeDMath.h"

using namespace ThreeDMath;

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
class CStarsModel {
public:
	enum {
		X = 0, Y, Z,
		NUM_STARS		= 256,
	};
protected:
	GLfloat		m_vertex[ NUM_STARS ][3];

	inline bool		InRange( int idx ) const {
		return(0<=idx&&idx<NUM_STARS);
	}

	inline void		SetVertex( int idx, float x, float y, float z ){
		m_vertex[ idx ][ X ]	= x;
		m_vertex[ idx ][ Y ]	= y;
		m_vertex[ idx ][ Z ]	= z;
	}

	inline double	Rand( void ) const {
		return((1.0 / 127.0) * (((float)(rand() & 255)) - 128.0));
	}
	
	void	SetDefault( void ){
		CVector3	vec;
		for( int i=0; i<NUM_STARS; i++ ){
			vec.Set( Rand(), Rand(), Rand() );
			vec.Normalize();
			vec.Cross( 2.0 );
			SetVertex( i, vec.x, vec.y, vec.z );
		}
	}
public:
	CStarsModel( void ){
		SetDefault();
	}

	GLfloat*	operator[]( int idx ) const {
		if( InRange(idx) )
			return (GLfloat*)(m_vertex[ idx ]);
		return NULL;
	}
};
/**********************************************************************************
 *
 *
 *
 */
namespace TYPE1 {
	class CArcBall {
	protected:
		bool				m_isDrag;
		CRect				m_rcView;
		CPoint				m_ptCenter;
		CVector3			m_vCenter;
		double				m_dRadius;
		double				m_dCoefX, m_dCoefY;

		CVector3			m_vFrom, m_vTo;
		CQuaternion			m_qFrom, m_qTo;

	protected:
		CVector3		MouseOnSphere( int x, int y ){
			CVector3	vec;
		//	vec.x	= (x - m_ptCenter.x) / m_dRadius;
		//	vec.y	= (y - m_ptCenter.y) / m_dRadius;

			vec.x	=		(x * m_dCoefX) - 1.0;
			vec.y	= 1.0 - (y * m_dCoefY);

			double	dMag	= (vec.x * vec.x) + (vec.y * vec.y);
			if( dMag > 1.0 )
			{
				vec.x	= vec.x * (1.0 / sqrt(dMag));
				vec.y	= vec.y * (1.0 / sqrt(dMag));
				vec.z	= 0.0;
			}
			else
			{
				vec.z	= sqrt( 1.0 - dMag );
			}

			return vec;
		}

	public:
		CArcBall( void ) : m_isDrag(false), m_dRadius(0.9) {
			m_vCenter.Set(0,0,0);
		}

	public:
		bool	IsDrag( void ) const { return(m_isDrag); }

		void	SetBounds( int nWidth, int nHeight ){
			m_rcView.Set( 0, 0, nWidth, nHeight );
			m_ptCenter	= m_rcView.CenterPoint();

			m_dCoefX	= 1.0 / (nWidth  - 1.0) * m_dRadius;
			m_dCoefY	= 1.0 / (nHeight - 1.0) * m_dRadius;
		}

		void	BeginDrag( int x, int y ){
			if( !m_isDrag ){
				m_isDrag	= true;

				m_vFrom	= MouseOnSphere(x, y);
				m_vTo.Identity();
			}
		}

		void	EndDrag( void ){
			m_isDrag	= false;
		}

		void	Dragging( int x, int y ){
			if( !m_isDrag )
				return;

			const double	EPSILON			= 1.0e-5;

			m_vTo	= MouseOnSphere(x, y);

			CVector3	vec = CVector3::Cross(m_vFrom, m_vTo);
			CQuaternion	q;

			if( vec.Length() > EPSILON )
			{
				q.S		= CVector3::Dot( m_vFrom, m_vTo );
				q.V.x	= vec.x;
				q.V.y	= vec.y;
				q.V.z	= vec.z;
			}
			else
			{
				q.Zero();
			}

			m_vFrom	= m_vTo;
			m_vTo.Identity();
		}
	};
}
/**********************************************************************************
 *
 *
 *
 */
namespace TYPE2 {
	class CArcBall {
	protected:
		bool			m_isDrag;
		CPoint			m_ptFrom, m_ptTo;
		CQuaternion		m_qtFrom, m_qtTo;
		CMatrix4x4		m_mtxRot;
		CRect			m_rect;

	public:
		inline bool	IsDrag( void ) const { return(m_isDrag); }

		void	SetBounds( int nWidth, int nHeight ){
			m_rect.Set(0, 0, nWidth, nHeight);
		}

		CArcBall( void ) : m_isDrag(false) {
		}

		void		BeginDrag( int x, int y ){
			if( m_isDrag == false ){
				m_isDrag	= true;
				m_ptFrom.Set( x, y );
			}
		}

		void		EndDrag( void ){
			if( m_isDrag == true ){
				m_isDrag	= false;
				m_qtFrom	= m_qtTo;

				m_mtxRot.Identity();
			}
		}

		void		Dragging( int x, int y ){
			if( m_isDrag )
			{
				const double	EPSILON			= 1.0e-5;

				m_ptTo.Set( x, y );
				
				double	dx	= (m_ptTo.x - m_ptFrom.x) * 1.33 / m_rect.Width();
				double	dy	= (m_ptTo.y - m_ptFrom.y) * 1.00 / m_rect.Height();

				double	len = sqrt( SQUARE(dx) + SQUARE(dy) );
				if( len > EPSILON )
				{
					double	r	= len * PI * 2;
					double	t	= sin(r) / len;

					CQuaternion	q;
					q.Set( cos(r), dx * t, dy * t, 0.0 );

					m_qtTo	= q * m_qtFrom;

					m_qtTo.GetRotation( m_mtxRot );

					m_ptFrom	= m_ptTo;
				}
			}
		}

		GLdouble*	GetRotation( void ){
			return (double*)m_mtxRot;
		}
	};
}
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
	CStarsModel			m_stars;

	CVector3			m_vEye;
	CVector3			m_vCenter;
	CVector3			m_vUp;

	TYPE2::CArcBall		m_ArcBall;

	CMatrix4x4			m_mtxRot;

	bool				m_isDrag;
	CPoint				m_ptFrom, m_ptTo;
	CQuaternion			m_qFrom, m_qTo;

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

	void	DrawStars( void ){
		glBegin( GL_POINTS );
		glColor3f( 1.0f, 1.0f, 1.0f );
		for( int i=0; i<m_stars.NUM_STARS; i++ ){
			glVertex3fv( m_stars[ i ] );
		}
		glEnd();
	}

public:
	void	Init( void ){
		TRACE(_T("Init.\n"));
		InitMenu();

		m_vEye.Set( 0, 0, -20.0 );
		m_vCenter.Set( 0, 0, 0 );
		m_vUp.Set( 0, 1, 0 );

		m_isDrag	= false;
		m_mtxRot.Identity();
	}
	
	void	Display( void ){
		TRACE(_T("Display.\n"));
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawPlane(2.0, 2.0, 0.0);
		DrawStars();

	//	::glPushMatrix();
	//	::glMultMatrixd( (double*)m_mtxRot );
		::glMultMatrixd( m_ArcBall.GetRotation() );

		DrawCubeFrame();

	//	::glPopMatrix();
	}

	void	Resize( int nWidth, int nHeight ){
		TRACE(_T("Reseize.\n"));

		m_ArcBall.SetBounds( nWidth, nHeight );

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
#if 0
	void	OnRButtonDown( int x, int y, UINT nKeyFlags ){
		if( m_isDrag == false ){
			TRACE(_T("Begin Drag"));
			m_isDrag	= true;
			m_ptFrom.Set( x, y );
		}
	}

	void	OnRButtonUp( int x, int y, UINT nKeyFlags ){
		if( m_isDrag ){
			TRACE(_T("End Drag"));
			m_isDrag	= false;
			m_qFrom		= m_qTo;

			m_mtxRot.Identity();
		}
	}

	void	OnMouseMove( int x, int y, UINT nKeyFlags ){
		if( m_isDrag ){
			TRACE(_T("Dragging"));
			m_ptTo.Set( x, y );
			
			double	dx	= (m_ptTo.x - m_ptFrom.x) * 1.33 / 600;
			double	dy	= (m_ptTo.y - m_ptFrom.y) * 1.00 / 600;

			double	len = sqrt( SQUARE(dx) + SQUARE(dy) );
			if( len > 0.0 )
			{
				double	r	= len * PI * 2;
				double	t	= sin(r) / len;

				CQuaternion	q;
				q.Set( cos(r), dx * t, dy * t, 0.0 );

				m_qTo	= q * m_qFrom;

				m_qTo.GetRotation( m_mtxRot );

				m_ptFrom	= m_ptTo;

				Invalidate(FALSE);
			}
		}
	}
#else
	void	OnLButtonDown( int x, int y, UINT nKeyFlags ){
		m_ArcBall.BeginDrag( x, y );
	}

	void	OnLButtonUp( int x, int y, UINT nKeyFlags ){
		m_ArcBall.EndDrag();
	}

	void	OnMouseMove( int x, int y, UINT nKeyFlags ){
		if( m_ArcBall.IsDrag() ){
			m_ArcBall.Dragging( x, y );

			Invalidate( FALSE );
		}
	}
#endif
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

	int	A[16] = {
		 1, 2, 3, 4,
		 5, 6, 7, 8,
		 9,10,11,12,
		13,14,15,16
	};

	int	B[4][4]	= {
		{ 1, 2, 3, 4},
		{ 5, 6, 7, 8},
		{ 9,10,11,12},
		{13,14,15,16},
	};

	int*	C = (int*)B;

	if( memcmp(A,C,sizeof(A)) == 0 ){
		TRACE(_T("TRUE"));
	}else{
		TRACE(_T("FALSE"));
	}

	CMainForm	Form;
	if( Form.Create(_T(APPLICATION_NAME), 0, 0, 100, 100, 600, 600) ){
		Form.ShowWindow( SW_SHOW );
		Form.UpdateWindow();
		Form.MessageLoop();
	}
	return 0;
}
