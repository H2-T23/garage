// ExampleOpenGL.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "ExampleOpenGL.h"

#include "UseToolkit.h"
#include "UseOpenGL.h"

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
		vecVertex[ 0 ].Set(  0.5,  0.5,  0.5 );
		vecVertex[ 1 ].Set(  0.5,  0.5,  0.5 ); 
		vecVertex[ 2 ].Set( -0.5, -0.5,  0.5 ); 
		vecVertex[ 3 ].Set( -0.5,  0.5,  0.5 ); 
		vecVertex[ 4 ].Set(  0.5,  0.5, -0.5 ); 
		vecVertex[ 5 ].Set(  0.5, -0.5, -0.5 ); 
		vecVertex[ 6 ].Set( -0.5,  0.5, -0.5 ); 
		vecVertex[ 7 ].Set( -0.5,  0.5, -0.5 ); 

		vecPolygon.resize( 6 );
		vecPolygon[ 0 ].Edge( vecVertex[ 0 ], vecVertex[ 1 ], vecVertex[ 2 ], vecVertex[ 3 ] );
		vecPolygon[ 1 ].Edge( vecVertex[ 4 ], vecVertex[ 5 ], vecVertex[ 6 ], vecVertex[ 7 ] );
		vecPolygon[ 2 ].Edge( vecVertex[ 0 ], vecVertex[ 4 ], vecVertex[ 5 ], vecVertex[ 1 ] );
		vecPolygon[ 3 ].Edge( vecVertex[ 3 ], vecVertex[ 2 ], vecVertex[ 6 ], vecVertex[ 7 ] );
		vecPolygon[ 4 ].Edge( vecVertex[ 0 ], vecVertex[ 3 ], vecVertex[ 7 ], vecVertex[ 4 ] );
		vecPolygon[ 5 ].Edge( vecVertex[ 1 ], vecVertex[ 2 ], vecVertex[ 6 ], vecVertex[ 5 ] );
	}

public:
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

	void	DrawAxis( void ){
		glColor3d( 1, 1, 1 );
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
		glBegin( GL_LINES );
		for( int i=0; i<1; i++ ){
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
	//	m_Camera.Set( 0.5, 1.5, 2.5 );
		m_Camera	= Model::Instance().m_Camera;
	}

	void	Display( void ){
		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );

		glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawAxis();

		DrawCube();

	//	glColor3d( 1, 0, 1 );
	//	glRectd( -0.5, -0.5, 0.5, 0.5 );
	}

	void	Resize( int nWidth, int nHeight ){
		glViewport( 0, 0, nWidth, nHeight );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		gluPerspective( 45, nWidth/nHeight, 0.1, 100 );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt( m_Camera.x, m_Camera.y, m_Camera.y, 0, 0, 0, 0, 1, 0 );
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
		IDC_SPIN_EDIT					,
		IDC_SPIN_UPDOWN					,
	};

protected:
	CButton					m_btnUpdate;
	TSpinCtrl<double>		m_spinX, m_spinY, m_spinZ;

	TCommandHandler<CInputPanel>	cmd;

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		if( !m_btnUpdate.Create(this, 10, 10, 100, 30, IDC_UPDATE_BUTTON, _T("Update")) ){
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
		m_spinY.SetPos( 0.5 );

		m_spinZ.SetRange( 0.0, 10.0, 0.05 );
		m_spinZ.SetPos( 0.5 );

		cmd.Initialize( this );
		cmd.Register( IDC_UPDATE_BUTTON, &CInputPanel::OnUpdateBtnClick );

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
