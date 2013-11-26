// Rect.java 

/***********************************************************
 *
 *
 */
public class Rect {
	public int	top, left, bottom, right;

	public Rect(){
		Set( 0, 0, 0, 0 );
	}

	public Rect( int t, int l, int b, int r ){
		Set( t, l, b, r );
	}

	public Rect( Rect rc ){
		Set( rc );
	}

	public void	Set( int t, int l, int b, int r ){
		top = t; bottom = b; left = l; right = r;
	}

	public void	Set( Rect rc ){
		Set( rc.top, rc.left, rc.bottom, rc.right );
	}

	public int	Width(){
		return(Math.abs(right - left));
	}

	public int	Height(){
		return(Math.abs(bottom - top));
	}
}

