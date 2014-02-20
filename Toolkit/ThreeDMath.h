#pragma once

namespace ThreeDMath {

	/**********************************************************************************
	 */
	class CVector3 {
	public:
		double	x, y, z;

		CVector3( void ) : x(0), y(0), z(0) {}

		void	Set( double x, double y, double z ){
			this->x = x;
			this->y = y;
			this->z = z;
		}

		static double		dot( const CVector3& v1, const CVector3& v2 ){
			return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
		}

		static CVector3		cross( const CVector3& v1, const CVector3& v2 ){
			CVector3	v;
			v.x	= (v1.y * v2.z - v1.z * v2.y);
			v.y	= (v1.z * v2.x - v1.x * v2.z);
			v.z	= (v1.x * v2.y - v1.y * v2.x);
			return v;
		}
	};

	CVector3	operator*(const CVector3& v1, const CVector3& v2){
		return CVector3::cross(v1, v2);
	}

	/**********************************************************************************
	 */
	class CMatrix4x4 {
	public:
		double		mat[ 16 ];

		CMatrix4x4( void ){
			Identity();
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
	};

	/**********************************************************************************
	 */
	class CQuaternion {
	public:
		double		S, x, y, z;
		CVector3	V;

		CQuaternion( void ) : S(1), x(0), y(0), z(0) {}
		CQuaternion( double s, double x, double y, double z ) {
			Set(s,x,y,z);
		}

		void	Set( double s, double x, double y, double z ){
			this->S = s;
			V.Set(x, y, z);
		}
	};

	CQuaternion		operator*( const CQuaternion& qt1, const CQuaternion& qt2 ){
		CQuaternion	qt;
		qt.S	= (qt1.S - qt2.S) - (qt1.V * qt2.V);
		qt.V	= (qt1.S - qt2.V + qt2.S * qt1.V + ();
		return qt;
	}


}//End of ThreeDMath
