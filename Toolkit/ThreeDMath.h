#pragma once

namespace ThreeDMath {
	const double	ZERO	= 0.0;
	const double	ONE		= 1.0;
	const double	PI		= 3.14159265359;

	// 度		= ラジアン x 180 / 円周率 
	inline double	DEGREE( double dRadius ){ return(dRadius * 180.0 / PI); }
	// ラジアン	= 度 x 円周率 / 180
	inline double	RADIUS( double dDegree ){ return(dDegree * PI / 180.0);	}
	/**********************************************************************************
	 */
	template<typename TYPE>
	inline TYPE		SQUARE(TYPE v){
		return(v*v);
	}
	/**********************************************************************************
	 */
	template<typename TYPE>
	inline TYPE		CUBE(TYPE v){
		return(v*v*v);
	}
	/**********************************************************************************
	 */
	class CVector3;
	class CMatrix3x3;
	class CMatrix4x4;
	class CQuaternion;
	/**********************************************************************************
	 */
	class CVector3 {
	public:
		double	x, y, z;

		CVector3( void ) : x(0), y(0), z(0) {}
		CVector3( double X, double Y, double Z ) : x(X), y(Y), z(Z) {}
		CVector3( const CVector3& vec ) : x(vec.x), y(vec.y), z(vec.z) {}

		void	Set( const CVector3& vec ){
			Set(vec.x, vec.y, vec.z);
		}
		
		inline void	Set( double x, double y, double z ){
			this->x = x;
			this->y = y;
			this->z = z;
		}

		void	Identity( void ){
			Set(1, 1, 1);
		}

		void	Zero( void ){
			Set(0, 0, 0);
		}

