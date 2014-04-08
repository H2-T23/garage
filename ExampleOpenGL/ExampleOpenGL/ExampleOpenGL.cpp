// ExampleOpenGL.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleOpenGL.h"

#include "UseToolkit.h"
#include "OpenGLPanel.h"

#include <math.h>

#include "UseToolkit.h"
//#include "UseOpenGL.h"
#include "OpenGL.h"

#include "ThreeDMath.h"

using namespace ThreeDMath;

class CWireSphere {
public:
	CVector3		m_vCenter;
	double			m_dRadius;
	CVector3		m_vColor;

	CWireSphere( void ) {
	}

	CWireSphere( CVector3& vCenter, double dRad, CVector3& vColor )
	 : m_vCenter(vCenter), m_dRadius(dRad), m_vColor(vColor) {
	}

	void		Set( CVector3& vCenter, double dRad, CVector3& vColor ){
		m_vCenter	= vCenter;
		m_dRadius	= dRad; 
		m_vColor	= vColor;
	}

	void		WireCircle( const double dRad, const int n ){
		double	dt	= 2.0 * ThreeDMath::PI / double(n);
		double	t	= 0.0;

		glBegin( GL_LINE_LOOP );
		do{
			glVertex3d( dRad * cos(t), 0.0, dRad * sin(t) );
			t += dt;
		}while( t < 2.0 * ThreeDMath::PI );
		glEnd();
	}

	void		WireCircleXZ( const double dRad, const int n ){
		double	dt	= 2.0 * ThreeDMath::PI / double(n); 
		double	t	= 0.0;

		glBegin( GL_LINE_LOOP );
		do{
			glVertex3d( dRad * cos(t), 0.0, dRad * sin(t) );
			t += dt;
		}while( t < 2.0 * ThreeDMath::PI );
		glEnd();
	}

	void		Draw( void ){

		glPushMatrix();
		glTranslated( m_vCenter.x, m_vCenter.y, m_vCenter.z );
		glRotated( 90, 1.0, 0.0, 0.0 );

		double	dz	= 2.0 * m_dRadius / 8.0;
		double	z	= -(m_dRadius - dz);
		do{
			glPushMatrix();
			glTranslated( 0, 0, z );
			WireCircle( sqrt(m_dRadius*m_dRadius - z*z), 32 );
			glPopMatrix();
			z += dz;
		}while( z < m_dRadius );

		double	dt	= 180.0 / 8.0;
		double	t	= 0.0;
		do{
			glPushMatrix();
			glRotated( t, 0.0, 0.0, 1.0 );
			WireCircleXZ( m_dRadius, 32 );
			glPopMatrix();
			t += dt;
		}while( t < 180.0 );

		glPopMatrix();
	}
};


static const GLfloat	white[]	= { ONE, ONE, ONE, ONE };
static const GLfloat	blue[]	= { ZERO, ZERO, ONE, ONE };
static const GLfloat	black[]	= { ZERO, ZERO, ZERO, ONE };


union Mat {
	float		m4x4[ 4 ][ 4 ];
	float		m16[ (4 * 4) ];
};

double	Transform[16]	= {	 ONE, ZERO, ZERO, ZERO,
							ZERO,  ONE, ZERO, ZERO,
							ZERO, ZERO,  ONE, ZERO,
							ZERO, ZERO, ZERO,  ONE	};

//double	LastRot[16]	= {	 ONE, ZERO, ZERO, ZERO,
//							ZERO,  ONE, ZERO, ZERO,
//							ZERO, ZERO,  ONE, ZERO,
//							ZERO, ZERO, ZERO,  ONE, };

//double	ThisRot[16]	= {	 ONE, ZERO, ZERO, ZERO,
//							ZERO,  ONE, ZERO, ZERO,
//							ZERO, ZERO,  ONE, ZERO,
//							ZERO, ZERO, ZERO,  ONE, };

CMatrix3x3		LastRot, ThisRot;

class CArcball {
protected:
	bool				m_bDrag;
	CVector3			m_vecBegin;
	CVector3			m_vecEnd;
	CQuaternion			m_qtRot;
	double				m_dWidth;
	double				m_dHeight;

