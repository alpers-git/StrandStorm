#version 450

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

// Model-view-projection into clip space
uniform mat4 to_clip_space;
// Model-view-projection into tex space
uniform mat4 to_tex_space;

// Fragment in texture space
out vec3 light_sp_pos;

void main() {
    gl_Position = to_clip_space * vec4(vPos, 1.0);
    light_sp_pos = (to_tex_space * vec4(vPos, 1.0)).xyz;
}