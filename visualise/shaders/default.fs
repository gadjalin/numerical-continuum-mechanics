#version 330

out vec4 FragColor;

in vec4 iColor;
in vec2 iTexCoord;

uniform sampler2D tex0;

void main()
{
   FragColor = iColor;
}

