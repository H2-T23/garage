
function	RadianToDegree(radian){
	return(radian * 180.0/Math.PI);
}

function	DegreeToRadian(degree){
	return(degree * Math.PI/180.0);
}

function	Point(x,y){
	this.x = x;
	this.y = y;
};

Point.prototype.rot = function(degree){
	_x = this.x;
	_y = this.y;
	_r = DegreeToRadian(degree);

	this.x	= Math.ceil( _x * Math.cos(_r) - _y * Math.sin(_r) );
	this.y	= Math.ceil( _x * Math.sin(_r) + _y * Math.cos(_r) );
//	if( 0 > this.x ){
//		this.x *= -1;
//	}
//	if( 0 > this.y ){
//		this.y *= -1;
//	}
};


function	Matrix2D(){
	this.initialize.apply(this,arguments);
};

Matrix2D.prototype = {
	// m00 m01 m02 
	// m10 m11 m12 
	// m20 m21 m22 

	initialize : function(arg){
		var count = arguments.length;
		if( count == 0 ){
			this.m00 = 1; this.m01 = 0; this.m02 = 0;
			this.m10 = 0; this.m11 = 1; this.m12 = 0;
			this.m20 = 0; this.m21 = 0; this.m22 = 1; 
		}
	},

	set : function(mx){
		this.m00 = mx.m00;		this.m01 = mx.m01;		this.m02 = mx.m02;
		this.m10 = mx.m10;		this.m11 = mx.m11;		this.m12 = mx.m12;
		this.m20 = mx.m20;		this.m21 = mx.m21;		this.m22 = mx.m22; 
	},

	multi : function(mx){
		var tx,ty,tz;

		tx = this.m00; ty = this.m01; tz = this.m02;
		this.m00 = tx * mx.m00 + ty * mx.m10 + tz * mx.m20;
		this.m01 = tx * mx.m01 + ty * mx.m11 + tz * mx.m21;
		this.m02 = tx * mx.m02 + ty * mx.m12 + tz * mx.m22;

		tx = this.m10; ty = this.m11; tz = this.m12;
		this.m10 = tx * mx.m00 + ty * mx.m10 + tz * mx.m20;
		this.m11 = tx * mx.m01 + ty * mx.m11 + tz * mx.m21;
		this.m12 = tx * mx.m02 + ty * mx.m12 + tz * mx.m22;

		tx = this.m20; ty = this.m21; tz = this.m22;
		this.m20 = tx * mx.m00 + ty * mx.m10 + tz * mx.m20;
		this.m21 = tx * mx.m01 + ty * mx.m11 + tz * mx.m21;
		this.m22 = tx * mx.m02 + ty * mx.m12 + tz * mx.m22;

		return this;
	},

	rot : function(r){
		var mx = new Matrix2D();
		mx.m00	= Math.cos(r);
		mx.m01	= -Math.sin(r);
		mx.m10	= Math.sin(r);
		mx.m11	= Math.cos(r);
		return this.multi(mx);
	},

	skew : function(x,y){
		var mx = new Matrix2D();
		mx.m01	= Math.tan(x);
		mx.m10	= Math.tan(y);
		return this.multi(mx);
	},

	scale : function(x,y){
		var mx = new Matrix2D();
		mx.m00	= x;
		mx.m11	= y;
		return this.multi(mx);
	},

	translate : function(x,y){
		var mx = new Matrix2D();
		mx.m02	= x;
		mx.m12	= y;
		return this.multi(mx);
	},

	transformXY : function(x,y){
		return new Point(	((this.m00 * x) + (this.m01 * y) + this.m02)	,
							((this.m10 * x) + (this.m11 * y) + this.m12)	);
	},

	transform : function(p){
		return new Point(	((this.m00 * p.x) + (this.m01 * p.y) + this.m02)	,
							((this.m10 * p.x) + (this.m11 * p.y) + this.m12)	);
	},
};
