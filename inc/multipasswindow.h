#pragma once
#include "glwindow.h"


class MultipassWindow : public Glwindow {
 public:
   // constructor is inherited from Glwindow
   MultipassWindow(int width, int height);

   // set up a multipass texture pair
   void addMPTex(const string &mpName, const GLint internalFormat = GL_RGBA32F); // default format = 4-color, non-clamped floating value per fragment
   // attach the multipass texture pair to a shader
   void attachMPTexIO(const string& shaderName, const string& mpName);
   // overload parent function with an extra feature to check for multipass textures
   void attachTexOut(const string& shaderName, const string& texName);

   // some shaders are set up to render over top of a texture, so we have a couple extra rendering steps
   void render(const string &shaderName);

   // access a different texture without changing the name
   void swapMPTex(const string& mpName);
 protected:
   vector<MPTexShell> mpTextures;
   MPTexShell* focusMPTex;
};