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
         static T* findByName(vector<T>& globs, const string& searchName, bool allowSearchFail = false);
};

struct UniformShell : GlobShell {
 public:
   GLenum type;
   void* location;
   //int size = 0;
   UniformShell();
   UniformShell(const string& name, GLint ID, GLenum type, void* location);
   static UniformShell* findByName(vector<UniformShell>& globs, const string& searchName, bool allowSearchFail = false);
};

struct TexShell : GlobShell {
 public:
   GLint type;
   TexShell();
   TexShell(const string& name, GLuint ID);
   static TexShell* findByName(vector<TexShell>& globs, const string& searchName, bool allowSearchFail = false);
};

struct ShaderShell : GlobShell {
 public:
   vector<UniformShell> uniforms;
   vector<TexShell*> texturesIn;
   vector<TexShell*> texturesOut;
   GLuint fboID;
   bool isMultipass = false;
   ShaderShell();
   ShaderShell(const string& name, GLuint ID);
   static ShaderShell* findByName(vector<ShaderShell>& globs, const string& searchName, bool allowSearchFail = false);
};

struct MPTexShell : TexShell {
 public:
   TexShell* swapTex;
   vector<ShaderShell*> shaders;
   MPTexShell();
   MPTexShell(const string& name, GLuint ID);
   static MPTexShell* findByName(vector<MPTexShell>& globs, const string& searchName, bool allowSearchFail = false);
};
