#version 460 core
in vec4 pixelRayCoord;

uniform vec2 offset;
//uniform vec2 z0;

out vec4 fragColor;

void main(){
    fragColor = pixelRayCoord;
    vec2 z = pixelRayCoord.xy;
    vec2 c = offset;
    c *= 0.7;
    c.x -= 0.2;

    int i;
    for(i = 0; i < 24; i++){
        z = vec2(z.x*z.x-z.y*z.y + c.x, 2.f*z.x*z.y + c.y);
        if( length(z) > 2.f ) break;
    }

    if (i == 24){
        fragColor = vec4(0.f);
        return;
    }

    vec3 v = vec3(1.f,0.f,0.f);
    vec3 q = vec3(sin(i/8.f));
    vec3 temp = cross(q.xyz, v) + v * cos(i/8.f);
    vec3 rotated = v + 2.0*cross(q.xyz, temp);
    fragColor = vec4(rotated,1.f);

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
