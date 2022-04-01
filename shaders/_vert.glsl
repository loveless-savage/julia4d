#version 460 core
layout (location=0) in vec2 screenCoord;
out vec4 pixelRayCoord;
out vec2 texCoord;

void main(){
   gl_Position = vec4(screenCoord,0.f,1.f);
   pixelRayCoord = gl_Position * 2.f;
   texCoord = screenCoord + vec2(1.f);
   texCoord /= 2;
}