#include "camera.h"
using namespace std;

// writing to png file
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif


//constructor
Imager::Imager (const string& filename, int px, int py, float& d)
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
		if(*depth == -1.0){ // TODO: add epsilon?
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
	stbi_write_png(filename.c_str(), pxW,pxH, 3, photo, pxW*3);
};


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
/* Camera */
// constructor
Camera::Camera() :
// these are some pretty good default values for camera positioning
         // when the camera is twice the distance from the origin as the object's size,
         distFromOrigin(5.0),
         // it leads to a comfortable 30degree viewing angle
         fovSphereRadius(2.5),
         // we will adjust this in a moment
         fovStepLength(0.0)
{
	pos = new mat4();
	rtemp = new mat4();

	marcher = new RayMarcher();

   // fovSphereRadius is already at the correct size, so don't change it...
   // ...but this command will sync fovStepLength to match the FOV Sphere
   adjustFovSphereRadius(0);

   // default values for image size
	pxW = 500;
	pxH = 500;
}

// destructor
Camera::~Camera(){
   delete pos;
   delete rtemp;
   delete marcher;
}

// rotate associated gimbal
void Camera::rotate(int axisA, int axisB, float ang){
	//cout << "rotating around axes " << axisA << "," << axisB << " by " << ang << endl;
	rtemp->buildRotor(axisA,axisB,ang);
	pos->mult(*rtemp);
   pos->updateVectorAxes(fovStepLength,fovStepLength);
   destabilize();
	//pos->dump();
};
// approach & retreat the camera from the origin
void Camera::adjustDistFromOrigin(float dr){
   distFromOrigin += dr;
   fovSphereRadius = distFromOrigin // scale from projected length to world length
                     * fovStepLength / sqrt(1+fovStepLength*fovStepLength); // triangle ratio
   destabilize();
}
// change the viewing radius, via the radius of the FOV sphere centered at the origin
void Camera::adjustFovSphereRadius(float dr){
   fovSphereRadius += dr;
   fovStepLength = fovSphereRadius // actual width of the object
                   / sqrt(distFromOrigin*distFromOrigin - fovSphereRadius * fovSphereRadius); // triangle ratio

   pos->updateVectorAxes(fovStepLength,fovStepLength);
   destabilize();
}


// associate camera with an OpenGL rendering window
void Camera::GLWindowTarget(Glwindow *target, const char* shaderName) {
   target->attachUniform(shaderName,"cameraX",GL_FLOAT_VEC4,  pos->x.dataPtr());
   target->attachUniform(shaderName,"cameraY",GL_FLOAT_VEC4,  pos->y.dataPtr());
   target->attachUniform(shaderName,"cameraDir",GL_FLOAT_VEC4,pos->z.dataPtr());
   target->attachUniform(shaderName,"cameraDist",GL_FLOAT,&distFromOrigin);
   target->attachUniform(shaderName,"fovStepLength",GL_FLOAT,&fovStepLength);
   target->attachUniform(shaderName,"fovSphereRadius",GL_FLOAT,&fovSphereRadius);
};

// resize future photos
void Camera::renderOptions(int w, int h, int iterMax, RayMarcher::renderType rtype){
	pxW = w;
	pxH = h;
	marcher->renderOptions(iterMax, rtype);
};

// render a photo at the current position
void Camera::takePhoto(const string& filename){
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
         pos->z, pos->x / fminf(pxW, pxH),
         pos->y / fminf(pxW, pxH), // 3 basis vectors
         distFromOrigin, // distance of the camera from the origin
         fovSphereRadius // size of the rear cutoff sphere
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

   // start a timer so we can track render times
   float renderTime = (float)clock() / CLOCKS_PER_SEC;

	cout << "Imager initialized. Starting render renderMain:" << endl;

	// loop once per pixel- px & py give the current pixel coordinates
	for(int py=0;py<pxH;py++){ for(int px=0;px<pxW;px++){

      // give coordinates of given pixel, returns preselected value from ray test
		depth = marcher->castRay(px-pxW/2,py-pxH/2);
				//5.0 * ((float)px)/((float)pxW) - 2.5,
				//5.0 * ((float)py)/((float)pxH) - 2.5

		imager1->renderPixel();
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

	cout << "rendering finished. printing to png file" << endl;
	
	// print data to png file w/ given width/height, 3 colors, & data location
	imager1->print();
	delete imager1;
	/*
	stbi_write_png(filename, pxW,pxH, 3, photo, pxW*3);
	delete[] photo;
	*/

   // compare the timer with starting time
   renderTime = (float)clock() / CLOCKS_PER_SEC - renderTime;
   cout << "render took " << renderTime << " seconds." << endl;
};

// print data values for debugging
void Camera::dumpPos(){ pos->dump(); }

// this tells the render pipeline whether to reprocess the same render again, or to render at a new angle
bool Camera::isStationary() const { return stationary; }
void Camera::destabilize() { stationary = false; }
void Camera::stabilize() { stationary = true; }
