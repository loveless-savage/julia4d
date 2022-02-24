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
void RayMarcher::orient(vec4 rootZ, vec4 rootX, vec4 rootY, float distFromOrigin, float fovSphereRadiusIn){
	origin = rootZ * (-1*distFromOrigin);
	step0  = rootZ;

   fovSphereRadius = fovSphereRadiusIn;
   float pxStepLength = 2 * fovSphereRadius // actual width of the object
                        / sqrt(distFromOrigin*distFromOrigin - fovSphereRadius * fovSphereRadius); // project world size to screen size
   stepX  = rootX * pxStepLength;
	stepY  = rootY * pxStepLength;
   cout << "origin: " << endl;
   origin.dump();
   cout << "step0: " << endl;
   step0.dump();
   cout << "stepX: " << endl;
   stepX.dump();
   cout << "stepY: " << endl;
   stepY.dump();
   cout << "pxStepLength: " << pxStepLength << endl;
};

// cast ray associated with given screen coordinates
float RayMarcher::castRay(float pX, float pY){
   //cout << "casting ray" << endl;
   // accumulate the total distance from camera to intersection w/ the fractal
   distSum = 1.0;

   // displace the starting ray a little to the side, as prescribed by the displacing vectors stepX & stepY
   step = step0;
   step += stepX*pX;
   step += stepY*pY;
   //step = step.normalize();

   // start the ray at the origin, which is not at (0,0,0,0) but is set a little ways out
   pos = origin;
   for(int i=0;i<16;i++) {
      // don't add the next ray step yet! If we go to far & penetrate the set we need to backtrack.
      // so instead of just passing pos, we pass pos+step*distSum
      if ( jtest(pos + step*distSum) ){
         // jtest() returns true if it falls within the set! Now we do one of two things:
         // 1. If we just barely penetrated the set, we are good to render
         if(distSum < 0.03) break; // TODO: tune threshold
         // 2. If we punctured by a bigger margin, backtrack a bit for more accuracy
         distSum *= 0.5; // TODO: tune backstepping
         i--;
         continue;
      };
      // if we haven't reached the set yet, check whether we've gone past the viewing sphere in the back
      if(pos.dot(step)>0.0 && pos.length()>fovSphereRadius){
         return -1.0;
      }
      // march the ray forward
      pos += step*distSum;
      // from our new vantage point we can check the next distance
      distSum = 0.5 * z_modulus * logf(z_modulus) / dpos.length();
   }

   //cout << "[" << dpos.length() << "/" << n << "]";

   switch (rtype) {
      case RayMarcher::renderType::dstep :
         return (pos-origin).length();
      case RayMarcher::renderType::distance :
         return distSum;
         /*
      case RayMarcher::renderType::dz :
         return sqrt(dpos[Z0A]*dpos[Z0A] + dpos[Z0B]*dpos[Z0B]);
      case RayMarcher::renderType::dc :
         return sqrt(dpos[C0A]*dpos[C0A] + dpos[C0B]*dpos[C0B]);
          */
      default:
         return 0.0;
   }
}

// while casting a ray, use current ray coordinates & run a julia test
float RayMarcher::jtest(vec4 posIn){
	// computation variables- real & imaginary parts
	za = posIn[Z0A]; // the macro Z0A accesses posIn[0]
	zb = posIn[Z0B]; // ... & vice versa

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
	dpos.set(1, 0, 0, 0);

	// rollover for above variables
	float zaQ, zbQ;
	vec4 dposQ;

	for(int n=0;n<iterMax;n++){
		// iterate calculation, mimicking complex numbers:
		// Z_n = Z_n-1^2 + C
		zaQ = za*za - zb*zb + posIn[C0A];
		zbQ = 2*za*zb + posIn[C0B];

		/* derivatives:
		 * d(Zn+1)/dZ0 = ( 2*za*d(z/dZ0)a - 2*zb*d(z/dZ0)b )
		 *            + ( 2*za*d(z/dZ0)b + 2*zb*d(z/dZ0)a ) * i;
		 * ... and vice versa for d(Zn+1)/dC
		 *
		 * since we have the macros Z0A,Z0B,C0A,C0B we can use them to access dposQ[] just like posIn[]
		 */
		dposQ[Z0A] = 2 * (za * dpos[Z0A] - zb * dpos[Z0B]);
      dposQ[Z0B] = 2 * (za * dpos[Z0B] + zb * dpos[Z0A]);

      dposQ[C0A] = 2 * (za * dpos[C0A] - zb * dpos[C0B]) + 1; // also, d(Zn+1)/dC = 1, so add 1 to the real part of /dC
		dposQ[C0B]= 2 * (za * dpos[C0B] + zb * dpos[C0A]);

		// roll values
		za = zaQ;
		zb = zbQ;
		dpos.set(dposQ[0], dposQ[1], dposQ[2], dposQ[3]);

      // absolute value of z == hypotenuse of a triangle
      z_modulus = hypotf(za,zb);
      if(z_modulus > 2.0) { // fails the divergence test?
         return false;
      }
   }

   return true;
};
