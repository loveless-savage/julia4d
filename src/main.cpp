#include "mat4.h"
#include "camera.h"
#include "glwindow.h"
#include "multipasswindow.h"

const float ANG = M_PI_4;
const int IMG_SIZE = 600;


int main(int argc, char** argv){
   MultipassWindow* w = new MultipassWindow(IMG_SIZE, IMG_SIZE);

   w->addShader("julia");
   w->addShader("mandelbrot");
   w->addShader("mpTest");
   w->addShader("render");

   float offsetamt[2] = {0.0,0.0};
   w->attachUniform("julia", "offset", GL_FLOAT_VEC2, offsetamt);

   w->addMultipassTex("juliaTex");
   w->addFBOTexture("mandelbrotTex");

   // mpTest will use juliaTex as a multipass, but render will use it ordinarily
   //myWindow->attachTexInput("mpTest", "juliaTex_I");
   w->attachTexInput("mpTest", "juliaTex");
   w->attachTexInput("render", "juliaTex");
   //myWindow->attachTexInput("render","mandelbrotTex");


   //myWindow->renderToTex("julia","juliaTex");
   //myWindow->renderToMPTex("mpTest", "juliaTex");
   //myWindow->renderMain("render",stayOpen);
   const float MOVEAMT = 0.001;
   do{
      if (w->isPressed(GLFW_KEY_UP)) {
         offsetamt[1] += MOVEAMT;
      } else if (w->isPressed(GLFW_KEY_DOWN)) {
         offsetamt[1] -= MOVEAMT;
      }
      if (w->isPressed(GLFW_KEY_RIGHT)) {
         offsetamt[0] += MOVEAMT;
      } else if (w->isPressed(GLFW_KEY_LEFT)) {
         offsetamt[0] -= MOVEAMT;
      }
      // draw julia fractal in background buffer
      w->renderToTex("julia", "juliaTex");
      //myWindow->renderToTex("mandelbrot","mandelbrotTex");
      //myWindow->swapTextures("juliaTex","juliaTex_I");
      //myWindow->renderToMPTex("mpTest", "juliaTex");

      // use render shader to port the final result to the screen
      w->renderMain("render");
   } while (w->stayOpen());
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
	cam->takePhoto("julia4d.png");

	delete cam;
	
	cout << "finished. closing program" << endl;
}
