#version 450

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;



layout(location = 0) uniform mat4 to_screen_space; // mvp
layout(location = 1) uniform mat4 to_view_space; //mv
layout(location = 2) uniform mat3 normals_to_view_space; 

void main() {
    gl_Position = to_screen_space * vec4(vPos, 1.0);
}