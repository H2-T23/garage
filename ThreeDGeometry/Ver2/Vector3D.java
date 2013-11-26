// Vector3D.java 

public class Vector3D {
	public float		x, y, z;

	public Vector3D(){
		this( 0.0f, 0.0f, 0.0f );
	}

	public Vector3D( Vector3D vec ){
		this( vec.x, vec.y, vec.z );
	}

	public Vector3D( float x, float y, float z ){
		this.set( x, y, z );	
	}

	public void		set( float x, float y, float z ){
		this.x	= x;
		this.y	= y;
		this.z	= z;
	}
}
