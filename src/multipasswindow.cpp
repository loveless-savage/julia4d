#include "multipasswindow.h"

// use constructor defined by Glwindow
MultipassWindow::MultipassWindow(int width, int height) : Glwindow(width,height) {}

// set up a multipass texture pair
void MultipassWindow::addMultipassTex(const string& mpName){
   addIOTexture(mpName);
   // this texture glob is never rendered to; it is used to hold the ID of the other framebuffer
   addIOTexture(mpName + "_I");
}

// attach the multipass texture pair to a shader
void MultipassWindow::attachMultipassTexIO(const string& shaderName, const string& mpName){
   attachTexIn(shaderName, mpName + "_I");
}

// render using the double-texture buffer so we can render across multiple frames
void MultipassWindow::renderToMPTex(const string& shaderName, const string& mpName){
   // first things first; since this is a multipass, start by swapping to the older of the two buffers
   swapTextures(mpName, mpName + "_I");

   focusShader = ShaderShell::findByName(shaders, shaderName);
   glUseProgram(focusShader->ID);
   // which framebuffer are we using?
   focusShader = ShaderShell::findByName(shaders, shaderName);
   glBindFramebuffer(GL_FRAMEBUFFER, focusShader->fboID);
   // we're only using two triangles so depth testing isn't necessary
   glDisable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT);
   if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      cout << "Framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
   }

   // input textures
   for (int i=0; i<focusShader->texturesIn.size(); i++) {
      focusTex = focusShader->texturesIn.at(i);
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, focusTex->ID);
      // add this so we can reference the texture "mpName_I" as "mpName" in the shader
      if(mpName + "_I" == focusTex->name){
         glUniform1i(glGetUniformLocation(focusShader->ID, mpName.c_str()), i);
      } else {
         glUniform1i(glGetUniformLocation(focusShader->ID, focusTex->name.c_str()), i);
      }
   }

   glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

// swap textures: we use this helper function for our multipass iterator
void MultipassWindow::swapTextures(const string& texA, const string& texB){
   focusTex = TexShell::findByName(textures, texA);
   focusShader = ShaderShell::findByName(shaders, texA);
   TexShell* focusTex2 = TexShell::findByName(textures, texB);
   ShaderShell* focusShader2 = ShaderShell::findByName(shaders, texB);

   GLuint texA_ID = focusTex->ID;
   focusTex->ID = focusTex2->ID;
   focusTex2->ID = texA_ID;

   GLuint ShaderA_fboID = focusShader->fboID;
   focusShader->fboID = focusShader2->fboID;
   focusShader2->fboID = ShaderA_fboID;
}