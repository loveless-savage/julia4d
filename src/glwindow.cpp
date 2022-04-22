#include "glwindow.h"



// constructor
Glwindow::Glwindow(int width, int height)
      : W(width), H(height), shouldStayOpen(true)
{
   glfwStart();
   window = glfwCreateWindow(W, H, "julia4D", NULL, NULL);
   if(!window){
      cerr << "Couldn't create window" << endl;
      throw exception();
   }
   glfwMakeContextCurrent(window);
   glewExperimental = true;
   if(glewInit() != GLEW_OK){
      cerr << "Couldn't initialize glew" << endl;
      throw exception();
   }
   setupGL();

   // the shader main_frag.glsl always renders to the screen, so set it up w/out an attached FBO
   shaders.push_back({});
   focusShader = &(shaders.at(0));
   focusShader->name = "main";
   focusShader->fboID = 0;
   loadShaderFiles();
};
// destructor
Glwindow::~Glwindow(){
   glfwDestroyWindow(window);
   glfwTerminate();
};

// set up a new buffer with a shader of its own
void Glwindow::addShader(const string& shaderName){
   shaders.push_back({});
   focusShader = &(shaders.back());
   focusShader->name = shaderName;
   loadShaderFiles();
   // every shader has an associated FrameBuffer Object (FBO)
   glGenFramebuffers(1,&(focusShader->fboID));
}

// set up a new texture to be used by whichever shader likes
void Glwindow::addIOTexture(const string& texName){
   textures.push_back({});
   focusTex = &(textures.back());
   focusTex->name = texName;
   glGenTextures(1,&(focusTex->ID));
   glBindTexture(GL_TEXTURE_2D, focusTex->ID);
   // give empty texture data, since we'll use a shader to write to it
   glTexImage2D(GL_TEXTURE_2D,
                0,
                GL_RGBA,
                W,
                H,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                nullptr);
   // other general texture settings
   texConfig();
}

// let the Glwindow object know to assign a texture as input to a shader
void Glwindow::attachTexIn(const string& shaderName, const string& texName){
   // fetch from the shader and texture vectors by name
   focusShader = ShaderShell::findByName(shaders, shaderName);
   focusShader->texturesIn.push_back( TexShell::findByName(textures, texName) );
}

// let the Glwindow object know to assign a texture as input to a shader
void Glwindow::attachTexOut(const string& shaderName, const string& texName) {
   focusShader = ShaderShell::findByName(shaders, shaderName);
   glBindFramebuffer(GL_FRAMEBUFFER,focusShader->fboID);

   // add texture to list of outputs
   focusShader->texturesOut.push_back(focusTex);

   // how many texture outputs are there? We will bind this one after all the others
   GLuint texOutCount = focusShader->texturesOut.size() - 1;
   // connect texture to fbo in the OpenGL context
   focusTex = TexShell::findByName(textures, texName);
   glFramebufferTexture2D(
         GL_FRAMEBUFFER,
         GL_COLOR_ATTACHMENT0 + texOutCount,
         GL_TEXTURE_2D,
         focusTex->ID,
         0
      );
   // error checking
   if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
      cerr << "Framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
   }
}

// load uniform variables from shader files
void Glwindow::attachUniform(const string& pgmName, const char* uName, GLenum type, void* glLocation){
   focusShader = ShaderShell::findByName(shaders, pgmName);
   GLint uLocation = glGetUniformLocation(focusShader->ID,uName);
   if (uLocation < 0){
      cerr << "ERROR: cannot retrieve uniform variable '" << uName << "' from program '" << pgmName << "' : loc=" << uLocation << endl;
   }
   focusShader->uniforms.push_back({uName, uLocation, type, glLocation});
}


