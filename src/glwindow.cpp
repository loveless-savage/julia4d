#include "glwindow.h"



// search a vector of globs to find the matching name
GlobShell* findGlobByName(vector<GlobShell>& globs, const string& searchName){
   // search through all members of the vector globs
   for(auto& glob : globs){
      if(glob.name == searchName){
         return &glob;
      }
   }
   // if we haven't returned yet there isn't a match in the vector
   cout << "Error: no match for glob named " << searchName << endl;
   throw exception();
}
/*
UniformShell* findGlobByName(vector<UniformShell>& globs, const string& searchName){
// search through all members of the vector globs
   for(auto& glob : globs){
      if(glob.name == searchName){
         return &glob;
      }
   }
   // if we haven't returned yet there isn't a match in the vector
   cout << "Error: no match for uniform named " << searchName << endl;
   throw exception();
}
 */
ShaderShell* findGlobByName(vector<ShaderShell>& globs, const string& searchName){
   // search through all members of the vector globs
   for(auto& glob : globs){
      if(glob.name == searchName){
         return &glob;
      }
   }
   // if we haven't returned yet there isn't a match in the vector
   cout << "Error: no match for shader named " << searchName << endl;
   throw exception();
}


// constructor
Glwindow::Glwindow(int width, int height)
      : W(width), H(height), shouldStayOpen(true)
{
   glfwStart();
   win = glfwCreateWindow(W, H, "julia4D",NULL,NULL);
   if(!win){
      cerr << "Couldn't create window" << endl;
      throw exception();
   }
   glfwMakeContextCurrent(win);
   glewExperimental = true;
   if(glewInit() != GLEW_OK){
      cerr << "Couldn't initialize glew" << endl;
      throw exception();
   }
   setupGL();

   // add default framebuffer as an option
   fbos.push_back({"main",0});
};
// destructor
Glwindow::~Glwindow(){
   glfwDestroyWindow(win);
   glfwTerminate();
};

// set up a new buffer with a shader of its own
void Glwindow::addShader(const string& shaderName){
   shaders.push_back({});
   focusShader = &(shaders.back());
   focusShader->name = shaderName;
   loadShaders();
   // by default, there is no attached texture
}

// set up a new texture to be used by whichever shader likes
void Glwindow::addFBOTexture(const string& texName){
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

   // each of these textures has an associated Framebuffer object
   fbos.push_back({});
   fbos.back().name = texName;
   glGenFramebuffers(1,&(fbos.back().ID));
   glBindFramebuffer(GL_FRAMEBUFFER,fbos.back().ID);

   // connect framebuffer and texture together
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, focusTex->ID, 0);
   // error checking
   if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
      cout << "Framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
      throw exception();
   }
}

// load uniform variables from shader files
void Glwindow::attachUniform(const string& pgmName, const char* uName, GLenum type, void* glLocation){
   focusShader = findGlobByName(shaders, pgmName);
   GLint uLocation = glGetUniformLocation(focusShader->ID,uName);
   if (uLocation < 0){
      cout << "ERROR: cannot retrieve uniform variable '" << uName << "' from program '" << pgmName << "' : loc=" << uLocation << endl;
      throw exception();
   }
   focusShader->uniforms.push_back({uName, uLocation, type, glLocation});
}

// associate a texture to a shader so they both get bound together
void Glwindow::attachTexInput(const string& shaderName, const string& texName){
   // fetch from the shader and texture vectors by name
   focusShader = findGlobByName(shaders, shaderName);
   focusShader->texturesIn.push_back(findGlobByName(textures, texName) );
}


// run this to render to the screen
void Glwindow::renderMain(const string& shaderName){
   focusShader = findGlobByName(shaders, shaderName);
   glUseProgram(focusShader->ID);

   // if we were using an off-screen framebuffer, this switches it back
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   // we're only using two triangles so depth testing isn't necessary
   glDisable(GL_DEPTH_TEST);
   glClearColor(0.f,0.f,0.f,1.f);
   glClear(GL_COLOR_BUFFER_BIT);

   for (int i=0; i<focusShader->texturesIn.size(); i++) {
      focusTex = focusShader->texturesIn.at(i);
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, focusTex->ID);
      glUniform1i(glGetUniformLocation(focusShader->ID, focusTex->name.c_str()), i);
   }

   // send it all off to the graphics card!
   glDrawArrays(GL_TRIANGLE_STRIP,0,4);

   // finalize drawing to the screen and listen for the escape key
   glfwSwapBuffers(win);
   glfwPollEvents();
   shouldStayOpen =
         glfwGetKey(win, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(win) == 0;
}

// in case we want to render off-screen
void Glwindow::renderToTex(const string& shaderName, const string& texName){
   // which framebuffer are we using?
   focusFBO = findGlobByName(fbos, texName);
   glBindFramebuffer(GL_FRAMEBUFFER, focusFBO->ID);
   // we're only using two triangles so depth testing isn't necessary
   glDisable(GL_DEPTH_TEST);
   glClear(GL_COLOR_BUFFER_BIT);
   if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      cout << "Framebuffer error " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << endl;
   }

   focusShader = findGlobByName(shaders, shaderName);
   glUseProgram(focusShader->ID);

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

   glDrawArrays(GL_TRIANGLE_STRIP,0,4);

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

// two triangles to cover the viewport
const GLfloat fullViewTriangles[8] = {
      -1.f,-1.f,
      -1.f,1.f,
      1.f,-1.f,
      1.f,1.f
};

// set up various circumstances related to OpenGL
void Glwindow::setupGL(){
   glfwSetInputMode(win, GLFW_STICKY_KEYS, GL_TRUE);
   glClearColor(1.,0.,0.,1.);

   // set up buffers to hold triangle vertices
   glGenVertexArrays(1,&vao);
   glGenBuffers(1,&vbo);
   // give our array of viewport-filling triangles to the OpenGL context
   glBindVertexArray(vao);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(fullViewTriangles), fullViewTriangles, GL_STATIC_DRAW);
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
void Glwindow::loadShaders(){
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
      cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
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
      cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
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
   return (glfwGetKey(win, key) == GLFW_PRESS);
}

// see if it's time to close the window
bool Glwindow::stayOpen() const { return shouldStayOpen; }