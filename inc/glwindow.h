#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct ShaderBuffer {
 public:
   string name;
   GLuint pgmID;
};

class Glwindow {
 public:
   // constructor
   Glwindow(int width, int height);
   // destructor
   ~Glwindow();

   // set up a new buffer with a shader of its own
   void addBuffer(const string &bufferName);
   // load shaders
   void loadShaders(int bufferIdx);
   // run this every frame
   int loop();

 private:
   // the window itself
   GLFWwindow* win;
   // OpenGL variables
   GLuint vao, vbo;
   // set up GLFW in general; call this before initializing any instance
   int glfwStart() const;
   // set up universal OpenGL contexts
   void setupGL();

   // handle several buffers at once: each buffer has
   // a name & shader program ID associated with it
   vector<ShaderBuffer> buffers;
};


// two triangles to cover the viewport
extern const GLfloat fullViewTriangles[12];