// in case we want to render off-screen
void Glwindow::render(const string &shaderName) {
   // bring shader and associated FBO into context
   focusShader = ShaderShell::findByName(shaders, shaderName);
   glUseProgram(focusShader->ID);
   glBindFramebuffer(GL_FRAMEBUFFER,focusShader->fboID);
   // error checking for FBO binding
   if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      cerr << "Framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
   }

   // activate input uniforms associated with this shader
   for (const UniformShell& focusU : focusShader->uniforms) {
      activateUniform(focusU);
   }

   // input textures
   for (int i=0; i<focusShader->texturesIn.size(); i++) {
      focusTex = focusShader->texturesIn.at(i);
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, focusTex->ID);
      glUniform1i(glGetUniformLocation(focusShader->ID, focusTex->name.c_str()), i);
   }

   // output textures are already bound, but we need to make them visible
   if (focusShader->fboID != 0) {
      GLuint texOutCount = focusShader->texturesOut.size();
      if (texOutCount == 0){
         cerr << "shader '" << shaderName << "' has no output textures bound!" << endl;
      }
      glDrawBuffers(texOutCount, Glwindow::colorAttachmentCodes);
   }

   // we're only using two triangles so depth testing isn't necessary
   glDisable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT);
   // send it all off to the graphics card!
   glDrawArrays(GL_TRIANGLE_STRIP,0,4);

}

// run this to render to the screen
void Glwindow::renderMain() {
   // rendering to the screen just includes a couple extra steps after normal rendering
   render("main");

   // finalize drawing to the screen
   glfwSwapBuffers(window);
   // collect keyboard and mouse input
   glfwPollEvents();
   // if escape key is pressed, send the signal to terminate the program
   shouldStayOpen =
         glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0;
}

////////////////////////////////////////////////////////////////

// set up GLFW in general; call this before initializing any instance
int Glwindow::glfwStart() {
   if(!glfwInit()){
      cerr << "Failed to intialize GLFW" << endl;
      return -1;
   }
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
   return 0;
};

// set up various circumstances related to OpenGL
void Glwindow::setupGL(){
   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
   glClearColor(1.,0.,0.,1.);

   // set up buffers to hold triangle vertices
   glGenVertexArrays(1,&vao);
   glGenBuffers(1,&vbo);
   // give our array of viewport-filling triangles to the OpenGL context
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Glwindow::fullViewTriangles), Glwindow::fullViewTriangles, GL_STATIC_DRAW);
   // pass triangle data to vertex shader
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(
         0,
         2,
         GL_FLOAT,
         GL_FALSE,
         2*sizeof(float),
         (void*)0
   );

   // TODO: bind texture uv's using vertex coordinates
   // or not... since we can access textures from shaders using screen coordinates
   //glEnableVertexAttribArray(1);
   //glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0 );
};

// load shaders
void Glwindow::loadShaderFiles(){
   // Read the Vertex Shader code from the file- always the same default vertex shader
   string vertShaderName = "shaders/_vert.glsl";
   ifstream VertexShaderStream(vertShaderName);
   stringstream sstr;
   string vertSrcString;
   if(VertexShaderStream.is_open()){
      sstr << VertexShaderStream.rdbuf();
      vertSrcString = sstr.str();
      VertexShaderStream.close();
   }else{
      cout << "Impossible to open shaders/_vert.glsl" << endl;
      getchar();
      return;
   }
   const char* const vertShaderSrc = vertSrcString.c_str();

   // collect & compile vertex shader
   GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
   glCompileShader(vertShader);
   // detect compilation errors
   int shaderCompileSuccess;
   glGetShaderiv(vertShader, GL_COMPILE_STATUS, &shaderCompileSuccess);
   if(!shaderCompileSuccess) {
      char infoLog[512];
      glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
      cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
   }


   // Read the Fragment Shader code from the file
   string fragShaderName = "shaders/" + focusShader->name + "_frag.glsl";
   ifstream FragmentShaderStream(fragShaderName);
   string fragSrcString;
   sstr.str(""); // clear vertex shader from string stream
   if(FragmentShaderStream.is_open()){
      sstr << FragmentShaderStream.rdbuf();
      fragSrcString = sstr.str();
      FragmentShaderStream.close();
   }

   const char* const fragShaderSrc = fragSrcString.c_str();
   // collect & compile fragment shader
   GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
   glCompileShader(fragShader);
   // detect compilation errors
   glGetShaderiv(fragShader, GL_COMPILE_STATUS, &shaderCompileSuccess);
   if(!shaderCompileSuccess) {
      char infoLog[512];
      glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
      cerr << fragShaderName << " FAILED TO COMPILE:\n" << infoLog << endl;
   }

   // create new shader program & link it to compiled shaders
   focusShader->ID = glCreateProgram();
   glAttachShader(focusShader->ID, vertShader);
   glAttachShader(focusShader->ID, fragShader);
   glLinkProgram (focusShader->ID);
   glGetProgramiv(focusShader->ID, GL_LINK_STATUS, &shaderCompileSuccess);
   if (!shaderCompileSuccess){
      char infoLog[512];
      glGetProgramInfoLog(focusShader->ID, 512, NULL, infoLog);
      cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << endl;
   }
   glDetachShader(focusShader->ID, vertShader);
   glDetachShader(focusShader->ID, fragShader);

   glDeleteShader(vertShader);
   glDeleteShader(fragShader);
}

