#pragma once
// standard includes
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "vec4.h"
using namespace std;

class mat4 {
public:
	// constructor
	mat4 ();
   // destructor
   ~mat4();

	// easy access to members of private data
	float &operator [](int idx);

	// build rotation matrix w/ arbitrary input axes
	void buildRotor (int axisA, int axisB, float ang);

	// multiply by another matrix
	void mult(mat4 &rotor);

   // access columns of the matrix
   float* axis(int axisNum) const;
   void updateVectorAxes(float c_x = 1.0,float c_y = 1.0,float c_z = 1.0,float c_w = 1.0);
   vec4 x,y,z,w;

   // print data values for debugging
   void dump() const;
private:
	// data storage variable
	float data[16];
};
