#version 450
precision highp float;

in vec3 c_space_pos;

uniform float dk;
uniform sampler2D depth_map;
uniform vec2 screen_res;

out vec4 opacities;

void main()
{
    float depth_val = texture(depth_map, gl_FragCoord.xy / screen_res).r;
    uint layer = uint(floor((depth_val - c_space_pos.z) / dk));
    opacities[layer] = depth_val - c_space_pos.z + float(layer) * dk;
}