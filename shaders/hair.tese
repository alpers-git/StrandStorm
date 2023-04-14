#version 460

layout(isolines, equal_spacing, ccw) in;

// Input is 4 bezier control points
in VertexData {
    vec3 pos;
} controlPoints[];

// Output is position of vertex along patch's bezier curve
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
    return mt3 * controlPoints[0].pos +
        3.0 * mt2 * t * controlPoints[1].pos +
        3.0 * mt * t2 * controlPoints[2].pos +
        t3 * controlPoints[3].pos;
}

uniform mat4 uTProj;
uniform mat4 uTView;

void main() {
    // gl_TessCoord.x is distance along each isoline, [0,1]
    vec3 p = bezier(gl_TessCoord.x);
    gl_Position = uTProj * uTView * vec4(p, 1.0);
    v_out.pos = p;
}