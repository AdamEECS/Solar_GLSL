#version 120

uniform sampler2D currentColorBuffer;
uniform sampler2D accumSourceBuffer;

varying vec2 texcoord;

const float MIX_RATIO = 0.2;

void main()
{
    gl_FragColor = vec4(texture2D(accumSourceBuffer, texcoord).rgb * (1.0 - MIX_RATIO) + texture2D(currentColorBuffer, texcoord).rgb * MIX_RATIO, 1.0);
}
