#include "gimbal.h"


// constructor
Gimbal::Gimbal(){
	pos = new mat4();
	rtemp = new mat4();
	// xyzw are just models for the 4 columns of the position vector
	x = new vec4((*pos)[0]);
	y = new vec4((*pos)[4]);
	z = new vec4((*pos)[8]);
	w = new vec4((*pos)[12]);
}

// destructor
Gimbal::~Gimbal(){ delete pos, rtemp, x, y, z, w; }

// rotate
void Gimbal::rotate(int axisA, int axisB, float ang){
	printf("rotating around axes %i,%i by %f",axisA,axisB,ang);
	rtemp->buildRotor(axisA,axisB,ang);
	pos->mult(*rtemp);
	pos->dump();
};

// print data values for debugging
void Gimbal::dump(){ pos->dump(); };
