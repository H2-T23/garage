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
		top		= x;
		left	= y;
		Width( w );
		Height( h );
	}

	int		Width( int w ){
		right = (left + w);
		return Width();
	}

	int		Height( int h ){
		bottom	= (top + h);
		return Height();
	}

	int		X( void ) const			{ return(top);			}
	int		Y( void ) const			{ return(left);			}
	int		Width( void ) const		{ return(right - left);	}
	int		Height( void ) const	{ return(bottom - top);	}
};
