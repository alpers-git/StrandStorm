#version 450

layout (location = 0) in vec3 pos;
layout(location = 1) uniform mat4 mvp;
layout (location = 2) out float distance;

void main() {
    gl_Position = mvp * vec4(pos, 1.0);
    distance = gl_Position.z;
}