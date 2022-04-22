#version 460 core
in vec4 pixelRayCoord;

uniform vec2 offset;
//uniform vec2 z0;

layout (location=0) out vec4 fragColor;
layout (location=1) out vec4 dFragColor;

void main(){
    fragColor = pixelRayCoord;
    vec2 z = pixelRayCoord.xy;
    vec2 c = offset;
    // derivative of z w/ respect to z0
    vec4 dz = vec4(1.f,0.f,0.f,0.f);

    int i;
    for(i = 0; i < 24; i++){
        // iterate derivative first so we can roll values correctly
        dz = vec4(
            z.x*dz.x - z.y*dz.y,
            z.x*dz.y + z.y*dz.x,
            z.x*dz.z - z.y*dz.w + 1.f,
            z.x*dz.w + z.y*dz.z
        );
        dz *= 2.f;
        // now interate z
        z = vec2(
            z.x*z.x - z.y*z.y + c.x,
            2.f*z.x*z.y + c.y
        );
        if( length(z) > 2.f ) break;
    }

    if (i == 24){
        fragColor = vec4(0.f);
        dFragColor = vec4(0.f);
        return;
    }

    // coloring based on iteration count
    fragColor = vec4(0.f);
    fragColor.x = i/32.f;
    // coloring based on derivative
    dFragColor = dz;


    /*
    fragColor = vec4(i/8.f);
    fragColor.x = i/8.f;
    //fragColor.x = max(fragColor.x, 0.f);
    //fragColor.x = min(fragColor.x, 1.f);

    fragColor.y = i/8.f - 1.f;
    //fragColor.y = max(fragColor.y, 0.f);
    //fragColor.y = min(fragColor.y, 1.f);

    fragColor.z = 1.f - i/8.f;
    //fragColor.z = max(fragColor.z, 0.f);
    //fragColor.z = min(fragColor.z, 1.f);
*/
}
