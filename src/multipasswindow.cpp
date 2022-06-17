#include "multipasswindow.h"

// use constructor defined by Glwindow
MultipassWindow::MultipassWindow(int width, int height) : Glwindow(width,height) {}

// set up a multipass texture pair
void MultipassWindow::addMPTex(const string &mpName, const GLint internalFormat) {
   addIOTexture(mpName, GL_RGBA32F);

   mpTextures.emplace_back();
   focusMPTex = &(mpTextures.back());
   focusMPTex->name = mpName;
   focusMPTex->swapTex = &(textures.back());
   glGenTextures(1,&(focusMPTex->ID));
   glBindTexture(GL_TEXTURE_2D, focusMPTex->ID);
   // give empty texture data, since we'll use a shader to write to it
   glTexImage2D(GL_TEXTURE_2D,
                0,
                internalFormat,
                W,
                H,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr);
   // other general texture settings
   texConfig();
}

// attach the multipass texture pair to a shader
void MultipassWindow::attachMPTexIO(const string& shaderName, const string& mpName){
   focusShader = ShaderShell::findByName(shaders, shaderName);

   // input texture will actually be wrapped in an MPTexShell object
   focusMPTex = MPTexShell::findByName(mpTextures, mpName, true);
   if (focusMPTex == nullptr) { // not a multipass texture
      cerr << "attachMPTexIO(): focusShader = '" << shaderName << "', but multipass texture '" << mpName << "' doesn't exist!" << endl;
      return;
   }
   focusShader->texturesIn.push_back(focusMPTex);

   // use overloaded function from class MultipassWindow
   attachTexOut(shaderName, mpName);

   focusShader->isMultipass = true;
}

void MultipassWindow::attachTexOut(const string &shaderName, const string &texName) {
   Glwindow::attachTexOut(shaderName, texName);

   // check for multipass textures
   focusMPTex = MPTexShell::findByName(mpTextures,texName, true);
   if (focusMPTex == nullptr) return;
   // if we didn't return, we need to add the shader to this multipass texture's index
   // this way, when we swap textures we can rebind it to this shader
   focusMPTex->shaders.push_back(focusShader);
}

// some shaders are set up to render over top of a texture, so we have a couple extra rendering steps
void MultipassWindow::render(const string &shaderName) {
   focusShader = ShaderShell::findByName(shaders,shaderName);

   // check the list of textures attached to the current shader, to see if any are multipass
   if (focusShader->isMultipass) {
      for (auto &checkTex: focusShader->texturesOut) {
         swapMPTex(checkTex->name); // if the texture isn't multipass, this does nothing
      }
   }

   Glwindow::render(shaderName);
}

// swap textures: we use this helper function for our multipass iterator
void MultipassWindow::swapMPTex(const string& mpName) {
   focusMPTex = MPTexShell::findByName(mpTextures,mpName, true);
   // this function does nothing if the texture isn't set up as a swap texture
   if (focusMPTex == nullptr) return;

   // first of all, swap ID's so we can access them swapped
   GLuint swap_ID = focusMPTex->ID;
   focusMPTex->ID = focusMPTex->swapTex->ID;
   focusMPTex->swapTex->ID = swap_ID;

   // now rebind the swapped texture to all associated shaders
   for (auto& currentShader : focusMPTex->shaders) {
      // where is the multipass texture in the vector of textures attached to the shader?
      GLuint texOutIdx = 0;
      for (int i = 0; i < currentShader->texturesOut.size(); i++){
         if (currentShader->texturesOut.at(i) == focusMPTex->swapTex) {
            texOutIdx = i;
            // we only want to rebind the current multipass texture - we'll get to any others later
            break;
         }
      }
      // connect texture to fbo in the OpenGL context
      glBindFramebuffer(GL_FRAMEBUFFER,currentShader->fboID);
      glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + texOutIdx,
            GL_TEXTURE_2D,
            focusTex->ID,
            0
      );
      // error checking
      if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
         cerr << "swapMPTex(): Framebuffer error binding shader '" << currentShader->name << "' to texture '" << mpName << "':" << endl;
         cerr << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
      }
   }
}