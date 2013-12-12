#pragma once

/**********************************************************************************
 *
 *
 *
 */
class CRect : public RECT {
public:
	CRect( void ){
		Set(0,0,0,0);
	}

	CRect( int x, int y, int w, int h ){
		Set(x,y,w,h);
	}

	void	Set( int x, int y, int w, int h ){
		top		= y;
		left	= x;
		Width( w );
		Height( h );
	}

	int		X( int x ){
		left = x;
		return X();
	}

	int		Y( int y ){
		top = y;
		return Y();
	}

	int		Width( int w ){
		right = (left + w);
		return Width();
	}

	int		Height( int h ){
		bottom	= (top + h);
		return Height();
	}

	int		X( void ) const			{ return(left);			}
	int		Y( void ) const			{ return(top);			}
	int		Width( void ) const		{ return(right - left);	}
	int		Height( void ) const	{ return(bottom - top);	}

	int		SetEmpty( void ){
		Set(0,0,0,0);
	}
	
	bool	IsEmpty( void ) const {
		return(0 == (top + bottom + left + right));
	}

	bool	IsValidRect( void ) const {
		return( (0 < Width()) && (0 < Height()) );
	}
};
