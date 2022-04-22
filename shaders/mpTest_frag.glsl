#version 460 core

in vec4 pixelRayCoord;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D juliaTex_I;
//uniform sampler2D mandelbrotTex;

void main() {
    // TODO: multipass
    float t = 0.003f;

    //fragColor = texture(juliaTex,texCoord).rgba;// + vec4(t);
    vec3 v = texture(juliaTex_I,texCoord).rgb;
    vec3 q = vec3(sin(t));
    vec3 temp = cross(q.xyz, v) + v * cos(t);
    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);
    //fragColor /= 2.f;
}
