#version 460 core

in vec2 texCoord;
//in vec4 stepX, stepY;

uniform int mode;
//uniform vec4 cameraDir;
uniform sampler2D juliaTex;
uniform sampler2D fixme;
uniform sampler2D dJuliaTex;
uniform sampler2D pos;
uniform sampler2D dzTex;

out vec4 fragColor;

void main() {
    /*
    // circular viewport
    vec2 z = texCoord.xy * 2.f;
    z -= vec2(1.f);
    if(length(z) > 1.f){
        fragColor = vec4(0.f);
        return;
    }
    */

    vec4 dzIn = texture(dJuliaTex,texCoord).rgba;

    float focusVal;
    switch (mode) {
        case 4:
        focusVal = dzIn.w;
        break;
        case 3:
        focusVal = dzIn.z;
        break;
        case 2:
        focusVal = dzIn.y;
        break;
        case 1:
        focusVal = dzIn.x;
        break;
        default:
        focusVal = texture(juliaTex,texCoord).x / 1.f; // only the x component was written to
    }
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
