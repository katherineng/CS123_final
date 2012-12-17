#extension GL_EXT_gpu_shader4: enable
#extension GL_ARB_gpu_shader_fp64: enable


//Credit to Ken Perlin


uniform sampler2D textureMap;
uniform int displacementEnabled;
uniform int p[512];

// smoothing function to prevent abrupt changes in displacement amount
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); 
}

// linear interpolation function
float lerp(float t, float a, float b) { 
    return a + t * (b - a); 
}

// gradient function
float grad(int hash, float x, float y, float z) {
    int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
    float u = h<8 ? x : y;                // INTO 12 GRADIENT DIRECTIONS.
    float v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

// given a vertex position (x, y, z), apply perlin noise
float perlinNoise(float x, float y, float z){
    float total;
    float width = .3;
    int grid = 2;
    float widthGrid;
    
    for (int i = 0; i < 5; i++, grid *= 2){
	widthGrid = width/grid;
	
	int X = ((int) floor(x/widthGrid)) & 255;
	int Y = ((int) floor(y/widthGrid)) & 255;
	int Z =  ((int) floor(z/widthGrid)) & 255;
	 
        x = x/widthGrid - ((int) floor(x/widthGrid)); //mod(x, currGrid);//int(x/currGrid);
        y = y/widthGrid - ((int) floor(y/widthGrid)); //mod(y, currGrid);//int(y/currGrid);
        z = z/widthGrid - ((int) floor(z/widthGrid)); //mod(z, currGrid);//int(z/currGrid);
	
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);
	
	int A = p[X] + Y;
	int AA = p[A] + Z;
	
	int AB = p[A+1] +Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;
	
        // total displacement based on linear interpolant of surrounding sample points
	total += (lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  
                                     grad(p[BA  ], x-1, y  , z   )), 
                             lerp(u, grad(p[AB  ], x  , y-1, z   ),  
                                     grad(p[BB  ], x-1, y-1, z   ))),
                     lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  
                                     grad(p[BA+1], x-1, y  , z-1 )),
                             lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                     grad(p[BB+1], x-1, y-1, z-1 ))))) * (1.0/(i+1));
    }
    return total;
}

void main(){
    gl_TexCoord[0] = gl_MultiTexCoord0;
    float displacement = mod(abs(perlinNoise(gl_Vertex.x,gl_Vertex.y, gl_Vertex.z)), 1.0);////min(max(0.0,perlinNoise(gl_Vertex.x,gl_Vertex.y, gl_Vertex.z)), 1.0);
    vec4 vertexPrime;

    if (displacementEnabled == 1){
        vertexPrime = gl_Vertex + displacement * .3 * vec4(gl_Normal, 0);
    } else {
        vertexPrime = gl_Vertex;
    }

    gl_Position = gl_ModelViewProjectionMatrix * vertexPrime;
}
