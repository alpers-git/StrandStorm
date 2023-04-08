#version 460

in vec3 vPos;

out VertexData {
     vec3 pos;      // World position
} V;

uniform mat4 uTModel;
uniform mat4 uTProj;
uniform mat4 uTView;

void main()
{
     V.pos = (uTModel * vec4(vPos, 1.0)).xyz;
     gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
}