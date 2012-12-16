uniform sampler2D textureMap;

void main (void)
{
    gl_FragColor = texture2D(textureMap, gl_TexCoord[0].st);
}
