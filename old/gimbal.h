#pragma once
#include "mat4.h"

class Gimbal {
public:
	// constructor
	Gimbal ();
	// destructor
	~Gimbal ();

	// rotate
	void rotate(int axisA, int axisB, float ang);

	// give various vectors to the Camera class for rendering
	// TODO

	// print data values for debugging
	void dump();

	// basis vectors for rotation
	vec4 *x,*y,*z,*w;
private:
	// rotation coordinates
	mat4 *pos, *rtemp;
	// additional camera positioning values
	float distFromOrigin, fovSphereRadius;
};
