// ExampleOpenGL.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleOpenGL.h"

#include "UseToolkit.h"
#include "OpenGLPanel.h"

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

/**********************************************************************************
 *
 *
 *
 */
class CArcBall {
protected:
	CVector3		m_vBegin;
	CVector3		m_vEnd;
	CQuaternion		m_qt;

	bool			m_bDrag;

	double			m_dWidth;
	double			m_dHeight;

	double			m_dCoefX;
	double			m_dCoefY;

	inline void		mapToSphere( int x, int y, CVector3& vec ) const {
		vec.x	= x * m_dCoefX;
		vec.y	= y	* m_dCoefY;
	}

public:
	CArcBall( void ) : m_bDrag(false) {
		setBounds(ZERO, ZERO);
	}
	CArcBall( double Width, double Height ) : m_bDrag(false) {
		setBounds(Width, Height);
	}
	~CArcBall( void ){
	}
	
	inline void		setBounds( double Width, double Height ){
		m_dWidth	= Width;
		m_dHeight	= Height;
		m_dCoefX	= (1.0 / ((Width - 1.0) * 0.5));
		m_dCoefY	= (1.0 / ((Height- 1.0) * 0.5));
	}

	inline bool	IsDrag( void ) const {
		return(m_bDrag);
	}

	void		Begin( int x, int y ){
		if( !m_bDrag ){
			m_bDrag	= true;
			m_qt.in
			mapToSphere( x, y, m_vBegin );
		}	
	}

	void		End( void ){
		if( m_bDrag ){
			m_bDrag	= false;
		}
	}

	void		Move( int x, int y ){
		if( m_bDrag )
		{
			mapToSphere( x, y, m_vEnd );
			CVector3	vPerp	= m_vBegin * m_vEnd;

			vPerp.Normalize();

			m_qt.V	= vPerp;
			m_qt.S	= CVector3::dot(m_vBegin, m_vEnd);
		}
	}
};
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
	Vertex		m_Camera;
	CArcBall	m_Arcball;

	CWireSphere	m_WireSphere;

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

public:
	void	Init( void ){
		m_Camera	= Model::Instance().m_Camera;

		m_WireSphere.m_vCenter.Set( 0, 0, 0 );
		m_WireSphere.m_dRadius	= RADIUS( 45 );
		m_WireSphere.m_vColor.Set( 0, 0, 0 );
	}

	void	Display( void ){
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		if( Model::Instance().m_bDrawAxis )
			DrawAxis();

	//	DrawCube();
		m_WireSphere.Draw();
	}

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

	void	OnSize( UINT state, int cx, int cy ){
		if( cx <= 0 || cy <= 0 )
			return;
	}

	void	OnCommand( UINT nID, HWND hWndCtrl, UINT nCodeNotify ){
		cmd.Dispach( nID, nCodeNotify );
	}

public:
	void	OnUpdateBtnClick( void ){
		Model::Instance().SetCamera(	m_spinX.GetPos()
									,	m_spinY.GetPos()
									,	m_spinZ.GetPos()	);
		Model::Instance().NotifyAll();
	}

	void	OnAxisCheck( void ){
		Model::Instance().m_bDrawAxis	= m_chkAxis.GetCheck() == 0 ? false : true;
		Model::Instance().NotifyAll();
	}

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

		return TRUE;
	}

	void	OnLButtonDown( int x, int y, UINT KeyFlags ){
		m_Splitter.BeginSplitterMoving();
	}

	void	OnLButtonUp( int x, int y, UINT KeyFlags ){
		m_Splitter.EndSplitterMoving();
	}

	void	OnMouseMove( int x, int y, UINT KeyFlags ){
		if( m_Splitter.IsSplitterMoving( KeyFlags ) )
			m_Splitter.MoveSplitter( x, y );
	}

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

	CMainForm	Form;
	if( Form.Create(_T("ExampleOpenGL"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}