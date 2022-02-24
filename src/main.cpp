#include "mat4.h"
#include "camera.h"
#include "glwindow.h"

const float ANG = M_PI_4;
const int IMG_SIZE = 600;


int main(int argc, char** argv){
   /*
   ofstream vertShader("vertShader.glsl");
   string vertSrc = "#version 460 core\n"
                    "layout (location=0) in vec3 pos;\n"
                    "void main(){\n"
                    "gl_Position = vec4(pos,1.f);\n"
                    "}";
   vertShader << vertSrc;
   vertShader.close();
    */
   Glwindow* myWindow = new Glwindow(IMG_SIZE,IMG_SIZE);
   myWindow->addBuffer("julia");
   myWindow->addBuffer("mandelbrot");
   bool stayOpen = false;
   do{
      stayOpen = myWindow->loop();
   } while (stayOpen);
   cout << "glGetError: " << glGetError() << endl;
	// pull rotation info from input

   return 0; // FIXME
	// initialize & orient camera object
   Camera *cam = new Camera();
   cam->rotate(1, 2, 0.1);
	cam->rotate(0, 1, ANG);
   /*
   cam->rotate(1, 2, ANG);
   cam->rotate(3, 1, ANG);
   cam->rotate(3, 2, ANG);
   cam->rotate(0, 3, ANG);
   */

	// render with the given position
	cam->renderOptions(IMG_SIZE,IMG_SIZE,30,RayMarcher::renderType::dstep);
	//cam->takePhoto("julia4d.png");

	delete cam;
	
	cout << "finished. closing program" << endl;
}
