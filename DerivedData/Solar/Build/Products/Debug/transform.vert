#version 120
uniform vec3 center;
varying vec4 color;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz + center, 1.0);
    color = gl_Color;
}
