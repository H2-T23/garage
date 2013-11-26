#pragma once

class CMatrix4D
{
public:
	double		mtrx[ 4 ][ 4 ];

// デバッグ用コード 
	void		GetString( CString& str, int nType = 0 ) const
	{
		str.Empty() ;

		str.Format(
			_T("-                                               -\n")
			_T("| [0][0]=%f,  [0][1]=%f,  [0][2]=%f,  [0][3]=%f |\n")
			_T("| [1][0]=%f,  [1][1]=%f,  [1][2]=%f,  [1][3]=%f |\n") 
			_T("| [2][0]=%f,  [2][1]=%f,  [2][2]=%f,  [2][3]=%f |\n") 
			_T("| [3][0]=%f,  [3][1]=%f,  [3][2]=%f,  [3][3]=%f |\n")
			_T("-                                               -\n"),
			mtrx[0][0],  mtrx[0][1],  mtrx[0][2],  mtrx[0][3],
			mtrx[1][0],  mtrx[1][1],  mtrx[1][2],  mtrx[1][3], 
			mtrx[2][0],  mtrx[2][1],  mtrx[2][2],  mtrx[2][3], 
			mtrx[3][0],  mtrx[3][1],  mtrx[3][2],  mtrx[3][3]);
	}

// コンストラクタ 
	CMatrix4D()
	{
		Identity();
	}

	CMatrix4D( double dValue )
	{
		Set( dValue ) ;
	}

	CMatrix4D(	double A, double B, double C, double D,
				double E, double F, double G, double H,
				double I, double J, double K, double L,
				double M, double N, double O, double P	)
	{
		Set( A,B,C,D, E,F,G,H, I,J,K,L, M,N,O,P );
	}

	CMatrix4D( CMatrix4D const& mtr )
	{
		Copy( mtr ) ;
	}

// 
	void		Set( double dValue )
	{
		memset( (double*)mtrx, dValue, sizeof(double)*4*4 ) ;
	}

	void		Set(	double A, double B, double C, double D,
						double E, double F, double G, double H,
						double I, double J, double K, double L,
						double M, double N, double O, double P	)
	{
		mtrx[0][0]	= A;  mtrx[0][1] = B;  mtrx[0][2] = C;  mtrx[0][3] = D;
		mtrx[1][0]	= E;  mtrx[1][1] = F;  mtrx[1][2] = G;  mtrx[1][3] = H;
		mtrx[2][0]	= I;  mtrx[2][1] = J;  mtrx[2][2] = K;  mtrx[2][3] = L;
		mtrx[3][0]	= M;  mtrx[3][1] = N;  mtrx[3][2] = O;  mtrx[3][3] = P;
	}

	void		Zero()
	{
		memset( (double*)mtrx, 0x0, sizeof(double)*4*4 ) ;
	}

	// 単位行列 
	void		Identity()
	{
		Zero() ;

		mtrx[0][0] = 1 ;
		mtrx[1][1] = 1 ;
		mtrx[2][2] = 1 ;
		mtrx[3][3] = 1 ;
	}

	// 転置 
	void		Transpose()
	{
		CMatrix4D	Clone( *this ) ;

		for( int i=0; i<4; i++ )
			for( int j=0; j<4; j++ )
				mtrx[ i ][ j ]	= Clone.mtrx[ j ][ i ];
	}

// Operator 
public:
	// 積(CVector3D)
	CVector3D	operator*( CVector3D const& vec ) const 
	{
		CVector3D	AnsVec3D ;

		AnsVec3D.x	= (mtrx[0][0] * vec.x)
					+ (mtrx[0][1] * vec.y) 
					+ (mtrx[0][2] * vec.z) 
					+ (mtrx[0][3] * vec.w);

		AnsVec3D.y	= (mtrx[1][0] * vec.x)
					+ (mtrx[1][1] * vec.y) 
					+ (mtrx[1][2] * vec.z) 
					+ (mtrx[1][3] * vec.w);

		AnsVec3D.z	= (mtrx[2][0] * vec.x)
					+ (mtrx[2][1] * vec.y) 
					+ (mtrx[2][2] * vec.z) 
					+ (mtrx[2][3] * vec.w);

		AnsVec3D.w	= (mtrx[3][0] * vec.x)
					+ (mtrx[3][1] * vec.y) 
					+ (mtrx[3][2] * vec.z) 
					+ (mtrx[3][3] * vec.w);

		return AnsVec3D ;
	}

