#include "mat4.h"

// constructor
mat4::mat4() {
	for (int i=0;i<16;i++) { // zero out the matrix
      data[i] = 0;
      if (i % 5 == 0) data[i] = 1; // add a strip of ones down the diagonal for the identity matrix
   }
};
// destructor
mat4::~mat4(){
   cout << "m";
}

// easy access to members of private data
float &mat4::operator [](int idx){
	return *(data+idx);
}

// build rotation matrix w/ arbitrary input axes
void mat4::buildRotor (int axisA, int axisB, float ang){
/* a 2d rotation matrix appears as follows:
 *  __________ __________
 * [ cos(ANG)  -sin(ANG) ]
 * [ sin(ANG)   cos(ANG) ]
 *  ~~~~~~~~~~ ~~~~~~~~~~
 * & when we project into higher dimensions, these can be superimposed on the other rows of an identity matrix:
 *
 *                      <axisA>               <axisB>
 *          _________ ___________ _________ ___________
 *         [    1          0          0          0     ]
 * <axisA> [    0       cos(ANG)      0      -sin(ANG) ]
 *         [    0          0          1          0     ]
 * <axisB> [    0       sin(ANG)      0       cos(ANG) ]
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

// get one axis of the mat4, formatted as a vec4
vec4 mat4::axis(int axisNum) const {
   if (axisNum < 0 || axisNum >= 4) {
      cout << "mat4 error: attempted to access row " << axisNum << endl;
      throw exception();
   }
   return vec4(&data[axisNum*4]);
}

// print data values for debugging
void mat4::dump() const {
	for (int i=0;i<16;i++){
		cout << ( (i%4==0)? "\n":"\t" );
		cout << data[i];
	}
	cout << "\n\n";
};