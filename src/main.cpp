#include "mat4.h"
#include "camera.h"
#include "glwindow.h"
#include "multipasswindow.h"

const float ANG = M_PI_4;
const int IMG_SIZE = 600;


int main(int argc, char** argv){
   Glwindow* w = new MultipassWindow(IMG_SIZE, IMG_SIZE);

   w->addShader("julia");
   //w->addShader("mandelbrot");
   //w->addShader("mpTest");
   w->addShader("main");

   float offsetamt[2] = {0.0,0.0};
   w->attachUniform("julia", "offset", GL_FLOAT_VEC2, offsetamt);
   int mode = 0;
   w->attachUniform("main", "mode", GL_INT, &mode);

   //w->addMultipassTex("juliaTex");
   w->addIOTexture("juliaTex");
   w->addIOTexture("dJuliaTex");
   //w->addTexture("mandelbrotTex");

   // mpTest will use juliaTex as a multipass, but render will use it ordinarily
   w->attachTexOut("julia","juliaTex");
   w->attachTexOut("julia","dJuliaTex");
   //w->attachTexInput("mpTest", "juliaTex_I");
   //w->attachTexInput("mpTest", "juliaTex");
   w->attachTexIn("main", "juliaTex");
   w->attachTexIn("main", "dJuliaTex");
   //w->attachTexInput("main","mandelbrotTex");


   //w->render("julia","juliaTex");
   //w->renderToMPTex("mpTest", "juliaTex");
   //w->renderMain("main",stayOpen);
   const float MOVEAMT = 0.001;
   do{
      // adjust z_0 with arrow keys
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
      w->render("julia");
      //w->render("mandelbrot");
      //w->renderToMPTex("mpTest", "juliaTex");

      // change visible parameter with number keys
      if (w->isPressed(GLFW_KEY_1)) {
         mode = 0;
      } else if (w->isPressed(GLFW_KEY_2)) {
         mode = 1;
      } else if (w->isPressed(GLFW_KEY_3)) {
         mode = 2;
      } else if (w->isPressed(GLFW_KEY_4)) {
         mode = 3;
      } else if (w->isPressed(GLFW_KEY_5)) {
         mode = 4;
      }

      // use render shader to port the final result to the screen
      w->renderMain();
   } while(w->stayOpen());
   cout << "glGetError: " << glGetError() << endl;
	// pull rotation info from input
   delete w;
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
