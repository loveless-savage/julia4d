#include "camera.h"
using namespace std;

// writing to png file
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif


//constructor
Imager::Imager (char* filename, int px, int py, float& d)
	: filename(filename),pxW(px),pxH(py),depth(&d)
{
	// allocate space for rendering data - pxW*pxH pixels, 3 colors per pixel (RGB)
	photo = new unsigned char[pxW*pxH*3];
	head = photo;
};

// destructor
Imager::~Imager (){
	delete[] photo;
};

// nice color spectrum mapping given distance - used for rendering
float spectrumSkew(float depth,int color){
	float skew = 255+255*cosf(M_PI*sinf(M_PI/3.0*(depth-color)));
	if(skew>255.0){
		return 255;
	}else{
		return skew;
	}
};

// render next pixel in image
void Imager::renderPixel(){
		if(*depth == -1.0){
			*head++ = 0;
			*head++ = 0;
			*head++ = 0;
		} else {
			*head++ = spectrumSkew(*depth,0);
			*head++ = spectrumSkew(*depth,1);
			*head++ = spectrumSkew(*depth,2);
		}
};

// print image to a file once rendering is finished
void Imager::print(){
	// print data to png file w/ given width/height, 3 colors, & data location
	stbi_write_png(filename, pxW,pxH, 3, photo, pxW*3);
};


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
/* Camera */
// constructor
Camera::Camera(){
	pos = new mat4();
	// xyzw are just pointers to the 4 columns of the position vector (w/ added vec4 functionality)
	x = new vec4((*pos)[0]);
	y = new vec4((*pos)[4]);
	z = new vec4((*pos)[8]);
	w = new vec4((*pos)[12]);

	rtemp = new mat4();

	marcher = new RayMarcher();

	// these are some pretty good default values for image size
	pxW = 500;
	pxH = 500;
}

// destructor
Camera::~Camera(){ delete pos, rtemp, x, y, z, w, marcher; }

// rotate associated gimbal
void Camera::rotate(int axisA, int axisB, float ang){
	cout << "rotating around axes " << axisA << "," << axisB << " by " << ang << endl;
	rtemp->buildRotor(axisA,axisB,ang);
	pos->mult(*rtemp);
	//pos->dump();
};

// resize future photos
void Camera::renderOptions(int w, int h, int iterMax, RayMarcher::renderType rtype){
	pxW = w;
	pxH = h;
	marcher->renderOptions(iterMax, rtype);
};

// render a photo at the current position
void Camera::takePhoto(char *filename){
/* distFromOrigin is fairly self-explanatory: distance from the camera to the origin
 * let D = distFromOrigin
 * fovSphereRadius describes the radius of the largest sphere to fall entirely within the camera's FOV
 * let r = fovSphereRadius
 * given the plane through the origin & normal to the camera position...
 * let n be the shortest distance along that plane which reaches from the center to the edge of view
 * then, it can be shown with a ratio of triangle side lengths that:
 * --> r/n = sqrt(D^2-r^2)/D
 * `-> n = D*r/sqrt(D^2-r^2)
 * to render each pixel, the rayMarcher requires an initial vector pointing in the pixel ray's direction
 * this is produced starting with a base vector pointing to the center of view, & adding a sideways skew
 * let S be the the distance needed to skew the base vector to point towards the nearest edge of the camera view
 * using a triangle ratio again, we can derive:
 * --> S / length(baseVector) = n / D
 * `-> S / 1 = (D*r/sqrt(D^2-r^2)) / D
 * `-> S = r/sqrt(D^2-r^2)
 * the displacement skewed in each dimension is simply a multiple of the tiny vectors stepX and stepY...
 * ... where those multiples are the #pixels away from the center horizontally & vertically
 * pxStepLength is the length of stepX, as well as stepY
 * let p = pxStepLength
 * let pxMin = the minimum distance (in # of pixels) from the center to the edge of view
 * then
 * `-> p = S / pxMin
 *       = r/sqrt(D^2-r^2) / (min(pxW,pxH)/2)
 * .: pxStepLength = 2*r/sqrt(D^2-r^2)/min(pxW,pxH)
 */

	// set up ray marcher w/ seed vectors
	marcher->orient(
		z,x,y, // 3 basis vectors
		distFromOrigin, // distance of the camera from the origin
		2*fovSphereRadius/sqrtf(distFromOrigin*distFromOrigin-fovSphereRadius*fovSphereRadius)/fminf(pxW,pxH) // step length to skew the base rendering vector between pixels
	);

	// capture value of scalar returned by test
	float depth;

	// capture data into an image using the Imager class
	Imager *imager1 = new Imager(filename,pxW,pxH,depth);
	/*
	// allocate space for rendering data - pxW*pxH pixels, 3 colors per pixel (RGB)
	photo = new unsigned char[pxW*pxH*3];
	head = photo;
	*/

	cout << "Imager initialized. Starting render loop:" << endl;

	// loop once per pixel- px & py give the current pixel coordinates
	for(int py=0;py<pxH;py++){ for(int px=0;px<pxW;px++){
 // TODO: collect test results indirectly
 		cout << "_";
		depth = marcher->test(
				5.0 * ((float)px)/((float)pxW) - 2.5,
				5.0 * ((float)py)/((float)pxH) - 2.5
			);
		cout << py*pxW+px;

		imager1->renderPixel();
		cout << ",";
		/*
		if(depth == -1.0){
			*head++ = 0;
			*head++ = 0;
			*head++ = 0;
		} else {
			*head++ = spectrumSkew(depth,0);
			*head++ = spectrumSkew(depth,1);
			*head++ = spectrumSkew(depth,2);
		}
		*/
	}}

	cout << "\nrendering finished. printing to png file" << endl;
	
	// print data to png file w/ given width/height, 3 colors, & data location
	imager1->print();
	delete imager1;
	/*
	stbi_write_png(filename, pxW,pxH, 3, photo, pxW*3);
	delete[] photo;
	*/
};

// print data values for debugging
void Camera::dumpPos(){ pos->dump(); };
