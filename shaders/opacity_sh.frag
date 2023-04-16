#version 450
precision highp float;

in vec3 v_space_pos;

uniform float dk;
uniform sampler2D depth_map;
uniform vec2 screen_res;

out vec4 opacities;

void main()
{
    float depth_val = texture(depth_map, gl_FragCoord.xy / screen_res).r;
    if(depth_val + dk > v_space_pos.z )
    {
        opacities.r = v_space_pos.z - depth_val;
    }
    else if(depth_val + 2.0 * dk < v_space_pos.z )
    {
        opacities.g = v_space_pos.z - depth_val - dk;
    }
    else if(depth_val + 3.0 * dk < v_space_pos.z )
    {
        opacities.b = v_space_pos.z - depth_val - 2.0 * dk;
    }
    else
    {
        opacities.a = v_space_pos.z - depth_val - 3.0 * dk;
    }
}