#pragma once

namespace ThreeDMath {
	const double	ZERO	= 0.0;
	const double	ONE		= 1.0;
	const double	PI		= 3.14159265359;

	// 度		= ラジアン x 180 / 円周率 
	inline double	DEGREE( double dRadius ){ return(dRadius * 180.0 / PI); }
	// ラジアン	= 度 x 円周率 / 180
	inline double	RADIUS( double dDegree ){ return(dDegree * PI / 180.0);	}

	template<typename TYPE>
	inline TYPE		SQUARE(TYPE v){ return(v*v); }

	template<typename TYPE>
	inline TYPE		CUBE(TYPE v){ return(v*v*v); }

	class CVector3;
	class CVector4;
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
		
		void	Set( double x, double y, double z ){
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

	public:
		static CVector3		add( const CVector3& v1, const CVector3& v2 ){
			return CVector3(	v1.x + v2.x
							,	v1.y + v2.y
							,	v1.z + v2.z	);
		}
		static CVector3		sub( const CVector3& v1, const CVector3& v2 ){
			return CVector3(	v1.x - v2.x
							,	v1.y - v2.y
							,	v1.z - v2.z	);
		}
		static double		dot( const CVector3& v1, const CVector3& v2 ){
			return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
		}
		static CVector3		cross( const CVector3& v1, double dScaler ){
			return CVector3(v1.x * dScaler, v1.y * dScaler, v1.z * dScaler);
		}
		static CVector3		cross( const CVector3& v1, const CVector3& v2 ){
			CVector3	v;
			v.x	= (v1.y * v2.z - v1.z * v2.y);
			v.y	= (v1.z * v2.x - v1.x * v2.z);
			v.z	= (v1.x * v2.y - v1.y * v2.x);
			return v;
		}
	};
	
	CVector3	operator+(const CVector3& v1, const CVector3& v2){
		return CVector3::add(v1, v2);
	}

	CVector3	operator-(const CVector3& v1, const CVector3& v2){
		return CVector3::sub(v1, v2);
	}

	CVector3	operator*(const CVector3& v1, double dScaler ){
		return CVector3::cross(v1, dScaler);
	}

	CVector3	operator*(double dScaler, const CVector3& v1 ){
		return CVector3::cross(v1, dScaler);
	}

	CVector3	operator*(const CVector3& v1, const CVector3& v2){
		return CVector3::cross(v1, v2);
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
		double	mat[ 9 ];
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
	};
	/**********************************************************************************
	 */
	class CQuaternion {
	public:
		double			S;
		CVector3		V;

		CQuaternion( void ) : S(1), V(0,0,0) {}
		CQuaternion( double s, double x, double y, double z ) {
			Set(s,x,y,z);
		}
		CQuaternion( double s, const CVector3& vec ){
			Set( s, vec );
		}
		CQuaternion( const CQuaternion& qt ){
			Set( qt );
		}

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

	public:
		static CQuaternion		add( const CQuaternion& q1, const CQuaternion& q2 ){
			return CQuaternion(q1.S + q2.S, q1.V + q2.V);
		}
		static CQuaternion		sub( const CQuaternion& q1, const CQuaternion& q2 ){
			return CQuaternion(q1.S - q2.S, q1.V - q2.V);
		}
		static CQuaternion		cross( const CQuaternion& q1, const CQuaternion& q2 ){
			return CQuaternion( q1.S * q2.S - CVector3::dot(q1.V, q2.V)	//２項目はベクトル内積
							,	q1.S * q2.V + q2.S * q1.V + CVector3::cross(q1.V, q2.V)	); //３項目はベクトル外積
		}
	};

	CQuaternion		operator+( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::add(q1, q2);
	}
	CQuaternion		operator-( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::sub(q1, q2);
	}
	CQuaternion		operator*( const CQuaternion& q1, const CQuaternion& q2 ){
		return CQuaternion::cross(q1, q2);
	}

}//End of ThreeDMath
