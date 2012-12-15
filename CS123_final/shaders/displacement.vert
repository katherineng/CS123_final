#extension GL_EXT_gpu_shader4: enable
#extension GL_ARB_gpu_shader_fp64: enable

varying float displacement;
uniform sampler2D textureMap;
uniform int p[512];

/*int p[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };*/
   
float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10); 
}

float lerp(float t, float a, float b) { 
    return a + t * (b - a); 
}

float grad(int hash, float x, float y, float z) {
    int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
    float u = h<8 ? x : y;                // INTO 12 GRADIENT DIRECTIONS.
    float v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

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
	 
	x = x/widthGrid - ((int) floor(x/widthGrid));//mod(x, currGrid);//int(x/currGrid);
	y = y/widthGrid - ((int) floor(y/widthGrid));//mod(y, currGrid);//int(y/currGrid);
	z = z/widthGrid - ((int) floor(z/widthGrid));//mod(z, currGrid);//int(z/currGrid);
	
	float u = fade(x);
	float v = fade(y);
	float w = fade(z);
	
	int A = p[X] + Y;
	int AA = p[A] + Z;
	
	int AB = p[A+1] +Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;
	
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
    displacement = mod(abs(perlinNoise(gl_Vertex.x,gl_Vertex.y, gl_Vertex.z)), 1.0);////min(max(0.0,perlinNoise(gl_Vertex.x,gl_Vertex.y, gl_Vertex.z)), 1.0);
    vec4 vertexPrime = gl_Vertex+displacement*.3*vec4(gl_Normal,0);
    gl_Position = gl_ModelViewProjectionMatrix * vertexPrime;    

    //gl_Position = ftransform();
 /*   
    displacement = gl_TexCoord[0].x ;
    vec4 texture = texture2D(textureMap, gl_TexCoord[0].st);
    float df = .3*texture.x + .59*texture.y + .11*texture.z;
    vec4 vertexPrime = gl_Vertex + df*7*vec4(gl_Normal,0);
    gl_Position = gl_ModelViewProjectionMatrix*vertexPrime;*/
}
