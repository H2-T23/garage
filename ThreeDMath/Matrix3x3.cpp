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
	double		x,y,z;
	Vector3		N;

public:
	Vector3(void):x(0),y(0),z(0){}

public:
	double	Length(void) const {
		return sqrt( SQUARE(x) + SQUARE(y) + SQUARE(z) );
	}

	void	Normalize( double thickness ){
		len = Length();
		if( len ){
			Scale( thickness / 1.0 );
		}else{
			N.z = thickness;
		}		
	}

	void	Scale( double scale ){
		N.x *= scale;
		N.y *= scale;
		N.z *= scale;
	}
};


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
