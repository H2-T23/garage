#pragma once

#include "UseGUIToolkit.h"
#include "OpenGL.h"
#include "Observar.h"
#include "ThreeDMath.h"

using namespace ThreeDMath;

/**********************************************************************************
 */
class CCamera {
public:
	bool			fBegin;
	CMatrix4x4		matRot;
	CVector3		ptBegin;
	CQuaternion		qtTarget, qtCurrent;

	CCamera( void ) : fBegin(false) {
	}

	void	MoveBegin( int x, int y ){
		if( fBegin == false ){
			fBegin = true;
			ptBegin.Set( x, y, 0 );
		}
	}

	void	Move( int x, int y ){
		double dx = (x - ptBegin.x);
		double dy = (y - ptBegin.y);

		double	len = sqrt( dx*dx * dy*dy );
		if( len )
		{
			double	radian	= len * 3.14;
			double	theta	= sin(radian) / len;

			CQuaternion after( cos(radian), dy * theta, dx * theta, 0 );



			GenerateRotate( qtTarget );
		}
	}

	void	GenerateRotate( CQuaternion& qt ){
	}
};

/**********************************************************************************
 */
class CSphere {
public:
	double		m_Radius;
	CVector3	m_Point;
};

/**********************************************************************************
 *
 *
 *
 */
class COpenGLParameter :  public CSubject {
public:
	struct {
		int		x, y, width, height;
	} viewport;
	struct {
		double	x, y, z;
	} eye;

	COpenGLParameter( void ){
		SetEye( 0.5, 1.5, 2.5 );
		SetViewport( 0.0, 0.0, 0.0, 0.0 );
	}

	void	SetEye( double x, double y, double z ){
		eye.x	= x;
		eye.y	= y;
		eye.z	= z;
	}

	void	SetViewport( int x, int y, int w, int h ){
		viewport.x		= x;
		viewport.y		= y;
		viewport.width	= w;
		viewport.height	= h;
	}

	void	Update( void ){
		NotifyAll();
	}
};

/**********************************************************************************
 *
 *
 *
 */
class COpenGLView : public CPanel, public IObservable {
public:
	COpenGLParameter*	m_pParameter;

	void	Bind( COpenGLParameter* pParameter ){
		m_pParameter = pParameter;
	}

protected:
	virtual DWORD		WindowStyle( void ) const { return(CPanel::WindowStyle() | WS_CLIPSIBLINGS | WS_CLIPCHILDREN); }

	COpenGL			GL;

	void	subAxis( float d, float s ){
		const double	pi = 3.14;

	//	glBegin( GL_QUAD_STRIP );
		glBegin( GL_LINES );
		{
			for( double i=0; i<=6.0; i++ ){
				double t	= (i * 2 * pi / 6);
				glNormal3f( cos(t), 0, sin(t) );
				glVertex3f( d * cos(t), -s, d * sin(t) );
				glVertex3f( d * cos(t),  s, d * sin(t) );
			}
		}
		glEnd();

		glTranslatef( 0, s, 0 );
		glRotatef( -90, 1, 0, 0 );
	}

	void	Axis( float d, float s ){
		glPushMatrix();
		subAxis( d, s );
		glPopMatrix();

		glPushMatrix();
		glRotatef( 90, 1, 0, 0 );
		subAxis( d, s );
		glPopMatrix();

		glPushMatrix();
		glRotatef( -90, 0, 0, 1 );
		subAxis( d, s );
		glPopMatrix();
	}

	void	DrawAxis( void ){
		GLdouble	axis[][ 3 ] = {
			{ 0, 0, 0 },
			{ 1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, 0, 1 },
		};

		glBegin( GL_LINES );
		for( int i=0; i<3; i++ ){
			glColor3dv( axis[ i + 1 ] );
			glVertex3dv( axis[ 0 ] );
			glVertex3dv( axis[ i + 1 ] );
		}
		glEnd();
	}

	void	DrawPlane( void ){
		const GLdouble	xsize	= 10.0;
		const GLdouble	zsize	= 10.0;
		const int		xnum	= 20;
		const int		znum	= 20;

		glBegin( GL_LINES );
		glColor3d( 0.5, 0.5, 0.5 );
		for( GLdouble x=-xsize; x<=xsize; x+=(xsize/xnum) ){
			glVertex3d( x, 0.0, -zsize );
			glVertex3d( x, 0.0,  zsize );
		}
		for( GLdouble z=-zsize; z<=zsize; z+=(zsize/znum) ){
			glVertex3d( -xsize, 0.0, z );
			glVertex3d(  xsize, 0.0, z );
		}
		glEnd();
	}

	GLint DrawPlane( GLdouble w, GLdouble d ,GLdouble h )
	{
		GLdouble norm[3];
		glPushMatrix();
	//	glBegin( GL_POLYGON );
		glBegin(GL_LINE_LOOP);
			norm[0]=1;norm[1]=0;norm[2]=0;
			glNormal3dv (norm);
			glVertex3d( w/2.0 , h ,   d/2.0 );
			glVertex3d( w/2.0 , h ,  -d/2.0  );
			glVertex3d(-w/2.0 , h ,  -d/2.0  );
			glVertex3d(-w/2.0 , h ,   d/2.0  );
		glEnd();
		glPopMatrix();

		return(0) ;
	}

