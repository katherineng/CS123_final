//uniform samplerCube CubeMap;
uniform sampler2D textureMap;
//varying vec3 normal, lightDir, r;
//uniform float x,y,z;
varying float displacement;

void main (void)
{/*
        vec4 final_color = textureCube( CubeMap, r);
        vec3 N = normalize(normal);
        vec3 L = normalize(lightDir);
        float lambertTerm = dot(N,L);
        if(lambertTerm > 0.0)
        {
                // Specular
                final_color += textureCube( CubeMap, r);
        }*/
    //texture2D(textureMap, gl_TexCoord[0].st);//
    gl_FragColor = texture2D(textureMap, gl_TexCoord[0].st);//vec4(0,displacement,0,1.0);//final_color;
}
