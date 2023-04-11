#version 460

layout(isolines, equal_spacing, ccw) in;

// Input is 4 bezier control points
in VertexData {
    vec3 pos;
} v_in[];

out VertexData {
    vec3 pos;
} v_out;

// Evaluate bezier curve at t
vec3 bezier(float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float mt = 1.0 - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    return mt3 * v_in[0].pos +
        3.0 * mt2 * t * v_in[1].pos +
        3.0 * mt * t2 * v_in[2].pos +
        t3 * v_in[3].pos;
}

void main() {
    vec3 p = bezier(gl_TessCoord.x);
    gl_Position = vec4(p, 1.0);
    v_out.pos = p;
}