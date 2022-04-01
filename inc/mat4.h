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
	void mult(mat4 rotor);

	// print data values for debugging
	void dump() const;

   // access columns of the matrix with vecShell handles
   vec4 axis(int axisNum) const;
private:
	// data storage variable
	float data[16];
};
