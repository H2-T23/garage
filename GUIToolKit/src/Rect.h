#pragma once

/****************************************************************************************
 *
 *
 *
 */
class CPoint : public RECT {
public:
	CPoint( void ){
		Set(0, 0);
	}

	CPoint( int x, int y ){
		Set(x, y);
	}

	CPoint( CPoint const& pt ){
		Set(pt.x, pt.y);
	}


	void	Set(int x, int y){
		this->x = x;
		this->y = y;
	}
};

/****************************************************************************************
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
		X(x);
		Y(y);
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

	int		SetRectEmpty( void ){
		Set(0,0,0,0);
	}
	
	bool	IsRectEmpty( void ) const {
		return(0 == (top + bottom + left + right));
	}

	bool	IsValidRect( void ) const {
		return( (0 < Width()) && (0 < Height()) );
	}
};
