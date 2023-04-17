#version 450

in vec4 vPos;

layout(location = 0) uniform mat4 to_clip_space; // mvp

void main() {
    gl_Position = to_clip_space * vPos;
}
