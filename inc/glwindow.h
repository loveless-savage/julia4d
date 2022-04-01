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

struct GlobShell {
 public:
   string name;
   GLuint ID;
};
struct UniformShell {
   string name;
   GLint ID;
   GLenum type;
   void* location;
   int size = 0;
};
struct ShaderShell : GlobShell {
 public:
   vector<GlobShell*> texturesIn;
   vector<UniformShell> uniforms;
   //vector<GlobShell*> fbosOut;
};
// search a vector of globs to find the matching name
GlobShell* findGlobByName(vector<GlobShell>& globs, const string& searchName);
//UniformShell* findGlobByName(vector<UniformShell>& globs, const string& searchName);
ShaderShell* findGlobByName(vector<ShaderShell>& globs, const string& searchName);


class Glwindow {
 public:
   // constructor
   Glwindow(int width, int height);
   // destructor
   ~Glwindow();

   // set up a new shader to be used on demand
   void addShader(const string& shaderName);
   // set up a new texture to be used by whichever shader likes
   void addFBOTexture(const string& texName);

   // load uniform variables from shader files
   void attachUniform(const string& pgmName, const char* uName, GLenum type, void* glLocation);
   // associate a texture to a shader so they both get bound together
   void attachTexInput(const string& shaderName, const string& texName);

   // run this every frame
   void renderMain(const string& shaderName);
   // in case we want to render off-screen
   void renderToTex(const string& shaderName, const string& texName);

   // ask about keyboard input
   bool isPressed(int key) const;
   // get pointer to the loop termination boolean
   bool stayOpen() const;

 protected:
   // the window itself
   GLFWwindow* win;
   // window size
   int W, H;
   // OpenGL variables
   GLuint vao, vbo;
   // turn this to false when it's time for the window to close
   bool shouldStayOpen;

   // handle several buffers at once: each entry has
   // a name, shader program ID & clear color associated with it
   vector<ShaderShell> shaders;
   ShaderShell* focusShader; // use this when we want to find a shader by name
   // handle several textures at once: each entry has
   // a name & shader program ID associated with it
   vector<GlobShell> textures;
   vector<GlobShell> fbos;
   GlobShell *focusTex, *focusFBO; // use this when we want to find a texture by name

   // set up GLFW in general; call this before initializing any instance
   static int glfwStart();
   // set up universal OpenGL contexts
   void setupGL();
   // load shaders
   void loadShaders();
   // universal texture settings
   void texConfig();
   // this wrapper makes uniform activation convenient, despite various types
   void activateUniform(const UniformShell& focusU);
};


// two triangles to cover the viewport
extern const GLfloat fullViewTriangles[8];
