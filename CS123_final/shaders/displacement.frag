/*uniform samplerCube CubeMap;
varying vec3 normal, lightDir, r;

void main (void)
{
        vec4 final_color = textureCube( CubeMap, r);
        vec3 N = normalize(normal);
        vec3 L = normalize(lightDir);
        float lambertTerm = dot(N,L);
        if(lambertTerm > 0.0)
        {
                // Specular
                final_color += textureCube( CubeMap, r);
        }
        gl_FragColor = final_color;
}*/

/*
        Generic Fragment Shader
        with Phong Directional lighting
*/

//////////////////////////
// Phong Directional FS //
//////////////////////////

// -- Lighting varyings (from Vertex Shader)
varying vec3 norm, lightDir0, halfVector0;
varying vec4 diffuse0, ambient;

vec4 phongDir_FS()
{
        vec3 halfV;
        float NdotL, NdotHV;

        // The ambient term will always be present
        vec4 color = ambient;

        // compute the dot product between normal and ldir
        NdotL = max(dot(norm, lightDir0),0.0);

        if (NdotL > 0.0) {
                color += diffuse0 * NdotL;
                halfV = normalize(halfVector0);
                NdotHV = max(dot(norm, halfV), 0.0);
                color +=	gl_FrontMaterial.specular *
                                gl_LightSource[0].specular *
                                pow(NdotHV, gl_FrontMaterial.shininess);
        }
        return color;
}

///////////////
// Main Loop //
///////////////

void main()
{
        // Call lighting function and return result
        gl_FragColor = phongDir_FS();
}
