//uniform samplerCube CubeMap;
uniform sampler2D textureMap;
//varying vec3 normal, lightDir, r;
//uniform float x,y,z;
varying float displacement;
varying vec3 normal;
varying vec3 pos;

void main (void)
{
    gl_FragColor = texture2D(textureMap, gl_TexCoord[0].st);
}
