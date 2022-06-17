#pragma once
// standard includes
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
using namespace std;

class vec4 {
 public:
   // constructors
   vec4 ();
   vec4 (const float *dataLoc);
   // destructor
   ~vec4 ();

   // operator overloading
   float& operator [](int idx) const;
   vec4& operator = (const vec4& target);
   vec4& operator = (const float* target);
   vec4 operator + (const vec4& target) const;
   vec4& operator += (const vec4& target);
   vec4 operator - (const vec4& target) const;
   vec4& operator -= (const vec4& target);
   vec4 operator * (float scalar) const;
   vec4& operator *= (float scalar);
   vec4 operator / (float scalar) const;
   vec4& operator /= (float scalar);

   // manually input values
   void set (float a, float b, float c, float d);

   // length of vector
   float length() const;
   // normalize vector & return result
   vec4 normalize();
   // dot product
   float dot(const vec4& target) const;
   // rotate
   void rotate(int axisA, int axisB, float ang);

   // return const pointer to data
   float* dataPtr() const;

   // print data values for debugging
   void dump() const;
 protected:
   // data storage variable - pointer so that we can embed it in a mat4
   float* data;
};