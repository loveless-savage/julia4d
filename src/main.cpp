#include "mat4.h"
#include "camera.h"
#include "glwindow.h"
#include "multipasswindow.h"

#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"

const float ANG = M_PI_4;
const int IMG_SIZE = 600;

/* TODO:
 * what - the actual execution of the julia algorithm in the shader
 * where - texture/data pipelines organized correctly
 * when - responding correctly to user input
 * how - post-iteration normal shading & appearance
 */


int main(){
   auto* w = new MultipassWindow(IMG_SIZE, IMG_SIZE);
// create shaders
   w->addShader("raymarch");
   w->addShader("rayReset");
   // FIXME
   w->addShader("julia2d");
   w->addShader("mpTest");
   //w->addShader("julia2dslice");

// create texture buffers
   w->addMPTex("pos", GL_R32F); // just a 1D, non-clamped floating value per fragment
   w->addMPTex("dzTex");
   // FIXME
   w->addMPTex("juliaTex", GL_R32F); // just a 1D, non-clamped floating value per fragment
   //w->addMPTex("juliaTex");
   //w->addIOTexture("dJuliaTex", GL_RGBA32F); // default is a 4-color, non-clamped floating value per fragment

   w->dump(false);
   //w->swapMPTex("dzTex");
   //w->dump(false);

// link shaders and texture buffers
   // mpTest will use juliaTex as a multipass, but render will use it ordinarily
   w->attachTexOut("raymarch","pos"); // FIXME: attachMPTexIO()
   //w->attachTexOut("rayReset","pos"); // FIXME: reactivate for rayReset
   w->attachTexOut("raymarch","dzTex");
   w->attachTexIn("main", "pos");
   w->attachTexIn("main", "dzTex");
   // FIXME
   w->attachTexOut("julia2d","juliaTex");
   w->attachMPTexIO("mpTest","juliaTex");
   //w->attachTexIn("mpTest", "juliaTex");
   //w->attachTexOut("julia2dslice","dJuliaTex");
   w->attachTexIn("main", "juliaTex");
   //w->attachTexIn("main", "dJuliaTex");

// create & link uniforms
   int mode = 5;
   w->attachUniform("main", "mode", GL_INT, &mode);
   int iterationCount = 24;
   w->attachUniform("raymarch", "iterationCount", GL_INT, &iterationCount);
   w->attachUniform("raymarch", "mode", GL_INT, &mode);
   // FIXME
   float c_0[2] = {-0.01, 0.8};
   w->attachUniform("julia2d","c_0",GL_FLOAT_VEC2,c_0);

   // initialize & orient camera object
   Camera *cam = new Camera();
   //cam->GLWindowTarget(w, "julia2dslice");
   cam->GLWindowTarget(w, "raymarch");
   /*
   cam->rotate(1, 2, ANG);
   cam->rotate(3, 1, ANG);
   cam->rotate(3, 2, ANG);
   cam->rotate(0, 3, ANG);
   */

   bool isActive = true;
   w->render("julia2d");
   const float MOVEAMT = 0.001;
   do{
      // adjust the Z-plane offset with arrow keys
      if (w->isPressed(GLFW_KEY_UP)) {
         cam->adjustDistFromOrigin(MOVEAMT);
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_DOWN)) {
         cam->adjustDistFromOrigin(-MOVEAMT);
         isActive = false;
      }
      /*
      if (w->isPressed(GLFW_KEY_RIGHT)) {
      } else if (w->isPressed(GLFW_KEY_LEFT)) {
      } */

      // rotate camera
      if (w->isPressed(GLFW_KEY_Q)){ // XY plane
         cam->rotate(1,0,MOVEAMT);
      } else if (w->isPressed(GLFW_KEY_E)) {
         cam->rotate(0,1,MOVEAMT);
      }
      if (w->isPressed(GLFW_KEY_A)){ // XZ plane
         cam->rotate(2,0,MOVEAMT);
         c_0[0] -= MOVEAMT;
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_D)) {
         cam->rotate(0,2,MOVEAMT);
         c_0[0] += MOVEAMT;
         isActive = false;
      }
      if (w->isPressed(GLFW_KEY_S)){ // YZ plane
         cam->rotate(2,1,MOVEAMT);
         c_0[1] -= MOVEAMT;
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_W)) {
         cam->rotate(1,2,MOVEAMT);
         c_0[1] += MOVEAMT;
         isActive = false;
      }
      if (w->isPressed(GLFW_KEY_U)){ // WY plane
         cam->rotate(1,3,MOVEAMT);
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_O)) {
         cam->rotate(3,1,MOVEAMT);
         isActive = false;
      }
      if (w->isPressed(GLFW_KEY_J)){ // WZ plane
         cam->rotate(2,3,MOVEAMT);
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_L)) {
         cam->rotate(3,2,MOVEAMT);
         isActive = false;
      }
      if (w->isPressed(GLFW_KEY_K)){ // WX plane
         cam->rotate(0,3,MOVEAMT);
         isActive = false;
      } else if (w->isPressed(GLFW_KEY_I)) {
         cam->rotate(3,0,MOVEAMT);
         isActive = false;
      }

      // draw julia fractal in background buffer
      //w->render("julia2dslice");
      //w->render("raymarch");
      if(isActive) {
         w->render("mpTest");
      }else {
         w->render("julia2d");
      }

      // change visible parameter with number keys
      if (w->isPressed(GLFW_KEY_1)) {
         mode = 1;
      } else if (w->isPressed(GLFW_KEY_2)) {
         mode = 2;
      } else if (w->isPressed(GLFW_KEY_3)) {
         mode = 3;
      } else if (w->isPressed(GLFW_KEY_4)) {
         mode = 4;
      } else if (w->isPressed(GLFW_KEY_5)) {
         mode = 5;
      }

      // use render shader to port the final result to the screen
      w->renderMain();
      isActive = true;
   } while(w->stayOpen());

   delete w, cam;
   return glGetError();

	// render with the given position
	cam->renderOptions(IMG_SIZE,IMG_SIZE,30,RayMarcher::renderType::dstep);
	cam->takePhoto("julia4d.png");
}
