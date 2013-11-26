#pragma once

#include "3DLib.h"

class C3DGeometry
{
public:
	C3DGeometry(void);
	~C3DGeometry(void);

	void	TransferScreen( Polar ViewPolar, CVector3D& rVec )
	{
		CMatrix4D	mtrx ;

		mtrx.mtrx[ 0 ][ 0 ]	= -1 * sin( ViewPolar.p ) ;
		mtrx.mtrx[ 0 ][ 1 ]	= cos( ViewPolar.p ) ;
		mtrx.mtrx[ 0 ][ 2 ]	= 0 ;

		mtrx.mtrx[ 1 ][ 0 ]	= -1 * sin( ViewPolar.q ) * cos( ViewPolar.p );
		mtrx.mtrx[ 1 ][ 1 ]	= -1 * sin( ViewPolar.q ) * sin( ViewPolar.p );
		mtrx.mtrx[ 1 ][ 2 ]	= cos( ViewPolar.q );

		mtrx.mtrx[ 2 ][ 0 ]	= -1 * cos( ViewPolar.q ) * cos( ViewPolar.p );
		mtrx.mtrx[ 2 ][ 1 ]	= -1 * cos( ViewPolar.q ) * sin( ViewPolar.p );
		mtrx.mtrx[ 2 ][ 2 ]	= -1 * sin( ViewPolar.q );

		TransferVertex( mtrx, ViewPolar, rVec ) ;
	}

	/***
		透視投影の投影変換を行う。
	*/
	void	TransferVertex( CMatrix4D const& rMtrx, Polar polar, CVector3D& rVec )
	{
		CVector3D	VecView ;
		PolarToVertex( polar, VecView ) ;		// 極座標を直行(デカルト)座標に変換 

		rVec -= VecView ;
		
		CVector3D	Ans = rMtrx * rVec ;
		Ans.z /= 300.0 ;						// 注視点を視点から遠ざける 

		rVec.x	= 300.0 + Ans.x / Ans.z ;
		rVec.y	= 250.0 + Ans.y / Ans.z ;
	}

	/***
		極座標を直行(デカルト)座標に変換 
	*/
	void	PolarToVertex( Polar polar, CVector3D& rVec )
	{
		rVec.x	= polar.r * cos( polar.q ) * cos( polar.p ) ;
		rVec.y	= polar.r * cos( polar.q ) * sin( polar.p ) ;
		rVec.z	= polar.r * sin( polar.q ) ;
	}
} ;
