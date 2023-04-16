#version 450

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;


out vec3 v_space_norm;
out vec3 w_space_pos;
//out vec3 v_space_pos;


layout(location = 0) uniform mat4 to_clip_space; // mvp
//layout(location = 1) uniform mat4 to_view_space; //mv
layout(location = 2) uniform mat3 normals_to_view_space; 
layout(location = 3) uniform mat4 to_world_space; //m

void main() {
    gl_Position = to_clip_space * vec4(vPos, 1.0);
    v_space_norm = normals_to_view_space * vNormal;
    //v_space_pos = (to_view_space * vec4(vPos, 1.0)).xyz;
    w_space_pos = (to_world_space * vec4(vPos, 1.0)).xyz;
}
     