// universal texture settings
void Glwindow::texConfig(){
// these settings should be redundant, since they relate to textures of different sizes to the image,
// but just for security purposes
   // border behavior: just show black if texture is too small
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
   float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
   glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

   // scaling behavior
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// this wrapper makes uniform activation convenient, despite various types
void Glwindow::activateUniform(const UniformShell& focusU){
   switch (focusU.type) {
      case GL_INT:
         glUniform1i(focusU.ID, (*(int *)(focusU.location)));
         break;
      case GL_INT_VEC2:
         glUniform2i(focusU.ID, *(int *)(focusU.location),
                               *((int *)(focusU.location)+1));
         break;
      case GL_INT_VEC3:
         glUniform3i(focusU.ID, *(int *)(focusU.location),
                               *((int *)(focusU.location)+1),
                               *((int *)(focusU.location)+2));
         break;
      case GL_INT_VEC4:
         glUniform4i(focusU.ID, *(int *)(focusU.location),
                               *((int *)(focusU.location)+1),
                               *((int *)(focusU.location)+2),
                               *((int *)(focusU.location)+3));
         break;

      case GL_FLOAT:
         glUniform1f(focusU.ID, *(float *)(focusU.location));
         break;
      case GL_FLOAT_VEC2:
         glUniform2f(focusU.ID, *(float *)(focusU.location),
                               *((float *)(focusU.location)+1));
         break;
      case GL_FLOAT_VEC3:
         glUniform3f(focusU.ID, *(float *)(focusU.location),
                               *((float *)(focusU.location)+1),
                               *((float *)(focusU.location)+2));
         break;
      case GL_FLOAT_VEC4:
         glUniform4f(focusU.ID, *(float *)(focusU.location),
                               *((float *)(focusU.location)+1),
                               *((float *)(focusU.location)+2),
                               *((float *)(focusU.location)+3));
         break;
      default:
         cout << "UNIFORM ERROR: unrecognized type for uniform " << focusU.name << endl;
         throw exception();
   }
}

// ask about keyboard input
bool Glwindow::isPressed(int key) const {
   return (glfwGetKey(window, key) == GLFW_PRESS);
}

// see if it's time to close the window
bool Glwindow::stayOpen() const { return shouldStayOpen; }

// two triangles to cover the viewport
const GLfloat Glwindow::fullViewTriangles[8] = {
      -1.f,-1.f,
      -1.f,1.f,
      1.f,-1.f,
      1.f,1.f
};

// array of color attachment codes to enable multiple output textures
const GLenum Glwindow::colorAttachmentCodes[16] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2,
      GL_COLOR_ATTACHMENT3,
      GL_COLOR_ATTACHMENT4,
      GL_COLOR_ATTACHMENT5,
      GL_COLOR_ATTACHMENT6,
      GL_COLOR_ATTACHMENT7,
      GL_COLOR_ATTACHMENT8,
      GL_COLOR_ATTACHMENT9,
      GL_COLOR_ATTACHMENT10,
      GL_COLOR_ATTACHMENT11,
      GL_COLOR_ATTACHMENT12,
      GL_COLOR_ATTACHMENT13,
      GL_COLOR_ATTACHMENT14,
      GL_COLOR_ATTACHMENT15
};