	GLint DrawBox( GLdouble d, GLdouble w, GLdouble h )
	{
		GLdouble norm[3];
	
		glPushMatrix();
		glTranslated(-d/2.0,-w/2.0,-h/2.0);
		glBegin( GL_POLYGON );
			norm[0]=0;norm[1]=-1;norm[2]=0;
			glNormal3dv (norm);
			glVertex3d( 0 , 0 , 0 );
			glVertex3d( d , 0 , 0 );
			glVertex3d( d , 0 , h );
			glVertex3d( 0 , 0 , h );
		glEnd();
		glBegin( GL_POLYGON );
			norm[0]=0;norm[1]=-1;norm[2]=0;
			glNormal3dv (norm);
			glVertex3d( 0 , w , 0 );
			glVertex3d( d , w , 0 );
			glVertex3d( d , w , h );
			glVertex3d( 0 , w , h );
		glEnd();

		glBegin( GL_POLYGON );
			norm[0]=0;norm[1]=0;norm[2]=1;
			glNormal3dv (norm);
			glVertex3d( 0 , 0 , h );
			glVertex3d( d , 0 , h );
			glVertex3d( d , w , h );
			glVertex3d( 0 , w , h );
		glEnd();
		glBegin( GL_POLYGON );
			norm[0]=0;norm[1]=0;norm[2]=1;
			glNormal3dv (norm);
			glVertex3d( 0 , 0 , 0 );
			glVertex3d( d , 0 , 0 );
			glVertex3d( d , w , 0 );
			glVertex3d( 0 , w , 0 );
		glEnd();

		glBegin( GL_POLYGON );
			norm[0]=-1;norm[1]=0;norm[2]=0;
			glNormal3dv (norm);
			glVertex3d( 0 , 0 , 0 );
			glVertex3d( 0 , 0 , h );
			glVertex3d( 0 , w , h );
			glVertex3d( 0 , w , 0 );
		glEnd();
		glBegin( GL_POLYGON );
			norm[0]=-1;norm[1]=0;norm[2]=0;
			glNormal3dv (norm);
			glVertex3d( d , 0 , 0 );
			glVertex3d( d , 0 , h );
			glVertex3d( d , w , h );
			glVertex3d( d , w , 0 );
		glEnd();
		glPopMatrix();

		return(0) ;
	}

	void	DrawBox( void ){
		static GLfloat vertices [8][3] =
		{
			{-0.5f, -0.5f,  0.5f},
			{ 0.5f, -0.5f,  0.5f},
			{ 0.5f,  0.5f,  0.5f},
			{-0.5f,  0.5f,  0.5f},
			{ 0.5f, -0.5f, -0.5f},
			{-0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},
			{ 0.5f,  0.5f, -0.5f}
		};

		glColor3f( 1, 0, 1 );

		// ‘O
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[0]);
			glVertex3fv(vertices[1]);
		//	glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[2]);
			glVertex3fv(vertices[3]);
		glEnd();

		// Œã
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[4]);
			glVertex3fv(vertices[5]);
		//	glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[6]);
			glVertex3fv(vertices[7]);
		glEnd();

		// ‰E
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[1]);
			glVertex3fv(vertices[4]);
		//	glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[7]);
			glVertex3fv(vertices[2]);
		glEnd();

		// ¶
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[5]);
			glVertex3fv(vertices[0]);
		//	glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[3]);
			glVertex3fv(vertices[6]);
		glEnd();

		// ã
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3fv(vertices[3]);
			glVertex3fv(vertices[2]);
			glVertex3fv(vertices[7]);
			glVertex3fv(vertices[6]);
		glEnd();

		// ‰º
	//	glBegin(GL_POLYGON);
		glBegin(GL_LINE_LOOP);
		//	glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3fv(vertices[1]);
			glVertex3fv(vertices[0]);
			glVertex3fv(vertices[5]);
			glVertex3fv(vertices[4]);
		glEnd();
	}

	void	DrawCube( void ){

	}

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
	
		if( GL.Startup( GetDC()->operator HDC() ) ){
			glClearColor( 0, 0, 0.5, 1 );
			glClearDepth( 1 );
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_CULL_FACE );
		}

		return TRUE;
	}

	virtual void	OnPaint( HDC hDC ){
		CDC	dc(hDC);

		wglMakeCurrent( hDC, GL.m_hRC );
#if 0
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glColor3f( 1, 0, 1 );
		glRectd(-0.5,-0.5,0.5,0.5);
#else
#endif
		resize( m_pParameter->viewport.x
			,	m_pParameter->viewport.y
			,	m_pParameter->viewport.width
			,	m_pParameter->viewport.height
			);
		display();

		SwapBuffers( hDC );
		wglMakeCurrent( NULL, NULL );
	}

	virtual void	OnSize( UINT state, int cx, int cy ) {
		if( cx == 0 && cy == 0 )
			return;

		CRect	rc;
		GetClientRect( &rc );

		GLfloat	w	= /*0.6 **/ rc.Width();
		GLfloat	h	= /*0.6 **/ rc.Height();
		
		m_pParameter->SetViewport( 0, 0, w, h );
		m_pParameter->Update();
	}

	void		display( void ){
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glLineWidth( 1.0 );
		DrawPlane();

		glLineWidth( 5.0 );
		DrawAxis();

		glLineWidth( 3.0 );
		DrawBox();

		glFlush();
	}

	void		resize( int x, int y, int w, int h ){
		glViewport( x, y, w, h );

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective(45, w/h, 0.1, 100);

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		gluLookAt(	m_pParameter->eye.x
				,	m_pParameter->eye.y
				,	m_pParameter->eye.z
				,	0, 0, 0
				,	0, 1, 0 );
		Debug::Trace( _T("Eye(%04f, %04f, %04f)\n"), m_pParameter->eye.x, m_pParameter->eye.y, m_pParameter->eye.z );
	}

	virtual void	Notify( void ){
		Invalidate();
	}
};

