#pragma once

class CRect : public RECT {
public:
	CRect( void ){
		Set(0,0,0,0);
	}

	CRect( int x, int y, int w, int h ){
		Set(x,y,w,h);
	}

	int		Set( int x, int y, int w, int h ){
		top		= x;
		left	= y;
		Width( w );
		Height( h );
	}

	int		Width( void )	{ return(right-left);	}
	int		Height( void )	{ return(bottom-top);	}

	int		Width( int w ){
		right = (left + w);
		return Width();
	}

	int		Height( int h ){
		bottom = (top + h);
		return Height();
	}
}: