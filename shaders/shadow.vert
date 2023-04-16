#version 450

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;



layout(location = 0) uniform mat4 to_clip_space; // mvp

void main() {
    gl_Position = to_clip_space * vec4(vPos, 1.0);
}