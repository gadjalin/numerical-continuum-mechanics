#version 330

layout (location = 0) in vec2 vPos;
layout (location = 1) in vec4 vColor;
layout (location = 2) in vec2 vTexCoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewProjectionMatrix;

out vec4 iColor;
out vec2 iTexCoord;

void main()
{
   gl_Position = ViewProjectionMatrix * ModelMatrix * vec4(vPos.xy, 0.0, 1.0);
   iColor = vColor;
   iTexCoord = vTexCoord;
}
