#pragma once
#include "gldatashells.h"
#include <fstream>
#include <sstream>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif // GLEW_STATIC
using namespace std;


class Glwindow {
 public:
   // constructor
   Glwindow(int width, int height);
   // destructor
   ~Glwindow();

   // set up a new shader to be used pre-screen render
   void addShader(const string& shaderName);
   // set up a new texture to be used by whichever shader likes
   void addIOTexture(const string &texName, const GLint internalFormat = GL_RGBA32F); // default format = 4-color, non-clamped floating value per fragment

   // associate a texture to a shader so they both get bound together
   void attachTexIn(const string& shaderName, const string& texName);
   void attachTexOut(const string& shaderName, const string& texName); //
   // load uniform variables from shader files
   void attachUniform(const string& pgmName, const char* uName, GLenum type, void* glLocation);

   // rendering process, with all associated Framebuffer/Texture/Uniform activation
   void render(const string &shaderName);
   // renderMain() does the final render, which displays on the screen
   void renderMain();

   // ask about keyboard input
   bool isPressed(int key) const;
   // is it time to close the window yet?
   bool stayOpen() const;

 protected:
   // the window itself
   GLFWwindow* window;
   // window size
   int W, H;
   // OpenGL variables to keep track of the 2 triangles spanning the viewport
   GLuint vao, vbo;
   // we use the same vertex shader for all the fragment shader programs
   GLuint vertShader;
   // turn this to false when it's time for the window to close
   bool shouldStayOpen;

   // handle several buffers at once
   vector<ShaderShell> shaders;
   vector<TexShell> textures;
   // use these pointers to do several operations at once on a specific object
   ShaderShell* focusShader;
   TexShell *focusTex;

   // set up GLFW in general; call this before initializing any instance
   static int glfwStart();
   // set up universal OpenGL contexts
   void setupGL();
   // load shaders
   void loadVertShaderFile(); // called only once
   void loadFragShaderFile(); // called every time a new fragment shader is requested
   // universal texture settings
   static void texConfig();
   // this wrapper makes uniform activation convenient, despite various types
   static void activateUniform(const UniformShell& focusU); // TODO: virtual GlobShell::activate()?

   // two triangles to cover the viewport
   static const GLfloat fullViewTriangles[8];
   // array of color attachment codes to enable multiple output textures
   static const GLenum colorAttachmentCodes[16];
};
