#version 460

in vec4 vPos;

uniform mat4 uTModel;
uniform mat4 uTView;
uniform mat4 uTProj;

void main()
{
     gl_Position = uTProj * uTView * uTModel * vPos;
}