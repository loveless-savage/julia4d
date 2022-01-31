#pragma once
// standard includes
#include <cstdlib>
#include <cmath>
#include <iostream>
using namespace std;

class vec4 {
 public:
   // constructor
   vec4 ();
   // destructor
   ~vec4 ();

   // operator overloading
   vec4& operator = (vec4 target);
   float& operator [](int idx);
   vec4 operator + (vec4 target);
   vec4& operator += (vec4 target);
   vec4 operator - (vec4 target);
   vec4& operator -= (vec4 target);
   vec4 operator * (float scalar);
   vec4& operator *= (float scalar);
   vec4 operator / (float scalar);
   vec4& operator /= (float scalar);

   // manually input values
   void set (float a, float b, float c, float d);

   // length of vector
   float length();
   // normalize vector
   vec4 normalize();
   // dot product
   float dot(vec4 target);

   // print data values for debugging
   void dump();

 protected:
   // data storage variable - pointer so that we can embed it in a mat4
   float* data;
};


// vecShell has all properties of vec4, except that it points to pre-allocated data
class vecShell : public vec4 {
 public:
   // constructor w/ ability to point it to already existing data values
   vecShell(float *dataIn);
   // destructor
   ~vecShell();
};
