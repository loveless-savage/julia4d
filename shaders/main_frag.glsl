#version 460 core

in vec2 texCoord;
//in vec4 stepX, stepY;

//uniform vec4 cameraDir;
uniform sampler2D pos;
uniform sampler2D dzTex;

out vec4 fragColor;

void main() {

    vec4 dzIn = texture(dzTex,texCoord).rgba;

    float focusVal;
    focusVal = texture(pos,texCoord).x / 1.f; // only the x component was written to
    if(focusVal < 0.f) {
        fragColor = vec4(0.f);
        return;
    }

    // rainbow spectrum for elevation map
    vec3 v = vec3(1.f,0.f,0.f);
    vec3 q = vec3(sin(focusVal));
    vec3 temp = cross(q.xyz, v) + v * cos(focusVal);
    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);
}
