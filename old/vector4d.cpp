/* implementation of 4D vector math
*/

#include <vector4d.h>

// change values of the vector
void vec4d :: set (float xi, float yi, float zi, float wi) {
	this->x = xi;
	this->y = yi;
	this->z = zi;
	this->w = wi;
}

// change values of the vector- different implementation
void vec4d :: set (vec4d blueprint) {
	this->x = blueprint.x;
	this->y = blueprint.y;
	this->z = blueprint.z;
	this->w = blueprint.w;
}

// spin a vector around 4 axes
void vec4d :: spin (vec4d vr) {
	// these two do not affect each other
	rot(this->x,this->z, vr.x);
	rot(this->y,this->w, vr.y);
	// these two must happen after the previous two for the rotation to behave correctly
	rot(this->y,this->z, vr.z);
	rot(this->x,this->w, vr.w);
}

// add another 4d vector
void vec4d :: add (vec4d addend) {
	this->x += addend.x;
	this->y += addend.y;
	this->z += addend.z;
	this->w += addend.w;
}

// scale
void vec4d :: scale (float scalar) {
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;
	this->w *= scalar;
}

// scale, but return a transformed vector without editing the object itself
vec4d vec4d :: nscale (float scalar) {
	vec4d temp (
		this->x * scalar,
		this->y * scalar,
		this->z * scalar,
		this->w * scalar);
	return temp;
}

// along the delta vector given, push the ray forward until it intersects the origin cube of given size
void vec4d :: calibrateRayToScope (vec4d delta, float cubeSize) {

	// condense this ray and the delta ray into arrays, for loop usage
	float a_this[4] = {this->x,this->y,this->z,this->w};
	float a_delta[4] = {delta.x,delta.y,delta.z,delta.w};

	float scalar=0;
	float temp=0;

	// for each axis...
	for(char i=0;i<4;i++){
		// skip axis if already bounded by it
		if( fabsf(a_this[i]) < cubeSize ) continue;
		// find distance of camera coordinate to scope box
		temp = copysignf(cubeSize,a_this[i]);
		temp -= a_this[i];
		// find scalar needed to get this distance from the delta ray
		temp /= a_delta[i];
		// find maximum of these scalars
		if(scalar < temp) scalar = temp;
		//printf("\t%f",temp);
	}

	// using max scalar we calculated, translate this ray so we are behind all nearest bounding planes
	this->add( delta.nscale(scalar) );
}

// check if the vector is bounded by the hypercube of specified size surrounding the origin
bool vec4d :: isBounded (float dim) {
	if( fmaxf(fmaxf(this->x,this->y) , fmaxf(this->z,this->w)) < dim ){
		return true;
	}
	return false;
}

// run julia test on vector, using Z = x+yi and C = z+wi
bool vec4d :: trapped (int itnum) {
	// Z = x+yi
	float complex Z = this->x + this->y *I;
	//float za = this->x;
	//float zb = this->y;
	// C = z+wi
	float complex C = this->z + this->w *I;
	//float ca = this->z;
	//float cb = this->w;

	// loop to test for set containment
	for(int it=0;it<itnum;it++){
		Z*=Z;
		Z+=C;
		//za_temp = za*za - zb*zb + ca;
		//zb = 2*za*zb + cb;
		//za = za_temp;
		if( cabsf(Z) > 2.0 ) return false;
		//if( __builtin_hypotf(za,zb) > 2.0 ) return false;
	}

	return true;
}

// find distance from another vector
float vec4d :: distFrom (vec4d other) {
	float squaresum = (this->x - other.x)*(this->x - other.x);
	squaresum += (this->y - other.y)*(this->y - other.y);
	squaresum += (this->z - other.z)*(this->z - other.z);
	squaresum += (this->w - other.w)*(this->w - other.w);
	return sqrtf(squaresum);
}

// dump coordinates to stream
void vec4d :: dump() {
	printf("\t%f,%f,%f,%f",this->x,this->y,this->z,this->w);
};

 
// given two float values, treat them as a cartesian vector and rotate them by angi radians
void vec4d :: rot (float& xi, float& yi, float angi) {
	// find vector heading
	double ang = (double)atan2f(yi, xi);
	// increase vector heading
	ang += (double)angi;
	// vector magnitude
	float len = yi * yi + xi * xi;
	len = sqrtf(len);
	// convert from polar back to cartesian
	xi = len * (float)cos(ang);
	yi = len * (float)sin(ang);
	return;
}
