#include "multipasswindow.h"


// use constructor defined by Glwindow
MultipassWindow::MultipassWindow(int width, int height, const string& mainShaderName) : Glwindow(width, height, mainShaderName) {}

MultipassWindow::~MultipassWindow() {
   for (auto& m : mpTextures) delete m;
   // automatically calls parent destructor after this instruction
}

// set up a multipass texture pair
void MultipassWindow::addMPTex(const string &mpName, const GLint internalFormat) {
   // to read from this texture couple, use "mpNameIn" instead of "mpName"
   addIOTexture(mpName, internalFormat);

   mpTextures.push_back(new MPTexShell());
   focusMPTex = mpTextures.back();
   focusMPTex->name = mpName;
   focusMPTex->swapTex = focusTex; // focusTex was set by addIOTexture() above

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
   Glwindow::attachTexOut(shaderName, mpName);

   focusShader->isMultipass = true;
}

void MultipassWindow::attachTexOut(const string &shaderName, const string &texName) {
   // this overload is the same as default behavior, with an extra step
   Glwindow::attachTexOut(shaderName, texName);

   // check whether this is a multipass texture
   focusMPTex = MPTexShell::findByName(mpTextures,texName, true);
   if (focusMPTex == nullptr) return;
   // if we didn't return, we need to add the shader to this multipass texture's index
   // this way, when we swap textures we can rebind it to this shader
   focusMPTex->writingShaders.push_back(focusShader);
}

// some shaders are set up to render over top of a texture, so we have a couple extra rendering steps
void MultipassWindow::render(const string &shaderName) {
   focusShader = ShaderShell::findByName(shaders,shaderName);

   // TODO: check whether this is a robust way to chain multipass swapping
   // swap all multipass textures associated with this shader
   if (focusShader->isMultipass) {
      for (auto &checkTex: focusShader->texturesOut) {
         swapMPTex(checkTex->name); // if the texture isn't multipass, this does nothing
      }
   }

   Glwindow::render(shaderName);
}

// swap textures: we use this helper function for our multipass iterator
// TODO: buffers get swapped multiple times when there are multiple textures
void MultipassWindow::swapMPTex(const string& mpName) {
   focusMPTex = MPTexShell::findByName(mpTextures,mpName, true);
   // this entire function does nothing if we tried to access a non-multipass texture
   if (focusMPTex == nullptr) return;

   //cout << "***SWAP*** mpID(texID): ";
   //cout << focusMPTex->ID << "(" << focusMPTex->swapTex->ID << ") => ";

   // first of all, swap ID's so we can access them swapped
   GLuint swap_ID = focusMPTex->ID;
   focusMPTex->ID = focusMPTex->swapTex->ID;
   focusMPTex->swapTex->ID = swap_ID;

   focusTex = TexShell::findByName(textures, mpName);
   //cout << focusMPTex->ID << "(" << focusMPTex->swapTex->ID << ")" << endl;
   //cout << "swapAddr direct = " << focusTex << ", chained = " << focusMPTex->swapTex << endl;
   // now rebind the swapped texture to all associated shaders
   for (auto& writingShader : focusMPTex->writingShaders) {
      focusShader = writingShader;
      // find the multipass texture in the vector of textures attached to the shader
      GLuint texOutIdx = 0;
      for (int i = 0; i < focusShader->texturesOut.size(); i++){
         if (focusShader->texturesOut.at(i) == focusMPTex->swapTex) {
            texOutIdx = i;
            // we only want to rebind the current multipass texture - we'll get to any others later
            break;
         }
      }
      //cout << "Rebinding texture " << focusShader->texturesOut.at(texOutIdx)->name;
      //cout << " to shader " << focusShader->name << endl;
      // connect opposite texture ID to fbo in the OpenGL context
      glBindFramebuffer(GL_FRAMEBUFFER,focusShader->fboID);
      glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + texOutIdx,
            GL_TEXTURE_2D,
            focusMPTex->swapTex->ID,
            0
      );
      // error checking
      if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
         cerr << "swapMPTex(): Framebuffer error binding shader '" << focusShader->name << "' to texture '" << mpName << "':" << endl;
         cerr << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
      }
   }
}

void MultipassWindow::dump(bool showShaders, bool showTextures, bool showMPTex) {
   cout << "<<<WINDOW DATA DUMP>>>" << endl;

   if(showShaders){
      cout << "shaders:" << endl;
      for (auto& s : shaders){
         focusShader = s;
         cout << "--" << focusShader->ID << " " << focusShader->name << endl;
      }
   }
   if(showTextures){
      cout << "textures:" << endl;
      for (auto& t : textures){
         focusTex = t;
         cout << "--" << focusTex->ID << " " << focusTex->name << endl;
      }
   }
   if(showMPTex){
      cout << "MPTextures:" << endl;
      for (auto& m : mpTextures){
         focusMPTex = m;
         cout << "--" << focusMPTex->ID << " (" << focusMPTex->swapTex->ID << ") " << focusMPTex->name << endl;
      }
   }
}
