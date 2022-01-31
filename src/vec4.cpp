#include "vec4.h"


// constructor
vec4::vec4() {
   data = new float[4];
};
// destructor
vec4::~vec4 (){
   // if we piggybacked off a mat4, we can't do this- it was already de-allocated by the mat4
   //if(isStandalone) delete[] data;
};

// operator overloading
float &vec4::operator[](int idx) {
   return *(data + idx);
}

vec4 &vec4::operator=(vec4 target) {
   data[0] = target[0];
   data[1] = target[1];
   data[2] = target[2];
   data[3] = target[3];
   return *this;
};

vec4 vec4::operator+(vec4 target) {
   vec4 result;
   result = *this;
   result[0] += target[0];
   result[1] += target[1];
   result[2] += target[2];
   result[3] += target[3];
   return result;
};

vec4 &vec4::operator+=(vec4 target) {
   data[0] += target[0];
   data[1] += target[1];
   data[2] += target[2];
   data[3] += target[3];
   return *this;
};

vec4 vec4::operator-(vec4 target) {
   vec4 result;
   result = *this;
   result[0] -= target[0];
   result[1] -= target[1];
   result[2] -= target[2];
   result[3] -= target[3];
   return result;
};

vec4 &vec4::operator-=(vec4 target) {
   data[0] -= target[0];
   data[1] -= target[1];
   data[2] -= target[2];
   data[3] -= target[3];
   return *this;
};

vec4 vec4::operator*(float scalar) {
   vec4 result;
   result = *this;
   result[0] *= scalar;
   result[1] *= scalar;
   result[2] *= scalar;
   result[3] *= scalar;
   return result;
};

vec4 &vec4::operator*=(float scalar) {
   data[0] *= scalar;
   data[1] *= scalar;
   data[2] *= scalar;
   data[3] *= scalar;
   return *this;
};

vec4 vec4::operator/(float scalar) {
   vec4 result;
   result = *this;
   result[0] /= scalar;
   result[1] /= scalar;
   result[2] /= scalar;
   result[3] /= scalar;
   return result;
};

vec4 &vec4::operator/=(float scalar) {
   data[0] /= scalar;
   data[1] /= scalar;
   data[2] /= scalar;
   data[3] /= scalar;
   return *this;
};

// manually input values
void vec4::set (float a, float b, float c, float d){
   data[0] = a;
   data[1] = b;
   data[2] = c;
   data[3] = d;
};

// vector length
float vec4::length(){
   float r = 0.0;
   for (int i=0;i<4;i++){
      r += data[i]*data[i];
   }
   return sqrt(r);
};
// normalize vector
vec4 vec4::normalize(){
   return (*this) / length();
};

// dot product
float vec4::dot(vec4 target){
   return data[0]*target[0]
      + data[1]*target[1]
      + data[2]*target[2]
      + data[3]*target[3];
}

// print data values for debugging
void vec4::dump(){
   cout << endl;
   for (int i=0;i<4;i++){
      cout << "\t" << data[i];
   }
   cout << endl;
};


// constructor w/ ability to point it to already existing data values
vecShell::vecShell(float *dataIn){
   data = dataIn;
}
// destructor- we do not delete any memory this time!
vecShell::~vecShell() = default;
