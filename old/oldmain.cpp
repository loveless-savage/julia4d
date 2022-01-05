/* draws a 3D slice of the 4D julia set with ray tracing techniques
*/


// standard includes
#include <stdio.h>
#include <stdlib.h>
// library for time delays
#include <unistd.h>
// writing to a png at the end
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
// 4D vector math- I wrote this with the specific functions I want from it
#include <vector4d.h>

// screen size in pixels
#define D_W 500
#define D_H 500

// which screen dimesion is smallest? store it- or rather, half of it
#if D_W < D_H
#define D_MIN D_W/2
#else
#define D_MIN D_H/2
#endif

// number of z layers in front of the zero plane- it will be mirrored on the far side of zero
#define D_D 64
// scope size in relative, complex units
#define D_SCOPE 4.0
// distance of virtual camera to origin in relative, complex units
#define D_CAMERA 8.0

// now we want the width of the normal zero plane in complex units- this we derive from the previous constants
// we use some pythagoras here, because it is based on the tangent of a sphere
#define D_PLANE D_SCOPE*D_CAMERA/__builtin_sqrt(D_CAMERA*D_CAMERA - D_SCOPE*D_SCOPE)

// rotation angles in radians
#define R_XZ 0.8
#define R_YW 0.0
#define R_YZ 0.0
#define R_XW 0.0

// vec4d wrapper for rotations
vec4d vr (R_XZ,R_YW,R_YZ,R_XW);

// space coordinates to be rotated with the world's rotation
vec4d vx (	D_PLANE/D_MIN,	0.0,	0.0,	0.0	);	// zero-plane width divided by resolution
vec4d vy (	0.0,	D_PLANE/D_MIN,	0.0,	0.0	);	// ditto, in the y-direction
vec4d vz (	0.0,	0.0,	D_CAMERA,	0.0	);		// distance of camera from origin
//vec4d vw (	0.0,	0.0,	0.0,	1.0	);
// deltas of each
vec4d d_z (	0.0,	0.0,	-D_SCOPE/D_D,	0.0	);	// radius of the scope divided by z-resolution; negative because away from the camera
//vec4d d_w (	0.0,	0.0,	0.0,	1.0	);
// d_x & d_y are used to slope the delta ray, so perspective takes effect
vec4d d_x (	D_SCOPE/D_D /D_CAMERA *D_PLANE/D_MIN, 	0.0,	0.0,	0.0	);	// length of d_z divided by distance from origin to camera
vec4d d_y (	0.0,	D_SCOPE/D_D /D_CAMERA *D_PLANE/D_MIN,	0.0,	0.0	);	// this ratio is multiplied by the relative pixel length, then by px/y for each pixel

// container for primary view ray and its delta
vec4d ray (0.0,0.0,0.0,0.0);
vec4d d_ray (0.0,0.0,0.0,0.0);

// length of each view ray
float depth;

// map values neatly
float l_trans(float val, float min1, float max1, float min2, float max2){
	val -= min1;
	val /= max1-min1;
	val *= max2-min2;
	val += min2;
	return val;
}

// nice color spectrum mapping given distance
float spectrumSkew(float depth){
	float skew = 255+255*cosf(M_PI*sinf(depth));
	if(skew>255.0){
		return 255;
	}else{
		return skew;
	}
};



int main(int argc, char** argv){

	if(argc<=5){
		fprintf(stderr, "you need 4 rotation values & a depth resolution value\n");
		return EXIT_FAILURE;
	}

	// collect rotation values from given parameters
	vr.set( atof(argv[1]), atof(argv[2]), atof(argv[3]), atof(argv[4]) );

	// last parameter adjusts number of layers
	d_z.scale(D_D/atof(argv[5]));
	d_x.scale(D_D/atof(argv[5]));
	d_y.scale(D_D/atof(argv[5]));

	// rotate world by specified angles, including axes
	vx.spin(vr);
	vy.spin(vr);
	vz.spin(vr);
	//vw.spin(vr);
	d_x.spin(vr);
	d_y.spin(vr);
	d_z.spin(vr);
	//d_w.spin(vr);


	// prepare image buffer- width*height*3colors
	unsigned char pts[D_W*D_H*3] = {0};
	unsigned char *head = pts;

	// loop once per pixel- px & py give the current pixel coordinates
	for(size_t py=0;py<D_H;py++){ for(size_t px=0;px<D_W;px++){
		//printf("%lu,%lu\t",py,px);

		// calculate the view ray for the current pixel
		//ray.set( vx.nscale(px-0.5*D_W) ); // px-0.5*D_W corresponds to how many pixels away from midline
		//ray.add( vy.nscale(py-0.5*D_H) );
		ray.set( vz );

		// calculate delta of the view ray
		d_ray.set( d_z );
		d_ray.add( d_x.nscale(px-0.5*D_W) ); // px-0.5*D_W corresponds to how many pixels away from midline
		d_ray.add( d_y.nscale(py-0.5*D_H) );

		// bring view ray back towards the camera until it is about to leave the origin cube
		ray.calibrateRayToScope(d_ray,D_SCOPE);

//*
		// loop each time ray ticks further //TODO
		while(true){
			// check if ray has exceeded the bounding box
			if(!ray.isBounded(D_SCOPE)){
				//printf("\tMISS");
				*head++ = 0;
				*head++ = 0;
				*head++ = 0;
				break;
			}
			// check each ray tick for julia test passing
			if( ray.trapped(16) ){
				depth = ray.distFrom(vz);
				//printf("\tHIT %f",depth);
				depth = l_trans(depth,0.0,1.0,0.0,5.0);
				//printf("\t%f",depth);

				*head++ = spectrumSkew(depth);
				*head++ = spectrumSkew(depth-M_PI/3.0);
				*head++ = spectrumSkew(depth+M_PI/3.0);
				break;
			}
			// tick ray
			ray.add( d_ray );
			//printf(".");
		}
/*
		depth = ray.distFrom(vz);
		//printf("\tHIT %f",depth);
		depth = l_trans(depth,3.0,9.0,0.0,12.0);
		//printf("\t%f",depth);

		*head++ = spectrumSkew(depth);
		*head++ = spectrumSkew(depth-M_PI/3.0);
		*head++ = spectrumSkew(depth+M_PI/3.0);
*/
		//printf("\tpixel done\n");
	} printf("."); fflush(stdout); usleep(10000); } // hundredth of a second between each row to give the CPU a breath

	stbi_write_png("julia4d.png", D_W, D_H, 3, pts, D_W*3);

}