	void	mapToSphere( double x, double y, CVector3& vec ){
		CVector3	tmp(0 ,0, 0);
		tmp.x	= (			(x * m_dWidth));
		tmp.y	= (ONE -	(y * m_dHeight));

		double	dLen	= ThreeDMath::SQUARE(tmp.x) + ThreeDMath::SQUARE(tmp.y);
		if( dLen > ONE )
		{
			double	dNorm	= ONE / sqrt(dLen);
			
			vec.x	= tmp.x * dNorm;
			vec.y	= tmp.y * dNorm;
			vec.z	= ZERO;
		}
		else
		{
			vec.x	= tmp.x;
			vec.y	= tmp.y;
			vec.z	= sqrt( ONE - dLen );
		}
	}

public:
	CArcball( void )
		: m_bDrag(false), m_dWidth(ZERO), m_dHeight(ZERO) {
	}

	CArcball( double w, double h )
		: m_bDrag(false) {
		SetBounds( w, h );
	}

	void	SetBounds( double w, double h ){
		m_dWidth	= (ONE / ((w - ONE) * 0.5));
		m_dHeight	= (ONE / ((h - ONE) * 0.5));
	}

	void	Begin( double x, double y ){
		if( !m_bDrag ){
			m_bDrag	= true;

			mapToSphere( x, y , m_vecBegin );
		}
	}

	void	End( void ){
		if( m_bDrag ){
			m_bDrag	= false;
		}
	}

	void	Drag( double x, double y ){
		CArcball::Drag(x, y, m_qtRot);
	}

	void	Drag( double x, double y, CQuaternion& qtRot ){
		if( m_bDrag )
		{
			mapToSphere( x, y, m_vecEnd );

			CVector3	vec	= m_vecBegin * m_vecEnd;
			if( vec.Length() > (1.0e-5) )
			{
				qtRot.V	= vec;
				qtRot.S	= CVector3::Dot( m_vecBegin, m_vecEnd );
			}
			else
			{
				qtRot.Zero();
			}
		}
	}
};

void		MakeRot( const CQuaternion& qt )
{
//	Matrix3fSetRotationFromQuat4f( &ThisRot, &ThisQuat );
//	Matrix3fMulMatrix3f( &ThisRot, &LastRot );
//	Matrix4fSetRotationFromMatrix3f( Transform, ThisRot );
	ThisRot.Convert( qt );
	CMatrix3x3	ans = CMatrix3x3::Cross( ThisRot, LastRot );
}

/**********************************************************************************
 *
 *
 *
 */
struct Vertex {
	double	x, y, z;

	void	Set( double x, double y, double z ){
		this->x = x;
		this->y = y;
		this->z = z;
	}

