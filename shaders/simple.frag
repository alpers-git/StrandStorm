#version 460

in VertexData {
    vec3 normal;   // World normal
    vec3 pos;      // World position
} V;

out vec4 fragColor;

void main() {
    vec3 normal = normalize(V.normal);
    
    fragColor = vec4(normal, 1.0);
}