//resolution of each test
#define RESOL 50

//square a 2D vector as a complex value
vec2 csq(vec2 z){
        return vec2( z.x*z.x-z.y*z.y , 2.0*z.x*z.y );
}

// square root of a complex number
vec2 csqrt(vec2 z){
// for mathematical proof: https://math.stackexchange.com/questions/44406/how-do-i-get-the-square-root-of-a-complex-number
    if(z.y == 0.0) return vec2(0.0,sqrt(length(z))); // doesn't work for negative numbers!
    // square root of the complex number z takes the form:
    // √z = (z+|z|) * √|z| / |z+|z||
    vec2 rootz = vec2(z.x + length(z), z.y);
    rootz *= sqrt(length(z)) / length(rootz);
    return rootz;
}

//given the 4D Julia set coordinates, test for divergence with a certain resolution
vec2 runTest(vec2 zi, vec2 c){
    bool cutoff = true;
    vec2 z = zi;

    int n = 0;
    while(n<RESOL){
        if( cutoff && length(z)>2.0 ) break;
        z = csq(z) + c;
        n++;
    }
    
    if( length(z)<=2.0 ) return vec2(0.0,0.0);
    
    for(int i = 0; i<n; i++){
        z = csqrt(z);
    }
    
    return z;
}

//calculate the color of the model given a depth
vec4 modelColor(float dep){
    if(length(dep) == 0.0) return vec4(0.0);

    float temp[3] = float[3](0.0,0.0,0.0);
    float depmod = mod(dep,0.2);
    if(dep==1.0) {
        return vec4(0.0);
    }

    int depnum = int((dep-depmod)*5.0);

    if((depnum&1)>0) depmod = 0.2-depmod;

    temp[(4-depnum) % 3]=5.0*depmod;

    depnum += 1;
    depnum >>= 1;

    temp[depnum]=1.0;

    return vec4(temp[0],temp[1],temp[2],1.0);
}

// checkerboard coloring
vec4 checkerboard(vec2 uz, float width){
    // eliminate the filled in set
    if(length(uz) == 0.0) return vec4(0.0);
    
    // find checkerboard position using step function
    vec2 check = floor(uz/width);
    
    if( mod(check.x + mod(check.y, 2.0), 2.0) == 0.0 ){
        return vec4(0.8);
    }else{
        return vec4(0.3);
    }
}

// Hubbard-Douady potential
float hubbard_douady(vec2 uz){
    float hd = log(length(uz));
    if(hd<0.0) return 0.0;
    return exp(-1.0*hd);
}


void mainImage( out vec4 fragColor, in vec2 fragCoord ){
    // Adjusted pixel coordinates; transform to window of Mandelbrot Set
    vec2 z = 2.0*fragCoord.xy-iResolution.xy;
    vec2 c = 2.0*iMouse.xy-iResolution.xy;
    z *= 2.0/min(iResolution.x,iResolution.y);
    c *= 2.0/min(iResolution.x,iResolution.y);
    
    //Test for divergence
    vec2 uVal = runTest(z,c);

    // Output to screen
    //fragColor = checkerboard(uVal,0.1);
    fragColor = vec4( vec3(hubbard_douady(uVal)),0.0);
}