	// スカラー倍 
	CMatrix4D		operator*( double dScalar ) const
	{
		CMatrix4D	Ans ;
		Ans.Zero();

		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				Ans.mtrx[ i ][ j ]	= mtrx[ i ][ j ] * dScalar ;
			}
		}
		return Ans ;
	}

	// 積(CMatrix4D)
	CMatrix4D		operator*( CMatrix4D const& rMtrx ) const 
	{
		CMatrix4D	Ans ;
		Ans.Zero();

		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				for( int k=0; k<4; k++ ){
					Ans.mtrx[ i ][ j ] += (mtrx[ i ][ k ] * rMtrx.mtrx[ k ][ j ]) ;
				}
			}
		}

		return Ans ;
	}
	
	// 和 
	CMatrix4D		operator+( CMatrix4D const& rMtrx ) const
	{
		CMatrix4D	Ans ;
		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				Ans.mtrx[ i ][ j ]	= mtrx[ i ][ j ] + rMtrx.mtrx[ i ][ j ] ;
			}
		}

		return Ans ;
	}

	// 差 
	CMatrix4D		operator-( CMatrix4D const& rMtrx ) const
	{
		CMatrix4D	Ans ;
		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				Ans.mtrx[ i ][ j ]	= mtrx[ i ][ j ] - rMtrx.mtrx[ i ][ j ] ;
			}
		}

		return Ans ;
	}

//--------------------------------------------------------
	// 代入 
	CMatrix4D const&	operator=( CMatrix4D const& rMtrx )
	{
		Copy( rMtrx ) ;
		return *this ;
	}

	// 和 
	CMatrix4D const&	operator+=( CMatrix4D const& rMtrx )
	{
		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				mtrx[ i ][ j ]	+= rMtrx.mtrx[ i ][ j ] ;
			}
		}

		return *this ;
	}

	// 差 
	CMatrix4D const&	operator-=( CMatrix4D const& rMtrx )
	{
		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				mtrx[ i ][ j ]	-= rMtrx.mtrx[ i ][ j ] ;
			}
		}

		return *this ;
	}

	// スカラー倍 
	CMatrix4D const&	operator*( double dScalar )
	{
		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				mtrx[ i ][ j ]	*= dScalar ;
			}
		}
		return *this;
	}

	// 積(CMatrix4D)
	CMatrix4D const&	operator*=( CMatrix4D const& rMtrx )
	{
		CMatrix4D	Ans ;
		Ans.Zero();

		for( int i=0; i<4; i++ ) {
			for( int j=0; j<4; j++ ) {
				for( int k=0; k<4; k++ ){
					Ans.mtrx[ i ][ j ] += (mtrx[ i ][ k ] * rMtrx.mtrx[ k ][ j ]) ;
				}
			}
		}

		Copy( Ans ) ;
		return *this;
	}

protected:
	bool	Copy( CMatrix4D const& mtr )
	{
		if( this != &mtr )
		{
		#if 1
			memcpy( (double*)mtrx, (double*)mtr.mtrx, sizeof(double)*4*4 ) ;
		#else
			for( int i=0; i<4; i++ )
				for( int j=0; j<4; j++ )
					mtrx[ i ][ j ] = mtr.mtrx[ i ][ j ];
		#endif
			return true;
		}
		return false;
	}

} ;
