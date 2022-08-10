#version 460 core
in vec2 texCoord;

//uniform vec2 z0;

layout (location=0) out float fragColor;

void main(){
    vec2 z = vec2(0.f);
    vec2 c = texCoord.xy * 4.f;
    c -= vec2(2.5f,2.f);

    int i;
    for(i = 0; i < 24; i++){
        z = vec2(z.x*z.x-z.y*z.y + c.x, 2.f*z.x*z.y + c.y);
        if( length(z) > 2.f ) break;
    }

    fragColor = i/8.f;
    /*
    fragColor.x = i/8.f - 1.f;
    fragColor.x = max(fragColor.x, 0.f);
    fragColor.x = min(fragColor.x, 1.f);

    fragColor.y = i/8.f - 2.f;
    fragColor.y = max(fragColor.y, 0.f);
    fragColor.y = min(fragColor.y, 1.f);

    fragColor.z = i/8.f;
    fragColor.z = max(fragColor.z, 0.f);
    fragColor.z = min(fragColor.z, 1.f);
    */
}
