#version 460

in VertexData {
    vec3 pos;      // World position
} V;

out vec4 fragColor;

void main() {
    fragColor = vec4(1.0);
}