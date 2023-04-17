#version 460

in vec3 light_clip_pos;

uniform sampler2D depthMap;
uniform sampler2D opacityMaps;
uniform float dk;
uniform vec4 hair_color;
uniform bool shadows_enabled;

const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float getOpacity() {
    float absorbtion = 0.0;
    for(int disk = 0; disk < 4; disk++)
    {
        float depth_val = texture(depthMap, light_clip_pos.xy + vec2(poissonDisk[disk]/700)).r;
        int layer = clamp(int(floor((depth_val - light_clip_pos.z) / dk)), 0, 3);

        vec4 op = texture(opacityMaps, light_clip_pos.xy  + vec2(poissonDisk[disk]/700));
        for (int i = 1; i <= layer; i++)
            op[i] += op[i - 1];
        absorbtion += mix(layer > 0 ? op[layer - 1] : 1.0, 
            op[layer], (depth_val - light_clip_pos.z) / dk - layer);
    }
    return 1.0 - absorbtion / 4.0;
}

out vec4 fragColor;


void main() {
    float opacity = shadows_enabled ? getOpacity() : 1.0;
    fragColor = vec4(hair_color.xyz * opacity, hair_color.w);
    //ambient
    fragColor += vec4(0.01, 0.01, 0.01, 0.0);
    //fragColor.a = 1.0;
}