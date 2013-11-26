#pragma once

class CVector3D
{
public:
	double		x,y,z,w ;

	void		GetString( CString& str )
	{
		str.Empty() ;
		str.Format( _T("x=%f, y=%f, z=%f, w=%f\n"), x,y,z,w ) ;
	}

//--------------------------------------------------------
	CVector3D()
	{
		Set( 1, 1, 1 ) ;
	}
	CVector3D( CVector3D const& rVec )
	{
		Copy( rVec ) ;
	}
	
	~CVector3D() {}

//--------------------------------------------------------
	void		Set( double X, double Y, double Z, double W = 1 )
	{
		x = X;
		y = Y;
		z = Z;
		w = W;
	}

//--------------------------------------------------------
	CVector3D const&	Normalize( CVector3D const& rVec )
	{
		CVector3D*	pVec	= new CVector3D ;

		double	dDiff_X		= x - rVec.x ;
		double	dDiff_Y		= y - rVec.y ;
		double	dDiff_Z		= z - rVec.z ;


		pVec->x	= dDiff_X / sqrt( pow(dDiff_X, 2) + pow(dDiff_Y, 2) ) ;
		pVec->y = dDiff_Y / sqrt( pow(dDiff_X, 2) + pow(dDiff_Y, 2) ) ;
		pVec->z	= dDiff_Z / sqrt( pow(dDiff_X, 2) + pow(dDiff_Y, 2) ) ;

		Copy( rVec ) ;
		return *this ;
	}

//--------------------------------------------------------
	CVector3D	operator+( CVector3D const& rVec ) const
	{
		CVector3D	Ans ;
		Ans.x	= x + rVec.x ;
		Ans.y	= y + rVec.y ;
		Ans.z	= z + rVec.z ;
		Ans.w	= w + rVec.w ;
		return Ans ;
	}

	CVector3D	operator-( CVector3D const& rVec ) const
	{
		CVector3D	Ans ;
		Ans.x	= x - rVec.x ;
		Ans.y	= y - rVec.y ;
		Ans.z	= z - rVec.z ;
		Ans.w	= w - rVec.w ;
		return Ans ;
	}

//--------------------------------------------------------
	CVector3D const&	operator=( CVector3D const& rVec )
	{
		Copy( rVec ) ;
		return *this ;
	}

	CVector3D const&	operator+=( CVector3D const& rVec )
	{
		x += rVec.x ;
		y += rVec.y ;
		z += rVec.z ;
		w += rVec.w ;
		return *this ;
	}

	CVector3D const&	operator-=( CVector3D const& rVec )
	{
		x -= rVec.x ;
		y -= rVec.y ;
		z -= rVec.z ;
		w -= rVec.w ;
		return *this ;
	}

protected:
	bool	Copy( CVector3D const& vec )
	{
		if( this != &vec )
		{
			x	= vec.x ;
			y	= vec.y ;
			z	= vec.z ;
			w	= vec.w ;
			return true ;
		}
		return false ;
	}
} ;