#version 450
precision mediump float;

//------------ Variying ------------
in vec3 v_space_norm;
in vec3 v_space_pos;

out vec4 color;

void main() {
    color = vec4(0, 0, 0, 1);
    vec3 v_space_norm = normalize(v_space_norm);

    vec3 light_position = vec3(0, 1, 0);//celing light

    vec3 l = normalize(light_position - v_space_pos);//normalize(l); //light vector
    vec3 h = normalize(l + vec3(0, 0, 1)); //half vector

    float cos_theta = dot(l, v_space_norm);
    if(cos_theta >= 0) {
        vec3 diffuse = vec3(max(cos_theta, 0));
        vec3 specular = vec3(pow(max(dot(h, v_space_norm), 0), 300));
        color += vec4((specular + diffuse), 1);
    }

    color = clamp(color + vec4(0.8,0.8,0.8, 1), 0, 1);//ambient
}