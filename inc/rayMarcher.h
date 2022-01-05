#pragma once
#include "mat4.h"


class RayMarcher {
public:
	// constructor
	RayMarcher();

	enum renderType {
		set,
		dz,
		dc,
		distance
	};

	// render options
	void renderOptions(int iterMaxi, renderType rtypei);

	// set up seed vectors
	void orient(vec4 *rootZ, vec4 *rootX, vec4 *rootY, float distFromOrigin, float pxStepLength);

	// test given pixel coordinates
	float test(float pX, float pY);

	// potential rendering scalars are public, as to be read from asynchronously
	float n=0;
	float z0a,z0b,ca,cb;
	float za,zb;
	vec4 step; // current location of ray; coordinates are [ z0a, z0b, ca, cb ]
	vec4 dz_step; // derivative of Z_n with respect to above variables ^

private:
	// seed vectors for rendering:
	vec4 origin, step0, stepX, stepY; // these seed vectors generate different step vectors between different pixels
	int iterMax; // what's the cutoff for recursing the julia function?
	renderType rtype;
};
