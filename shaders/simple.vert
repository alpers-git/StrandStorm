#version 450

out vec4 color;
layout (location = 2) in float dist;
void main() {
     color = vec4(1/dist, dist, 1.0f,  1.0f);
}