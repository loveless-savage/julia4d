#include "glwindow.h"


// constructor
Glwindow::Glwindow(int width, int height){
   glfwStart();
   win = glfwCreateWindow(width, height, "heey",NULL,NULL);
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
};
// destructor
Glwindow::~Glwindow(){
   glfwDestroyWindow(win);
   glfwTerminate();
};

// set up a new buffer with a shader of its own
void Glwindow::addBuffer(const string &bufferName){
   buffers.push_back({});
   buffers.back().name = bufferName;
   loadShaders(buffers.size()-1);
}

// run this every frame
int Glwindow::loop(){
   glClear(GL_COLOR_BUFFER_BIT);

   glUseProgram(buffers.at(0).pgmID); // TODO: select buffer based on string input name

   glDrawArrays(GL_TRIANGLE_STRIP,0,4);

   glfwSwapBuffers(win);
   glfwPollEvents();
   return (glfwGetKey(win, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(win) == 0);
}

// set up GLFW in general; call this before initializing any instance
int Glwindow::glfwStart() const {
   if(!glfwInit()){
      cerr << "Failed to intialize GLFW" << endl;
      return -1;
   }
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,6);
   return 0;
};

// two triangles to cover the viewport
const GLfloat fullViewTriangles[12] = {
      -1.f,-1.f,0.f,
      -1.f,1.f,0.f,
      1.f,-1.f,0.f,
      1.f,1.f,0.f
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
         3,
         GL_FLOAT,
         GL_FALSE,
         3*sizeof(float),
         (void*)0
   );
};


// load shaders
void Glwindow::loadShaders(int bufferIdx){
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
   string fragShaderName = "shaders/" + buffers.at(bufferIdx).name + "_frag.glsl";
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
   buffers.at(bufferIdx).pgmID = glCreateProgram();
   glAttachShader(buffers.at(bufferIdx).pgmID, vertShader);
   glAttachShader(buffers.at(bufferIdx).pgmID, fragShader);
   glLinkProgram(buffers.at(bufferIdx).pgmID);
   glGetProgramiv(buffers.at(bufferIdx).pgmID, GL_LINK_STATUS, &shaderCompileSuccess);
   if (!shaderCompileSuccess){
      char infoLog[512];
      glGetProgramInfoLog(buffers.at(bufferIdx).pgmID, 512, NULL, infoLog);
      cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << endl;
   }
   glDetachShader(buffers.at(bufferIdx).pgmID, vertShader);
   glDetachShader(buffers.at(bufferIdx).pgmID, fragShader);

   glDeleteShader(vertShader);
   glDeleteShader(fragShader);
}
