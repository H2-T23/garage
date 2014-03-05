#pragma once

/****************************************************************************************
 *
 *
 *
 */
class CPoint : public POINT {
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

	void		Set( CPoint const& pt ){
		this->x = pt.x;
		this->y = pt.y;
	}

	CPoint&	operator=( CPoint const& pt ){
		x = pt.x;
		y = pt.y;
		return *this;
	}

	CPoint&	operator+( CPoint const& pt ){
		this->x += pt.x;
		this->y += pt.y;
		return *this;
	}

	CPoint&	operator-( CPoint const& pt ){
		this->x -= pt.x;
		this->y -= pt.y;
		return *this;
	}

	CPoint&	operator*( CPoint const& pt ){
		this->x *= pt.x;
		this->y *= pt.y;
		return *this;
	}

	CPoint&	operator/( CPoint const& pt ){
		this->x /= pt.x;
		this->y /= pt.y;
		return *this;
	}

	CPoint&	operator%( CPoint const& pt ){
		this->x %= pt.x;
		this->y %= pt.y;
		return *this;
	}
};

CPoint		operator-( CPoint const& ptA, CPoint const& ptB ) {
	CPoint	pt;
	pt.x	= (ptA.x - ptB.x);
	pt.y	= (ptA.y - ptB.y);
	return pt;
}

/****************************************************************************************
 *
 *
 *
 */
class CSize : public SIZE {
public:
	CSize( void ){
		Set(0, 0);
	}

	CSize( int cx, int cy ){
		Set(cx, cy);
	}

	CSize( CSize const& sz ){
		Set(sz.cx, sz.cy);
	}

	void	Set( int cx, int cy ){
		this->cx = cx;
		this->cy = cy;
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

	void	Set( RECT& rc ){
		top		= rc.top;
		left	= rc.left;
		bottom	= rc.bottom;
		right	= rc.right;
	}

	void	Set( int x, int y, int w, int h ){
		X( x );
		Y( y );
		Width( w );
		Height( h );
	}

	void	Set( POINT& pt, SIZE& sz ){
		X( pt.x );
		Y( pt.y );
		Width( sz.cx );
		Height( sz.cy );
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

	void	SetRectEmpty( void ){
		Set(0,0,0,0);
	}
	
	bool	IsRectEmpty( void ) const {
		return(0 == (top + bottom + left + right));
	}

	bool	IsValidRect( void ) const {
		return( (0 < Width()) && (0 < Height()) );
	}

	bool	PtInRect( CPoint const& pt ) const {
		return(::PtInRect(this, pt) == FALSE ? false : true);
	}

	CPoint	CenterPoint( void ) const {
		CPoint pt;
		pt.x	= (left + Width()/2);
		pt.y	= (top  + Height()/2);
		return pt;
	}

	CPoint	TopLeft( void ) const {
		CPoint pt(left, top);
		return pt;
	}

	CPoint	BottomRight( void ) const {
		CPoint pt(right, bottom);
		return pt;
	}
};
