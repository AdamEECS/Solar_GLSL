#version 120

uniform sampler2D tex0;
uniform sampler2D tex1;

varying vec2 texcoord;

void main()
{
    gl_FragColor = texture2D(tex0, texcoord) * texture2D(tex1, texcoord);
}
