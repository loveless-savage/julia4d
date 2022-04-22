#pragma once
#include <iostream>
#include <string>
#include <vector>
#ifndef GLEW_STATIC
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#endif // GLEW_STATIC

using namespace std;

struct GlobShell {
 public:
   string name;
   GLuint ID;
 protected:
   GlobShell();
   GlobShell(const string& name, GLuint ID);
   template <class T>
         static T* findByName(vector<T>& globs, const string& searchName);
};

struct UniformShell : GlobShell {
 public:
   GLenum type;
   void* location;
   //int size = 0;
   UniformShell();
   UniformShell(const string& name, GLint ID, GLenum type, void* location);
   static UniformShell* findByName(vector<UniformShell>& globs, const string& searchName);
};

struct TexShell : GlobShell {
 public:
   TexShell();
   TexShell(const string& name, GLuint ID);
   static TexShell* findByName(vector<TexShell>& globs, const string& searchName);
};

struct FBOShell : GlobShell {
 public:
   vector<TexShell*> texturesOut;
   FBOShell();
   FBOShell(const string& name, GLuint ID);
   static FBOShell* findByName(vector<FBOShell>& globs, const string& searchName);
};

struct ShaderShell : GlobShell {
 public:
   vector<UniformShell> uniforms;
   vector<TexShell*> texturesIn;
   vector<TexShell*> texturesOut;
   GLuint fboID;
   FBOShell* fbo;
   ShaderShell();
   ShaderShell(const string& name, GLuint ID);
   static ShaderShell* findByName(vector<ShaderShell>& globs, const string& searchName);
};
