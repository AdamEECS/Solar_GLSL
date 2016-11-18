#version 120

uniform vec3 center;

varying vec2 texcoord;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz + center, 1.0);
    texcoord = gl_MultiTexCoord0.xy;
}
