#version 460 core
layout (location=0) in vec3 pos;
out vec4 pixelRayCoord;

void main(){
   gl_Position = vec4(pos,1.f);
   pixelRayCoord = gl_Position * 2.f;
}