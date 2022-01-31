#include  "mat4.h"
#include  "camera.h"

const float ANG = M_PI_4;
const int IMG_SIZE = 500;


int main(int argc, char** argv){
	// pull rotation info from input

	// initialize & orient camera object
   Camera *cam = new Camera();
   cam->rotate(0, 1, ANG);
   /*
	cam->rotate(0, 2, ANG);
   cam->rotate(1, 2, ANG);
   cam->rotate(3, 1, ANG);
   cam->rotate(3, 2, ANG);
   cam->rotate(0, 3, ANG);
   */

	// render with the given position
	cam->renderOptions(IMG_SIZE,IMG_SIZE,30,RayMarcher::renderType::dstep);
	cam->takePhoto("julia4d.png");

	delete cam;
	
	cout << "\nrender finished. closing program\n";
}
