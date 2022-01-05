#include "mat4.h"

// constructor
mat4::mat4(){
	for (int i=0;i<16;i++){ // zero out the matrix
		data[i] = 0;
		if (i%5 == 0) data[i] = 1; // add a strip of ones down the diagonal for the identity matrix
	}
};

// easy access to members of private data
float &mat4::operator [](int idx){
	return *(data+idx);
}

// build rotation matrix w/ arbitrary input axes
void mat4::buildRotor (int axisA, int axisB, float ang){
/* a 2d rotation matrix appears as follows:
 *  __________ __________
 * [ cos(ang)  -sin(ang) ]
 * [ sin(ang)   cos(ang) ]
 *  ~~~~~~~~~~ ~~~~~~~~~~
 * & when we project into higher dimensions, these can be superimposed on the other rows of an identity matrix:
 *
 *                      <axisA>               <axisB>
 *          _________ ___________ _________ ___________
 *         [    1          0          0          0     ]
 * <axisA> [    0       cos(ang)      0      -sin(ang) ]
 *         [    0          0          1          0     ]
 * <axisB> [    0       sin(ang)      0       cos(ang) ]
 *          ~~~~~~~~~ ~~~~~~~~~~~ ~~~~~~~~~ ~~~~~~~~~~~
 */
	for (int i=0;i<16;i++){
		if( i==axisA*4+axisB ){ // lower left corner
			data[i] = sin(ang);
		}else if( i==axisB*4+axisA ){ // upper right corner
			data[i] = -1*sin(ang);
		}else if( i==axisA*5 || i==axisB*5 ){ // lying on the diagonal
			data[i] = cos(ang);
		}else if (i%5 == 0) { // rest of the diagonal
			data[i] = 1;
		}else{ // zero out the rest of the matrix
			data[i] = 0;
		}
	}
};

// multiply by another matrix
void mat4::mult(mat4 rotor){
	int i,j; // define our counter variables here for reuse
	// copy current matrix so we don't lose it
	float temp[16];
	for(i=0;i<16;i++) temp[i] = data[i];

	// each of the 16 addresses in the matrix...
	for(i=0;i<16;i++){
		data[i] = 0;
		// ... is a sum of 4 products
		for(j=0;j<4;j++){
			data[i] +=
				rotor[i%4 + j*4] // row i%4
				*temp[i-i%4 + j]; // column i (rounded to the nearest multiple of 4)
		}
	}
};

// print data values for debugging
void mat4::dump(){
	for (int i=0;i<16;i++){
		cout << ( (i%4==0)? "\n":"\t" );
		cout << data[i];
	}
	cout << "\n\n";
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

// constructor
vec4::vec4() : isStandalone(true) {
	data = new float[4];
};
// constructor w/ ability to point it to already existing data values
vec4::vec4(float &data) : data(&data), isStandalone(false) {};
// destructor
vec4::~vec4 (){
	// if we piggybacked off a mat4, we can't do this- it was already de-allocated by the mat4
	if(isStandalone) delete[] data;
};

// operator overloading
float& vec4::operator [](int idx){
	return *(data+idx);
}
void vec4::operator = (vec4 target){
	data[0] = target[0];
    data[1] = target[1];
    data[2] = target[2];
    data[3] = target[3];
};
vec4 vec4::operator + (vec4 target){
	vec4 result;
	result[0] += target[0];
    result[1] += target[1];
    result[2] += target[2];
    result[3] += target[3];
	return result;
};
vec4 vec4::operator - (vec4 target){
	vec4 result;
	result[0] -= target[0];
    result[1] -= target[1];
    result[2] -= target[2];
    result[3] -= target[3];
	return result;
};
vec4 vec4::operator * (float scalar){
	vec4 result;
	result[0] *= scalar;
    result[1] *= scalar;
    result[2] *= scalar;
    result[3] *= scalar;
	return result;
};
vec4 vec4::operator / (float scalar){
	vec4 result;
	result[0] /= scalar;
    result[1] /= scalar;
    result[2] /= scalar;
    result[3] /= scalar;
	return result;
};

// manually input values
void vec4::set (float a, float b, float c, float d){
	data[0] = a;
	data[1] = b;
	data[2] = c;
	data[3] = d;
};

// normalize vector
vec4 vec4::normalize(){
	float r = 0;
	for (int i=0;i<4;i++){
		r += data[i]*data[i];
	}
	return (*this) / sqrt(r);
};

// print data values for debugging
void vec4::dump(){
	cout << "\n";
	for (int i=0;i<4;i++){
		cout << "\t" << data[i];
	}
	cout << "\n";
};
