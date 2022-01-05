#include "rayMarcher.h"
// macros to make the code more readable
#define Z0A 0
#define Z0B 1
#define C0A 2
#define C0B 3

// constructor
RayMarcher::RayMarcher(){
	this->renderOptions(30, RayMarcher::renderType::set);
}

// render options
void RayMarcher::renderOptions(int iterMaxi, RayMarcher::renderType rtypei){
	iterMax = iterMaxi;
	rtype = rtypei;
};

// set up seed vectors
void RayMarcher::orient(vec4 *rootZ, vec4 *rootX, vec4 *rootY, float distFromOrigin, float pxStepLength){
	origin = (*rootZ) * (-1*distFromOrigin);
	step0 = (*rootZ);
	stepX = (*rootX) * pxStepLength;
	stepY = (*rootY) * pxStepLength;
};

// test given pixel coordinates
float RayMarcher::test(float pX, float pY){
	// accumulate the total distance from camera to intersection w/ the fractal
	float distSum=0;

	// displace the starting ray a little to the side, as perscribed by the displacing vectors stepX & stepY
	step = step0 + (stepX*pX) + (stepY*pY);
	step = step.normalize();

	/*for testing*/ step.set(pX,pY,-0.8, 0.18);

	// computation variables- real & imaginary parts
	za = step[Z0A]; // the macro Z0A accesses step[0]
	zb = step[Z0B]; // ... & vice versa

	/* C doesn't change with iteration, but Z does, so we differentiate Z_n...
	 * with respect to all initial variables, in a 4x2 matrix like so:
	 * [ dzna/dz0a  dznb/dz0a ]
	 * [ dzna/dz0b  dznb/dz0b ]
	 * [ dzna/dca   dznb/dca  ]
	 * [ dzna/dcb   dznb/dcb  ]
	 * as it turns out, the Cauchy-Riemmann equations tell us that
	 *  d(Zn)/d(z0b) == d(Zn)/d(z0a) * i; and
	 *  d(Zn)/d(cb) == d(Zn)/d(ca) * i;
	 *  thus our matrix simplifies to the following:
	 *  [ 
	 */
	dz_step.set(1,0,0,0);

	// rollover for above variables
	float zaQ, zbQ;
	vec4 dzQ_step;

	n=0;
	while(n<iterMax){
		if(za*za+zb*zb > 4.0) break; // fails the divergence test?

		// iterate calculation, mimicking complex numbers:
		// Z_n = Z_n-1^2 + C
		zaQ = za*za - zb*zb + step[C0A];
		zbQ = 2*za*zb + step[C0B];

		/* derivatives:
		 * d(Zn+1)/dZ0 = ( 2*za*d(z/dZ0)a - 2*zb*d(z/dZ0)b )
		 *            + ( 2*za*d(z/dZ0)b + 2*zb*d(z/dZ0)a ) * i;
		 * ... and vice versa for d(Zn+1)/dC
		 *
		 * since we have the macros Z0A,Z0B,C0A,C0B we can use them to access dzQ_step[] just like step[]
		 */
		dzQ_step[Z0A] = 2*(za*dz_step[Z0A] - zb*dz_step[Z0B]);
		dzQ_step[Z0B] = 2*(za*dz_step[Z0B] + zb*dz_step[Z0A]);

		dzQ_step[C0A] = 2*(za*dz_step[C0A] - zb*dz_step[C0B]) + 1; // also, d(Zn+1)/dC = 1, so add 1 to the real part of /dC
		dzQ_step[C0B]= 2*(za*dz_step[C0B]+ zb*dz_step[C0A]);

		// roll values
		za = zaQ;
		zb = zbQ;
		dz_step.set(dzQ_step[0],dzQ_step[1],dzQ_step[2],dzQ_step[3]);

		n++;
	}
	
	// if it still passes the divergence test, color it black
	if(n == iterMax) return -1.0;

	switch (rtype) {
	case RayMarcher::renderType::distance :
		 return -1.0;// TODO
	case RayMarcher::renderType::dz :
		return dz_step[Z0B];//sqrt(dza[0]*dza[0]+dzb[0]*dzb[0]);
	case RayMarcher::renderType::dc :
		return dz_step[C0A];//sqrt(dz_ac*dz_ac+dz_bc*dz_bc);
	default:
		return 0.1*n;
	}
};
