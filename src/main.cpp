#include  "mat4.h"
#include  "camera.h"
#include <fstream>

mat4 m, mult;
Camera *cam;
const float ang = M_PI_4;



int main(int argc, char** argv){
	// pull rotation info from input

	// initialize & orient camera object
	cam = new Camera();
	cam->rotate(0,1,ang);
	cam->rotate(0,2,ang);
	cam->rotate(1,2,ang);
	cam->rotate(3,1,ang);
	cam->rotate(3,2,ang);
	cam->rotate(0,3,ang);

	// render with the given position
	cam->renderOptions(200,200,30,RayMarcher::renderType::dz);
	cam->takePhoto("julia4d.png");

	delete cam;
	
	cout << "\nlet's go\n";
}
