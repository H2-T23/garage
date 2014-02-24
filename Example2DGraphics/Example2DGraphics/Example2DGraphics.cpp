// Example2DGraphics.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <math.h>

#include "Example2DGraphics.h"


#include "UseToolkit.h"


#define PI						(3.141592653589793)
#define	DegreeToRadian(deg)		((deg * PI)/180.0)
#define	RadianToDegree(rag)		((rag * 180.0)/PI)

class Vec3x3 {
public:
	double	x,y,z;
	Vec3x3( void ) : x(0), y(0), z(1) {}
	Vec3x3( double X, double Y, double Z = 1 ) : x(X), y(Y), z(Z) {}

	Vec3x3( const Vec3x3& v ){
		Set(v);
	}

	void	Set( const Vec3x3& v ){
		Set(v.x, v.y, v.z);
	}
	void	Set( double X, double Y, double Z = 1 ){
		this->x = X;
		this->y = Y;
		this->z = Z;
	}

	Vec3x3&	operator=( const Vec3x3& v ){
		if( this != &v ){
			Set(v);
		}
		return(*this);
	}

	void		ToString( TString& str ){
		str.Format( _T("[%0.2f  %0.2f  %0.2f]\n"), x, y, z );
	}

	TString		ToString(){
		TString	str;
		ToString( str );
		return(str);
	}

	POINT	GetPoint( void ) const {
		POINT	pt;
		pt.x	= int(x + 0.5);
		pt.y	= int(y + 0.5);
		return(pt);
	}
};

class Mat3x3 {
public:
	int			ELEMENTS;
	double		mat[3][3];

	Mat3x3( void ){
		Identity();
	}

	Mat3x3(	double m00, double m01, double m02
		,	double m10, double m11, double m12
		,	double m20, double m21, double m22	)
	{
		this->Set(m00,m01,m02,m10,m11,m12,m20,m21,m22);
	}

	void	Zero( void ){
		memset(mat,0,sizeof(mat));
	}

	void	Identity( void ){
		Zero();
		for( int i=0; i<3; i++ ){
			mat[ i ][ i ]	= 1;
		}
	}

	void	Set( const Mat3x3& m ){
		this->Set(	m.mat[0][0],	m.mat[0][1],	m.mat[0][2]
				,	m.mat[1][0],	m.mat[1][1],	m.mat[1][2]
				,	m.mat[2][0],	m.mat[2][1],	m.mat[2][2]	);
	}

	void	Set(	double m00, double m01, double m02
				,	double m10, double m11, double m12
				,	double m20, double m21, double m22	)
	{
		mat[0][0]=m00;	mat[0][1]=m01;	mat[0][2]=m02;
		mat[1][0]=m10;	mat[1][1]=m11;	mat[1][2]=m12;
		mat[2][0]=m20;	mat[2][1]=m21;	mat[2][2]=m22;
	}

	void		ToString( TString& str ) const {
		str.Format( _T(	"|%0.2f   %0.2f   %0.2f|\n"	)
					_T(	"|%0.2f   %0.2f   %0.2f|\n"	)
					_T(	"|%0.2f   %0.2f   %0.2f|\n"	)	
					, mat[0][0],mat[0][1],mat[0][2]
					, mat[1][0],mat[1][1],mat[1][2]
					, mat[2][0],mat[2][1],mat[2][2]
					);
	}

	TString		ToString( void ) const {
		TString	str;
		ToString( str );
		return(str);
	}

	Mat3x3&		operator=( const Mat3x3& m ){
		if( this != &m ){
			memcpy(mat, m.mat, sizeof(mat));
		}
		return(*this);
	}

	Mat3x3&		SetScale( double Sx, double Sy, double Sz = 1 ){
		mat[0][0] = Sx;
		mat[1][1] = Sy;
		mat[2][2] = Sz;
		return(*this);
	}

	Mat3x3&		SetTranlsation( double Tx, double Ty, double Tz = 1 ){
		mat[0][2] = Tx;
		mat[1][2] = Ty;
		mat[2][2] = Tz;
		return(*this);
	}

	Mat3x3&		SetRotation( double dTheta ){
		double	dCosTheta	= cos( DegreeToRadian(dTheta) );
		double	dSinTheta	= sin( DegreeToRadian(dTheta) );
		mat[0][0]	= dCosTheta;	mat[0][1]	= -(dSinTheta);
		mat[1][0]	= dSinTheta;	mat[1][1]	= dCosTheta;
		return(*this);
	}

	Mat3x3&		InverseX( void ){
		mat[1][1] *= -1;
		return(*this);
	}

	Mat3x3&		InverseY( void ){
		mat[0][0] *= -1;
		return(*this);
	}


