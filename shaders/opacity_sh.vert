#version 450

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;



layout(location = 0) uniform mat4 to_clip_space; // mvp
//layout(location = 1) uniform mat4 to_view_space; //mv

out vec3 c_space_pos;

void main() {
    gl_Position = to_clip_space * vec4(vPos, 1.0);
    c_space_pos = (to_clip_space * vec4(vPos, 1.0)).xyz;
}