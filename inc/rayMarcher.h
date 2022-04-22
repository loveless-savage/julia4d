#pragma once
#include "mat4.h"


class RayMarcher {
public:
	// constructor
	RayMarcher();

	enum renderType {
		set,
		distance,
      dstep
	};

	// render options
	void renderOptions(int iterMaxi, renderType rtypei);

	// set up seed vectors
	void orient(const vec4& rootZ, const vec4& rootX, const vec4& rootY, float distFromOrigin, float fovSphereRadiusIn);

   // cast ray associated with given screen coordinates
   float castRay(float pX, float pY);


private:
	// seed vectors for rendering:
	vec4 origin, step0, stepX, stepY; // these seed vectors generate different step vectors between different pixels
   vec4 step; // // this vector describes the direction of each ray; changed once per pixel
	int iterMax; // what's the cutoff for recursing the julia function?

   // ray casting variables
   vec4 pos; // current location of ray; coordinates are [ z0a, z0b, ca, cb ]
   vec4 dpos; // derivative of Z_n with respect to above variables ^
   float distSum; // how far do we march the ray this time?
   float fovSphereRadius; // how far from the origin do we say we've passed the fractal?

   // julia test variables- stored outside jtest() so they can be accessed asynchronously
   float za,zb; // iteration value- real & imaginary
   float z_modulus; // complex length of iteration value

   // while casting a ray, use current ray coordinates & run a julia test
   float jtest(vec4 posIn);

	renderType rtype; // which value are we rendering? FOR TESTING

};
