#version 460 core
layout (location=0) in vec2 screenCoord;

// load these uniforms for per-pixel interpolation
uniform vec4 cameraX, cameraY;

out vec2 texCoord;
out vec4 stepX, stepY;

void main(){
   gl_Position = vec4(screenCoord, 0.f, 1.f);
   texCoord = screenCoord + vec2(1.f);
   texCoord /= 2;

   // sideways displacement, based on where on the screen our pixel is
   // TODO: interpolate texCoord only, scale stepX & stepY pre-shader
   stepX = cameraX;
   stepY = cameraY;
   // (this value will be interpolated)
   stepX *= screenCoord.x;
   stepY *= screenCoord.y;

   // ray direction will be interpolated
}