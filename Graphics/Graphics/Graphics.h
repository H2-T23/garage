#pragma once

#include <math.h>

namespace GraphicsMath1 
{
	class Vector3;

	Vector3		Mult( const Vector3& vec, double dScaler );
	Vector3		Div( const Vector3& vec, double dScaler );
	Vector3		Add( const Vector3& v1, const Vector3& v2 );
	Vector3		Sub( const Vector3& v1, const Vector3& v2 );

	float		Dot( const Vector3& v1, const Vector3& v2 );
	Vector3		Cross( const Vector3& v1, const Vector3& v2 );

	class Vector3 {
	public:
		double	x,y,z;

	public:
		Vector3( void ){
			Zero();
		}
		Vector3( double X, double Y, double Z ){
			Set(X,Y,Z);
		}
		Vector3( double vec[3] ){
			Set(vec);
		}
		Vector3( const Vector3& vec ){
			Set(vec);
		}

	public:
		void	Set( double X, double Y, double Z ){
			x=X; y=Y; z=Z;
		}
		void	Set( double vec[3] ){
			x=vec[0]; y=vec[1]; z=vec[2];
		}
		void	Set( const Vector3& vec ){
			x=vec.x; y=vec.y; z=vec.z;
		}

		Vector3&	Copy( const Vector3& vec ){
			Set(vec);
			return(*this);
		}

	public:
		void	Zero( void ){
			Set(0, 0, 0);
		}
		
	public:
		double	Length( void ) const{
			return ::sqrt( (x*x) + (y*y) + (z*z) );
		}

		double	Normalize( void ){
			const double	eps = 0.000001;
			double	len	= Length();
			if( len <= eps ){
				len	= 1.0;
			}

			double	xx	= x / len;
			double	yy	= y / len;
			double	zz	= z / len;

			if( fabs(x) < eps )
				xx	= 0.0;
			if( fabs(y) < eps )
				yy	= 0.0;
			if( fabs(z) < eps )
				zz	= 0.0;

			Set(xx, yy, zz);
		}

	public:
		Vector3&	operator=( const Vector3& vec ){
			return Copy(vec);
		}

	public:
		Vector3&	operator+=( const Vector3& vec ){
			*this = Add(*this, vec);
			return(*this);
		}

		Vector3&	operator-=( const Vector3& vec ){
			*this = Sub(*this, vec);
			return(*this);
		}

		Vector3&	operator*=( double dScaler ){
			*this = Mult(*this, dScaler);
			return(*this);
		}

		Vector3&	operator/=( double dScaler ){
			*this = Div(*this, dScaler);
			return(*this);
		}
	};

	Vector3		operator+( const Vector3& v1, const Vector3& v2 ){
		return Add(v1, v2);
	}

	Vector3		operator-( const Vector3& v1, const Vector3& v2 ){
		return Sub(v1, v2);
	}

	Vector3		operator*( const Vector3& vec, double dScaler ){
		return Mult(vec, dScaler);
	}

	Vector3		operator*( double dScaler, const Vector3& vec ){
		return Mult(vec, dScaler);
	}

	Vector3		operator/( const Vector3& vec, double dScaler ){
		return Div(vec, dScaler);
	}
	

	Vector3		Add( const Vector3& v1, const Vector3& v2 ){
		return Vector3(	(v1.x + v2.x)
					,	(v1.y + v2.y)
					,	(v1.z + v2.y)
					);
	}

	Vector3		Sub( const Vector3& v1, const Vector3& v2 ){
		return Vector3(	(v1.x - v2.x)
					,	(v1.y - v2.y)
					,	(v1.z - v2.y)
					);
	}

	Vector3		Mult( const Vector3& vec, double dScaler ){
		return Vector3(	(vec.x * dScaler)
					,	(vec.y * dScaler)
					,	(vec.z * dScaler)
					);
	}

	Vector3		Div( const Vector3& vec, double dScaler ){
		return Vector3(	(vec.x / dScaler)
					,	(vec.y / dScaler)
					,	(vec.z / dScaler)
					);
	}

	float		Dot( const Vector3& v1, const Vector3& v2 ){
		return((v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z));
	}	

	Vector3		Cross( const Vector3& v1, const Vector3& v2 ){
		return Vector3( (v1.y * v2.z - v1.z * v2.y)
					,	(v1.z * v2.x - v1.x * v2.z)
					,	(v1.x * v2.y - v1.y * v2.x)
					);
	}







	void	VectorTest( void ){
		Vector3	v1, v2, v3;

		v3	= v1 / 1;
	}


	class Matrix3 {
	};
}