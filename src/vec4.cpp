#include "vec4.h"
#include "mat4.h"


// constructor
vec4::vec4() {
   data = new float[4];
};
vec4::vec4 (const float *dataLoc){
   data = new float[4];
   *this = dataLoc;
}
// destructor
vec4::~vec4 (){
   delete data;
};

// operator overloading
float &vec4::operator[](int idx) const {
   return *(data + idx);
}

vec4 &vec4::operator=(const vec4& target) {
   data[0] = target[0];
   data[1] = target[1];
   data[2] = target[2];
   data[3] = target[3];
   return *this;
};

vec4& vec4::operator = (const float* target) {
   data[0] = target[0];
   data[1] = target[1];
   data[2] = target[2];
   data[3] = target[3];
   return *this;
};

vec4 vec4::operator+(const vec4& target) const {
   vec4 result;
   result = *this;
   result[0] += target[0];
   result[1] += target[1];
   result[2] += target[2];
   result[3] += target[3];
   return result;
};

vec4 &vec4::operator+=(const vec4& target) {
   data[0] += target[0];
   data[1] += target[1];
   data[2] += target[2];
   data[3] += target[3];
   return *this;
};

vec4 vec4::operator-(const vec4& target) const {
   vec4 result;
   result = *this;
   result[0] -= target[0];
   result[1] -= target[1];
   result[2] -= target[2];
   result[3] -= target[3];
   return result;
};

vec4 &vec4::operator-=(const vec4& target) {
   data[0] -= target[0];
   data[1] -= target[1];
   data[2] -= target[2];
   data[3] -= target[3];
   return *this;
};

vec4 vec4::operator*(float scalar) const {
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

vec4 vec4::operator/(float scalar) const {
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
float vec4::length() const {
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
float vec4::dot(const vec4& target) const {
   return data[0]*target[0]
      + data[1]*target[1]
      + data[2]*target[2]
      + data[3]*target[3];
}

// rotate
// TODO: direct trig adjustment
void vec4::rotate(int axisA, int axisB, float ang){
   mat4 rtemp;
   rtemp.buildRotor(axisA,axisB,ang);

   vec4 result;
   result[0] = this->dot(rtemp.x);
   result[1] = this->dot(rtemp.y);
   result[2] = this->dot(rtemp.z);
   result[3] = this->dot(rtemp.w);
   this->operator=(result);
}

// return const pointer to data
float* vec4::dataPtr() const {
   return data;
};


// print data values for debugging
void vec4::dump() const {
   cout << right << setprecision(6) << fixed;
   for (int i=0;i<4;i++){
      cout << setw(12) << data[i];
   }
   cout << endl;
}