#version 460 core

in vec4 pixelRayCoord;
in vec2 texCoord;

out vec4 fragColor;

uniform int mode;
uniform sampler2D juliaTex;
uniform sampler2D dJuliaTex;
//uniform sampler2D mandelbrotTex;

void main() {
    vec4 dz = texture(dJuliaTex,texCoord).rgba;

    float focusVal;
    switch (mode) {
        case 4:
        focusVal = dz.w;
        break;
        case 3:
        focusVal = dz.z;
        break;
        case 2:
        focusVal = dz.y;
        break;
        case 1:
        focusVal = dz.x;
        break;
        default:
        focusVal = texture(juliaTex,texCoord).r;
    }
    if(focusVal == 1.f){
        fragColor = vec4(0.f);
        return;
    }
    //focusVal *= 4.f;
    vec3 v = vec3(1.f,0.f,0.f);
    vec3 q = vec3(sin(focusVal));
    vec3 temp = cross(q.xyz, v) + v * cos(focusVal);


    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);
    //fragColor = texture(juliaTex,texCoord).rgba;
    //fragColor /= 2.f;
}
