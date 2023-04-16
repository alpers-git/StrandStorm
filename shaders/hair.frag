#version 460

out vec4 fragColor;

uniform vec4 hairColor;

void main() {
    fragColor = hairColor;
}