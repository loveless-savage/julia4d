#version 460 core

layout (location=0) in vec4 pixelRayCoord;
in vec2 texCoord;

out vec4 fragColor;

uniform int frame;
uniform sampler2D dJuliaTex;
//uniform sampler2D mandelbrotTex;

void main() {
    // TODO: multipass
    float t = frame/1000;
    t /= 100;

    /*
    //fragColor = texture(juliaTex,texCoord).rgba;// + vec4(t);
    vec3 v = texture(juliaTex,texCoord).rgb;
    vec3 q = vec3(sin(t));
    vec3 temp = cross(q.xyz, v) + v * cos(t);
    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);
    //fragColor /= 2.f;
    */
    fragColor = texture(dJuliaTex, texCoord);
    fragColor.xyz += 0.5 * vec3(sin(t)-0.3);
}
