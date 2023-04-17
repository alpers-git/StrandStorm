#version 460

in vec3 vPos;

uniform mat4 uTModel;
uniform mat4 uTView;
uniform mat4 uTProj;
uniform mat4 toLightClipSpace;

out vec3 light_clip_pos;

void main()
{
    gl_Position = uTProj * uTView * uTModel * vec4(vPos, 1.0);
    vec4 lcp = toLightClipSpace * vec4(vPos, 1.0);
    light_clip_pos = lcp.xyz/lcp.w;
}
