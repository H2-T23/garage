// Matrix3x3.cpp 
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "TString.h"

#define	SQUARE(x)		(x*x)
#define	CUBE(x)			(x*x*x)

/*****************************************************************************************
 *
 *
 *
 *
 */
class Vector3 {
public:
	double		x, y, z;

public:
	Vector3(void):x(0),y(0),z(0){}
	Vector3(double _x,double _y,double _z):x(_x),y(_y),z(_z){}

public:
	double	Length(void) const {
		return sqrt( SQUARE(x) + SQUARE(y) + SQUARE(z) );
	}

	void	Normalize( double thickness ){
		double len = Length();
		if( len ){
			Scale( thickness / 1.0 );
		}else{
			z = thickness;
		}		
	}

	void	Scale( double scale ){
		x *= scale;
		y *= scale;
		z *= scale;
	}
};

double		magnitude( const Vector3& v )
{
	return sqrt( SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z) );
}

Vector3		normalize( const Vector3& v )
{
	double mag = magnitude( v );
	Vector3	ans;
	ans.x	= v.x / mag;
	ans.y	= v.y / mag;
	ans.z	= v.z / mag;
	return ans;
}

Vector3		addition( const Vector3& v1, const Vector3 v2 )
{
	Vector3	ans;
	ans.x	= v1.x + v2.x;
	ans.y	= v1.y + v2.y;
	ans.z	= v1.z + v2.z;
	return ans;
}

Vector3		subtraction( const Vector3& v1, const Vector3& v2 )
{
	Vector3	ans;
	ans.x	= v1.x - v2.x;
	ans.y	= v1.y - v2.y;
	ans.z	= v1.z - v2.z;
	return ans;
}

double		dot( const Vector3& v1, const Vector3& v2 )
{
	return( v1.x * v2.x 
		+	v1.y * v2.y
		+	v1.z * v2.z	);
}

Vector3		cross( const Vector3& v1, const Vector3& v2 ) 
{
//	Vector3 ans(	v1.y * v2.z - v1.z * v2.y
//				,	v1.z * v2.x - v1.x * v2.z
//				,	v1.x * v2.y - v1.y * v2.x	);
	Vector3	ans;
	ans.x	= v1.y * v2.z - v1.z * v2.y;
	ans.y	= v1.z * v2.x - v1.x * v2.z;
	ans.z	= v1.x * v2.y - v1.y * v2.x;
	return ans;
}

Vector3	NormalVector( const Vector3& v1,  const Vector3& v2, const Vector3& v3 )
{
	Vector3 a = subtraction(v1,v2);
	Vector3 b = subtraction(v1,v3);
	
	Vector3 ans	= cross(a,b);
	
	
	return ans;
}





/*****************************************************************************************
	normal vetor
	2次元 ある線に垂直なベクトル
	3次元 ある面に垂直なベクトル

                     C
	    +------------+
	   /|           /|
	  / |          / |
	 /  |        D/  |
	+------------+  ------> N
	|   +--------|---+
	|  /         |  /B
	| /          | /
	|/           |/
	+------------+
                 A

	右手系の正規直交座標系において、
	直方体の１つの面の頂点をＡ，Ｂ，Ｃ，Ｄとすると、
	面ＡＢＣＤの法線ベクトルは、
	
		Ｎ＝ＡＢＸＡＤ
	
	となる。
	なお、Ｘはベクトルの外積である。
	
	線分ＡＢと線分ＤＣがｘ軸に平行で、
	かつ、線分ＡＤとＢＣがＺ軸に平行な場合、
	ノルムが線分ＡＤと線分ＢＣの長さの積となり、
	
		Ｎ＝ＡＢｘＡＤ＝｜ＡＢ｜｜ＡＤ｜ｊ
	
	となる。
	ここでｊはｙ軸方向の単位ベクトルである。
	
	なお、法線ベクトルのノルムを１としたものを
	単位法線ベクトル(normal unit vector)という。
	

	＋法線ベクトル
		＋	面法線ベクトル		面の向きを表す単位ベクトル
				
			３点の面１つに対して

				Ａ（ｘ1，ｙ1，ｚ1）
				Ｂ（ｘ2，ｙ2，ｚ2）
				Ｃ（ｘ3，ｙ3，ｚ3）

				Ｎ＝（Ｂ－Ａ）Ｘ（Ｃ－Ａ）

			 A, B, C の面法線ベクトル
			 normalize( cross((B - A), (C - A)) )
			「B - A」
				B.x - A.x 
				B.y - A.y 
				B.z - A.z 

			「C - A」
				B.x - A.x 
				B.y - A.y 
				B.z - A.z 

		＋	頂点法線ベクトル	面の頂点がどちらを向いているかを表す法線ベクトル


			面法線を元に頂点法線をもめる。
	
			①頂点法線が求める頂点を共有している面をリストアップする
			②リストアップした面を面法線を足し合わせて正規化する


	
	
 */



