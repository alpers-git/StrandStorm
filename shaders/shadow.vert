#version 450

in vec4 vPos;

layout(location = 0) uniform mat4 to_screen_space; // mvp
layout(location = 1) uniform mat4 to_view_space; //mv
layout(location = 2) uniform mat3 normals_to_view_space; 

void main() {
    gl_Position = to_screen_space * vPos;
}