#version 460

in vec3 vPos;

uniform mat4 uTModel;
uniform mat4 uTView;
uniform mat4 uTProj;

void main()
{
     gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
}