	static Vertex*	CreateVertex( double x, double y, double z ){
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CEdge {
public:
	Vertex*		from;
	Vertex*		to;

	void	Set( Vertex* from, Vertex* to ){
		this->from	= from;
		this->to	= to;
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CPolygon4 {
public:
	CEdge		edge[ 4 ];

	void	Edge( Vertex& a, Vertex& b, Vertex& c, Vertex& d ){
		edge[ 0 ].Set( &a, &b );
		edge[ 1 ].Set( &b, &c );
		edge[ 2 ].Set( &c, &d );
		edge[ 3 ].Set( &d, &a );
	}
};
/**********************************************************************************
 *
 *
 *
 */
class Model : public CSubject {
public:
	static Model&	Instance( void ){
		static Model	model;
		return(model);
	}

private:
	Model(const Model&);
	Model& operator=(const Model&);

private:
	Model(){
		Constractor();
	}

	void	Constractor( void ){
		m_Camera.Set( 0.5, 1.5, 2.5 );
		m_LookAt.Set( 0, 0, 0 );

		vecVertex.resize( 8 );
		vecVertex[ 0 ].Set( -0.5, -0.5,  0.5 );
		vecVertex[ 1 ].Set( -0.5,  0.5,  0.5 ); 
		vecVertex[ 2 ].Set(  0.5,  0.5,  0.5 ); 
		vecVertex[ 3 ].Set(  0.5, -0.5,  0.5 ); 
		vecVertex[ 4 ].Set( -0.5, -0.5, -0.5 ); 
		vecVertex[ 5 ].Set( -0.5,  0.5, -0.5 ); 
		vecVertex[ 6 ].Set(  0.5,  0.5, -0.5 ); 
		vecVertex[ 7 ].Set(  0.5, -0.5, -0.5 ); 

		vecPolygon.resize( 6 );
		vecPolygon[ 0 ].Edge( vecVertex[ 0 ], vecVertex[ 3 ], vecVertex[ 2 ], vecVertex[ 1 ] );
		vecPolygon[ 1 ].Edge( vecVertex[ 2 ], vecVertex[ 3 ], vecVertex[ 7 ], vecVertex[ 6 ] );
		vecPolygon[ 2 ].Edge( vecVertex[ 3 ], vecVertex[ 0 ], vecVertex[ 4 ], vecVertex[ 7 ] );
		vecPolygon[ 3 ].Edge( vecVertex[ 1 ], vecVertex[ 2 ], vecVertex[ 6 ], vecVertex[ 5 ] );
		vecPolygon[ 4 ].Edge( vecVertex[ 4 ], vecVertex[ 5 ], vecVertex[ 6 ], vecVertex[ 7 ] );
		vecPolygon[ 5 ].Edge( vecVertex[ 5 ], vecVertex[ 4 ], vecVertex[ 0 ], vecVertex[ 1 ] );
	}

public:
	bool					m_bDrawAxis;
	Vertex					m_Camera;
	Vertex					m_LookAt;
	std::vector<Vertex>		vecVertex;
	std::vector<CPolygon4>	vecPolygon;

	void	SetCamera( double x, double y, double z ){
		m_Camera.Set( x, y, z );
	}

	void	SetLookAt( double x, double y, double z ){
		m_LookAt.Set( x, y, z );
	}
};

/**********************************************************************************
 *
 *
 *
 */
class COpenGLPanel : public IOpenGLPanel, public IObservable {
protected:
	Vertex			m_Camera;

	bool			bTrack;
	CQuaternion		qtCurr, qtLast;
	CPoint			ptBegin;

	CRect			m_rect;

	CArcball		m_Arcball;

	/**********************************************************************************
	 */
	void	DrawAxis( void ){
		glColor3d( 1, 1, 1 );
		glLineWidth( 5 );
		glBegin( GL_LINES );
		glVertex3d(-1,  0,  0);
		glVertex3d(+1,  0,  0);
		glVertex3d( 0, -1,  0);
		glVertex3d( 0, +1,  0);
		glVertex3d( 0,  0, -1);
		glVertex3d( 0,  0, +1);
		glEnd();
	}
	/**********************************************************************************
	 */
	void	DrawCube( void ){
		glColor3d( 1, 1, 1 );
		glLineWidth( 1 );
		glBegin( GL_LINES );
		for( int i=0; i<6; i++ ){
			for( int j=0; j<4; j++ )
			{
				glVertex3d( Model::Instance().vecPolygon[ i ].edge [ j ].from->x
						,	Model::Instance().vecPolygon[ i ].edge [ j ].from->y
						,	Model::Instance().vecPolygon[ i ].edge [ j ].from->z	
						);

				glVertex3d( Model::Instance().vecPolygon[ i ].edge [ j ].to->x
						,	Model::Instance().vecPolygon[ i ].edge [ j ].to->y
						,	Model::Instance().vecPolygon[ i ].edge [ j ].to->z	
						);
			}
		}
		glEnd();
	}
	/**********************************************************************************
	 */
	void	DrawFrame( const float L = 0.5 ){
		static GLfloat	vData[][3] = {
			{ -L, -L, -L }, { L, -L, -L }, { L, L, -L }, { -L, L, -L },
			{ -L, -L,  L }, { L, -L,  L }, { L, L,  L }, { -L, L,  L }
		};
		static GLint tindices[][4] = {
			{ 0, 1, 2, 3 }, { 0, 1, 5, 4 }, { 3, 0, 4, 7 },
			{ 1, 2, 6, 5 }, { 2, 3, 7, 6 }, { 5, 4, 7, 6 }
		};
		
		glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, blue );
		glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, black );
		for( int i=0; i<3; ++i ){
			glBegin( GL_QUADS );
				glNormal3fv( &vData[ (tindices[i][0]) ][0] );
				glVertex3fv( &vData[ (tindices[i][0]) ][0] );

				glNormal3fv( &vData[ (tindices[i][1]) ][0] );
				glVertex3fv( &vData[ (tindices[i][1]) ][0] );
	
				glNormal3fv( &vData[ (tindices[i][2]) ][0] );
				glVertex3fv( &vData[ (tindices[i][2]) ][0] );

				glNormal3fv( &vData[ (tindices[i][3]) ][0] );
				glVertex3fv( &vData[ (tindices[i][3]) ][0] );
			glEnd();
		}
	}
	/**********************************************************************************
	 */
	void	DrawShape( const float X = (0.525731112119133606 / 2.0), const float Z = (0.850650808352039932 / 2.0) ){
		static GLfloat		vData[][ 3 ] = {
			{ -X, ZERO,  Z }, { X, ZERO, Z }, { -X, ZERO, -Z },
			{  X, ZERO, -Z }, { ZERO, Z, X }, { ZERO,  Z, -X },
			{ ZERO, -Z, X }, { ZERO, -Z, -X }, { Z, X, ZERO },
			{ -Z, X, ZERO }, { Z, -X, ZERO }, { -Z, -X, ZERO }
		};

		static GLint		tindices[][ 3 ] = {
			{ 0, 4, 1 }, { 0, 9, 4 }, { 9, 5, 4 }, { 4, 5, 8 },
			{ 4, 8, 1 }, { 8, 10, 1 }, { 8, 3, 10 }, { 5, 3, 8 },
			{ 5, 2, 3 }, { 2, 7, 3 }, { 7, 10, 3 }, { 7, 6, 10 },
			{ 7, 11, 6 }, { 11, 0, 6 }, { 0, 1, 6 }, { 6, 1, 10 }, 
			{ 9, 0, 11 }, { 9, 11, 2 }, { 9, 2, 5 }, { 7, 2, 11 } 
		};

		::glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white );
		for( int i=0; i<20; i++ )
		{
			glBegin( GL_TRIANGLES );
			{
				glNormal3fv( &vData[ (tindices[ i ][ 0 ]) ][ 0 ] );
				glVertex3fv( &vData[ (tindices[ i ][ 0 ]) ][ 0 ] );

				glNormal3fv( &vData[ (tindices[ i ][ 1 ]) ][ 0 ] );
				glVertex3fv( &vData[ (tindices[ i ][ 1 ]) ][ 0 ] );

				glNormal3fv( &vData[ (tindices[ i ][ 2 ]) ][ 0 ] );
				glVertex3fv( &vData[ (tindices[ i ][ 2 ]) ][ 0 ] );
			}
			glEnd();
		}
	}

public:
	/**********************************************************************************
	 */
	void	Init( void ){
		m_Camera	= Model::Instance().m_Camera;

		static GLfloat	light1[] = { ZERO, ZERO, 10.0, ONE };

		glClearColor( ZERO, ZERO, ZERO, ONE );
		glEnable( GL_DEPTH_TEST	);
		glEnable( GL_NORMALIZE	);

		glEnable( GL_LIGHTING	);
		glLightfv( GL_LIGHT0, GL_DIFFUSE, black );

		glEnable( GL_LIGHT0 );
		glLightfv( GL_LIGHT1, GL_DIFFUSE, white );
		glLightfv( GL_LIGHT1, GL_POSITION, light1 );

		glEnable( GL_LIGHT1 );

		bTrack	= false;
	
		Trackball( qtCurr, ZERO, ZERO, ZERO, ZERO );
	}
	/**********************************************************************************
	 */
	void	Trackball( CQuaternion& qt, double x1, double y1, double x2, double y2 ){
		if( (x1 == x2) && (y1 == y2) )
		{
			qt.Identity();
		}
		else
		{
			const double	R	= 0.8;

			double	distance1	= sqrt( ThreeDMath::SQUARE(x1) + ThreeDMath::SQUARE(y1) );
			double	z1	= ThreeDMath::SQUARE(R) / distance1;
			if( R > distance1 ){
				z1	= sqrt( 2.0 * ThreeDMath::SQUARE(R) - (ThreeDMath::SQUARE(x1) + ThreeDMath::SQUARE(y1)) );
			}
			const CQuaternion	q1(0.0, x1, y1, z1);


			double	distance2	= sqrt( ThreeDMath::SQUARE(x2) + ThreeDMath::SQUARE(y2) );
			double	z2	= ThreeDMath::SQUARE(R) / distance2;
			if( R > distance2 ){
				z2	= sqrt( 2.0 * ThreeDMath::SQUARE(R) - (ThreeDMath::SQUARE(x2) + ThreeDMath::SQUARE(y2)) );
			}
			const CQuaternion	q2(0.0, x2, y2, z2);


			CQuaternion	q3	= q1 * q2;
			q3.Normalize();

			CQuaternion	q4	= q1 - q2;
			double	norm	= q4.Norm();
			norm	= norm  / (2.0 * R * 0.70710678118654752440);
			if( norm > 1.0 ){
				norm	= 1.0;
			}

			qt.Set( cos(asin(norm)), (q3.V.x * norm), (q3.V.y * norm), (q3.V.z * norm) );
		}
	}
	/**********************************************************************************
	 */
	void	OnMouseMove			( int x, int y, UINT KeyFlags )	{
		if( bTrack ){
			DBG::TRACE( _T("Trackball::OnMouseMove(%d, %d, %u)"),x,y,KeyFlags);

			m_Arcball.Drag( double(x), double(y) );

			double	x1		= (2.0 * ptBegin.x - m_rect.Width())	/ m_rect.Width();
			double	y1		= (m_rect.Height() - 2.0 * ptBegin.y)	/ m_rect.Height();
			double	x2		= (2.0 *         x - m_rect.Width())	/ m_rect.Width();
			double	y2		= (m_rect.Height() - 2.0 *         y)	/ m_rect.Height();

			Trackball( qtLast, x1, y1, x2, y2 );
			ptBegin.Set( x, y );

			CQuaternion	qt = (qtLast * qtCurr);
			qtCurr = qt;

			static int	cnt = 0;
			if( ++cnt % 97 ){
				GLdouble	dScaler	= qtCurr.Norm();
				qtCurr = CQuaternion::Div(qtCurr, dScaler);
			}

			Invalidate();
		}
	}
	/**********************************************************************************
	 */
	void	OnLButtonDown		( int x, int y, UINT KeyFlags )	{
		if( MK_LBUTTON == KeyFlags ){
			bTrack	= true;
			ptBegin.Set( x, y );

			m_Arcball.Begin( double(x), double(y) );

			DBG::TRACE( _T("Trackball::OnLButtonDown(%d, %d, %u)"),x,y,KeyFlags);
		}
	}
	/**********************************************************************************
	 */
	void	OnLButtonUp			( int x, int y, UINT KeyFlags ) {
		if( bTrack ){
			bTrack	= false;

			m_Arcball.End();

			DBG::TRACE( _T("Trackball::OnLButtonUp(%d, %d, %u)"),x,y,KeyFlags);
		}
	}
	/**********************************************************************************
	 */
	void	Display( void ){
		DBG::TRACE( _T("Trackball::Display()"));

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		GLdouble	view[4][4] = {
			{  ONE, ZERO, ZERO, ZERO },
			{ ZERO,  ONE, ZERO, ZERO },
			{ ZERO, ZERO,  ONE, ZERO },
			{ ZERO, ZERO, -4.0,  ONE },
		//	{ ZERO, ZERO, ZERO,  ONE },
		};
		glLoadIdentity();
		glMultMatrixd( &view[0][0] );

		glPushMatrix();
		{
			GLdouble	rot[4][4] = {0};
			qtCurr.GetRotation( rot );

			glMultMatrixd( &rot[0][0] );

			static GLint	displayList = 0;
			static bool	bInit = true;
			if( bInit )
			{
				bInit	= false;

				displayList	= ::glGenLists( 1 );
				::glNewList( displayList, GL_COMPILE_AND_EXECUTE );
				{
					if( Model::Instance().m_bDrawAxis )
						DrawAxis();

					DrawFrame();
					DrawCube();
					DrawShape();
				}
				::glEndList();
			}
			else
			{
				::glCallList( displayList );
			}
		}
		glPopMatrix();
	}
	/**********************************************************************************
	 */
	void	Resize( int nWidth, int nHeight ){
		glViewport( 0, 0, nWidth, nHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		gluPerspective( 45, nWidth/nHeight, 0.1, 100 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( m_Camera.x, m_Camera.y, m_Camera.z, 0, 0, 0, 0, 1, 0 );
	
		m_Arcball.setBounds( nWidth, nHeight );
	}

	void	OnMouseMove			( int x, int y, UINT KeyFlags )	{
		if( m_Arcball.IsDrag() ){
			m_Arcball.Move(x, y);
		}
	}
	void	OnLButtonDown		( int x, int y, UINT KeyFlags )	{
		if( KeyFlags == MK_LBUTTON )
			m_Arcball.Begin(x, y);
	}
	void	OnLButtonUp			( int x, int y, UINT KeyFlags )	{
		m_Arcball.End();
	}
	/**********************************************************************************
	 */
	void	Notify( void ){
		CRect	rc;
		GetClientRect( &rc );

		m_Camera	= Model::Instance().m_Camera;

		Resize( rc.Width(), rc.Height() );
		Invalidate();
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CInputPanel : public CPanel, public IObservable {
public:
	enum {
		IDC_UPDATE_BUTTON		= 1001	,
		IDC_CHECK_AXIS					,
		IDC_SPIN_EDIT					,
		IDC_SPIN_UPDOWN_X				,
		IDC_SPIN_UPDOWN_Y				,
		IDC_SPIN_UPDOWN_W				,
	};

protected:
	CButton						m_btnUpdate;
	CCheckBox					m_chkAxis;

	TSpinCtrl<double,IDC_SPIN_UPDOWN_X>			m_spinX;
	TSpinCtrl<double,IDC_SPIN_UPDOWN_Y>			m_spinY;
	TSpinCtrl<double,IDC_SPIN_UPDOWN_W>			m_spinZ;

	TCommandHandler<CInputPanel>	cmd;

	/**********************************************************************************
	 */
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( !m_btnUpdate.Create(this, 10, 10, 100, 30, IDC_UPDATE_BUTTON, _T("Update")) ){
			return FALSE;
		}

		if( !m_chkAxis.Create(this, 110, 10, 100, 30, IDC_CHECK_AXIS, _T("AXIS")) ){
			return FALSE;
		}

		if( !m_spinX.Create(this, 10, 50, 100, 25) ){
			return FALSE;
		}

		if( !m_spinY.Create(this, 110, 50, 100, 25) ){
			return FALSE;
		}

		if( !m_spinZ.Create(this, 210, 50, 100, 25) ){
			return FALSE;
		}

		m_spinX.SetRange( 0.0, 10.0, 0.05 );
		m_spinX.SetPos( 0.5 );

		m_spinY.SetRange( 0.0, 10.0, 0.05 );
		m_spinY.SetPos( 1.5 );

		m_spinZ.SetRange( 0.0, 10.0, 0.05 );
		m_spinZ.SetPos( 2.5 );

		cmd.Initialize( this );
		cmd.Register( IDC_UPDATE_BUTTON, &CInputPanel::OnUpdateBtnClick );
		cmd.Register( IDC_CHECK_AXIS, &CInputPanel::OnAxisCheck );

		SetFontChildren();
		return TRUE;
	}
	/**********************************************************************************
	 */
	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;
	}
	/**********************************************************************************
	 */
	void	OnCommand( UINT nID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach( nID, nCodeNotify );
	}

public:
	/**********************************************************************************
	 */
	void	OnUpdateBtnClick( void ){
		Model::Instance().SetCamera(	m_spinX.GetPos()
									,	m_spinY.GetPos()
									,	m_spinZ.GetPos()	);
		Model::Instance().NotifyAll();
	}
	/**********************************************************************************
	 */
	void	OnAxisCheck( void ){
		Model::Instance().m_bDrawAxis	= m_chkAxis.GetCheck() == 0 ? false : true;
		Model::Instance().NotifyAll();
	}

	/**********************************************************************************
	 */
	void	Notify( void ){
	}
};
/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
protected:
	CHorizontalSplitter		m_Splitter;
	COpenGLPanel			m_wndOpenGL;
	CInputPanel				m_wndInput;

	/**********************************************************************************
	 */
	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){

		if( !m_wndOpenGL.Create(this, 0, 0, 0, 0) ){
			return FALSE;
		}

		if( !m_wndInput.Create(this, 0, 0, 0, 0) ){
			return FALSE;
		}

		m_Splitter.Bind( this );
		m_Splitter.Append( &m_wndOpenGL );
		m_Splitter.Append( &m_wndInput	);
		m_Splitter.SplitterPos( 300 );

		Model::Instance().add( &m_wndOpenGL	);
		Model::Instance().add( &m_wndInput	);

		SetFontChildren();
		return TRUE;
	}
	/**********************************************************************************
	 */
	void	OnLButtonDown( int x, int y, UINT KeyFlags ){
		m_Splitter.BeginSplitterMoving();
	}
	/**********************************************************************************
	 */
	void	OnLButtonUp( int x, int y, UINT KeyFlags ){
		m_Splitter.EndSplitterMoving();
	}
	/**********************************************************************************
	 */
	void	OnMouseMove( int x, int y, UINT KeyFlags ){
		if( m_Splitter.IsSplitterMoving( KeyFlags ) )
			m_Splitter.MoveSplitter( x, y );
	}
	/**********************************************************************************
	 */
	void	OnSize( UINT state, int cx, int cy ){
		m_Splitter.AdjustWindow( cx, cy );
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

	//	MakeRot();

	CMainForm	Form;
	if( Form.Create(_T("ExampleOpenGL"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
