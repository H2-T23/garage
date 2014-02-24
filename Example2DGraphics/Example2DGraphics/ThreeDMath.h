#pragma once

#define PI						(3.141592653589793)
#define	DegreeToRadian(deg)		((deg * PI)/180.0)
#define	RadianToDegree(rag)		((rag * 180.0)/PI)

#define	SQUARE(x)		(x*x)
#define	CUBE(x)			(x*x*x)


namespace ThreeDMath {

namespace Helper {
	void			MatrixAddition( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				int	nIdx	= ((i * nElements) + j);
				pdAns[ nIdx ]	= (pdA[ nIdx ] + pdB[ nIdx ]);
			}
		}
	}
	void			MatrixSubtraction( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				int	nIdx	= ((i * nElements) + j);
				pdAns[ nIdx ]	= (pdA[ nIdx ] - pdB[ nIdx ]);
			}
		}
	}
	void			MatrixCross( const int nElements, double* pdAns, double* pdA, double* pdB ){
		for( int i=0; i<nElements; i++ ){
			for( int j=0; j<nElements; j++ ){
				double sum	= 0;
				for( int k=0; k<nElements; k++ ){
					sum	+=	(	pdA[ ((i * nElements) + k) ] * pdB[ ((k * nElements) + j) ]	);
				}

				pdAns[ ((i * nElements) + j) ]	= sum;
			}
		}
	}
}

class Vector3 {
public:
	double				vec[ 3 ];		
	const double&		x, y, z;

	Vector3( void ) : x(&vec[0]), y(&vec[1]), z(&vec[2] {
	}
};

class Matrix3x3 {
	double				mtx[ 3 ][ 3 ];
};

}// End of ThreeDMath.
