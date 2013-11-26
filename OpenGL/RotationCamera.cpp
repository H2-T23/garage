// RotationCamera.cpp 

class Quaternion {
public:
	double x,y,z,w;
	Quaternion(void):x(0),y(0),z(0),w(1){}
};

Quaternion&		operator*(Quaternion& q1, Quaternion q1){
	Quaternion ans;
		
	ans.w	= q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;
	ans.x	= q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
	ans.y	= q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
	ans.z	= q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;

	return ans;
}

Quaternion		target;
Quaternion		currnt;


void	qtor( double* r, Quaternion q ){
	double	xx	= (q.x * q.x * 2);
	double	yy	= (q.y * q.y * 2);
	double	zz	= (q.z * q.z * 2);
	
	double	xy	= (q.x * q.y * 2);
	double	yz	= (q.y * q.z * 2);
	double	zx	= (q.z * q.x * 2);
	
	double	xw	= (q.x * q.w * 2);
	double	yw	= (q.y * q.w * 2);
	double	zw	= (q.z * q.w * 2);
	
	double	rot[] = {
		1.0 - yy - zz	, xy + zw		, zx - yw		, 0.0	,
		xy - zw			, 1.0 - zz - xx	, yz + xw		, 0.0	,
 		zx + yw			, yz - xw		, 1.0 - xx - yy	, 0.0	,
 		0.0				, 0.0			, 0.0			, 1.0	
	};
	
	for( int i=0; i<16; i++ ){
		matRot[ i ] = rot[ i ];
	}
}

void	Initailze( void ){
}

void	OnMouseMove( int x, int y ){
	double dx = (x - MouseX) * 1.333 / WIDTH;
	double dy = (y - MouseY) * 1.000 / HEIGHT;
	
	double len = sqrt( dx*dx + dy*dy );
	if( len ){
		double radian	= len * PI;
		double thita	= sin( radian ) / len;
		
		Quataion qutAfter = { cos(radian), dy*thta, dx*thta, 0.0 };
		
		target = qutAfter * current;
		qtor( rotate, tagete );
	}
}

void	OnMosueClikc(int state, int x, int y ){
	switch( state ){
	case DOWN:
		MouseX = x;
		MosueY = y;
		break;
		
	case UP:
		current = target;
		break;
		
	default:
		break;
	}
}
