#version 460 core

in vec2 texCoord;

uniform int frame;
uniform sampler2D juliaTex;

out float fragColor;

void main() {
    // circular viewport
    vec2 z = texCoord.xy * 2.f;
    z -= vec2(1.f);
    // rotate texture coordinate vector
    float t = 0.001;
    z = vec2(
        cos(t)*z.x-sin(t)*z.y,
        sin(t)*z.x+cos(t)*z.y
    );
    z = 0.5f*(z + vec2(1.f));
    // avoid error pixels
    if( z.x >= 1.f || z.x <= 0.f
     || z.y >= 1.f || z.y <= 0.f){
        fragColor = 0.f;
        return;
    }
    fragColor = texture(juliaTex, z).x;
}
