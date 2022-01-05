#pragma once
#include "mat4.h"
#include "rayMarcher.h"


class Imager {
public:
	//constructor
	Imager (char* filename, int px, int py, float& d);
	// destructor
	~Imager ();

	// render next pixel in image
	void renderPixel();

	// print image to a file once rendering is finished
	void print();
	char* filename; // file name to write to - public for customization

private:
	// image size in pixels
	int pxW, pxH;

	// pointers for beginning & write locations of the photo
	unsigned char *photo, *head;

	// capture location of scalar returned by test
	float *depth;
};


class Camera {
public:
	// constructor
	Camera ();
	// destructor
	~Camera ();

	// rotate associated gimbal
	void rotate(int axisA, int axisB, float ang);

	// resize future photos
	void renderOptions(int w, int h, int iterMax, RayMarcher::renderType rtype);

	// render a photo at the current position
	void takePhoto(char *filename);

	// print data values for debugging
	void dumpPos();
	
	// basis vectors described by the rotation matrix
	vec4 *x,*y,*z,*w;
	// additional camera positioning values
	float distFromOrigin, fovSphereRadius;
private:
	// rotation coordinates
	mat4 *pos, *rtemp;

	// rayMarcher handles all the math behind ray marching & julia set iteration
	RayMarcher *marcher;

	// image size in pixels
	int pxW, pxH;

	// pointers for beginning & write locations of the photo
	unsigned char *photo, *head;
};
