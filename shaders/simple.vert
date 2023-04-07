#version 460

in vec3 vPos;
in vec3 vNormal;

out VertexData {
     vec3 normal;   // World normal
     vec3 pos;      // World position
} V;

uniform mat4 uTModel;
uniform mat4 uTProj;
uniform mat4 uTView;

void main()
{
     V.normal = mat3(uTModel) * vNormal;
     V.pos = (uTModel * vec4(vPos, 1.0)).xyz;
     gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
}