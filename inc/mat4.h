#pragma once
// standard includes
//#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

class mat4 {
public:
	// constructor
	mat4 ();
	
	// easy access to members of private data
	float &operator [](int idx);

	// build rotation matrix w/ arbitrary input axes
	void buildRotor (int axisA, int axisB, float ang);

	// multiply by another matrix
	void mult(mat4 rotor);

	// print data values for debugging
	void dump();

private:  
	// data storage variable
	float data[16];
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

class vec4 {
public:
	// constructor
	vec4 ();
	// constructor w/ ability to point it to already existing data values
	vec4 (float &data);
	// destructor
	~vec4 ();
	
	// operator overloading
	float& operator [](int idx);
	void operator = (vec4 target);
	vec4 operator + (vec4 target);
	vec4 operator - (vec4 target);
	vec4 operator * (float scalar);
	vec4 operator / (float scalar);

	// manually input values
	void set (float a, float b, float c, float d);

	// normalize vector
	vec4 normalize();

	// print data values for debugging
	void dump();

private:  
	// data storage variable - pointer so that we can embed it in a mat4
	float* data;
	// in case we just want to initialize a vec4 on its own
	bool isStandalone;
};