/**********************************************************************************
 *
 *
 *
 */
class COpenGLInput : public TCommandListener<COpenGLInput,CPanel> , public IObservable {
public:
	COpenGLParameter*	m_pParameter;

	void	Bind( COpenGLParameter* pParameter ){
		m_pParameter = pParameter;
	}

protected:
	CSpin					m_spin;
	CButton					m_btnApply;
	CGroupBox				m_grpEye;
	CNumberEdit<double>		m_edtEyeX, m_edtEyeY, m_edtEyeZ;
	CGroupBox				m_grpViewport;
	CNumberEdit<int>		m_edtViewportX, m_edtViewportY, m_edtViewportW, m_edtViewportH;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		
		CRect		rc( lpCreateStruct->x, lpCreateStruct->y, 
						lpCreateStruct->cx, lpCreateStruct->cy );

		m_btnApply.Create(this, 10, 500, 100, 30, 3000, _T("Apply"));

		m_spin.Create( this, 10, 10, 80, 30, 1001 );
		m_spin.SetRange( 10, 100 );
		m_spin.SetPos( 50 );

		if( m_grpViewport.Create(this, 10, 50, 200, 100, 1010, _T("Viewport")) ){
			m_edtViewportX.Create( &m_grpViewport,  10, 25, 80, 25, 1011 );
			m_edtViewportY.Create( &m_grpViewport, 100, 25, 80, 25, 1012 );
			m_edtViewportW.Create( &m_grpViewport,  10, 55, 80, 25, 1013 );
			m_edtViewportH.Create( &m_grpViewport, 100, 55, 80, 25, 1014 );
		}

		if( m_grpEye.Create(this, 10, 170, 300, 70, 1100, _T("Eye")) ){
			m_edtEyeX.Create( &m_grpEye,  10, 25, 80, 25, 1101 );
			m_edtEyeY.Create( &m_grpEye, 100, 25, 80, 25, 1102 );
			m_edtEyeZ.Create( &m_grpEye, 190, 25, 80, 25, 1102 );
		}

		m_edtEyeX.Set( 10.5 );
		m_edtEyeY.Set( 1.5 );
		m_edtEyeZ.Set( 2.5 );

		AddCommandListener( 3000, &COpenGLInput::OnApply );

		return TRUE;
	}

	void			OnApply( int id ){
		m_pParameter->SetViewport( m_edtViewportX.Get(), m_edtViewportY.Get(), m_edtViewportW.Get(), m_edtViewportH.Get() );

		m_pParameter->SetEye(	double(m_edtEyeX.Get())
							,	double(m_edtEyeY.Get())
							,	double(m_edtEyeZ.Get())	);

		m_pParameter->Update();
	}

	virtual void	Notify( void ){
		m_edtViewportX.Set( m_pParameter->viewport.x );
		m_edtViewportY.Set( m_pParameter->viewport.y );
		m_edtViewportW.Set( m_pParameter->viewport.width );
		m_edtViewportH.Set( m_pParameter->viewport.height );

		UpdateWindow();
	}
};

/**********************************************************************************
 *
 *
 *
 */
class COpenGLPanel : public CPanel {
protected:
	COpenGLView			m_wndView;
	COpenGLInput		m_wndInput;
	COpenGLParameter	m_parameter;

	virtual BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		m_wndView.Bind( &m_parameter );
		m_wndInput.Bind( &m_parameter );

		m_wndView.Create( this, 0, 0, 0, 0, _T("OpenGLView") );
		m_wndInput.Create( this, 0, 0, 0, 0, _T("OpenGLParameter") );

		m_parameter.CSubject::add( &m_wndView );
		m_parameter.CSubject::add( &m_wndInput );

		return TRUE;
	}

	virtual void	OnSize( UINT state, int cx, int cy ){
		CRect	rc;
		GetClientRect( &rc );

		int	w1	= rc.Width() * 2/3;
		int w2	= rc.Width() * 1/3;


		m_wndView.MoveWindow( 0, 0, w1, rc.Height(), TRUE );
		m_wndInput.MoveWindow( w1, 0, w2, rc.Height(), TRUE );
	}
};
