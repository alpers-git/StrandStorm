#version 460

in vec3 light_clip_pos;

uniform sampler2D depthMap;
uniform sampler2D opacityMaps;
uniform float dk;

uniform vec4 hair_color;

float getOpacity() {
    float depth_val = texture(depthMap, light_clip_pos.xy).r;

    uint layer = uint(floor((depth_val - light_clip_pos.z) / dk));
    vec4 op = texture(opacityMaps, light_clip_pos.xy);
    for (int i = 1; i < layer; i++) {
        op[i] += op[i - 1];
    }
    return mix(op[layer - 1], op[layer], (depth_val - light_clip_pos.z) / dk - layer);
}

out vec4 fragColor;


void main() {
    fragColor = vec4(hair_color.xyz *  getOpacity(), hair_color.w);
    //fragColor.a = 1.0;
}