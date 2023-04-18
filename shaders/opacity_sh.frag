#version 450
precision highp float;

in vec3 light_sp_pos;

uniform float dk;
uniform sampler2D depth_map;

out vec4 opacities;

void main()
{
    float surfDepth = texture(depth_map, light_sp_pos.xy).r;
    float relDepth = light_sp_pos.z - surfDepth;
    int layer = min(3, int(floor(relDepth / dk)));
    if (layer >= 0) {
        vec4 c = vec4(1.0);
        c[layer] = relDepth - (layer * dk);
        opacities = c;
    } 
    else {
        discard;
    }
}