	Mat3x3&		Multi( const Mat3x3& m ){
		Mat3x3	ans;
		ans.Zero();
		for( int i=0; i<3; i++ ){
			for( int j=0; j<3; j++ ){
				for( int k=0; k<3; k++ ){
					ans.mat[ i ][ j ] += (mat[ i ][ k ] * m.mat[ k ][ j ]);				
				}
			}
		}
		Set( ans );
		return(*this);
	}

	Mat3x3&		Multi( double dScalar ){
		for( int i=0; i<3; i++ ){
			for( int j=0; j<3; j++ ){
				mat[ i ][ j ] *= dScalar;
			}
		}
		return(*this);
	}

	Vec3x3		Multi( const Vec3x3& v ){
		Vec3x3	vec;
		vec.x	= (mat[0][0] * v.x) + (mat[0][1] * v.y) + (mat[0][2] * v.z);
		vec.y	= (mat[1][0] * v.x) + (mat[1][1] * v.y) + (mat[1][2] * v.z);
		vec.z	= (mat[2][0] * v.x) + (mat[2][1] * v.y) + (mat[2][2] * v.z);
		return(vec);
	}

public:
	static void			MatrixAddition( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				int	nIdx	= ((i * nElements) + j);
				pdAns[ nIdx ]	= (pdA[ nIdx ] + pdB[ nIdx ]);
			}
		}
	}
	static void			MatrixSubtraction( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				int	nIdx	= ((i * nElements) + j);
				pdAns[ nIdx ]	= (pdA[ nIdx ] - pdB[ nIdx ]);
			}
		}
	}
	static void			MatrixCross( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				double sum	= 0;
				for( int k=0; k<nElements; k++ ){
					sum	+=	(	pdA[ ((i * nElements) + k) ] * pdB[ ((k * nElements) + j) ]	);
				}

				pdAns[ ((i * nElements) + j) ]	= sum;
			}
		}
	}
	static Mat3x3		Multi( const Mat3x3& matA, const Mat3x3& matB ){
		Mat3x3	matAns;
		for( int i=0; i<3; i++ ){
			for( int j=0; j<3; j++ )
			{
				double	sum = 0;
				for( int k=0; k<3; k++ ){
					sum += matA.mat[ i ][ k ] * matB.mat[ k ][ j ];
				}

				matAns.mat[ i ][ j ]	= sum;
			}
		}
		return(matAns);
	}

};


/**********************************************************************************
 *
 *
 *
 */
class CMainForm : public CForm {
public:
	enum { IDC_SPIN1 = 1001, IDC_SPIN2, IDC_SPIN3 };

protected:
	CRect	rcViewport;
	CPoint	ptCenter;
	CPoint	ptAxisX[2], ptAxisY[2];

	int			nDegree;
	Mat3x3		mat, matScale, matTrans, matRot, matInverseX, matInverseY;
	Mat3x3		matViewport, matViewportInverse;

	TSpinCtrl<int, IDC_SPIN1>			m_spin1;
	TSpinCtrl<int, IDC_SPIN2>			m_spin2;
	TSpinCtrl<int, IDC_SPIN3>			m_spin3;

	TNotifyHandler<CMainForm>				notify;

	void	OnDeltapos( LPNMHDR lpNmhdr ){
		switch( lpNmhdr->idFrom ){
		case IDC_SPIN1:
			DBG::TRACE(_T("UDN_DELTAPOS#1"));
			nDegree = m_spin1.GetPos();
			Invalidate();
			break;
		case IDC_SPIN2:
			DBG::TRACE(_T("UDN_DELTAPOS#2"));
			break;
		case IDC_SPIN3:
			DBG::TRACE(_T("UDN_DELTAPOS#3"));
			break;
		}
	}

	void	OnNotify( WPARAM wParam, LPARAM lParam ){
		notify.Dispach(wParam, lParam);
	}

