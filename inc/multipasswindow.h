#pragma once
#include "glwindow.h"


class MultipassWindow : public Glwindow {
 public:
   // constructor is inherited from Glwindow
   MultipassWindow(int width, int height);

   // set up a multipass texture pair
   void addMultipassTex(const string& mpName);
   // attach the multipass texture pair to a shader
   void attachMultipassTexIO(const string& shaderName, const string& mpName);

   // render using the double-texture buffer so we can render across multiple frames
   void renderToMPTex(const string& shaderName, const string& mpName);

   // access a different texture without changing the name
   void swapTextures(const string& texA, const string& texB);
 private:
   vector<GlobShell> mpTextures;
};