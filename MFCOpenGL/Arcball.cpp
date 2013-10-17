// Arcball.cpp 
#include <stdio.h>
#include <math.h>

#define		ZERO			(0.0)
#define		ONE				(1.0)

#define		SQUARE(x)		(x*x)
#define		CUBIC(x)		(x*x*x)

class CVector3 {
public:
	double			x, y, z;
	
	CVector3( void ) : x(ZERO), y(ZERO), z(ZERO) {
	}
	
	CVector3( double X, double Y, double Z ){
		Set(X,Y,Z);
	}
	
	void		Set( const CVector3& rThis ){
		x=rThis.x; 
		y=rThis.y; 
		z=rThis.z;
	}
	
	void		Set( double X, double Y, double Z ){
		x=X;
		y=Y;
		z=Z;
	}
		
	void		Normalize( void ){
	}
	
	double		Norm( void ){
		return(sqrt(x*x + y*y + z*z));
	}
	
	double		Dot( const CVector3& vec ){
		
	}
	
	CVector3&	Cross( const CVector3& vec ){
		return(*this);
	}
	
	
public:
	CVector3&		operator=( const CVector3& rThis ){
		if( this != &rThis ){
			Set( rThis );
		}
		return(*this);
	}
};



class CArcball {
public:
	CVector3			vecStartRot;
	CVector3			vecCurrentRot;
	double				dBallRadius;
	
private:
	CVector3			ConvertXY( int x, int y ){
		int		dDistance		= SQUARE(x) + SQUARE(x);
		double	dRadiusSquared	= SQUARE(dBallRadius);
		if( dDistance > dRadiusSquared )
		{
			return(CVector3(x,y,0));
		}	
		return(CVector3(x,y,sqrt(dRadiusSquared - dDistance)));
	}
};