	BOOL	OnCreate( LPCREATESTRUCT lpCreateStruct ){
		nDegree	= 90;

		if( m_spin1.Create(this, 500, 10, 100, 25) ){
			m_spin1.SetFont();
			m_spin1.SetRange( 0, 360, 1 );
			m_spin1.SetPos( nDegree );
		}
		if( m_spin2.Create(this, 600, 10, 100, 25) ){
			m_spin2.SetFont();
			m_spin2.SetRange( 10, 100, 1 );
			m_spin2.SetPos( 50 );
		}
		if( m_spin3.Create(this, 700, 10, 100, 25) ){
			m_spin3.SetFont();
			m_spin3.SetRange( 10, 100, 1 );
			m_spin3.SetPos( 50 );
		}

		Vec3x3		vecA( 10, 10 );
		matScale.SetScale( 1, 1  );
		matTrans.SetTranlsation( 300, 300 );
		matRot.SetRotation( nDegree );

		matInverseX.InverseX(); 
		matInverseY.InverseY();

		mat.Multi( matScale );
		mat.Multi( matTrans );
		mat.Multi( matRot	);

		Vec3x3	vecAns = mat.Multi( vecA );

		DBG::TRACE( matScale.ToString() );
		DBG::TRACE( matTrans.ToString() );
		DBG::TRACE( matRot.ToString()	);
		DBG::TRACE( mat.ToString()		);
		DBG::TRACE( vecAns.ToString()	);


		notify.Initialize( this );
		notify.Register( UDN_DELTAPOS, IDC_SPIN1, &CMainForm::OnDeltapos );
		notify.Register( UDN_DELTAPOS, IDC_SPIN2, &CMainForm::OnDeltapos );
		notify.Register( UDN_DELTAPOS, IDC_SPIN3, &CMainForm::OnDeltapos );

		return	TRUE ;
	}

	void	SetViewport( int x, int y, int w, int h ){
		rcViewport.Set( x, y, w, h );
		ptCenter = rcViewport.CenterPoint();

		matViewport.Identity();
		matViewport.SetTranlsation( ptCenter.x, ptCenter.y );

		matViewportInverse.Identity();
		matViewportInverse.SetTranlsation( -ptCenter.x, -ptCenter.y );

		DBG::TRACE(_T("Viewport(%d,%d,%d%d)"), rcViewport.left, rcViewport.top, rcViewport.right, rcViewport.bottom );
		DBG::TRACE(_T("Center(%d,%d)"), ptCenter.x, ptCenter.y );
	}

	void	OnPaint( HDC hDC ){
		CDC	dc(hDC);

		POINT	pt[3];
		Vec3x3	vertexs[3];

		vertexs[0].Set( 10, 10 );
		vertexs[1].Set( 90, 90 );
		vertexs[2].Set( 10, 90 );

		dc.DrawLine( vertexs[0].GetPoint(), vertexs[1].GetPoint() );
		dc.DrawLine( vertexs[1].GetPoint(), vertexs[2].GetPoint() );
		dc.DrawLine( vertexs[2].GetPoint(), vertexs[0].GetPoint() );

		matRot.SetRotation( nDegree );

		mat.Identity();
	//	mat.Multi( matViewportInverse	);
		mat.Multi( matRot				);
	//	mat.Multi( matInverseX			);
	//	mat.Multi( matViewport			);

		DBG::TRACE( _T("[BEFORE]\n%s"), (LPCTSTR)mat.ToString() );
		Mat3x3::MatrixAddition( 3, &mat.mat[0][0], &mat.mat[0][0], &matViewport.mat[0][0] );
	//	mat.SetTranlsation( ptCenter.x, ptCenter.y );
		DBG::TRACE( _T("[AFTER]\n%s"), (LPCTSTR)mat.ToString() );

		for( int i=0; i<3; i++ ){
			Vec3x3 v( mat.Multi( vertexs[ i ] ) );

		//	v = matViewport.Multi( v );

			pt[ i ].x	= v.GetPoint().x;
			pt[ i ].y	= v.GetPoint().y;
		}

		dc.DrawLine( pt[0], pt[1] );
		dc.DrawLine( pt[1], pt[2] );
		dc.DrawLine( pt[2], pt[0] );

		DBG::TRACE( _T("[%d, %d]"), pt[0].x, pt[0].y );
		DBG::TRACE( _T("[%d, %d]"), pt[1].x, pt[1].y );
		DBG::TRACE( _T("[%d, %d]"), pt[2].x, pt[2].y );

		dc.DrawRect( rcViewport );
		CPoint	ptA, ptB;

		Vec3x3	AxisX( rcViewport.Width(), 0 );

		ptA.x = rcViewport.top;
		ptA.y = ptCenter.y;

		ptB.x = rcViewport.bottom;
		ptB.y = ptCenter.y;

		dc.DrawLine(ptA, ptB);

		ptA.Set(10,10);
		ptB.Set(100,100);

		dc.DrawLine(ptA, ptB);
	}

	void	OnSize( UINT state, int cx, int cy ){
		CRect	rc;
		GetClientRect( &rc );
		SetViewport( rc.X()+100, rc.Y()+100, rc.Width()-200, rc.Height()-200 );
		Invalidate();
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
	if( Form.Create(_T("2DGraphics"),0,0,100,100,600,600) ){
		Form.ShowWindow( nCmdShow );
		Form.UpdateWindow();
		return Form.MessageLoop();
	}
	return 0;
}
