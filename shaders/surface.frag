#version 450
precision mediump float;

//------------ Variying ------------
in vec3 v_space_norm;
in vec3 v_space_pos;

//------------ Uniform ------------
layout(location = 1) uniform mat4 to_view_space; //mv
uniform vec3 light_pos;
uniform vec3 light_color;
uniform float light_intensity;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;


out vec4 color;

void main() {
    color = vec4(0, 0, 0, 1);
    vec3 v_space_norm = normalize(v_space_norm);

    //vec3 v_light_position = (vec4(light_pos, 0) * to_view_space).xyz;

    vec3 l = normalize(light_pos - v_space_pos);//normalize(l); //light vector
    vec3 h = normalize(l + vec3(0, 0, 1)); //half vector

    float cos_theta = dot(l, v_space_norm);
    if(cos_theta >= 0) {
        vec3 diffuse = diffuse * vec3(max(cos_theta, 0));
        vec3 specular = specular * vec3(pow(max(dot(h, v_space_norm), 0), shininess));
        color +=  vec4(light_intensity * light_color *(specular + diffuse), 1);
    }

    color = clamp(color + vec4(ambient, 1), 0, 1);//ambient
}