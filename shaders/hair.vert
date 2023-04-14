#version 460

in vec3 vPos;

void main()
{
     gl_Position = vec4(vPos, 1.0);
}