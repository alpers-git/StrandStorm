#version 460

in vec3 vPos;
in vec3 vTangent;

uniform mat4 uTModel;
uniform mat4 uTView;
uniform mat4 uTProj;
uniform mat4 uTLight;
uniform mat4 to_view_space;

out vec3 light_clip_pos;
out vec3 fTangent;
out vec3 viewDir;

void main()
{
    gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
    vec4 lcp = uTLight * vec4(vPos, 1.0);
    light_clip_pos = lcp.xyz/lcp.w;
    fTangent = (to_view_space * vec4(vTangent,0.0)).xyz;
    viewDir = -(to_view_space * vec4(vPos,1.0)).xyz;
}