		double	Length( void ) const {
			return sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z));
		}

		void	Normalize( void ){
			const double EPS	= 0.000001;
			double	len	= Length();
			if( len <= EPS ){
				len	= ONE;
			}
			x /= len;	if(fabs(x) < EPS) x = ZERO;
			y /= len;	if(fabs(y) < EPS) y = ZERO;
			z /= len;	if(fabs(z) < EPS) z = ZERO;
		}

		double				Length( void ){
			return double(sqrt(SQUARE(x) + SQUARE(y) + SQUARE(z)));
		}

		double				Dot( const CVector3& vec ) const {
			return CVector3::Dot(*this, vec);
		}

		CVector3			Cross( const double dScaler ) {
			return CVector3::Cross(*this, dScaler);
		}

		CVector3			Cross( const CVector3& vec ) const {
			return CVector3::Cross(*this, vec);
		}

	public:
		static CVector3		Add( const CVector3& v1, const CVector3& v2 ){
			return CVector3(	v1.x + v2.x
							,	v1.y + v2.y
							,	v1.z + v2.z	);
		}
		static CVector3		Sub( const CVector3& v1, const CVector3& v2 ){
			return CVector3(	v1.x - v2.x
							,	v1.y - v2.y
							,	v1.z - v2.z	);
		}
		static double		Dot( const CVector3& v1, const CVector3& v2 ){
			return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
		}
		static CVector3		Cross( const CVector3& v1, double dScaler ){
			return CVector3(v1.x * dScaler, v1.y * dScaler, v1.z * dScaler);
		}
		static CVector3		Cross( const CVector3& v1, const CVector3& v2 ){
			return CVector3(	(v1.y * v2.z - v1.z * v2.y)
							,	(v1.z * v2.x - v1.x * v2.z)
							,	(v1.x * v2.y - v1.y * v2.x)	);
		}
	};
	CVector3	operator+(const CVector3& v1, const CVector3& v2){
		return CVector3::Add(v1, v2);
	}
	CVector3	operator-(const CVector3& v1, const CVector3& v2){
		return CVector3::Sub(v1, v2);
	}
	CVector3	operator*(const CVector3& v1, double dScaler ){
		return CVector3::Cross(v1, dScaler);
	}
	CVector3	operator*(double dScaler, const CVector3& v1 ){
		return CVector3::Cross(v1, dScaler);
	}
	CVector3	operator*(const CVector3& v1, const CVector3& v2){
		return CVector3::Cross(v1, v2);
	}
	/**********************************************************************************
	 */
	void	MatrixCross( const int elements, double* pMat, const double* pMatA, const double* pMatB ){
		for( int i=0; i<elements; i++ ){
			for( int j=0; j<elements; j++ )
			{
				*(pMat + i * elements + j)	= 0.0;

				int	idx, idxA, idxB;
				for( int k=0; k<elements; k++ )
				{
					idx		= (i * elements + j);
					idxA	= (i * elements + k);
					idxB	= (k * elements + j);

					TRACE(_T("%d, %d, %d"), idx, idxA, idxB );

					*(pMat + i * elements + j) += *(pMatA + i * elements + k) * *(pMatB + k * elements + j);
				}
			}
		}
	}
	/**********************************************************************************
	 */
	class CVector4 {
	public:
		double	x, y, z, w;
	};
	/**********************************************************************************
	 */
	class CMatrix3x3 {
	public:
		union {
			double	a[ 3 ][ 3 ];
			double	b[ 9 ];
		} mat;

		CMatrix3x3( void ){
			Identity();
		}

		CMatrix3x3(	double m00, double m01, double m02
				,	double m10, double m11, double m12
				,	double m20, double m21, double m22
				){
			Set( m00, m01, m02,
				 m10, m11, m12,
				 m20, m21, m22 );
		}

		void	Set(	double m00, double m01, double m02
					,	double m10, double m11, double m12
					,	double m20, double m21, double m22
					){
			mat.a[ 0 ][ 0 ]	= m00;	mat.a[ 0 ][ 1 ]	= m01;	mat.a[ 0 ][ 2 ]	= m02;
			mat.a[ 1 ][ 0 ]	= m10;	mat.a[ 1 ][ 1 ]	= m11;	mat.a[ 1 ][ 2 ]	= m12;
			mat.a[ 2 ][ 0 ]	= m20;	mat.a[ 2 ][ 1 ]	= m21;	mat.a[ 2 ][ 2 ]	= m22;
		}

		void	Zero( void ){
			Set( 0, 0, 0, 
				 0, 0, 0,
				 0, 0, 0 );
		}

		void	Identity( void ){
			Set( 1, 0, 0,
				 0, 1, 0, 
				 0, 0, 1 );
		}

		double&		at( int i ){
			return mat.b[ i ];
		}

		double&		at( int i, int j ){
			return mat.a[ i ][ j ];
		}

		void		Convert( const CQuaternion& qt );

	public:
		static CMatrix3x3	Cross( const CMatrix3x3& m1, const CMatrix3x3& m2 ){
			CMatrix3x3	ans;
			ans.Zero();

			MatrixCross(3, ans.mat.b, m1.mat.b, m2.mat.b);

			return ans;
		}
	};
	/**********************************************************************************
	 */
	class CMatrix4x4 {
	public:
		double		mat[ 16 ];

		CMatrix4x4( void ){
			Identity();
		}
		CMatrix4x4( const CMatrix4x4& vec ){
			Set( vec );
		}

		void	Set( const CMatrix4x4& vec ){
			memcpy( mat, vec.mat, sizeof(mat) );
		}

		void	Set(	double m00, double m01, double m02, double m03 
					,	double m10, double m11, double m12, double m13 
					,	double m20, double m21, double m22, double m23 
					,	double m30, double m31, double m32, double m33 
					){
			mat[  0 ]=m00;	mat[  1 ]=m01;	mat[  2 ]=m02;	mat[  3 ]=m03;
			mat[  4 ]=m10;	mat[  5 ]=m11;	mat[  6 ]=m12;	mat[  7 ]=m13;
			mat[  8 ]=m20;	mat[  9 ]=m21;	mat[ 10 ]=m22;	mat[ 11 ]=m23;
			mat[ 12 ]=m30;	mat[ 13 ]=m31;	mat[ 14 ]=m32;	mat[ 15 ]=m33;
		}

		void	Set( const double m[16] ){
			memcpy( mat, m, sizeof(mat) );
		}

		void	Set( const double m[4][4] ){
			memcpy( mat, m, sizeof(mat) );
		}

		void	Identity( void ){
			Set(	1, 0, 0, 0
				,	0, 1, 0, 0
				,	0, 0, 1, 0
				,	0, 0, 0, 1	);
		}

		void	Zero( void ){
			Set(	0, 0, 0, 0
				,	0, 0, 0, 0
				,	0, 0, 0, 0
				,	0, 0, 0, 0	);
		}

		double	Det( void ){
			return 0.0;
		}

		operator double*() { return(mat); }
	};
	/**********************************************************************************
	 */
	class CQuaternion {
	public:
		double			S;
		CVector3		V;

		CQuaternion( void ) : S(1.0), V(0.0,0.0,0.0) {}
		CQuaternion( double s, const CVector3& vec ) : S(s), V(vec) {}
		CQuaternion( double s, double x, double y, double z ) : S(s), V(x,y,z) {}
		CQuaternion( const CQuaternion& q ) : S(q.S), V(q.V) {}

		void	Set( const CQuaternion& qt ){
			Set( qt.S, qt.V );
		}
		void	Set( double s, const CVector3& vec ){
			Set( s, vec.x, vec.y, vec.z );
		}
		void	Set( double s, double x, double y, double z ){
			S = s;
			V.Set(x, y, z);
		}

		void	Zero( void ){
			Set( 0, 0, 0, 0 );
		}

		void	Identity( void ){
			Set( 1, 0, 0, 0 );
		}

		double	Norm( void ) const {
			return sqrt(SQUARE(S) + SQUARE(V.x) + SQUARE(V.y) + SQUARE(V.z));
		}

		void	Normalize( void ){
			double	norm	= Norm();
			CQuaternion	q = CQuaternion::Div(*this, norm);
			Set( q );
		}

		void	GetRotation( double m[][4] ){
			m[0][0] = 1.0 - 2.0 * (V.y * V.y + V.z * V.z);
			m[0][1] =       2.0 * (V.x * V.y - V.z * S);
			m[0][2] =       2.0 * (V.z * V.x + S * V.y);
			m[0][3] = 0.0;
			m[1][0] =       2.0 * (V.x * V.y + V.z * S);
			m[1][1] = 1.0 - 2.0 * (V.z * V.z + V.x * V.x);
			m[1][2] =       2.0 * (V.y * V.z - S * V.x);
			m[1][3] = 0.0;
			m[2][0] =       2.0 * (V.z * V.x - S * V.y);
			m[2][1] =       2.0 * (V.y * V.z + V.x * S);
			m[2][2] = 1.0 - 2.0 * (V.y * V.y + V.x * V.x);
			m[2][3] = 0.0;
			m[3][0] = 0.0;
			m[3][1] = 0.0;
			m[3][2] = 0.0;
			m[3][3] = 1.0;
		}

		void	GetRotation( CMatrix4x4& mtx ){
			mtx.mat[0] = 1.0 - 2.0 * (V.y * V.y + V.z * V.z);
			mtx.mat[1] =       2.0 * (V.x * V.y - V.z * S);
			mtx.mat[2] =       2.0 * (V.z * V.x + S * V.y);
			mtx.mat[3] = 0.0;
			mtx.mat[4] =       2.0 * (V.x * V.y + V.z * S);
			mtx.mat[5] = 1.0 - 2.0 * (V.z * V.z + V.x * V.x);
			mtx.mat[6] =       2.0 * (V.y * V.z - S * V.x);
			mtx.mat[7] = 0.0;
			mtx.mat[8] =       2.0 * (V.z * V.x - S * V.y);
			mtx.mat[9] =       2.0 * (V.y * V.z + V.x * S);
			mtx.mat[10] = 1.0 - 2.0 * (V.y * V.y + V.x * V.x);
			mtx.mat[11] = 0.0;
			mtx.mat[12] = 0.0;
			mtx.mat[13] = 0.0;
			mtx.mat[14] = 0.0;
			mtx.mat[15] = 1.0;
		}

	public:
		static CQuaternion	Add(const CQuaternion& q1, const CQuaternion& q2){
			return CQuaternion(	q1.S	+ q2.S
							,	q1.V.x	+ q2.V.x
							,	q1.V.y	+ q2.V.y
							,	q1.V.z	+ q2.V.z	);
		}

		static CQuaternion	Sub(const CQuaternion& q1, const CQuaternion& q2){
			return CQuaternion(	q1.S	- q2.S
							,	q1.V.x	- q2.V.x
							,	q1.V.y	- q2.V.y
							,	q1.V.z	- q2.V.z	);
		}

		static CQuaternion	Cross( const CQuaternion& q1, const double dScaler ){
			return CQuaternion(	q1.S	* dScaler
							,	q1.V.x	* dScaler
							,	q1.V.y	* dScaler
							,	q1.V.z	* dScaler	);
		}

		static CQuaternion	Cross( const double dScaler, const CQuaternion& q1 ){
			return CQuaternion::Cross(q1, dScaler);
		}

		static CQuaternion	Cross( const CQuaternion& q1, const CQuaternion& q2 ){
			return CQuaternion(	q1.S * q2.S - CVector3::Dot(q1.V, q2.V)
							,	q1.S * q2.V + q2.S * q1.V + CVector3::Cross(q1.V, q2.V)	);
		}

		static CQuaternion	Div( const CQuaternion& q1, const double dScaler ){
			return CQuaternion(	q1.S	/ dScaler
							,	q1.V.x	/ dScaler
							,	q1.V.y	/ dScaler
							,	q1.V.z	/ dScaler	);
		}
	};

	CQuaternion		operator+( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::Add(q1, q2);
	}

	CQuaternion		operator-( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::Sub(q1, q2);
	}

	CQuaternion		operator*( const CQuaternion& q1, const double dScaler ){
		return CQuaternion::Cross(q1, dScaler);
	}

	CQuaternion		operator*( const double dScaler, const CQuaternion& q1 ){
		return CQuaternion::Cross(dScaler, q1);
	}

	CQuaternion		operator*( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::Cross(q1, q2);
	}
	/**********************************************************************************
	 */
	void	CMatrix3x3::Convert( const CQuaternion& qt ){
		double	N	= SQUARE(qt.S) + SQUARE(qt.V.x) + SQUARE(qt.V.y) + SQUARE(qt.V.z);
		double	S	= ZERO;
		if( N > ZERO ){
			S	= 2.0 / N;
		}

		double	sx	= qt.V.x * S;
		double	sy	= qt.V.y * S;
		double	sz	= qt.V.z * S;

		double	xx	= qt.V.x * (qt.V.x * S);
		double	xy	= qt.V.x * (qt.V.y * S);
		double	xz	= qt.V.x * (qt.V.z * S);

		double	yx	= qt.V.y * (qt.V.x * S);
		double	yy	= qt.V.y * (qt.V.y * S);
		double	yz	= qt.V.y * (qt.V.z * S);

		double	zx	= qt.V.z * (qt.V.x * S);
		double	zy	= qt.V.z * (qt.V.y * S);
		double	zz	= qt.V.z * (qt.V.z * S);

	//	Set((1.0f - yy - zz),	(xy + sz),			(xz - sy),		
	//		(xy - sz),			(1.0f - xx - zz),	(yz + sx),		
	//		(xz + sy),			(yz - sx),			(1.0f - xx - yy));

		Set((1.0f - yy + zz),	(xy - sz),			(xz + sy),
			(xy + sz),			(1.0f - xx + zz),	(yz - sx),
			(xz - sy),			(yz + sx),			(1.0f - xx + yy));
	}
	/**********************************************************************************
	 */
	class CArcball {
	public:
		bool			m_bDrag;
		double			m_radius;
		CVector3		m_start, m_end;

		CArcball( void ) : m_bDrag(false) {
		}

		inline bool		IsDrag( void ) const {
			return m_bDrag;
		}

		CVector3		ConvertXY( int x, int y ){
			double	distance	= ThreeDMath::SQUARE(x) + ThreeDMath::SQUARE(y);
			if( distance > ThreeDMath::SQUARE(m_radius) )
			{
				return CVector3( x, y, 0 );
			}
			else
			{
				return CVector3( x, y, sqrt(ThreeDMath::SQUARE(m_radius) - distance) );
			}
		}

		void		SetBounds( int w, int h ){
		}

		void		Begin( int x, int y ){
		}

		void		End( void ){
		}

		void		Drag( int x, int y ){
			m_end	= ConvertXY( x, y );
			m_end.Normalize();
		}
	};
	/**********************************************************************************
	 */
	void	Test( void )
	{
		CVector3	a(1,0,0), b(0,1,0), c(0,0,0);
		c = a * b;

		CQuaternion	q1, q2, q3;
		q3	= q1 * q2;
	}
	/**********************************************************************************
	 */
}//End of ThreeDMath
