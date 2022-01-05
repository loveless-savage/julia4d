/* 4D vector math, customized for the 4D julia set
*/

#ifndef VECTOR4D_H
#define VECTOR4D_H

#include <stdio.h>
#include <math.h>
#include <complex.h>

class vec4d {
public:
	float x, y, z, w;
	vec4d ( float x, float y, float z, float w) : x(x),y(y),z(z),w(w) {}

	// change values of the vector
	void set(float xi, float yi, float zi, float wi);

	// change values of the vector- different implementation
	void set(vec4d blueprint);

	// spin a vector around 4 axes
	void spin(vec4d vr);

	// add other 4d vectors
	void add(vec4d addend);

	// scale
	void scale(float scalar);

	// scale, but return a transformed vector without editing the object itself
	vec4d nscale(float scalar);

	// along the delta vector given, pull the ray back until it intersects the origin cube of given size
	void calibrateRayToScope(vec4d delta, float cubeSize);

	// check if the vector is bounded by the hypercube of specified size surrounding the origin
	bool isBounded(float dim);

	// run julia test on vector, using Z = x+yi and C = z+wi
	bool trapped(int itnum);

	// find distance from another vector
	float distFrom(vec4d other);

	// dump coordinates to stream
	void dump();

private:
	// given two float values, treat them as a cartesian vector and rotate them by angi radians
	void rot(float& xi, float& yi, float angi);
};

#endif
