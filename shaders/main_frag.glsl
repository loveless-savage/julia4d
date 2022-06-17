#version 460 core

in vec2 texCoord;
//in vec4 stepX, stepY;

out vec4 fragColor;

uniform int mode;
//uniform vec4 cameraDir;
uniform sampler2D juliaTex;
uniform sampler2D dJuliaTex;
uniform sampler2D pos;

void main() {
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
        focusVal = texture(pos,texCoord).x / 2.f; // only the x component was written to
    }
    //focusVal *= 32.f;
    vec3 v = vec3(1.f,0.f,0.f);
    vec3 q = vec3(sin(focusVal));
    vec3 temp = cross(q.xyz, v) + v * cos(focusVal);


    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);
    //fragColor = texture(juliaTex,texCoord).rgba;
    //fragColor /= 2.f;
    //fragColor += cameraDir/0.5f;
    //fragColor += stepX + stepY;
}
