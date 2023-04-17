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
    float absorption = 0.0;
    for(int disk = 0; disk < 4; disk++)
    {
        // Depth value at hair surface
        float surfDepth = texture(depthMap, light_clip_pos.xy + vec2(poissonDisk[disk]/700)).r;
        // Depth relative to surface
        float relDepth = (surfDepth - light_clip_pos.z) / dk;
        // Opacity map layer
        int layer = clamp(int(floor(relDepth)), 0, 3);
        // Opacity values
        vec4 op = texture(opacityMaps, light_clip_pos.xy  + vec2(poissonDisk[disk]/700));
        for (int i = 1; i <= 3; i++)
            op[i] += op[i - 1];
        absorption += mix(layer > 0 ? op[layer - 1] : 1.0,
            op[layer], clamp(relDepth - layer, 0.0, 1.0));
    }
    return 1.0 - absorption / 4.0;
}

out vec4 fragColor;


void main() {
    // Compute opacity value and blend
    float shadowFraction = shadows_enabled ? getOpacity() : 1.0;
    fragColor = vec4(hair_color.rgb * shadowFraction, hair_color.a) + vec4(0.01, 0.01, 0.01, 0.0);


    // Depth value at hair surface
    float surfDepth = texture(depthMap, light_clip_pos.xy).r;
    // Depth relative to surface (value between 0.0 and 3.0)
    float relDepth = clamp((light_clip_pos.z - surfDepth) / dk, 0.0, 3.0);
    // Opacity map layer
    int curLayers[2] = {int(floor(relDepth)), int(ceil(relDepth))};
    // Opacities at current fragment
    vec4 op = texture(opacityMaps, light_clip_pos.xy);
    for (int i = 1; i <= 3; i++)
        op[i] += op[i - 1];
    // Interpolate opacity values
    float t = clamp(relDepth - floor(relDepth), 0.0, 1.0);
    float accumDepth = mix(op[curLayers[0]], op[curLayers[1]], t);
    float opacity = 1.0 - accumDepth;

    vec4 layerColors[4] = {
        vec4(1.0, 0.0, 0.0, 1.0),
        vec4(0.0, 1.0, 0.0, 1.0),
        vec4(1.0, 0.0, 1.0, 1.0),
        vec4(0.0, 0.0, 1.0, 1.0)
    };
    // fragColor = layerColors[curLayers[0]];
    // fragColor = mix(layerColors[curLayers[0]], layerColors[curLayers[1]], t);
    fragColor = vec4(vec3(opacity) * vec3(1.0, 0.0, 1.0), 1.0);
}