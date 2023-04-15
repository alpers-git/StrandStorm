#version 450
precision mediump float;

//------------ Variying ------------
in vec3 v_space_norm;
in vec3 w_space_pos;
in vec3 v_space_pos;

//------------ Uniform ------------
//layout(location = 1) uniform mat4 to_view_space; //mv
uniform mat4 to_light_view_space;
uniform vec3 light_dir;
uniform vec3 light_color;
uniform float light_intensity;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

uniform sampler2DShadow shadow_map;


out vec4 color;

const vec2 poissonDisk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main() {
    color = vec4(0, 0, 0, 1);
    vec3 v_space_norm = normalize(v_space_norm);

    //vec3 v_light_position = (vec4(light_pos, 0) * to_view_space).xyz;

    vec3 l = normalize(-light_dir);//normalize(l); //light vector
    vec3 h = normalize(l + vec3(0, 0, 1)); //half vector

    float cos_theta = dot(l, v_space_norm);
    if(cos_theta >= 0) {
        vec3 diffuse = diffuse * vec3(max(cos_theta, 0));
        vec3 specular = specular * vec3(pow(max(dot(h, v_space_norm), 0), shininess));

        vec4 lv_space_pos = to_light_view_space * vec4(w_space_pos, 1.0);
        float shadow = 0;
        for (int i=0;i<1;i++){
            if(textureProj(shadow_map, lv_space_pos /*+ vec4(poissonDisk[i]/700, 0, 0)*/) < lv_space_pos.z)
                shadow += 0.99;
        }
        color +=  vec4((light_intensity - shadow) * light_color *(specular + diffuse), 1);
    }

    color = clamp(color + vec4(ambient, 1), 0, 1);//ambient
}