/*****************************************************************************************
 *
 *
 *
 *
 */
class Matrix3x3 {
public:
	double		mat[3][3];

public:
	Matrix3x3( void ){
		Identity();
	}

public:
	void	Identity( void ){
		memset(mat, 0, sizeof(mat));
		mat[ 0 ][ 0 ] = 1;
		mat[ 1 ][ 1 ] = 1;
		mat[ 2 ][ 2 ] = 1;
	}

	void	Copy( const Matrix3x3& m ){
		memcpy(mat, m.mat, sizeof(mat));
	}

	void	Set(	double m00, double m01, double m02
				,	double m10, double m11, double m12
				,	double m20, double m21, double m22	){
		mat[0][0]=m00;	mat[0][1]=m01;	mat[0][2]=m02;
		mat[1][0]=m10;	mat[1][1]=m11;	mat[1][2]=m12;
		mat[2][0]=m20;	mat[2][1]=m21;	mat[2][2]=m22;
	}

	void	Multi( const Matrix3x3& m ){
		Matrix3x3	ans;
		for( int i=0; i<3; i++ ){
			for( int j=0; j<3; j++ )
			{
				double sum = 0;
				for( int k=0; k<3; k++ ){
					sum += mat[ i ][ k ] * m.mat[ k ][ j ];
				}

				ans.mat[ i ][ j ] = sum;
			}
		}

		Copy( ans );
	}


	void		Print( void ) const {
		_tprintf(
			_T("|%3.3f %3.3f %3.3f|\n")
			_T("|%3.3f %3.3f %3.3f|\n")
			_T("|%3.3f %3.3f %3.3f|\n")
			,	mat[0][0], mat[0][1], mat[0][2]
			,	mat[1][0], mat[1][1], mat[1][2]
			,	mat[2][0], mat[2][1], mat[2][2]	);
	}

	TString		ToString( void ) const {
		TString str;
		str.format(
			_T("|%3.3f %3.3f %3.3f|\n")
			_T("|%3.3f %3.3f %3.3f|\n")
			_T("|%3.3f %3.3f %3.3f|\n")
			,	mat[0][0], mat[0][1], mat[0][2]
			,	mat[1][0], mat[1][1], mat[1][2]
			,	mat[2][0], mat[2][1], mat[2][2]	);
		return str;
	}
};

void	main( void )
{
	Matrix3x3	A, B;

	_tprintf( _T("%s"), A.ToString().c_str() );

	A.Multi( B );
	_tprintf( _T("%s"), A.ToString().c_str() );


	double* p = (double*)(A.mat);
	for( int i=0; i<(3*3); i++ ){
		_tprintf( _T("%3.3f "), *(p + i) );
	}
	_tprintf( _T("\n") );
	_tprintf( _T("%s"), A.ToString().c_str() );

	B.Set(1,2,3,4,5,6,7,8,9);
	A.Multi( B );
	_tprintf( _T("%s"), A.ToString().c_str() );

	A.Multi( B );
	_tprintf( _T("%s"), A.ToString().c_str() );
}
